/** 
 * @file llfloaterao.cpp
 * @brief clientside animation overrider
 * by Skills Hak
 * rewritten by Skills
 * and again
 */

#include "llviewerprecompiledheaders.h"

#include "floaterao.h"

#include "llagent.h"
#include "llvoavatar.h"
#include "llanimationstates.h"
#include "lluictrlfactory.h"
#include "llinventoryview.h"
#include "llstartup.h"
#include "llpreviewnotecard.h"
#include "llviewertexteditor.h"
#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llspinctrl.h"
#include "chatbar_as_cmdline.h"

#include "llinventory.h"
#include "llinventoryview.h"
#include "roles_constants.h"
#include "llviewerregion.h"

#include "llpanelinventory.h"
#include "llinventorybridge.h"

//#include "DiamondAoInt.h"
#include "jc_lslviewerbridge.h"
#include "floaterexploreanimations.h"
#include "llboost.h"
#include <boost/regex.hpp>

void cmdline_printchat(std::string message);

// TIMERS -------------------------------------------------------

AOStandTimer* mAOStandTimer;

AOStandTimer::AOStandTimer() : LLEventTimer( gSavedPerAccountSettings.getF32("PhoenixAOStandInterval") )
{
	AOStandTimer::tick();
}
AOStandTimer::~AOStandTimer()
{
}
void AOStandTimer::reset()
{
	mPeriod = gSavedPerAccountSettings.getF32("PhoenixAOStandInterval");
	mEventTimer.reset();
}
BOOL AOStandTimer::tick()
{
	if (gSavedPerAccountSettings.getBOOL("PhoenixAOStandCycling"))
	{
		LUA_CALL0("AOChangeStand");
	}
	return FALSE;
}

AOInvTimer::AOInvTimer() : LLEventTimer( (F32)1.0 )
{
}
AOInvTimer::~AOInvTimer()
{
}
BOOL AOInvTimer::tick()
{
	//todo: find a better way to preload the AO on login even though it is disabled 
	if (!(gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled"))) return TRUE; 


	if(LLStartUp::getStartupState() >= STATE_INVENTORY_SEND)
	{
		LLUUID phoenix_category = JCLSLBridge::findCategoryByNameOrCreate(phoenix_category_name);
		if(gInventory.isCategoryComplete(phoenix_category))
		{
			return LLFloaterAO::init();
		}
		else
		{
			gInventory.fetchDescendentsOf(phoenix_category);
		}
	}
	return FALSE;
}
// NC DROP -------------------------------------------------------

class AONoteCardDropTarget : public LLView
{
public:
	AONoteCardDropTarget(const std::string& name, const LLRect& rect, void (*callback)(LLViewerInventoryItem*));
	~AONoteCardDropTarget();

	void doDrop(EDragAndDropType cargo_type, void* cargo_data);

	//
	// LLView functionality
	virtual BOOL handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
								   EDragAndDropType cargo_type,
								   void* cargo_data,
								   EAcceptance* accept,
								   std::string& tooltip_msg);
protected:
	void	(*mDownCallback)(LLViewerInventoryItem*);
};


AONoteCardDropTarget::AONoteCardDropTarget(const std::string& name, const LLRect& rect,
						  void (*callback)(LLViewerInventoryItem*)) :
	LLView(name, rect, NOT_MOUSE_OPAQUE, FOLLOWS_ALL),
	mDownCallback(callback)
{
}

AONoteCardDropTarget::~AONoteCardDropTarget()
{
}

void AONoteCardDropTarget::doDrop(EDragAndDropType cargo_type, void* cargo_data)
{
}

BOOL AONoteCardDropTarget::handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
									 EDragAndDropType cargo_type,
									 void* cargo_data,
									 EAcceptance* accept,
									 std::string& tooltip_msg)
{
	BOOL handled = FALSE;
	if(getParent())
	{
		handled = TRUE;
		llinfos << "AONoteCardDropTarget::handleDragAndDrop(): LLViewerInventoryItem* inv_item = (LLViewerInventoryItem*)cargo_data;" << llendl;
		LLViewerInventoryItem* inv_item = (LLViewerInventoryItem*)cargo_data;
		llinfos << "AONoteCardDropTarget::handleDragAndDrop(): if(gInventory.getItem(inv_item->getUUID()))" << llendl;
		if(gInventory.getItem(inv_item->getUUID()))
		{
			llinfos << "AONoteCardDropTarget::handleDragAndDrop(): *accept=ACCEPT_YES_COPY_SINGLE;" << llendl;
			*accept = ACCEPT_YES_COPY_SINGLE;
			if(drop)
			{
				llinfos << "AONoteCardDropTarget::handleDragAndDrop(): mDropCallback(inv_item);" << llendl;
				mDownCallback(inv_item);
			}
		}
		else
		{
			llinfos << "AONoteCardDropTarget::handleDragAndDrop(): *accept=ACCEPT_NO;" << llendl;
			*accept = ACCEPT_NO;
		}
	}
	
	llinfos << "AONoteCardDropTarget::handleDragAndDrop(): return handled;" << llendl;
	return handled;
}

