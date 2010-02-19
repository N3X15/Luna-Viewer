
/** 
 * @file LLPanelEmerald.cpp
 * @brief General preferences panel in preferences floater
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llpanelemerald.h"
#include "lggbeammapfloater.h"
// linden library includes
#include "llradiogroup.h"
#include "llbutton.h"
#include "lluictrlfactory.h"

#include "llcombobox.h"
#include "llslider.h"
#include "lltexturectrl.h"

#include "lggbeammaps.h"

// project includes
#include "llviewercontrol.h"
#include "llviewerwindow.h"
#include "llsdserialize.h"

#include "lltabcontainer.h"

#include "llinventorymodel.h"
#include "llfilepicker.h"
#include "llstartup.h"

#include "lltexteditor.h"

#include "llagent.h"

#include "lldirpicker.h"

#include "llweb.h" // [$PLOTR$/]
#include "lggbeamcolormapfloater.h"
#include "llsliderctrl.h"
#include "mfdkeywordfloater.h"
#include "lgghunspell_wrapper.h"

////////begin drop utility/////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class JCInvDropTarget
//
// This handy class is a simple way to drop something on another
// view. It handles drop events, always setting itself to the size of
// its parent.
//
// altered to support a callback so i can slap it in things and it just return the item to a func of my choice
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class JCInvDropTarget : public LLView
{
public:
	JCInvDropTarget(const std::string& name, const LLRect& rect, void (*callback)(LLViewerInventoryItem*));
	~JCInvDropTarget();

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


JCInvDropTarget::JCInvDropTarget(const std::string& name, const LLRect& rect,
						  void (*callback)(LLViewerInventoryItem*)) :
	LLView(name, rect, NOT_MOUSE_OPAQUE, FOLLOWS_ALL),
	mDownCallback(callback)
{
}

JCInvDropTarget::~JCInvDropTarget()
{
}

void JCInvDropTarget::doDrop(EDragAndDropType cargo_type, void* cargo_data)
{
	llinfos << "JCInvDropTarget::doDrop()" << llendl;
}

BOOL JCInvDropTarget::handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
									 EDragAndDropType cargo_type,
									 void* cargo_data,
									 EAcceptance* accept,
									 std::string& tooltip_msg)
{
	BOOL handled = FALSE;
	if(getParent())
	{
		handled = TRUE;
		// check the type
		//switch(cargo_type)
		//{
		//case DAD_ANIMATION:
		//{
			LLViewerInventoryItem* inv_item = (LLViewerInventoryItem*)cargo_data;
			if(gInventory.getItem(inv_item->getUUID()))
			{
				*accept = ACCEPT_YES_COPY_SINGLE;
				if(drop)
				{
					//printchat("accepted");
					mDownCallback(inv_item);
				}
			}
			else
			{
				*accept = ACCEPT_NO;
			}
		//	break;
		//}
		//default:
		//	*accept = ACCEPT_NO;
		//	break;
		//}
	}
	return handled;
}
////////end drop utility///////////////

LLPanelEmerald* LLPanelEmerald::sInstance;

JCInvDropTarget * LLPanelEmerald::mObjectDropTarget;
JCInvDropTarget * LLPanelEmerald::mBuildObjectDropTarget;

LLPanelEmerald::LLPanelEmerald()
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_emerald.xml");
	if(sInstance)delete sInstance;
	sInstance = this;
}

LLPanelEmerald::~LLPanelEmerald()
{
	sInstance = NULL;
	delete mObjectDropTarget;
	delete mBuildObjectDropTarget;
	mObjectDropTarget = NULL;
	mBuildObjectDropTarget = NULL;
}

void LLPanelEmerald::initHelpBtn(const std::string& name, const std::string& xml_alert)
{
	childSetAction(name, onClickHelp, new std::string(xml_alert));
}

void LLPanelEmerald::onClickHelp(void* data)
{
	std::string* xml_alert = (std::string*)data;
	LLNotifications::instance().add(*xml_alert);
}

BOOL LLPanelEmerald::postBuild()
{
	refresh();
	getChild<LLComboBox>("material")->setSimple(gSavedSettings.getString("EmeraldBuildPrefs_Material"));
	getChild<LLComboBox>("combobox shininess")->setSimple(gSavedSettings.getString("EmeraldBuildPrefs_Shiny"));
	

	LLSliderCtrl* mShapeScaleSlider = getChild<LLSliderCtrl>("EmeraldBeamShapeScale",TRUE,FALSE);
	mShapeScaleSlider->setCommitCallback(&LLPanelEmerald::beamUpdateCall);
	mShapeScaleSlider->setCallbackUserData(this);

	LLSliderCtrl* mBeamsPerSecondSlider = getChild<LLSliderCtrl>("EmeraldMaxBeamsPerSecond",TRUE,FALSE);
	mBeamsPerSecondSlider->setCommitCallback(&LLPanelEmerald::beamUpdateCall);
	mBeamsPerSecondSlider->setCallbackUserData(this);

	getChild<LLComboBox>("material")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("combobox shininess")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("EmeraldBeamShape_combo")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("BeamColor_combo")->setCommitCallback(onComboBoxCommit);
	getChild<LLTextureCtrl>("texture control")->setDefaultImageAssetID(LLUUID("89556747-24cb-43ed-920b-47caed15465f"));
	getChild<LLTextureCtrl>("texture control")->setCommitCallback(onTexturePickerCommit);

	getChild<LLComboBox>("EmeraldSpellBase")->setCommitCallback(onSpellBaseComboBoxCommit);

		
	//childSetCommitCallback("material",onComboBoxCommit);
	//childSetCommitCallback("combobox shininess",onComboBoxCommit);
	getChild<LLButton>("EmeraldPrefs_Stealth")->setClickedCallback(onStealth, this);
	getChild<LLButton>("EmeraldPrefs_FullFeatures")->setClickedCallback(onNoStealth, this);


	getChild<LLButton>("keyword_allert")->setClickedCallback(onKeywordAllertButton,this);
	
	getChild<LLButton>("EmSpell_EditCustom")->setClickedCallback(onSpellEditCustom, this);
	getChild<LLButton>("EmSpell_GetMore")->setClickedCallback(onSpellGetMore, this);
	getChild<LLButton>("EmSpell_Add")->setClickedCallback(onSpellAdd, this);
	getChild<LLButton>("EmSpell_Remove")->setClickedCallback(onSpellRemove, this);
	

	getChild<LLButton>("BeamColor_new")->setClickedCallback(onCustomBeamColor, this);
	getChild<LLButton>("BeamColor_refresh")->setClickedCallback(onRefresh,this);
	getChild<LLButton>("BeamColor_delete")->setClickedCallback(onBeamColorDelete,this);
			
	getChild<LLButton>("custom_beam_btn")->setClickedCallback(onCustomBeam, this);
	getChild<LLButton>("refresh_beams")->setClickedCallback(onRefresh,this);
	getChild<LLButton>("delete_beam")->setClickedCallback(onBeamDelete,this);

	getChild<LLButton>("revert_production_voice_btn")->setClickedCallback(onClickVoiceRevertProd, this);
	getChild<LLButton>("revert_debug_voice_btn")->setClickedCallback(onClickVoiceRevertDebug, this);

	getChild<LLButton>("EmeraldBreastReset")->setClickedCallback(onClickBoobReset, this);
	

	childSetCommitCallback("production_voice_field", onCommitApplyControl);//onCommitVoiceProductionServerName);
	childSetCommitCallback("debug_voice_field", onCommitApplyControl);//onCommitVoiceDebugServerName);

	childSetCommitCallback("EmeraldCmdLinePos", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineGround", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineHeight", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineTeleportHome", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineRezPlatform", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineMapTo", onCommitApplyControl);	
	childSetCommitCallback("EmeraldCmdLineCalc", onCommitApplyControl);

	childSetCommitCallback("EmeraldCmdLineDrawDistance", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdTeleportToCam", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineKeyToName", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineOfferTp", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineTP2", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineAO", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineClearChat", onCommitApplyControl);

	childSetCommitCallback("X Modifier", onCommitSendAppearance);
	childSetCommitCallback("Y Modifier", onCommitSendAppearance);
	childSetCommitCallback("Z Modifier", onCommitSendAppearance);
	childSetValue("EmeraldDoubleClickTeleportMode", gSavedSettings.getBOOL("EmeraldDoubleClickTeleportMode"));
	childSetValue("EmeraldUseOTR", LLSD((S32)gSavedSettings.getU32("EmeraldUseOTR"))); // [$PLOTR$]
	getChild<LLButton>("otr_help_btn")->setClickedCallback(onClickOtrHelp, this);      // [/$PLOTR$]

	initHelpBtn("EmeraldHelp_TeleportLogin",	"EmeraldHelp_TeleportLogin");
	initHelpBtn("EmeraldHelp_Voice",			"EmeraldHelp_Voice");
	initHelpBtn("EmeraldHelp_Shields",			"EmeraldHelp_Shields");
	initHelpBtn("EmeraldHelp_IM",				"EmeraldHelp_IM");
	initHelpBtn("EmeraldHelp_Chat",				"EmeraldHelp_Chat");
	initHelpBtn("EmeraldHelp_Misc",				"EmeraldHelp_Misc");
	initHelpBtn("EmeraldHelp_CmdLine",			"EmeraldHelp_CmdLine");
	initHelpBtn("EmeraldHelp_Avatar",			"EmeraldHelp_Avatar");
	initHelpBtn("EmeraldHelp_Build",			"EmeraldHelp_Build");
	initHelpBtn("EmeraldHelp_IRC",				"EmeraldHelp_IRC");
	initHelpBtn("EmeraldHelp_UtilityStream",	"EmeraldHelp_UtilityStream");
	initHelpBtn("EmeraldHelp_Inventory",		"EmeraldHelp_Inventory");
	initHelpBtn("EmeraldHelp_Effects",			"EmeraldHelp_Effects");
	initHelpBtn("EmeraldHelp_SpellCheck",		"EmeraldHelp_SpellCheck");

	LLView *target_view = getChild<LLView>("im_give_drop_target_rect");
	if(target_view)
	{
		if (mObjectDropTarget)//shouldn't happen
		{
			delete mObjectDropTarget;
		}
		mObjectDropTarget = new JCInvDropTarget("drop target", target_view->getRect(), IMAutoResponseItemDrop);//, mAvatarID);
		addChild(mObjectDropTarget);
	}
	target_view = getChild<LLView>("build_item_add_disp_rect");
	if(target_view)
	{
		if (mBuildObjectDropTarget) delete mBuildObjectDropTarget;
		mBuildObjectDropTarget = new JCInvDropTarget("build drop target", target_view->getRect(),BuildAutoResponseItemDrop);
		addChild(mBuildObjectDropTarget);
	}

	if(LLStartUp::getStartupState() == STATE_STARTED)
	{
		LLUUID itemid = (LLUUID)gSavedPerAccountSettings.getString("EmeraldInstantMessageResponseItemData");
		LLViewerInventoryItem* item = gInventory.getItem(itemid);
		if(item)
		{
			childSetValue("im_give_disp_rect_txt","Currently set to: "+item->getName());
		}else if(itemid.isNull())
		{
			childSetValue("im_give_disp_rect_txt","Currently not set");
		}else
		{
			childSetValue("im_give_disp_rect_txt","Currently set to a item not on this account");
		}
		itemid = (LLUUID)gSavedSettings.getString("EmeraldBuildPrefs_Item");
		item = gInventory.getItem(itemid);
		if(item)
		{
			childSetValue("build_item_add_disp_rect_txt","Currently set to: "+item->getName());
		}else if(itemid.isNull())
		{
			childSetValue("build_item_add_disp_rect_txt","Currently not set");
		}else
		{
			childSetValue("build_item_add_disp_rect_txt","Currently set to a item not on this account");
		}
	}else
	{
		childSetValue("im_give_disp_rect_txt","Not logged in");
		childSetValue("build_item_add_disp_rect_txt","Not logged in");
	}

	LLWString auto_response = utf8str_to_wstring( gSavedPerAccountSettings.getString("EmeraldInstantMessageResponse") );
	LLWStringUtil::replaceChar(auto_response, '^', '\n');
	LLWStringUtil::replaceChar(auto_response, '%', ' ');
	childSetText("im_response", wstring_to_utf8str(auto_response));
	childSetValue("EmeraldInstantMessageResponseFriends", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageResponseFriends"));
	childSetValue("EmeraldInstantMessageResponseMuted", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageResponseMuted"));
	childSetValue("EmeraldInstantMessageResponseAnyone", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageResponseAnyone"));
	childSetValue("EmeraldInstantMessageShowResponded", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageShowResponded"));
	childSetValue("EmeraldInstantMessageShowOnTyping", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageShowOnTyping"));
	childSetValue("EmeraldInstantMessageResponseRepeat", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageResponseRepeat" ));
	childSetValue("EmeraldInstantMessageResponseItem", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageResponseItem"));
	childSetValue("EmeraldInstantMessageAnnounceIncoming", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageAnnounceIncoming"));
	childSetValue("EmeraldInstantMessageAnnounceStealFocus", gSavedPerAccountSettings.getBOOL("EmeraldInstantMessageAnnounceStealFocus"));
	childSetValue("EmeraldShadowsON", gSavedSettings.getBOOL("EmeraldShadowsToggle"));

	childSetAction("set_mirror", onClickSetMirror, this);
	childSetCommitCallback("mirror_location", onCommitApplyControl);

	childSetAction("set_includeHDD", onClickSetHDDInclude, this);
	childSetCommitCallback("include_location", onCommitApplyControl);

	getChild<LLCheckBoxCtrl>("telerequest_toggle")->setCommitCallback(onConditionalPreferencesChanged);
	getChild<LLCheckBoxCtrl>("mldct_toggle")->setCommitCallback(onConditionalPreferencesChanged);

	refresh();
	return TRUE;
}

void LLPanelEmerald::refresh()
{
	
	LLComboBox* comboBox = getChild<LLComboBox>("EmeraldBeamShape_combo");

	if(comboBox != NULL) 
	{
		comboBox->removeall();
		comboBox->add("===OFF===");
		std::vector<std::string> names = gLggBeamMaps.getFileNames();
		for(int i=0; i<(int)names.size(); i++) 
		{
			comboBox->add(names[i]);
		}
		comboBox->setSimple(gSavedSettings.getString("EmeraldBeamShape"));
	}

	comboBox = getChild<LLComboBox>("BeamColor_combo");
	if(comboBox != NULL) 
	{
		comboBox->removeall();
		comboBox->add("===OFF===");
		std::vector<std::string> names = gLggBeamMaps.getColorsFileNames();
		for(int i=0; i<(int)names.size(); i++) 
		{
			comboBox->add(names[i]);
		}
		comboBox->setSimple(gSavedSettings.getString("EmeraldBeamColorFile"));
	}
	comboBox = getChild<LLComboBox>("EmeraldSpellBase");
	if(comboBox != NULL) 
	{
		comboBox->removeall();
		std::vector<std::string> names = glggHunSpell->getDicts();
		for(int i=0; i<(int)names.size(); i++) 
		{
			comboBox->add(names[i]);
		}
		comboBox->setSimple(gSavedSettings.getString("EmeraldSpellBase"));
	}
	comboBox = getChild<LLComboBox>("EmSpell_Avail");
	if(comboBox != NULL) 
	{
		comboBox->removeall();

		comboBox->add("");
		std::vector<std::string> names = glggHunSpell->getAvailDicts();
		for(int i=0; i<(int)names.size(); i++) 
		{
			comboBox->add(names[i]);
		}
		comboBox->setSimple(std::string(""));
	}
	comboBox = getChild<LLComboBox>("EmSpell_Installed");
	if(comboBox != NULL) 
	{
		comboBox->removeall();

		comboBox->add("");
		std::vector<std::string> names = glggHunSpell->getInstalledDicts();
		for(int i=0; i<(int)names.size(); i++) 
		{
			comboBox->add(names[i]);
		}
		comboBox->setSimple(std::string(""));
	}

	//epic hax (TODO: make this less hax)
	onConditionalPreferencesChanged(getChild<LLCheckBoxCtrl>("telerequest_toggle"), NULL);

	//mSkin = gSavedSettings.getString("SkinCurrent");
	//getChild<LLRadioGroup>("skin_selection")->setValue(mSkin);
}

void LLPanelEmerald::IMAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedPerAccountSettings.setString("EmeraldInstantMessageResponseItemData", item->getUUID().asString());
	sInstance->childSetValue("im_give_disp_rect_txt","Currently set to: "+item->getName());
}
void LLPanelEmerald::BuildAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedSettings.setString("EmeraldBuildPrefs_Item", item->getUUID().asString());
	sInstance->childSetValue("build_item_add_disp_rect_txt","Currently set to: "+item->getName());
}

void LLPanelEmerald::apply()
{
	LLTextEditor* im = sInstance->getChild<LLTextEditor>("im_response");
	LLWString im_response;
	if (im) im_response = im->getWText(); 
	LLWStringUtil::replaceTabsWithSpaces(im_response, 4);
	LLWStringUtil::replaceChar(im_response, '\n', '^');
	LLWStringUtil::replaceChar(im_response, ' ', '%');
	glggHunSpell->setNewHighlightSetting(gSavedSettings.getBOOL("EmeraldSpellDisplay"));
	gSavedPerAccountSettings.setString("EmeraldInstantMessageResponse", std::string(wstring_to_utf8str(im_response)));

	//gSavedPerAccountSettings.setString(
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseMuted", childGetValue("EmeraldInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseFriends", childGetValue("EmeraldInstantMessageResponseFriends").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseMuted", childGetValue("EmeraldInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseAnyone", childGetValue("EmeraldInstantMessageResponseAnyone").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageShowResponded", childGetValue("EmeraldInstantMessageShowResponded").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageShowOnTyping", childGetValue("EmeraldInstantMessageShowOnTyping").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseRepeat", childGetValue("EmeraldInstantMessageResponseRepeat").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseItem", childGetValue("EmeraldInstantMessageResponseItem").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageAnnounceIncoming", childGetValue("EmeraldInstantMessageAnnounceIncoming").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageAnnounceStealFocus", childGetValue("EmeraldInstantMessageAnnounceStealFocus").asBoolean());
	gSavedSettings.setBOOL("EmeraldDoubleClickTeleportMode", childGetValue("EmeraldDoubleClickTeleportMode").asBoolean());
	if(((gSavedSettings.getU32("RenderQualityPerformance")>=3) && gSavedSettings.getBOOL("WindLightUseAtmosShaders") && gSavedSettings.getBOOL("VertexShaderEnable")) && childGetValue("EmeraldShadowsON").asBoolean())
	{
		gSavedSettings.setBOOL("RenderUseFBO", childGetValue("EmeraldShadowsON").asBoolean());
		gSavedSettings.setBOOL("RenderDeferred", childGetValue("EmeraldShadowsON").asBoolean());
	}
	else if(!childGetValue("EmeraldShadowsON").asBoolean())
	{
		if(gSavedSettings.getBOOL("RenderDeferred"))
		{
			gSavedSettings.setBOOL("RenderDeferred", childGetValue("EmeraldShadowsON").asBoolean());
			gSavedSettings.setBOOL("RenderUseFBO", childGetValue("EmeraldShadowsON").asBoolean());
		}
	}
	else if(((gSavedSettings.getU32("RenderQualityPerformance")<3) && !gSavedSettings.getBOOL("WindLightUseAtmosShaders") && !gSavedSettings.getBOOL("VertexShaderEnable")) && childGetValue("EmeraldShadowsON").asBoolean())
	{
		childSetValue("EmeraldShadowsON",false);
		LLNotifications::instance().add("NoShadows");
		llwarns<<"Attempt to enable shadow rendering while graphics settings not on ultra!"<<llendl;
	}
	gSavedSettings.setBOOL("EmeraldShadowsToggle", childGetValue("EmeraldShadowsON").asBoolean());
	gSavedSettings.setU32("EmeraldUseOTR", (U32)childGetValue("EmeraldUseOTR").asReal());
	gLggBeamMaps.forceUpdate();
}

void LLPanelEmerald::cancel()
{
}
void LLPanelEmerald::onClickVoiceRevertProd(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	gSavedSettings.setString("vivoxProductionServerName", "bhr.vivox.com");
	self->getChild<LLLineEditor>("production_voice_field")->setValue("bhr.vivox.com");
}

void LLPanelEmerald::onClickBoobReset(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	LLControlVariable *var;

	var = self->findControl("EmeraldBoobMass");
	self->getChild<LLSliderCtrl>("EmeraldBoobMass")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("EmeraldBoobHardness");
	self->getChild<LLSliderCtrl>("EmeraldBoobHardness")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("EmeraldBoobVelMax");
	self->getChild<LLSliderCtrl>("EmeraldBoobVelMax")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("EmeraldBoobFriction");
	self->getChild<LLSliderCtrl>("EmeraldBoobFriction")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("EmeraldBoobVelMin");
	self->getChild<LLSliderCtrl>("EmeraldBoobVelMin")->setValue(var->getDefault());
	var->resetToDefault();
}

void LLPanelEmerald::onCustomBeam(void* data)
{
	//LLPanelEmerald* self =(LLPanelEmerald*)data;
	LggBeamMap::show(true, data);

}
void LLPanelEmerald::onKeywordAllertButton(void * data)
{
	MfdKeywordFloaterStart::show(true,data);
}
void LLPanelEmerald::onCustomBeamColor(void* data)
{
	LggBeamColorMap::show(true,data);
}
void LLPanelEmerald::onSpellAdd(void* data)
{
	LLPanelEmerald* panel = (LLPanelEmerald*)data;
	if(panel)
	{
		glggHunSpell->addButton(panel->childGetValue("EmSpell_Avail").asString());
	}
	panel->refresh();
}
void LLPanelEmerald::onSpellRemove(void* data)
{
	LLPanelEmerald* panel = (LLPanelEmerald*)data;
	if(panel)
	{
		glggHunSpell->removeButton(panel->childGetValue("EmSpell_Installed").asString());
	}
	panel->refresh();
}
void LLPanelEmerald::onSpellGetMore(void* data)
{
	glggHunSpell->getMoreButton(data);
}
void LLPanelEmerald::onSpellEditCustom(void* data)
{
	glggHunSpell->editCustomButton();
}
void LLPanelEmerald::onStealth(void* data)
{
	//LLPanelEmerald* self =(LLPanelEmerald*)data;
	LLNotifications::instance().add("EmeraldStealth", LLSD(),LLSD(), callbackEmeraldStealth);
	

}
void LLPanelEmerald::callbackEmeraldStealth(const LLSD &notification, const LLSD &response)
{
	//gSavedSettings.setWarning("EmeraldOTR", FALSE);
	S32 option = LLNotification::getSelectedOption(notification, response);
	if ( option == 0 )
	{
		gSavedSettings.setU32("EmeraldUseOTR",(U32)0);
		gSavedSettings.setBOOL("EmeraldRainbowBeam",false);
		gSavedSettings.setString("EmeraldBeamShape","===OFF===");
		gSavedSettings.setBOOL("EmeraldCryoDetection",false);
		gSavedSettings.setBOOL("EmeraldGUSEnabled",false);
		gSavedSettings.setBOOL("EmeraldClothingLayerProtection",false);
		gSavedSettings.setBOOL("EmeraldParticleChat",false);
		gSavedSettings.setBOOL("EmeraldRadarChatKeys",false);
		gSavedSettings.setBOOL("EmeraldUseBridgeOnline",false);
		gSavedSettings.setBOOL("EmeraldUseBridgeRadar",false);
		gSavedSettings.setBOOL("EmeraldMoveLockDCT",false);
	}
}
void LLPanelEmerald::onNoStealth(void* data)
{
	//LLPanelEmerald* self =(LLPanelEmerald*)data;
	
	LLNotifications::instance().add("EmeraldNoStealth", LLSD(),LLSD(), callbackEmeraldNoStealth);
	

}

void LLPanelEmerald::callbackEmeraldNoStealth(const LLSD &notification, const LLSD &response)
{
	//gSavedSettings.setWarning("EmeraldOTR", FALSE);
	S32 option = LLNotification::getSelectedOption(notification, response);
	if ( option == 0 )
	{
		gSavedSettings.setU32("EmeraldUseOTR",(U32)2);
		gSavedSettings.setBOOL("EmeraldRainbowBeam",true);
		gSavedSettings.setString("EmeraldBeamShape","Emerald");
		//gSavedSettings.setBOOL("EmeraldCryoDetect",true);
		//gSavedSettings.setBOOL("EmeraldGUSEnabled",true);
		gSavedSettings.setBOOL("EmeraldClothingLayerProtection",true);
		gSavedSettings.setBOOL("EmeraldBuildBridge",true);
		gSavedSettings.setBOOL("EmeraldUseBridgeOnline",true);
		gSavedSettings.setBOOL("EmeraldUseBridgeRadar",true);		
		gSavedSettings.setBOOL("EmeraldMoveLockDCT",true);

	}
}
void LLPanelEmerald::beamUpdateCall(LLUICtrl* crtl, void* userdata)
{
	gLggBeamMaps.forceUpdate();
}
void LLPanelEmerald::onComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{
	
	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		gSavedSettings.setString(box->getControlName(),box->getValue().asString());
	}	
}
void LLPanelEmerald::onSpellBaseComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{

	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		glggHunSpell->newDictSelection(box->getValue().asString());
		//LLPanelEmerald* panel = (LLPanelEmerald*)userdata;//box->getParent();
		if(sInstance)sInstance->refresh();

	}
	//LLPanelEmerald* panel = (LLPanelEmerald*)userdata;
	//if(panel)panel->refresh();
}
void LLPanelEmerald::onTexturePickerCommit(LLUICtrl* ctrl, void* userdata)
{
	LLTextureCtrl*	image_ctrl = (LLTextureCtrl*)ctrl;
	if(image_ctrl)
	{
		gSavedSettings.setString("EmeraldBuildPrefs_Texture", image_ctrl->getImageAssetID().asString());
	}
}

void LLPanelEmerald::onClickOtrHelp(void* data)           // [$PLOTR$]
{
    LLWeb::loadURL("http://www.cypherpunks.ca/otr/");
}                                                         // [/$PLOTR$]

void LLPanelEmerald::onRefresh(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	self->refresh();
	
	
}
void LLPanelEmerald::onClickVoiceRevertDebug(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	gSavedSettings.setString("vivoxDebugServerName", "bhd.vivox.com");
	self->getChild<LLLineEditor>("debug_voice_field")->setValue("bhd.vivox.com");

 
 
}
void LLPanelEmerald::onBeamDelete(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	
	LLComboBox* comboBox = self->getChild<LLComboBox>("EmeraldBeamShape_combo");

	if(comboBox != NULL) 
	{
		std::string filename = comboBox->getValue().asString()+".xml";
		std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "beams", filename));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beams", filename));
		
		if(gDirUtilp->fileExists(path_name1))
		{
			LLFile::remove(path_name1);
			gSavedSettings.setString("EmeraldBeamShape","===OFF===");
		}
		if(gDirUtilp->fileExists(path_name2))
		{
			LLFile::remove(path_name2);
			gSavedSettings.setString("EmeraldBeamShape","===OFF===");
		}
	}
	self->refresh();
}
void LLPanelEmerald::onBeamColorDelete(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;

	LLComboBox* comboBox = self->getChild<LLComboBox>("BeamColor_combo");

	if(comboBox != NULL) 
	{
		std::string filename = comboBox->getValue().asString()+".xml";
		std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "beamsColors", filename));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beamsColors", filename));

		if(gDirUtilp->fileExists(path_name1))
		{
			LLFile::remove(path_name1);
			gSavedSettings.setString("EmeraldBeamColorFile","===OFF===");
		}
		if(gDirUtilp->fileExists(path_name2))
		{
			LLFile::remove(path_name2);
			gSavedSettings.setString("EmeraldBeamColorFile","===OFF===");
		}
	}
	self->refresh();
}



//workaround for lineeditor dumbness in regards to control_name
void LLPanelEmerald::onCommitApplyControl(LLUICtrl* caller, void* user_data)
{
	LLLineEditor* line = (LLLineEditor*)caller;
	if(line)
	{
		LLControlVariable *var = line->findControl(line->getControlName());
		if(var)var->setValue(line->getValue());
	}
}

void LLPanelEmerald::onCommitSendAppearance(LLUICtrl* ctrl, void* userdata)
{
	gAgent.sendAgentSetAppearance();
	//llinfos << llformat("%d,%d,%d",gSavedSettings.getF32("EmeraldAvatarXModifier"),gSavedSettings.getF32("EmeraldAvatarYModifier"),gSavedSettings.getF32("EmeraldAvatarZModifier")) << llendl;
}

void LLPanelEmerald::onClickSetMirror(void* user_data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)user_data;

	std::string cur_name(gSavedSettings.getString("EmeraldInvMirrorLocation"));
	std::string proposed_name(cur_name);
	
	LLDirPicker& picker = LLDirPicker::instance();
	if (! picker.getDir(&proposed_name ) )
	{
		return; //Canceled!
	}

	std::string dir_name = picker.getDirName();
	if (!dir_name.empty() && dir_name != cur_name)
	{
		self->childSetText("mirror_location", dir_name);
		gSavedSettings.setString("EmeraldInvMirrorLocation", dir_name);
	}
}

void LLPanelEmerald::onClickSetHDDInclude(void* user_data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)user_data;

	std::string cur_name(gSavedSettings.getString("EmeraldHDDIncludeLocation"));
	std::string proposed_name(cur_name);
	
	LLDirPicker& picker = LLDirPicker::instance();
	if (! picker.getDir(&proposed_name ) )
	{
		return; //Canceled!
	}

	std::string dir_name = picker.getDirName();
	if (!dir_name.empty() && dir_name != cur_name)
	{
		self->childSetText("include_location", dir_name);
		gSavedSettings.setString("EmeraldHDDIncludeLocation", dir_name);
	}
}

void LLPanelEmerald::onConditionalPreferencesChanged(LLUICtrl* ctrl, void* userdata)
{
	LLPanelEmerald* self = (LLPanelEmerald*)ctrl->getParent();
	if(!self)return;
	LLCheckBoxCtrl* teleport = self->getChild<LLCheckBoxCtrl>("telerequest_toggle");
	LLCheckBoxCtrl* movelock = self->getChild<LLCheckBoxCtrl>("mldct_toggle");
	if(!(teleport && movelock))return;
	//bool teep = (bool)teleport->getValue().asBoolean();
	bool moov = (bool)movelock->getValue().asBoolean();
	if(moov)
	{
		teleport->setEnabled(true);
	}
	else
	{
		teleport->setValue(LLSD(true));
		gSavedSettings.setBOOL("EmeraldRequestLocalTeleports", true);
		teleport->setEnabled(false);
	}
}

/*
//static 
void LLPanelEmerald::onClickSilver(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	gSavedSettings.setString("SkinCurrent", "silver");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("silver");
}*/
