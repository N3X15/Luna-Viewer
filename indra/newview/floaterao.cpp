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

#include "DiamondAoInt.h"
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
		LLFloaterAO::stand_iterator++;
		LLFloaterAO::ChangeStand(FALSE);
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
		LLViewerInventoryItem* inv_item = (LLViewerInventoryItem*)cargo_data;
		if(gInventory.getItem(inv_item->getUUID()))
		{
			*accept = ACCEPT_YES_COPY_SINGLE;
			if(drop)
			{
				mDownCallback(inv_item);
			}
		}
		else
		{
			*accept = ACCEPT_NO;
		}
	}
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
	if(LLStartUp::getStartupState() == STATE_STARTED)
	{
		LLUUID itemidimport = (LLUUID)gSavedPerAccountSettings.getString("PhoenixAOConfigNotecardID");
		LLViewerInventoryItem* itemimport = gInventory.getItem(itemidimport);
		if(itemimport)
		{
			childSetValue("ao_nc_text","Currently set to: "+itemimport->getName());
		}
		else if(itemidimport.isNull())
		{
			childSetValue("ao_nc_text","Currently not set");
		}
		else
		{
			childSetValue("ao_nc_text","Currently set to a item not on this account");
		}
	}
	else
	{
		childSetValue("ao_nc_text","Not logged in");
	}
	childSetAction("more_btn", onClickMore, this);
	childSetAction("less_btn", onClickLess, this);

	childSetAction("reloadcard",onClickReloadCard,this);
	childSetAction("opencard",onClickOpenCard,this);
	childSetAction("prevstand",onClickPrevStand,this);
	childSetAction("nextstand",onClickNextStand,this);
	
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
	BOOL success = FALSE;

	mAnimTypes.clear();
	struct_anim_types loader_anim_types;
	loader_anim_types.name = "stand";			loader_anim_types.state = STATE_AGENT_STAND;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "walk";				loader_anim_types.state = STATE_AGENT_WALK;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "run";				loader_anim_types.state = STATE_AGENT_RUN;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "jump";				loader_anim_types.state = STATE_AGENT_JUMP;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "sit";				loader_anim_types.state = STATE_AGENT_SIT;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "gsit";				loader_anim_types.state = STATE_AGENT_GROUNDSIT;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "crouch";			loader_anim_types.state = STATE_AGENT_CROUCH;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "cwalk";			loader_anim_types.state = STATE_AGENT_CROUCHWALK;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "fall";				loader_anim_types.state = STATE_AGENT_FALLDOWN;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "hover";			loader_anim_types.state = STATE_AGENT_HOVER;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "fly";				loader_anim_types.state = STATE_AGENT_FLY;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "flyslow";			loader_anim_types.state = STATE_AGENT_FLYSLOW;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "flyup";			loader_anim_types.state = STATE_AGENT_HOVER_UP;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "flydown";			loader_anim_types.state = STATE_AGENT_HOVER_DOWN;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "land";				loader_anim_types.state = STATE_AGENT_LAND;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "standup";			loader_anim_types.state = STATE_AGENT_STANDUP;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "prejump";			loader_anim_types.state = STATE_AGENT_PRE_JUMP;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "turnleft";			loader_anim_types.state = STATE_AGENT_TURNLEFT;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "turnright";		loader_anim_types.state = STATE_AGENT_TURNRIGHT;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "typing";			loader_anim_types.state = STATE_AGENT_TYPING;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "floating";			loader_anim_types.state = STATE_AGENT_FLOATING;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "swimmingforward";	loader_anim_types.state = STATE_AGENT_SWIMMINGFORWARD;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "swimmingup";		loader_anim_types.state = STATE_AGENT_SWIMMINGUP;			mAnimTypes.push_back(loader_anim_types);
	loader_anim_types.name = "swimmingdown";		loader_anim_types.state = STATE_AGENT_SWIMMINGDOWN;			mAnimTypes.push_back(loader_anim_types);

    if (!sInstance)
	sInstance = new LLFloaterAO();

	LLUUID configncitem = (LLUUID)gSavedPerAccountSettings.getString("PhoenixAOConfigNotecardID");
	if(LLStartUp::getStartupState() >= STATE_INVENTORY_SEND)
	{
		LLUUID phoenix_category = JCLSLBridge::findCategoryByNameOrCreate(phoenix_category_name);
		if(gInventory.isCategoryComplete(phoenix_category))
		{
			if (configncitem.notNull())
			{
				success = FALSE;
				const LLInventoryItem* item = gInventory.getItem(configncitem);
				if(item)
				{
					BOOL in_emcat = FALSE;
					LLUUID parent_id = item->getParentUUID();
					LLViewerInventoryCategory* parent = gInventory.getCategory(item->getParentUUID());
					if(parent_id != phoenix_category)
					{
						while(parent)
						{
							parent_id = parent->getParentUUID();
							if(parent_id == phoenix_category)
							{
								in_emcat = TRUE;
								break;
							}else if(parent_id.isNull())break;
							parent = gInventory.getCategory(parent_id);
						}
					}else in_emcat = TRUE;
					
					if(in_emcat == FALSE)
					{
						cmdline_printchat("Your AO notecard and animations must be in the #Phoenix folder in order to function correctly.");
						/*LLViewerInventoryCategory* parent = gInventory.getCategory(item->getParentUUID());
						if(parent->getPreferredType() == LLAssetType::AT_NONE)
						{
							LLNotifications::instance().add("NotifyAOMove", LLSD(),LLSD());
							move_inventory_item(gAgent.getID(),gAgent.getSessionID(),parent->getUUID(),phoenix_category,parent->getName(), NULL);
						}else
						{
							cmdline_printchat("Unable to move AO, as its containing folder is a category.");
						}
						*/
					}

					if (gAgent.allowOperation(PERM_COPY, item->getPermissions(),GP_OBJECT_MANIPULATE) || gAgent.isGodlike())
					{
						if(!item->getAssetUUID().isNull())
						{
							///////////////////////////
							mAOStands.clear();
							mAOTokens.clear();
							mAODefaultAnims.clear();

							struct_tokens tokenloader;
							tokenloader.token = 
							tokenloader.token = "[ Sitting On Ground ]";	tokenloader.state = STATE_AGENT_GROUNDSIT; mAOTokens.push_back(tokenloader);    // 0
							tokenloader.token = "[ Sitting ]";				tokenloader.state = STATE_AGENT_SIT; mAOTokens.push_back(tokenloader);              // 1
							tokenloader.token = "[ Crouching ]";			tokenloader.state = STATE_AGENT_CROUCH; mAOTokens.push_back(tokenloader);            // 3
							tokenloader.token = "[ Crouch Walking ]";		tokenloader.state = STATE_AGENT_CROUCHWALK; mAOTokens.push_back(tokenloader);       // 4
							tokenloader.token = "[ Standing Up ]";			tokenloader.state = STATE_AGENT_STANDUP; mAOTokens.push_back(tokenloader);          // 6
							tokenloader.token = "[ Falling ]";				tokenloader.state = STATE_AGENT_FALLDOWN; mAOTokens.push_back(tokenloader);              // 7
							tokenloader.token = "[ Flying Down ]";			tokenloader.state = STATE_AGENT_HOVER_DOWN; mAOTokens.push_back(tokenloader);          // 8
							tokenloader.token = "[ Flying Up ]";			tokenloader.state = STATE_AGENT_HOVER_UP; mAOTokens.push_back(tokenloader);            // 9
							tokenloader.token = "[ Flying Slow ]";			tokenloader.state = STATE_AGENT_FLYSLOW; mAOTokens.push_back(tokenloader);          // 10
							tokenloader.token = "[ Flying ]";				tokenloader.state = STATE_AGENT_FLY; mAOTokens.push_back(tokenloader);               // 11
							tokenloader.token = "[ Hovering ]";				tokenloader.state = STATE_AGENT_HOVER; mAOTokens.push_back(tokenloader);             // 12
							tokenloader.token = "[ Jumping ]";				tokenloader.state = STATE_AGENT_JUMP; mAOTokens.push_back(tokenloader);              // 13
							tokenloader.token = "[ Pre Jumping ]";			tokenloader.state = STATE_AGENT_PRE_JUMP; mAOTokens.push_back(tokenloader);          // 14
							tokenloader.token = "[ Running ]";				tokenloader.state = STATE_AGENT_RUN; mAOTokens.push_back(tokenloader);              // 15
							tokenloader.token = "[ Turning Right ]";		tokenloader.state = STATE_AGENT_TURNRIGHT; mAOTokens.push_back(tokenloader);        // 16
							tokenloader.token = "[ Turning Left ]";			tokenloader.state = STATE_AGENT_TURNLEFT; mAOTokens.push_back(tokenloader);         // 17
							tokenloader.token = "[ Walking ]";				tokenloader.state = STATE_AGENT_WALK; mAOTokens.push_back(tokenloader);              // 18
							tokenloader.token = "[ Landing ]";				tokenloader.state = STATE_AGENT_LAND; mAOTokens.push_back(tokenloader);              // 19
							tokenloader.token = "[ Standing ]";				tokenloader.state = STATE_AGENT_STAND; mAOTokens.push_back(tokenloader);             // 20
							tokenloader.token = "[ Swimming Down ]";		tokenloader.state = STATE_AGENT_SWIMMINGDOWN; mAOTokens.push_back(tokenloader);        // 21
							tokenloader.token = "[ Swimming Up ]";			tokenloader.state = STATE_AGENT_SWIMMINGUP; mAOTokens.push_back(tokenloader);          // 22
							tokenloader.token = "[ Swimming Forward ]";		tokenloader.state = STATE_AGENT_SWIMMINGFORWARD; mAOTokens.push_back(tokenloader);     // 23
							tokenloader.token = "[ Floating ]";				tokenloader.state = STATE_AGENT_FLOATING; mAOTokens.push_back(tokenloader);             // 24
							tokenloader.token = "[ Typing ]";				tokenloader.state = STATE_AGENT_TYPING; mAOTokens.push_back(tokenloader); //25
 
							struct_default_anims default_anims_loader;
							default_anims_loader.orig_id = ANIM_AGENT_WALK;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_WALK;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_RUN;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_RUN;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_PRE_JUMP;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_PRE_JUMP;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_JUMP;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_JUMP;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_TURNLEFT;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_TURNLEFT;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_TURNRIGHT;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_TURNRIGHT;		mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = ANIM_AGENT_SIT;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_SIT;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_SIT_FEMALE;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_SIT;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_SIT_GENERIC;			default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_SIT;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_SIT_GROUND;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_GROUNDSIT;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_SIT_GROUND_CONSTRAINED;	default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_GROUNDSIT;		mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = ANIM_AGENT_HOVER;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_HOVER;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_HOVER_DOWN;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_HOVER_DOWN;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_HOVER_UP;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_HOVER_UP;		mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = ANIM_AGENT_CROUCH;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_CROUCH;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_CROUCHWALK;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_CROUCHWALK;		mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = ANIM_AGENT_FALLDOWN;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_FALLDOWN;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_STANDUP;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_STANDUP;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_LAND;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_LAND;			mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = ANIM_AGENT_FLY;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_FLY;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = ANIM_AGENT_FLYSLOW;				default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_FLYSLOW;		mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = ANIM_AGENT_TYPE;					default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_TYPING;			mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = LLUUID("159258dc-f57c-4662-8afd-c55b81d13849");	default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_FLOATING;		mAODefaultAnims.push_back(default_anims_loader);

							default_anims_loader.orig_id = LLUUID("159258dc-f57c-4662-8afd-c55b81d13849");	default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_SWIMMINGFORWARD;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = LLUUID("159258dc-f57c-4662-8afd-c55b81d13849");	default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_SWIMMINGUP;		mAODefaultAnims.push_back(default_anims_loader);
							default_anims_loader.orig_id = LLUUID("159258dc-f57c-4662-8afd-c55b81d13849");	default_anims_loader.ao_id = LLUUID::null; default_anims_loader.state = STATE_AGENT_SWIMMINGDOWN;	mAODefaultAnims.push_back(default_anims_loader);
							///////////////////////////
							LLUUID* new_uuid = new LLUUID(configncitem);
							LLHost source_sim = LLHost::invalid;
							invfolderid = item->getParentUUID();
							gAssetStorage->getInvItemAsset(source_sim,
															gAgent.getID(),
															gAgent.getSessionID(),
															item->getPermissions().getOwner(),
															LLUUID::null,
															item->getUUID(),
															item->getAssetUUID(),
															item->getType(),
															&onNotecardLoadComplete,
															(void*)new_uuid,
															TRUE);
							success = TRUE;
						}
					}
				}else
				{
					//static BOOL startedfetch = FALSE;
					if(fullfetch == FALSE)
					{
						fullfetch = TRUE;
						//no choice, can't move the AO till we find it, should only have to happen once
						gInventory.startBackgroundFetch();
						return FALSE;
					}
				}
			}
		}
	}

	if (!success)
	{
		if(configncitem.notNull())
		{
			cmdline_printchat("Could not read the specified Config Notecard");
			cmdline_printchat("If your specified notecard is not in the #phoenix folder, please move it there.");
		}
		gSavedPerAccountSettings.setBOOL("PhoenixAOEnabled",FALSE);
	}
	return success;
}