AONoteCardDropTarget * LLFloaterAO::mAOItemDropTarget;

// -------------------------------------------------------

struct struct_anim_types
{
	std::string name;
	int state;
};

std::vector<struct_anim_types> mAnimTypes;

struct struct_combobox
{
	LLComboBox* combobox;
	std::string name;
	int state;
};

std::vector<struct_combobox> mComboBoxes;

struct struct_default_anims
{
	LLUUID orig_id;
	LLUUID ao_id;
	int state;
};
std::vector<struct_default_anims> mAODefaultAnims;

struct struct_all_anims
{
	LLUUID ao_id;
	BOOL isPlaying;
	std::string anim_name;
	int state;
};
std::vector<struct_all_anims> mAOAllAnims;

struct struct_stands
{
	LLUUID ao_id;
	std::string anim_name;
};
std::vector<struct_stands> mAOStands;

struct struct_tokens
{
	std::string token;
	int state;
};
std::vector<struct_tokens> mAOTokens;

struct AOAssetInfo
{
	std::string path;
	std::string name;
};

//BOOL LLFloaterAO::fullfetch = FALSE;

int LLFloaterAO::mAnimationState = 0;
int LLFloaterAO::stand_iterator = 0;
LLComboBox* mcomboBox_stands;

LLUUID LLFloaterAO::invfolderid = LLUUID::null;
LLUUID LLFloaterAO::mCurrentStandId = LLUUID::null;

LLTabContainer* mTabContainer;
LLFloaterAO* LLFloaterAO::sInstance = NULL;

LLFloaterAO::LLFloaterAO():LLFloater(std::string("floater_ao"))
{
	llassert_always(sInstance == NULL);
	sInstance = this;
	LLUICtrlFactory::getInstance()->buildFloater(sInstance, "floater_ao.xml");
	sInstance->setVisible(FALSE);
	mTabContainer = getChild<LLTabContainer>("tabcontainer");

	if (mAOAllAnims.empty()) // ao hasn't been loaded yet (login with ao off) do it now
	{
		LLUUID phoenix_category = JCLSLBridge::findCategoryByNameOrCreate(phoenix_category_name);
		if((LLStartUp::getStartupState() >= STATE_INVENTORY_SEND) && gInventory.isCategoryComplete(phoenix_category))
		{
			LLFloaterAO::init();
		}
	}

	loadComboBoxes();
	updateLayout(sInstance);
}

LLFloaterAO::~LLFloaterAO()
{
    sInstance=NULL;
	for (std::vector<struct_combobox>::iterator iter = mComboBoxes.begin(); iter != mComboBoxes.end(); ++iter)
	{
		//if (iter->combobox) 
		iter->combobox = 0;
	}
	delete mAOItemDropTarget;
	mAOItemDropTarget = NULL;
}

