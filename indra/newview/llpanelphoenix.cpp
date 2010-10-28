
/** 
 * @file LLPanelPhoenix.cpp
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

#include "llpanelphoenix.h"
// linden library includes
#include "llradiogroup.h"
#include "llbutton.h"
#include "llcolorswatch.h"
#include "lluictrlfactory.h"

#include "llcombobox.h"
#include "llslider.h"
#include "lltexturectrl.h"

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
//#include "lggbeamcolormapfloater.h"
//#include "lggbeammapfloater.h"
//#include "lggbeammaps.h"
//#include "lggbeamscolors.h"
#include "llsliderctrl.h"
//#include "mfdkeywordfloater.h"
//#include "lgghunspell_wrapper.h"
#include "llspinctrl.h"
//#include "lggautocorrectfloater.h"

#include "llprimitive.h"
#include "a_phoenixviewerlink.h"

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

LLPanelPhoenix* LLPanelPhoenix::sInstance;

JCInvDropTarget * LLPanelPhoenix::mObjectDropTarget;
JCInvDropTarget * LLPanelPhoenix::mBuildObjectDropTarget;

LLPanelPhoenix::LLPanelPhoenix()
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_phoenix.xml");
	if(sInstance)delete sInstance;
	sInstance = this;
}

LLPanelPhoenix::~LLPanelPhoenix()
{
	sInstance = NULL;
	delete mObjectDropTarget;
	delete mBuildObjectDropTarget;
	mObjectDropTarget = NULL;
	mBuildObjectDropTarget = NULL;
}

void LLPanelPhoenix::initHelpBtn(const std::string& name, const std::string& xml_alert)
{
	childSetAction(name, onClickHelp, new std::string(xml_alert));
}

void LLPanelPhoenix::onClickHelp(void* data)
{
	std::string* xml_alert = (std::string*)data;
	LLNotifications::instance().add(*xml_alert);
}

BOOL LLPanelPhoenix::postBuild()
{
	refresh();

//	LLComboBox* tagcombo = getChild<LLComboBox>("TagCombo");

	getChild<LLComboBox>("material")->setSimple(gSavedSettings.getString("PhoenixBuildPrefs_Material"));
	getChild<LLComboBox>("combobox shininess")->setSimple(gSavedSettings.getString("PhoenixBuildPrefs_Shiny"));
	

	//LLSliderCtrl* mShapeScaleSlider = getChild<LLSliderCtrl>("AscentBeamShapeScale",TRUE,FALSE);
	//mShapeScaleSlider->setCommitCallback(&LLPanelAscent::beamUpdateCall);
	//mShapeScaleSlider->setCallbackUserData(this);

	//LLSliderCtrl* mBeamsPerSecondSlider = getChild<LLSliderCtrl>("AscentMaxBeamsPerSecond",TRUE,FALSE);
	//mBeamsPerSecondSlider->setCommitCallback(&LLPanelAscent::beamUpdateCall);
	//mBeamsPerSecondSlider->setCallbackUserData(this);

	getChild<LLComboBox>("material")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("combobox shininess")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("PhoenixBeamShape_combo")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("BeamColor_combo")->setCommitCallback(onComboBoxCommit);
	getChild<LLTextureCtrl>("texture control")->setDefaultImageAssetID(LLUUID("89556747-24cb-43ed-920b-47caed15465f"));
	getChild<LLTextureCtrl>("texture control")->setCommitCallback(onTexturePickerCommit);

	getChild<LLComboBox>("PhoenixSpellBase")->setCommitCallback(onSpellBaseComboBoxCommit);

	getChild<LLCheckBoxCtrl>("ColorTagsClient_toggle")->setCommitCallback(onTagsBoxCommit);

		
	//childSetCommitCallback("material",onComboBoxCommit);
	//childSetCommitCallback("combobox shininess",onComboBoxCommit);


	getChild<LLButton>("keyword_allert")->setClickedCallback(onKeywordAllertButton,this);
	getChild<LLButton>("ac_button")->setClickedCallback(onAutoCorrectButton,this);
	
	
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

	getChild<LLButton>("PhoenixBreastReset")->setClickedCallback(onClickBoobReset, this);
	

	childSetCommitCallback("production_voice_field", onCommitApplyControl);//onCommitVoiceProductionServerName);
	childSetCommitCallback("debug_voice_field", onCommitApplyControl);//onCommitVoiceDebugServerName);

	childSetCommitCallback("PhoenixCmdLinePos", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineGround", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineHeight", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineTeleportHome", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineRezPlatform", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineMapTo", onCommitApplyControl);	
	childSetCommitCallback("PhoenixCmdLineCalc", onCommitApplyControl);

	childSetCommitCallback("PhoenixCmdLineDrawDistance", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdTeleportToCam", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineKeyToName", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineOfferTp", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineTP2", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineAO", onCommitApplyControl);
	childSetCommitCallback("PhoenixCmdLineClearChat", onCommitApplyControl);

	childSetCommitCallback("X Modifier", onCommitAvatarModifier);
	childSetCommitCallback("Y Modifier", onCommitAvatarModifier);
	childSetCommitCallback("Z Modifier", onCommitAvatarModifier);
	if(gAgent.getID() != LLUUID::null)
	{
		getChild<LLSpinCtrl>("X Modifier")->set(gSavedPerAccountSettings.getF32("PhoenixAvatarXModifier"));
		getChild<LLSpinCtrl>("Y Modifier")->set(gSavedPerAccountSettings.getF32("PhoenixAvatarYModifier"));
		getChild<LLSpinCtrl>("Z Modifier")->set(gSavedPerAccountSettings.getF32("PhoenixAvatarZModifier"));
	}else
	{
		getChild<LLSpinCtrl>("X Modifier")->setEnabled(FALSE);
		getChild<LLSpinCtrl>("Y Modifier")->setEnabled(FALSE);
		getChild<LLSpinCtrl>("Z Modifier")->setEnabled(FALSE);
	}


	childSetValue("PhoenixUseOTR", LLSD((S32)gSavedSettings.getU32("PhoenixUseOTR"))); // [$PLOTR$]
	getChild<LLButton>("otr_help_btn")->setClickedCallback(onClickOtrHelp, this);      // [/$PLOTR$]

	initHelpBtn("PhoenixHelp_TeleportLogin",	"PhoenixHelp_TeleportLogin");
	initHelpBtn("PhoenixHelp_Voice",			"PhoenixHelp_Voice");
	initHelpBtn("PhoenixHelp_Shields",			"PhoenixHelp_Shields");
	initHelpBtn("PhoenixHelp_IM",				"PhoenixHelp_IM");
	initHelpBtn("PhoenixHelp_Chat",				"PhoenixHelp_Chat");
	initHelpBtn("PhoenixHelp_Misc",				"PhoenixHelp_Misc");
	initHelpBtn("PhoenixHelp_CmdLine",			"PhoenixHelp_CmdLine");
	initHelpBtn("PhoenixHelp_Avatar",			"PhoenixHelp_Avatar");
	initHelpBtn("PhoenixHelp_Build",			"PhoenixHelp_Build");
	initHelpBtn("PhoenixHelp_IRC",				"PhoenixHelp_IRC");
	initHelpBtn("PhoenixHelp_UtilityStream",	"PhoenixHelp_UtilityStream");
	initHelpBtn("PhoenixHelp_Inventory",		"PhoenixHelp_Inventory");
	initHelpBtn("PhoenixHelp_Effects",			"PhoenixHelp_Effects");
	initHelpBtn("PhoenixHelp_SpellCheck",		"PhoenixHelp_SpellCheck");

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
		LLUUID itemid = (LLUUID)gSavedPerAccountSettings.getString("PhoenixInstantMessageResponseItemData");
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
		itemid = (LLUUID)gSavedSettings.getString("PhoenixBuildPrefs_Item");
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

	LLWString auto_response = utf8str_to_wstring( gSavedPerAccountSettings.getString("PhoenixInstantMessageResponse") );
	LLWStringUtil::replaceChar(auto_response, '^', '\n');
	LLWStringUtil::replaceChar(auto_response, '%', ' ');
	childSetText("im_response", wstring_to_utf8str(auto_response));
	childSetValue("PhoenixInstantMessageResponseFriends", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageResponseFriends"));
	childSetValue("PhoenixInstantMessageResponseMuted", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageResponseMuted"));
	childSetValue("PhoenixInstantMessageResponseAnyone", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageResponseAnyone"));
	childSetValue("PhoenixInstantMessageShowResponded", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageShowResponded"));
	//Kadah - Force IMShowOnTyping to resolve a bug were if AnnounceIncoming was on, autoreply would not be sent
	const bool AnnounceIncoming = gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageAnnounceIncoming");
	childSetValue("PhoenixInstantMessageShowOnTyping", AnnounceIncoming || gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageShowOnTyping"));
	getChild<LLCheckBoxCtrl>("PhoenixInstantMessageShowOnTyping")->setEnabled(!AnnounceIncoming);
	childSetValue("PhoenixInstantMessageResponseRepeat", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageResponseRepeat" ));
	childSetValue("PhoenixInstantMessageResponseItem", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageResponseItem"));
	childSetValue("PhoenixInstantMessageAnnounceIncoming", AnnounceIncoming);
	getChild<LLCheckBoxCtrl>("PhoenixInstantMessageAnnounceIncoming")->setCommitCallback(onPhoenixInstantMessageAnnounceIncoming);
	childSetValue("PhoenixInstantMessageAnnounceStealFocus", gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageAnnounceStealFocus"));
	childSetValue("PhoenixShadowsON", gSavedSettings.getBOOL("PhoenixShadowsToggle"));

	childSetAction("set_mirror", onClickSetMirror, this);
	childSetCommitCallback("mirror_location", onCommitApplyControl);

	childSetAction("set_includeHDD", onClickSetHDDInclude, this);
	childSetCommitCallback("include_location", onCommitApplyControl);

	//PhoenixLSLExternalEditor
	childSetAction("set_xed", onClickSetXed, this);
	childSetCommitCallback("xed_location", onCommitApplyControl);
	childSetValue("xed_location", gSavedSettings.getString("PhoenixLSLExternalEditor"));

	getChild<LLCheckBoxCtrl>("telerequest_toggle")->setCommitCallback(onConditionalPreferencesChanged);
	getChild<LLCheckBoxCtrl>("mldct_toggle")->setCommitCallback(onConditionalPreferencesChanged);
	
	getChild<LLColorSwatchCtrl>("friend_tag_color_swatch")->set(gSavedSettings.getColor4("PhoenixFriendNameColor"));

	refresh();
	return TRUE;
}

void LLPanelPhoenix::refresh()
{
	
//	LLComboBox* comboBox = getChild<LLComboBox>("AscentBeamShape_combo");
}

void LLPanelPhoenix::IMAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedPerAccountSettings.setString("PhoenixInstantMessageResponseItemData", item->getUUID().asString());
	sInstance->childSetValue("im_give_disp_rect_txt","Currently set to: "+item->getName());
}
void LLPanelPhoenix::BuildAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedSettings.setString("PhoenixBuildPrefs_Item", item->getUUID().asString());
	sInstance->childSetValue("build_item_add_disp_rect_txt","Currently set to: "+item->getName());
}

void LLPanelPhoenix::apply()
{
	LLTextEditor* im = sInstance->getChild<LLTextEditor>("im_response");
	LLWString im_response;
	if (im) im_response = im->getWText(); 
	LLWStringUtil::replaceTabsWithSpaces(im_response, 4);
	LLWStringUtil::replaceChar(im_response, '\n', '^');
	LLWStringUtil::replaceChar(im_response, ' ', '%');
	gSavedPerAccountSettings.setString("PhoenixInstantMessageResponse", std::string(wstring_to_utf8str(im_response)));

	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageResponseMuted", childGetValue("PhoenixInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageResponseFriends", childGetValue("PhoenixInstantMessageResponseFriends").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageResponseMuted", childGetValue("PhoenixInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageResponseAnyone", childGetValue("PhoenixInstantMessageResponseAnyone").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageShowResponded", childGetValue("PhoenixInstantMessageShowResponded").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageShowOnTyping", childGetValue("PhoenixInstantMessageShowOnTyping").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageResponseRepeat", childGetValue("PhoenixInstantMessageResponseRepeat").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageResponseItem", childGetValue("PhoenixInstantMessageResponseItem").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageAnnounceIncoming", childGetValue("PhoenixInstantMessageAnnounceIncoming").asBoolean());
	gSavedPerAccountSettings.setBOOL("PhoenixInstantMessageAnnounceStealFocus", childGetValue("PhoenixInstantMessageAnnounceStealFocus").asBoolean());
	if(((gSavedSettings.getU32("RenderQualityPerformance")>=3) && gSavedSettings.getBOOL("WindLightUseAtmosShaders") && gSavedSettings.getBOOL("VertexShaderEnable")) && childGetValue("PhoenixShadowsON").asBoolean())
	{
		gSavedSettings.setBOOL("RenderUseFBO", childGetValue("PhoenixShadowsON").asBoolean());
		gSavedSettings.setBOOL("RenderDeferred", childGetValue("PhoenixShadowsON").asBoolean());
	}
	else if(!childGetValue("PhoenixShadowsON").asBoolean())
	{
		if(gSavedSettings.getBOOL("RenderDeferred"))
		{
			gSavedSettings.setBOOL("RenderDeferred", childGetValue("PhoenixShadowsON").asBoolean());
			gSavedSettings.setBOOL("RenderUseFBO", childGetValue("PhoenixShadowsON").asBoolean());
		}
	}
	else if(((gSavedSettings.getU32("RenderQualityPerformance")<3) && !gSavedSettings.getBOOL("WindLightUseAtmosShaders") && !gSavedSettings.getBOOL("VertexShaderEnable")) && childGetValue("PhoenixShadowsON").asBoolean())
	{
		childSetValue("PhoenixShadowsON",false);
		LLNotifications::instance().add("NoShadows");
		llwarns<<"Attempt to enable shadow rendering while graphics settings not on ultra!"<<llendl;
	}
	gSavedSettings.setBOOL("PhoenixShadowsToggle", childGetValue("PhoenixShadowsON").asBoolean());
	gSavedSettings.setU32("PhoenixUseOTR", (U32)childGetValue("PhoenixUseOTR").asReal());
//	gLggBeamMaps.forceUpdate();
}

void LLPanelPhoenix::cancel()
{
}
void LLPanelPhoenix::onClickVoiceRevertProd(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;
	gSavedSettings.setString("vivoxProductionServerName", "bhr.vivox.com");
	self->getChild<LLLineEditor>("production_voice_field")->setValue("bhr.vivox.com");
}

void LLPanelPhoenix::onClickBoobReset(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;
	LLControlVariable *var;

	var = self->findControl("PhoenixBoobMass");
	self->getChild<LLSliderCtrl>("PhoenixBoobMass")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("PhoenixBoobHardness");
	self->getChild<LLSliderCtrl>("PhoenixBoobHardness")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("PhoenixBoobVelMax");
	self->getChild<LLSliderCtrl>("PhoenixBoobVelMax")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("PhoenixBoobFriction");
	self->getChild<LLSliderCtrl>("PhoenixBoobFriction")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("PhoenixBoobVelMin");
	self->getChild<LLSliderCtrl>("PhoenixBoobVelMin")->setValue(var->getDefault());
	var->resetToDefault();
}

void LLPanelPhoenix::onCustomBeam(void* data)
{
	//LLPanelAscent* self =(LLPanelAscent*)data;
	//LggBeamMap::show(true, data);

}
void LLPanelPhoenix::onKeywordAllertButton(void * data)
{
//	MfdKeywordFloaterStart::show(true,data);
}
void LLPanelPhoenix::onAutoCorrectButton(void * data)
{
//	lggAutoCorrectFloaterStart::show(TRUE,data);
}
void LLPanelPhoenix::onCustomBeamColor(void* data)
{
//	LggBeamColorMap::show(true,data);
}
void LLPanelPhoenix::onSpellAdd(void* data)
{

}
void LLPanelPhoenix::onSpellRemove(void* data)
{

}
void LLPanelPhoenix::onSpellGetMore(void* data)
{
}
void LLPanelPhoenix::onSpellEditCustom(void* data)
{
}

void LLPanelPhoenix::beamUpdateCall(LLUICtrl* crtl, void* userdata)
{
}
void LLPanelPhoenix::onTagComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{
}
void LLPanelPhoenix::onComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{

	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		gSavedSettings.setString(box->getControlName(),box->getValue().asString());
	}	
}
void LLPanelPhoenix::onTagsBoxCommit(LLUICtrl* ctrl, void* userdata)
{
	PhoenixViewerLink::getInstance()->downloadClientTags();
}
void LLPanelPhoenix::onSpellBaseComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{

	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		//glggHunSpell->newDictSelection(box->getValue().asString());
		//LLPanelAscent* panel = (LLPanelAscent*)userdata;//box->getParent();
		if(sInstance)sInstance->refresh();

	}
	//LLPanelPhoenix* panel = (LLPanelPhoenix*)userdata;
	//if(panel)panel->refresh();
}
void LLPanelPhoenix::onTexturePickerCommit(LLUICtrl* ctrl, void* userdata)
{
	LLTextureCtrl*	image_ctrl = (LLTextureCtrl*)ctrl;
	if(image_ctrl)
	{
		gSavedSettings.setString("PhoenixBuildPrefs_Texture", image_ctrl->getImageAssetID().asString());
	}
}

void LLPanelPhoenix::onClickOtrHelp(void* data)           // [$PLOTR$]
{
    LLWeb::loadURL("http://www.cypherpunks.ca/otr/");
}                                                         // [/$PLOTR$]

void LLPanelPhoenix::onRefresh(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;
	self->refresh();
	
	
}
void LLPanelPhoenix::onClickVoiceRevertDebug(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;
	gSavedSettings.setString("vivoxDebugServerName", "bhd.vivox.com");
	self->getChild<LLLineEditor>("debug_voice_field")->setValue("bhd.vivox.com");

 
 
}
void LLPanelPhoenix::onBeamDelete(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;
	
	LLComboBox* comboBox = self->getChild<LLComboBox>("PhoenixBeamShape_combo");

	if(comboBox != NULL) 
	{
		std::string filename = comboBox->getValue().asString()+".xml";
		std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "beams", filename));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beams", filename));
		
		if(gDirUtilp->fileExists(path_name1))
		{
			LLFile::remove(path_name1);
			gSavedSettings.setString("PhoenixBeamShape","===OFF===");
		}
		if(gDirUtilp->fileExists(path_name2))
		{
			LLFile::remove(path_name2);
			gSavedSettings.setString("PhoenixBeamShape","===OFF===");
		}
	}
	self->refresh();
}
void LLPanelPhoenix::onBeamColorDelete(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;

	LLComboBox* comboBox = self->getChild<LLComboBox>("BeamColor_combo");

	if(comboBox != NULL) 
	{
		std::string filename = comboBox->getValue().asString()+".xml";
		std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "beamsColors", filename));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beamsColors", filename));

		if(gDirUtilp->fileExists(path_name1))
		{
			LLFile::remove(path_name1);
			gSavedSettings.setString("PhoenixBeamColorFile","===OFF===");
		}
		if(gDirUtilp->fileExists(path_name2))
		{
			LLFile::remove(path_name2);
			gSavedSettings.setString("PhoenixBeamColorFile","===OFF===");
		}
	}
	self->refresh();
}



//workaround for lineeditor dumbness in regards to control_name
void LLPanelPhoenix::onCommitApplyControl(LLUICtrl* caller, void* user_data)
{
	LLLineEditor* line = (LLLineEditor*)caller;
	if(line)
	{
		LLControlVariable *var = line->findControl(line->getControlName());
		if(var)var->setValue(line->getValue());
	}
}

void LLPanelPhoenix::onCommitAvatarModifier(LLUICtrl* ctrl, void* userdata)
{
	gSavedPerAccountSettings.setF32("PhoenixAvatarXModifier", sInstance->getChild<LLSpinCtrl>("X Modifier")->get());
	gSavedPerAccountSettings.setF32("PhoenixAvatarYModifier", sInstance->getChild<LLSpinCtrl>("Y Modifier")->get());
	gSavedPerAccountSettings.setF32("PhoenixAvatarZModifier", sInstance->getChild<LLSpinCtrl>("Z Modifier")->get());
	gAgent.sendAgentSetAppearance();
}

void LLPanelPhoenix::onClickSetMirror(void* user_data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)user_data;

	std::string cur_name(gSavedSettings.getString("PhoenixInvMirrorLocation"));
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
		gSavedSettings.setString("PhoenixInvMirrorLocation", dir_name);
	}
}

void LLPanelPhoenix::onClickSetHDDInclude(void* user_data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)user_data;

	std::string cur_name(gSavedSettings.getString("PhoenixHDDIncludeLocation"));
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
		gSavedSettings.setString("PhoenixHDDIncludeLocation", dir_name);
	}
}
void LLPanelPhoenix::onClickSetXed(void* user_data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)user_data;

	std::string cur_name(gSavedSettings.getString("PhoenixLSLExternalEditor"));
	
	LLFilePicker& picker = LLFilePicker::instance();
	if (! picker.getOpenFile(LLFilePicker::FFLOAD_APP) )
	{
		return; //Canceled!
	}
	std::string file_name = picker.getFirstFile();
	if (!file_name.empty() && file_name != cur_name)
	{
		self->childSetText("xed_location", file_name);
		gSavedSettings.setString("PhoenixLSLExternalEditor", file_name);
		
	} else {
		//self->childSetText("xed_location", " ");
	  gSavedSettings.setString("PhoenixLSLExternalEditor", " ");
	}
}

//Kadah - Force IMShowOnTyping to resolve a bug were if AnnounceIncoming was on, autoreply would not be sent
void LLPanelPhoenix::onPhoenixInstantMessageAnnounceIncoming(LLUICtrl* ctrl, void* userdata)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)ctrl->getParent();
	const bool AnnounceIncoming = self->childGetValue("PhoenixInstantMessageAnnounceIncoming").asBoolean();
	LLCheckBoxCtrl* ShowOnTyping = self->getChild<LLCheckBoxCtrl>("PhoenixInstantMessageShowOnTyping");
	ShowOnTyping->setEnabled(!AnnounceIncoming);
	if (AnnounceIncoming)
	{
		ShowOnTyping->setValue(true);
	} else {
		ShowOnTyping->setValue(gSavedPerAccountSettings.getBOOL("PhoenixInstantMessageShowOnTyping")); //restore to last value
	}
}

void LLPanelPhoenix::onConditionalPreferencesChanged(LLUICtrl* ctrl, void* userdata)
{
	/*LLPanelPhoenix* self = (LLPanelPhoenix*)ctrl->getParent();
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
		gSavedSettings.setBOOL("PhoenixRequestLocalTeleports", true);
		teleport->setEnabled(false);
	}*/
}

/*
//static 
void LLPanelPhoenix::onClickSilver(void* data)
{
	LLPanelPhoenix* self = (LLPanelPhoenix*)data;
	gSavedSettings.setString("SkinCurrent", "silver");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("silver");
}*/