// AO LOGIC -------------------------------------------------------

void LLFloaterAO::run()
{
	setAnimationState(STATE_AGENT_IDLE); // reset state
	int state = getAnimationState(); // check if sitting or hovering
	if (gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled"))
	{
		DiamondAoInt::AOStatusUpdate(true);
		//if we are sitting but sits are disabled, dont play any anims but start the stand timer in the background
		//otherwise just start overriding
		if (!((state == STATE_AGENT_SIT) && !gSavedPerAccountSettings.getBOOL("PhoenixAOSitsEnabled")))
		startAOMotion(GetAnimIDFromState(state),TRUE, FALSE);

		if (mAOStandTimer)
		{
			mAOStandTimer->reset();
			ChangeStand(FALSE);
		}
		else
		{
			mAOStandTimer =	new AOStandTimer();
		}
	}
	else
	{
		DiamondAoInt::AOStatusUpdate(false);
		//ao is off so stop everything
		for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
		{
			if (iter->isPlaying)
			{
				stopAOMotion(iter->ao_id,TRUE);
			}
		}

	}
}

void LLFloaterAO::startAOMotion(const LLUUID& id, const BOOL stand, const BOOL announce)
{
	int startmotionstate = STATE_AGENT_IDLE;
	LLUUID override_id;
	BOOL sitting = FALSE;
	BOOL underwater = FALSE;

	if (gAgent.getAvatarObject())
	{
		sitting = gAgent.getAvatarObject()->mIsSitting;
		underwater = gAgent.getAvatarObject()->mBelowWater;
	}

	if (!stand) // we are moving, it's a default linden anim uuid, try to override it
	{
		startmotionstate = GetStateFromOrigAnimID(id);

		// if under water, modify states to underwater behaviour
		if (underwater) startmotionstate = modifyUnderwaterState(startmotionstate);

		// is this anim category marked as random?
		if (isRandom(startmotionstate) && !announce)
		{
			override_id = GetRandomAnimIDFromState(startmotionstate);
		}
		else
		{
			if (underwater) override_id = GetAnimIDFromState(startmotionstate);
			else override_id = GetAnimID(id);
		}
	}
	else // it's an override anim uuid, probably a stand
	{
		startmotionstate = GetStateFromAOAnimID(id);

		// if under water, modify states to underwater behaviour
		if (underwater) startmotionstate = modifyUnderwaterState(startmotionstate);

		// is this anim category marked as random?
		if (isRandom(startmotionstate) && !announce)
		{
			override_id = GetRandomAnimIDFromState(startmotionstate);
		}
		else
		{
			if (underwater) override_id = GetAnimIDFromState(startmotionstate);
			else override_id = id;
		}
	}

	if ((sitting) && (startmotionstate != STATE_AGENT_SIT) && (startmotionstate != STATE_AGENT_GROUNDSIT)) return;
//	if ((!sitting) && ((startmotionstate == STATE_AGENT_SIT) || (startmotionstate == STATE_AGENT_GROUNDSIT))) return;
	if (startmotionstate == STATE_AGENT_IDLE) return;
	if ((startmotionstate == STATE_AGENT_SIT) && !(gSavedPerAccountSettings.getBOOL("PhoenixAOSitsEnabled"))) return;
	if (override_id.isNull() || !gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled")) return;

	for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
	{
		if ((iter->isPlaying) && (iter->ao_id != override_id)) //(iter->state != startmotionstate) && 
		{
			iter->isPlaying = FALSE;
			//cmdline_printchat(llformat("startmotionstate animtypefromstate %s stopping %s",GetAnimTypeFromState(startmotionstate).c_str(),iter->ao_id.asString().c_str()));
			gAgent.sendAnimationRequest(iter->ao_id, ANIM_REQUEST_STOP);
		}
		else if ((!iter->isPlaying) && (iter->ao_id == override_id))
		{
			iter->isPlaying = TRUE;
			//cmdline_printchat(llformat("startmotionstate: %d stand %d / isRandom %d / sitting %d // animtypefromstate %s // id %s",startmotionstate,stand,isRandom(startmotionstate),sitting,GetAnimTypeFromState(startmotionstate).c_str(),override_id.asString().c_str()));
			if (announce) cmdline_printchat(llformat("Changing animation to %s.",iter->anim_name.c_str()));
			if ((startmotionstate == STATE_AGENT_STAND)&&(sInstance)&&(mcomboBox_stands)) mcomboBox_stands->selectByValue(iter->anim_name.c_str());

			gAgent.sendAnimationRequest(override_id, ANIM_REQUEST_START);
			setAnimationState(startmotionstate);
		}
	}
	// cleanup timer checking for duplicate or stuck anims due to packet loss/lag
	if(mAODelayTimer.getElapsedTimeF32() >= F32(1.f))
	{
	    std::list<LLAnimHistoryItem*> history = LLFloaterExploreAnimations::animHistory[gAgent.getID()];
	    for (std::list<LLAnimHistoryItem*>::iterator iter = history.begin(); iter != history.end(); ++iter)
	    {
		    LLAnimHistoryItem* item = (*iter);
		    int state = GetStateFromAOAnimID(item->mAssetID);
		    if (!state) continue;
		    if  ( (item->mPlaying) && (state != getAnimationState()) )
		    {
				//cmdline_printchat(llformat("stopping duplicate anim: %d // %s // %s",getAnimationState(),GetAnimTypeFromState(getAnimationState()).c_str(),item->mAssetID.asString().c_str()));
			    gAgent.sendAnimationRequest(item->mAssetID, ANIM_REQUEST_STOP);
		    }
	    }
	    mAODelayTimer.reset();
	}
}

void LLFloaterAO::stopAOMotion(const LLUUID& id, const BOOL stand)
{
	int stopmotionstate = STATE_AGENT_IDLE;
	LLUUID override_id;

	if (!stand)
	{
		override_id = GetAnimID(id);
		stopmotionstate = GetStateFromOrigAnimID(id);
	}
	else
	{
		override_id = id;
		stopmotionstate = GetStateFromAOAnimID(id);
	}

	if (stopmotionstate == STATE_AGENT_IDLE) return;
	if (override_id.isNull()) return; //|| !gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled") //always stop, enabled or not

	BOOL stopped = FALSE;
	for (std::vector<struct_all_anims>::iterator iter = mAOAllAnims.begin(); iter != mAOAllAnims.end(); ++iter)
	{
		if ((iter->isPlaying) && (iter->state == stopmotionstate)) // let's see if this works properly with both random and normal anims
		// if ((iter->isPlaying) && (iter->ao_id == override_id))  // used to be this 
		{
			iter->isPlaying = FALSE;
			//cmdline_printchat(llformat("stopmotionstate: %d stand %d / isRandom %d / sitting %d // animtypefromstate %s // id %s",stopmotionstate,stand,isRandom(stopmotionstate),0,GetAnimTypeFromState(stopmotionstate).c_str(),iter->ao_id.asString().c_str()));
			gAgent.sendAnimationRequest(iter->ao_id, ANIM_REQUEST_STOP);
			setAnimationState(STATE_AGENT_IDLE);
			stopped = TRUE;
		}
	}
	if (stopped)
	{
		if(!mAODelayTimer.getStarted())
		{
			mAODelayTimer.start();
		}
		else
		{
			mAODelayTimer.reset();
		}
		ChangeStand(FALSE);
	}
}

void LLFloaterAO::ChangeStand(const BOOL announce)
{
	if (gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled"))
	{
		if (gAgent.getAvatarObject())
		{
			if (gSavedPerAccountSettings.getBOOL("PhoenixAONoStandsInMouselook") && gAgent.cameraMouselook()) return;
			if (gAgent.getAvatarObject()->mIsSitting) return;
		}
		if ((getAnimationState() == STATE_AGENT_IDLE) || (getAnimationState() == STATE_AGENT_STAND))// stands have lowest priority
		{
			if (!(mAOStands.size() > 0)) return;
			if (gSavedPerAccountSettings.getBOOL("PhoenixAORandomizestand"))
			{
				stand_iterator = ll_rand(mAOStands.size()-1);
			}
			if (stand_iterator < 0) stand_iterator = int( mAOStands.size()-stand_iterator);
			if (stand_iterator > int( mAOStands.size()-1)) stand_iterator = 0;

			int stand_iterator_previous = stand_iterator -1;

			if (stand_iterator_previous < 0) stand_iterator_previous = int( mAOStands.size()-1);
			
			if (mAOStands[stand_iterator].ao_id.notNull())
			{
				startAOMotion(mAOStands[stand_iterator].ao_id, TRUE, announce);

				setAnimationState(STATE_AGENT_STAND);
				setCurrentStandId(mAOStands[stand_iterator].ao_id);
			}
		}
	} 
	else
	{
		stopAOMotion(getCurrentStandId(), TRUE); //stop if ao is off
	}
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
	gSavedPerAccountSettings.setString("PhoenixAOConfigNotecardID", item->getUUID().asString());
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