void LLFloaterAO::toggle(void*)
{
	if (sInstance)
	{
		if(!sInstance->getVisible())
		{
			sInstance->open();
		}
		else
		{
			sInstance->close();
		}
	}
	else
	{
		sInstance = new LLFloaterAO();
		sInstance->setVisible(TRUE);
	}
}

void LLFloaterAO::onOpen()
{
	sInstance->setVisible(TRUE);
	sInstance->setFocus(TRUE);
}

void LLFloaterAO::onClose(bool app_quitting)
{
	sInstance->setVisible(FALSE);
	sInstance->setFocus(FALSE);
}

BOOL LLFloaterAO::postBuild()
{
	LLView *target_view = getChild<LLView>("ao_notecard");
	if(target_view)
	{
		if (mAOItemDropTarget)
		{
			delete mAOItemDropTarget;
		}
		mAOItemDropTarget = new AONoteCardDropTarget("drop target", target_view->getRect(), AOItemDrop);//, mAvatarID);
		addChild(mAOItemDropTarget);
	}
	
	childSetValue("ao_nc_text","(Lua handles this)");
	
	childSetAction("more_btn", onClickMore, this);
	childSetAction("less_btn", onClickLess, this);

	childSetAction("reloadcard",onClickReloadCard,this);
	childSetAction("opencard",	onClickOpenCard,this);
	childSetAction("prevstand",	onClickPrevStand,this);
	childSetAction("nextstand",	onClickNextStand,this);
	
	childSetCommitCallback("PhoenixAOEnabled",onClickToggleAO);
	childSetCommitCallback("PhoenixAOSitsEnabled",onClickToggleSits);
	childSetCommitCallback("PhoenixAOStandCycling",onCheckBoxCommit);
	childSetCommitCallback("PhoenixAONoStandsInMouselook",onClickNoMouselookStands);
	childSetCommitCallback("standtime",onSpinnerCommit);
	
	return TRUE;
}

void LLFloaterAO::updateLayout(LLFloaterAO* floater)
{
	if (floater)
	{
		BOOL advanced = gSavedSettings.getBOOL("PhoenixAOAdvanced");
		if (advanced)
		{
			floater->reshape(610,380);
		}
		else
		{
			floater->reshape(200,380);
		}
		
		floater->childSetVisible("more_btn", !advanced);
		floater->childSetVisible("less_btn", advanced);

		floater->childSetVisible("tabcontainer", advanced);
		floater->childSetVisible("tabdefaultanims", advanced);

		// Set relevant per-account stuff.
		floater->childSetValue("PhoenixAOEnabled", gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled"));
		floater->childSetValue("PhoenixAOSitsEnabled", gSavedPerAccountSettings.getBOOL("PhoenixAOSitsEnabled"));
		floater->childSetValue("PhoenixAOStandCycling", gSavedPerAccountSettings.getBOOL("PhoenixAOStandCycling"));
		floater->childSetValue("PhoenixAONoStandsInMouselook", gSavedPerAccountSettings.getBOOL("PhoenixAONoStandsInMouselook"));
		floater->childSetValue("standtime", gSavedPerAccountSettings.getF32("PhoenixAOStandInterval"));

		for (std::vector<struct_combobox>::iterator iter = mComboBoxes.begin(); iter != mComboBoxes.end(); ++iter)
		{
			if (iter->name == "stand") continue;
			floater->childSetVisible("textdefault"+iter->name, advanced);
			floater->childSetVisible(iter->name, advanced);
		}
		mTabContainer->selectFirstTab();
	}
}

void LLFloaterAO::updateLayout2(LLFloaterAO* floater)
{
	if(floater)
	{
		if(LLStartUp::getStartupState() == STATE_STARTED)
		{
			LLUUID itemidimport = (LLUUID)gSavedPerAccountSettings.getString("PhoenixAOConfigNotecardID");
			LLViewerInventoryItem* itemimport = gInventory.getItem(itemidimport);
			if(itemimport)
			{
				floater->childSetValue("ao_nc_text","Currently set to: "+itemimport->getName());
			}
			else if(itemidimport.isNull())
			{
				floater->childSetValue("ao_nc_text","Currently not set");
			}
			else
			{
				floater->childSetValue("ao_nc_text","Currently set to a item not on this account");
			}
		}
		else
		{
			floater->childSetValue("ao_nc_text","Not logged in");
		}
		LLFloaterAO::init();
	}
	else 
	{
		sInstance = new LLFloaterAO();
		updateLayout2(sInstance);
	}
}

BOOL LLFloaterAO::fullfetch = FALSE;

BOOL LLFloaterAO::init()
{
	return TRUE;
}

// AO LOGIC -------------------------------------------------------

void LLFloaterAO::run()
{
}

void LLFloaterAO::startAOMotion(const LLUUID& id, const BOOL stand, const BOOL announce)
{
	
}

void LLFloaterAO::stopAOMotion(const LLUUID& id, const BOOL stand)
{
	
}

void LLFloaterAO::ChangeStand(const BOOL announce)
{
	
}

int LLFloaterAO::getAnimationState()
{
	if (gAgent.getAvatarObject())
	{
		if (gAgent.getAvatarObject()->mBelowWater)
		{
			setAnimationState(STATE_AGENT_FLOATING);
		}
		else if (gAgent.getFlying()) 
		{
			setAnimationState(STATE_AGENT_HOVER);
		}
		else if (gAgent.getAvatarObject()->mIsSitting) 
		{
			if (mAnimationState != STATE_AGENT_GROUNDSIT)
			{
				setAnimationState(STATE_AGENT_SIT);
			}
		}
	}
	return mAnimationState;
}

void LLFloaterAO::setAnimationState(const int state)
{
	mAnimationState = state;
}

LLUUID LLFloaterAO::getCurrentStandId()
{
	return mCurrentStandId;
}

void LLFloaterAO::setCurrentStandId(const LLUUID& id)
{
	mCurrentStandId = id;
}

void LLFloaterAO::AOItemDrop(LLViewerInventoryItem* item)
{
	llinfos << "LLFloaterAO::AOItemDrop("<< item->getUUID() <<"): LUA_CALL" << llendl;
	LUA_CALL("OnAONotecard") << item->getUUID() << LUA_END;

	llinfos << "LLFloaterAO::AOItemDrop("<< item->getUUID() <<"): gSavedPerAccountSettings" << llendl;
	gSavedPerAccountSettings.setString("PhoenixAOConfigNotecardID",item->getAssetUUID().asString());
	
	llinfos << "LLFloaterAO::AOItemDrop("<< item->getUUID() <<"): childSetValue" << llendl;
	sInstance->childSetValue("ao_nc_text","Currently set to: "+item->getName());
}

LLUUID LLFloaterAO::GetAnimID(const LLUUID& id)
{
	for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
	{
		if (iter->orig_id == id) return iter->ao_id;
	}
	return LLUUID::null;
}

LLUUID LLFloaterAO::GetAnimIDFromState(const int state)
{
	for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
	{
		if (iter->state == state) return iter->ao_id;
	}
	return LLUUID::null;
}

LLUUID LLFloaterAO::GetAnimIDByName(std::string name)
{
	for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
	{
		if (iter->anim_name == name)
		{
			return iter->ao_id;
		}
	}
	return LLUUID::null;
}

LLUUID LLFloaterAO::GetRandomAnimID(const LLUUID& id)
{
	return LLUUID::null;
}

LLUUID LLFloaterAO::GetRandomAnimIDFromState(const int state)
{
	std::vector<struct_all_anims> mAORandomizeAnims;
	struct_all_anims random_anims_loader;
	if (!mAOAllAnims.empty())
	{
		for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
		{
			if (iter->state == state)
			{
				random_anims_loader.ao_id = iter->ao_id; mAORandomizeAnims.push_back(random_anims_loader);
			}
		}
		return mAORandomizeAnims[ll_rand(mAORandomizeAnims.size()-1)].ao_id;
	}
	else
	{
		return LLUUID::null;
	}
}

int LLFloaterAO::modifyUnderwaterState(const int state)
{
	int modifiedstate;
	switch (state)
	{
		case STATE_AGENT_STANDUP:
		case STATE_AGENT_LAND:
		case STATE_AGENT_CROUCH:
		case STATE_AGENT_CROUCHWALK:
		case STATE_AGENT_PRE_JUMP:
		case STATE_AGENT_JUMP:
		case STATE_AGENT_TURNLEFT:
		case STATE_AGENT_TURNRIGHT:
		case STATE_AGENT_STAND:
		case STATE_AGENT_FLY:
		case STATE_AGENT_HOVER:
		case STATE_AGENT_FLOATING: 
			modifiedstate = STATE_AGENT_FLOATING; 
			break;

		case STATE_AGENT_FLYSLOW:
		case STATE_AGENT_WALK:
		case STATE_AGENT_RUN:
		case STATE_AGENT_SWIMMINGFORWARD: 
			modifiedstate = STATE_AGENT_SWIMMINGFORWARD; 
			break;

		case STATE_AGENT_HOVER_UP:
		case STATE_AGENT_SWIMMINGUP: 
			modifiedstate = STATE_AGENT_SWIMMINGUP; 
			break;

		case STATE_AGENT_FALLDOWN:
		case STATE_AGENT_HOVER_DOWN:
		case STATE_AGENT_SWIMMINGDOWN: 
			modifiedstate = STATE_AGENT_SWIMMINGDOWN; 
			break;

		case STATE_AGENT_SIT:
			modifiedstate = STATE_AGENT_SIT; 
			break;
		case STATE_AGENT_GROUNDSIT:
			modifiedstate = STATE_AGENT_GROUNDSIT; 
			break;

		case STATE_AGENT_TYPING:
			modifiedstate = STATE_AGENT_TYPING; 
			break;

		case STATE_AGENT_IDLE:
		default:
			modifiedstate = STATE_AGENT_IDLE; //wot
		break;
	}
	return modifiedstate;
}


LLUUID LLFloaterAO::GetOrigAnimIDFromState(const int state)
{
	for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
	{
		if (iter->state == state) return iter->orig_id;
	}
	return LLUUID::null;
}

int LLFloaterAO::GetStateFromOrigAnimID(const LLUUID& id)
{
	for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
	{
		if (iter->orig_id == id) return iter->state;
	}
	return STATE_AGENT_IDLE;
}

int LLFloaterAO::GetStateFromAOAnimID(const LLUUID& id)
{
	for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
	{
		if (iter->ao_id == id) return iter->state;
	}
	return STATE_AGENT_IDLE;
}

int LLFloaterAO::GetStateFromToken(std::string strtoken)
{
	for (std::vector<struct_tokens>::iterator iter = mAOTokens.begin(); iter != mAOTokens.end(); ++iter)
	{
		if (iter->token == strtoken) return iter->state;
	}
	return STATE_AGENT_IDLE;
}

std::string LLFloaterAO::GetAnimTypeFromState(const int state)
{
	for (std::vector<struct_anim_types>::iterator iter = mAnimTypes.begin(); iter != mAnimTypes.end(); ++iter)
	{
		if (iter->state == state) return iter->name;
	}
	return "unknown";
}

BOOL LLFloaterAO::isRandom(const int state)
{
	for (std::vector<struct_anim_types>::iterator iter = mAnimTypes.begin(); iter != mAnimTypes.end(); ++iter)
	{
		if (iter->state == state) 
			return gSavedPerAccountSettings.getBOOL("PhoenixAORandomize"+iter->name);
	}
	return FALSE;
}

// UI -------------------------------------------------------

LLUICtrl* LLFloaterAO::getComboBox(std::string name)
{
	for (std::vector<struct_combobox>::iterator iter = mComboBoxes.begin(); iter != mComboBoxes.end(); ++iter)
	{
		if ((iter->combobox) && (iter->name == name)) return iter->combobox;
	}
	return NULL;
}

void LLFloaterAO::onSpinnerCommit(LLUICtrl* ctrl, void* userdata)
{
	LLSpinCtrl* spin = (LLSpinCtrl*) ctrl;
	if(spin)
	{
		if (spin->getName() == "standtime")
		{
			gSavedPerAccountSettings.setF32("PhoenixAOStandInterval", spin->get());
			if (mAOStandTimer) mAOStandTimer->reset();
		}
	}
}

void LLFloaterAO::onCheckBoxCommit(LLUICtrl* ctrl, void* userdata)
{
	LLSpinCtrl* checkbox = (LLSpinCtrl*) ctrl;
	if(checkbox)
	{
		gSavedPerAccountSettings.setBOOL(checkbox->getName(), checkbox->getValue().asBoolean());
	}
}

void LLFloaterAO::onClickMore(void* data)
{
	gSavedSettings.setBOOL( "PhoenixAOAdvanced", TRUE );
	updateLayout(sInstance);
}
void LLFloaterAO::onClickLess(void* data)
{
	gSavedSettings.setBOOL( "PhoenixAOAdvanced", FALSE );
	updateLayout(sInstance);
}

void LLFloaterAO::onClickToggleAO(LLUICtrl *checkbox, void*)
{
	gSavedPerAccountSettings.setBOOL("PhoenixAOEnabled", checkbox->getValue().asBoolean());
	llinfos << "Save PhoenixAOEnabled: " << checkbox->getValue().asBoolean() << llendl;
	run();
}

void LLFloaterAO::onClickToggleSits(LLUICtrl *checkbox, void*)
{
	gSavedPerAccountSettings.setBOOL("PhoenixAOSitsEnabled", checkbox->getValue().asBoolean());
	run();
}

void LLFloaterAO::onClickNoMouselookStands(LLUICtrl *checkbox, void*)
{
	gSavedPerAccountSettings.setBOOL("PhoenixAONoStandsInMouselook", checkbox->getValue().asBoolean());
}

void LLFloaterAO::onComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{
	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		if (box->getName() == "stand")
		{
			stand_iterator = box->getCurrentIndex();
			if (mAOStandTimer) mAOStandTimer->reset();
			ChangeStand(TRUE);
		}
		else
		{
			std::string stranim = box->getValue().asString();

			for (std::vector<struct_combobox>::iterator iter = mComboBoxes.begin(); iter != mComboBoxes.end(); ++iter)
			{
				if (box->getName() == iter->name)
				{
					gSavedPerAccountSettings.setString("PhoenixAODefault"+iter->name,stranim);
					int boxstate = iter->state;

					for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
					{
						if (boxstate == iter->state)
						{
							iter->ao_id = GetAnimIDByName(stranim);
							if (getAnimationState() == boxstate) // only play anim for the current state, change sits for example
							{
								startAOMotion(iter->ao_id,TRUE,TRUE);
							}
							return;
						}
					}
				}
			}
		}
	}
}

void LLFloaterAO::onClickPrevStand(void* user_data)
{
	if (!(mAOStands.size() > 0)) return;
	stand_iterator=stand_iterator-1;
	if (stand_iterator < 0) stand_iterator = int( mAOStands.size()-stand_iterator);
	if (stand_iterator > int( mAOStands.size()-1)) stand_iterator = 0;
	if (mAOStandTimer) mAOStandTimer->reset();
	ChangeStand(TRUE);
}

void LLFloaterAO::onClickNextStand(void* user_data)
{
	if (!(mAOStands.size() > 0)) return;
	stand_iterator=stand_iterator+1;
	if (stand_iterator < 0) stand_iterator = int( mAOStands.size()-stand_iterator);
	if (stand_iterator > int( mAOStands.size()-1)) stand_iterator = 0;
	if (mAOStandTimer) mAOStandTimer->reset();
	ChangeStand(TRUE);
}

void LLFloaterAO::onClickReloadCard(void* user_data)
{
//	if(gInventory.isEverythingFetched())
//	{
		LLFloaterAO::init();
//	}
}

void LLFloaterAO::onClickOpenCard(void* user_data)
{
//	if(gInventory.isEverythingFetched())
//	{
		LLUUID configncitem = (LLUUID)gSavedPerAccountSettings.getString("PhoenixAOConfigNotecardID");
		if (configncitem.notNull())
		{
			const LLInventoryItem* item = gInventory.getItem(configncitem);
			if(item)
			{
				if (gAgent.allowOperation(PERM_COPY, item->getPermissions(),GP_OBJECT_MANIPULATE) || gAgent.isGodlike())
				{
					if(!item->getAssetUUID().isNull())
					open_notecard((LLViewerInventoryItem*)item, std::string("Note: ") + item->getName(), LLUUID::null, FALSE);
				}
			}
		}
//	}
}

void LLFloaterAO::loadComboBoxes()
{
	if (sInstance)
	{
		// empty
		for (std::vector<struct_combobox>::iterator iter = mComboBoxes.begin(); iter != mComboBoxes.end(); ++iter)
		{
			if (iter->combobox)
			{
				iter->combobox->clear();
				iter->combobox->removeall();
			}
		}

		mComboBoxes.clear();
		struct_combobox loader_combobox;
		for (std::vector<struct_anim_types>::iterator iter = mAnimTypes.begin(); iter != mAnimTypes.end(); ++iter)
		{
			loader_combobox.combobox = sInstance->getChild<LLComboBox>(iter->name);
			loader_combobox.name = iter->name;
			loader_combobox.state = iter->state;
			mComboBoxes.push_back(loader_combobox);
		}

		mcomboBox_stands = (LLComboBox*) getComboBox("stand");

		for (std::vector<struct_combobox>::iterator iter = mComboBoxes.begin(); iter != mComboBoxes.end(); ++iter)
		{
			sInstance->getChild<LLComboBox>(iter->name)->setCommitCallback(onComboBoxCommit);
			sInstance->childSetCommitCallback("PhoenixAORandomize"+iter->name,onCheckBoxCommit);
			sInstance->childSetValue("PhoenixAORandomize"+iter->name, gSavedPerAccountSettings.getBOOL("PhoenixAORandomize"+iter->name));
		}

		// fill
		for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
		{
			for (std::vector<struct_combobox>::iterator comboiter = mComboBoxes.begin(); comboiter != mComboBoxes.end(); ++comboiter)
			{
				if ((comboiter->combobox) && (comboiter->state == iter->state))
				{
					if (!(comboiter->combobox->selectByValue(iter->anim_name.c_str()))) //check if exists then add
					{
						comboiter->combobox->add(iter->anim_name.c_str(), ADD_BOTTOM, TRUE); 
					}
				}
			}

		}

		// reading default anim settings and updating combobox selections
		for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
		{
			if (iter->state == STATE_AGENT_STAND) continue; // there is no default stand o.o
			for (std::vector<struct_combobox>::iterator comboiter = mComboBoxes.begin(); comboiter != mComboBoxes.end(); ++comboiter)
			{
				if (iter->state == comboiter->state)
				{
					std::string defaultanim = gSavedPerAccountSettings.getString("PhoenixAODefault"+comboiter->name);
					if (GetAnimIDByName(defaultanim) != LLUUID::null) iter->ao_id = GetAnimIDByName(defaultanim);
					if (comboiter->combobox) SetDefault(comboiter->combobox,iter->ao_id,defaultanim);
				}
			}
		}
	}
}

void LLFloaterAO::onNotecardLoadComplete(LLVFS *vfs,const LLUUID& asset_uuid,LLAssetType::EType type,void* user_data, S32 status, LLExtStat ext_status)
{
	if(status == LL_ERR_NOERR)
	{
		S32 size = vfs->getSize(asset_uuid, type);
		U8* buffer = new U8[size];
		vfs->getData(asset_uuid, type, buffer, 0, size);

		if(type == LLAssetType::AT_NOTECARD)
		{
			LLViewerTextEditor* edit = new LLViewerTextEditor("",LLRect(0,0,0,0),S32_MAX,"");
			if(edit->importBuffer((char*)buffer, (S32)size))
			{
				llinfos << "ao nc decode success" << llendl;
				std::string card = edit->getText();
				edit->die();

				LUA_CALL("OnAONotecardRead") << card << LUA_END;

				mAOAllAnims.clear();
				mAOStands.clear();
				//mAODefaultAnims.clear();

				struct_stands standsloader;
				struct_all_anims all_anims_loader;

				typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
				boost::char_separator<char> sep("\n");
				tokenizer tokline(card, sep);

				for (tokenizer::iterator line = tokline.begin(); line != tokline.end(); ++line)
				{
					std::string strline(*line);

					boost::regex type("^(\\s*)(\\[ )(.*)( \\])");
					boost::smatch what; 
					if (boost::regex_search(strline, what, type)) 
					{
						boost::char_separator<char> sep("|,");
						std::string stranimnames(boost::regex_replace(strline, type, ""));
						tokenizer tokanimnames(stranimnames, sep);
						for (tokenizer::iterator anim = tokanimnames.begin(); anim != tokanimnames.end(); ++anim)
						{
							std::string strtoken(what[0]);
							std::string stranim(*anim);
							int state = GetStateFromToken(strtoken.c_str());
							LLUUID animid(getAssetIDByName(stranim));

							if (!(animid.notNull()))
							{
								cmdline_printchat(llformat("Warning: animation '%s' could not be found (Section: %s).",stranim.c_str(),strtoken.c_str()));
							}
							else
							{
								all_anims_loader.ao_id = animid; all_anims_loader.anim_name = stranim.c_str(); all_anims_loader.state = state; mAOAllAnims.push_back(all_anims_loader);

								if (state == STATE_AGENT_STAND)
								{
									standsloader.ao_id = animid; standsloader.anim_name = stranim.c_str(); mAOStands.push_back(standsloader);
									continue;
								}

								for (std::vector<struct_default_anims>::iterator iter = mAODefaultAnims.begin(); iter != mAODefaultAnims.end(); ++iter)
								{
									if (state == iter->state) iter->ao_id = animid;
								}
							}
						}
					} 
				}
				llinfos << "ao nc read sucess" << llendl;

				loadComboBoxes();

				run();
			}
			else
			{
				llinfos << "ao nc decode error" << llendl;
			}
		}
	}
	else
	{
		llinfos << "ao nc read error" << llendl;
	}
}

BOOL LLFloaterAO::SetDefault(void* userdata, LLUUID ao_id, std::string defaultanim)
{
	if (sInstance && (userdata))
	{
		LLComboBox *box = (LLComboBox *) userdata;
		if (LLUUID::null == ao_id)
		{
			box->clear();
			box->removeall();
		}
		else
		{
			box->selectByValue(defaultanim);
		}
	}
	return TRUE;
}

class ObjectNameMatches : public LLInventoryCollectFunctor
{
public:
	ObjectNameMatches(std::string name)
	{
		sName = name;
	}
	virtual ~ObjectNameMatches() {}
	virtual bool operator()(LLInventoryCategory* cat,
							LLInventoryItem* item)
	{
		if(item)
		{
			if (item->getParentUUID() == LLFloaterAO::invfolderid)
			{
				return (item->getName() == sName);
			}
			return false;
		}
		return false;
	}
private:
	std::string sName;
};

const LLUUID& LLFloaterAO::getAssetIDByName(const std::string& name)
{
	if (name.empty()) return LLUUID::null;

	LLViewerInventoryCategory::cat_array_t cats;
	LLViewerInventoryItem::item_array_t items;
	ObjectNameMatches objectnamematches(name);
	gInventory.collectDescendentsIf(LLUUID::null,cats,items,FALSE,objectnamematches);

	if (items.count())
	{
		return items[0]->getAssetUUID();
	}
	return LLUUID::null;
};
