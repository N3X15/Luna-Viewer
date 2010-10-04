
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

LLPanelAscent* LLPanelAscent::sInstance;

JCInvDropTarget * LLPanelAscent::mObjectDropTarget;
JCInvDropTarget * LLPanelAscent::mBuildObjectDropTarget;

LLPanelAscent::LLPanelAscent()
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_phoenix.xml");
	if(sInstance)delete sInstance;
	sInstance = this;
}

LLPanelAscent::~LLPanelAscent()
{
	sInstance = NULL;
	delete mObjectDropTarget;
	delete mBuildObjectDropTarget;
	mObjectDropTarget = NULL;
	mBuildObjectDropTarget = NULL;
}

void LLPanelAscent::initHelpBtn(const std::string& name, const std::string& xml_alert)
{
	childSetAction(name, onClickHelp, new std::string(xml_alert));
}

void LLPanelAscent::onClickHelp(void* data)
{
	std::string* xml_alert = (std::string*)data;
	LLNotifications::instance().add(*xml_alert);
}

BOOL LLPanelAscent::postBuild()
{
	refresh();

//	LLComboBox* tagcombo = getChild<LLComboBox>("TagCombo");

	getChild<LLComboBox>("material")->setSimple(gSavedSettings.getString("AscentBuildPrefs_Material"));
	getChild<LLComboBox>("combobox shininess")->setSimple(gSavedSettings.getString("AscentBuildPrefs_Shiny"));
	

	//LLSliderCtrl* mShapeScaleSlider = getChild<LLSliderCtrl>("AscentBeamShapeScale",TRUE,FALSE);
	//mShapeScaleSlider->setCommitCallback(&LLPanelAscent::beamUpdateCall);
	//mShapeScaleSlider->setCallbackUserData(this);

	//LLSliderCtrl* mBeamsPerSecondSlider = getChild<LLSliderCtrl>("AscentMaxBeamsPerSecond",TRUE,FALSE);
	//mBeamsPerSecondSlider->setCommitCallback(&LLPanelAscent::beamUpdateCall);
	//mBeamsPerSecondSlider->setCallbackUserData(this);

	getChild<LLComboBox>("material")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("combobox shininess")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("AscentBeamShape_combo")->setCommitCallback(onComboBoxCommit);
	getChild<LLComboBox>("BeamColor_combo")->setCommitCallback(onComboBoxCommit);
	getChild<LLTextureCtrl>("texture control")->setDefaultImageAssetID(LLUUID("89556747-24cb-43ed-920b-47caed15465f"));
	getChild<LLTextureCtrl>("texture control")->setCommitCallback(onTexturePickerCommit);

	getChild<LLComboBox>("AscentSpellBase")->setCommitCallback(onSpellBaseComboBoxCommit);

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

	getChild<LLButton>("AscentBreastReset")->setClickedCallback(onClickBoobReset, this);
	

	childSetCommitCallback("production_voice_field", onCommitApplyControl);//onCommitVoiceProductionServerName);
	childSetCommitCallback("debug_voice_field", onCommitApplyControl);//onCommitVoiceDebugServerName);

	childSetCommitCallback("AscentCmdLinePos", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineGround", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineHeight", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineTeleportHome", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineRezPlatform", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineMapTo", onCommitApplyControl);	
	childSetCommitCallback("AscentCmdLineCalc", onCommitApplyControl);

	childSetCommitCallback("AscentCmdLineDrawDistance", onCommitApplyControl);
	childSetCommitCallback("AscentCmdTeleportToCam", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineKeyToName", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineOfferTp", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineTP2", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineAO", onCommitApplyControl);
	childSetCommitCallback("AscentCmdLineClearChat", onCommitApplyControl);

	childSetCommitCallback("X Modifier", onCommitAvatarModifier);
	childSetCommitCallback("Y Modifier", onCommitAvatarModifier);
	childSetCommitCallback("Z Modifier", onCommitAvatarModifier);
	if(gAgent.getID() != LLUUID::null)
	{
		getChild<LLSpinCtrl>("X Modifier")->set(gSavedPerAccountSettings.getF32("AscentAvatarXModifier"));
		getChild<LLSpinCtrl>("Y Modifier")->set(gSavedPerAccountSettings.getF32("AscentAvatarYModifier"));
		getChild<LLSpinCtrl>("Z Modifier")->set(gSavedPerAccountSettings.getF32("AscentAvatarZModifier"));
	}else
	{
		getChild<LLSpinCtrl>("X Modifier")->setEnabled(FALSE);
		getChild<LLSpinCtrl>("Y Modifier")->setEnabled(FALSE);
		getChild<LLSpinCtrl>("Z Modifier")->setEnabled(FALSE);
	}


	childSetValue("AscentUseOTR", LLSD((S32)gSavedSettings.getU32("AscentUseOTR"))); // [$PLOTR$]
	getChild<LLButton>("otr_help_btn")->setClickedCallback(onClickOtrHelp, this);      // [/$PLOTR$]

	initHelpBtn("AscentHelp_TeleportLogin",	"AscentHelp_TeleportLogin");
	initHelpBtn("AscentHelp_Voice",			"AscentHelp_Voice");
	initHelpBtn("AscentHelp_Shields",			"AscentHelp_Shields");
	initHelpBtn("AscentHelp_IM",				"AscentHelp_IM");
	initHelpBtn("AscentHelp_Chat",				"AscentHelp_Chat");
	initHelpBtn("AscentHelp_Misc",				"AscentHelp_Misc");
	initHelpBtn("AscentHelp_CmdLine",			"AscentHelp_CmdLine");
	initHelpBtn("AscentHelp_Avatar",			"AscentHelp_Avatar");
	initHelpBtn("AscentHelp_Build",			"AscentHelp_Build");
	initHelpBtn("AscentHelp_IRC",				"AscentHelp_IRC");
	initHelpBtn("AscentHelp_UtilityStream",	"AscentHelp_UtilityStream");
	initHelpBtn("AscentHelp_Inventory",		"AscentHelp_Inventory");
	initHelpBtn("AscentHelp_Effects",			"AscentHelp_Effects");
	initHelpBtn("AscentHelp_SpellCheck",		"AscentHelp_SpellCheck");

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
		LLUUID itemid = (LLUUID)gSavedPerAccountSettings.getString("AscentInstantMessageResponseItemData");
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
		itemid = (LLUUID)gSavedSettings.getString("AscentBuildPrefs_Item");
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

	LLWString auto_response = utf8str_to_wstring( gSavedPerAccountSettings.getString("AscentInstantMessageResponse") );
	LLWStringUtil::replaceChar(auto_response, '^', '\n');
	LLWStringUtil::replaceChar(auto_response, '%', ' ');
	childSetText("im_response", wstring_to_utf8str(auto_response));
	childSetValue("AscentInstantMessageResponseFriends", gSavedPerAccountSettings.getBOOL("AscentInstantMessageResponseFriends"));
	childSetValue("AscentInstantMessageResponseMuted", gSavedPerAccountSettings.getBOOL("AscentInstantMessageResponseMuted"));
	childSetValue("AscentInstantMessageResponseAnyone", gSavedPerAccountSettings.getBOOL("AscentInstantMessageResponseAnyone"));
	childSetValue("AscentInstantMessageShowResponded", gSavedPerAccountSettings.getBOOL("AscentInstantMessageShowResponded"));
	//Kadah - Force IMShowOnTyping to resolve a bug were if AnnounceIncoming was on, autoreply would not be sent
	const bool AnnounceIncoming = gSavedPerAccountSettings.getBOOL("AscentInstantMessageAnnounceIncoming");
	childSetValue("AscentInstantMessageShowOnTyping", AnnounceIncoming || gSavedPerAccountSettings.getBOOL("AscentInstantMessageShowOnTyping"));
	getChild<LLCheckBoxCtrl>("AscentInstantMessageShowOnTyping")->setEnabled(!AnnounceIncoming);
	childSetValue("AscentInstantMessageResponseRepeat", gSavedPerAccountSettings.getBOOL("AscentInstantMessageResponseRepeat" ));
	childSetValue("AscentInstantMessageResponseItem", gSavedPerAccountSettings.getBOOL("AscentInstantMessageResponseItem"));
	childSetValue("AscentInstantMessageAnnounceIncoming", AnnounceIncoming);
	getChild<LLCheckBoxCtrl>("AscentInstantMessageAnnounceIncoming")->setCommitCallback(onAscentInstantMessageAnnounceIncoming);
	childSetValue("AscentInstantMessageAnnounceStealFocus", gSavedPerAccountSettings.getBOOL("AscentInstantMessageAnnounceStealFocus"));
	childSetValue("AscentShadowsON", gSavedSettings.getBOOL("AscentShadowsToggle"));

	childSetAction("set_mirror", onClickSetMirror, this);
	childSetCommitCallback("mirror_location", onCommitApplyControl);

	childSetAction("set_includeHDD", onClickSetHDDInclude, this);
	childSetCommitCallback("include_location", onCommitApplyControl);

	//AscentLSLExternalEditor
	childSetAction("set_xed", onClickSetXed, this);
	childSetCommitCallback("xed_location", onCommitApplyControl);
	childSetValue("xed_location", gSavedSettings.getString("AscentLSLExternalEditor"));

	getChild<LLCheckBoxCtrl>("telerequest_toggle")->setCommitCallback(onConditionalPreferencesChanged);
	getChild<LLCheckBoxCtrl>("mldct_toggle")->setCommitCallback(onConditionalPreferencesChanged);
	
	getChild<LLColorSwatchCtrl>("friend_tag_color_swatch")->set(gSavedSettings.getColor4("AscentFriendNameColor"));

	refresh();
	return TRUE;
}

void LLPanelAscent::refresh()
{
	
//	LLComboBox* comboBox = getChild<LLComboBox>("AscentBeamShape_combo");
}

void LLPanelAscent::IMAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedPerAccountSettings.setString("AscentInstantMessageResponseItemData", item->getUUID().asString());
	sInstance->childSetValue("im_give_disp_rect_txt","Currently set to: "+item->getName());
}
void LLPanelAscent::BuildAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedSettings.setString("AscentBuildPrefs_Item", item->getUUID().asString());
	sInstance->childSetValue("build_item_add_disp_rect_txt","Currently set to: "+item->getName());
}

void LLPanelAscent::apply()
{
	LLTextEditor* im = sInstance->getChild<LLTextEditor>("im_response");
	LLWString im_response;
	if (im) im_response = im->getWText(); 
	LLWStringUtil::replaceTabsWithSpaces(im_response, 4);
	LLWStringUtil::replaceChar(im_response, '\n', '^');
	LLWStringUtil::replaceChar(im_response, ' ', '%');
	gSavedPerAccountSettings.setString("AscentInstantMessageResponse", std::string(wstring_to_utf8str(im_response)));

	gSavedPerAccountSettings.setBOOL("AscentInstantMessageResponseMuted", childGetValue("AscentInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageResponseFriends", childGetValue("AscentInstantMessageResponseFriends").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageResponseMuted", childGetValue("AscentInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageResponseAnyone", childGetValue("AscentInstantMessageResponseAnyone").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageShowResponded", childGetValue("AscentInstantMessageShowResponded").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageShowOnTyping", childGetValue("AscentInstantMessageShowOnTyping").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageResponseRepeat", childGetValue("AscentInstantMessageResponseRepeat").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageResponseItem", childGetValue("AscentInstantMessageResponseItem").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageAnnounceIncoming", childGetValue("AscentInstantMessageAnnounceIncoming").asBoolean());
	gSavedPerAccountSettings.setBOOL("AscentInstantMessageAnnounceStealFocus", childGetValue("AscentInstantMessageAnnounceStealFocus").asBoolean());
	if(((gSavedSettings.getU32("RenderQualityPerformance")>=3) && gSavedSettings.getBOOL("WindLightUseAtmosShaders") && gSavedSettings.getBOOL("VertexShaderEnable")) && childGetValue("AscentShadowsON").asBoolean())
	{
		gSavedSettings.setBOOL("RenderUseFBO", childGetValue("AscentShadowsON").asBoolean());
		gSavedSettings.setBOOL("RenderDeferred", childGetValue("AscentShadowsON").asBoolean());
	}
	else if(!childGetValue("AscentShadowsON").asBoolean())
	{
		if(gSavedSettings.getBOOL("RenderDeferred"))
		{
			gSavedSettings.setBOOL("RenderDeferred", childGetValue("AscentShadowsON").asBoolean());
			gSavedSettings.setBOOL("RenderUseFBO", childGetValue("AscentShadowsON").asBoolean());
		}
	}
	else if(((gSavedSettings.getU32("RenderQualityPerformance")<3) && !gSavedSettings.getBOOL("WindLightUseAtmosShaders") && !gSavedSettings.getBOOL("VertexShaderEnable")) && childGetValue("AscentShadowsON").asBoolean())
	{
		childSetValue("AscentShadowsON",false);
		LLNotifications::instance().add("NoShadows");
		llwarns<<"Attempt to enable shadow rendering while graphics settings not on ultra!"<<llendl;
	}
	gSavedSettings.setBOOL("AscentShadowsToggle", childGetValue("AscentShadowsON").asBoolean());
	gSavedSettings.setU32("AscentUseOTR", (U32)childGetValue("AscentUseOTR").asReal());
}

void LLPanelAscent::cancel()
{
}
void LLPanelAscent::onClickVoiceRevertProd(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;
	gSavedSettings.setString("vivoxProductionServerName", "bhr.vivox.com");
	self->getChild<LLLineEditor>("production_voice_field")->setValue("bhr.vivox.com");
}

void LLPanelAscent::onClickBoobReset(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;
	LLControlVariable *var;

	var = self->findControl("AscentBoobMass");
	self->getChild<LLSliderCtrl>("AscentBoobMass")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("AscentBoobHardness");
	self->getChild<LLSliderCtrl>("AscentBoobHardness")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("AscentBoobVelMax");
	self->getChild<LLSliderCtrl>("AscentBoobVelMax")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("AscentBoobFriction");
	self->getChild<LLSliderCtrl>("AscentBoobFriction")->setValue(var->getDefault());
	var->resetToDefault();

	var = self->findControl("AscentBoobVelMin");
	self->getChild<LLSliderCtrl>("AscentBoobVelMin")->setValue(var->getDefault());
	var->resetToDefault();
}

void LLPanelAscent::onCustomBeam(void* data)
{
	//LLPanelAscent* self =(LLPanelAscent*)data;
	//LggBeamMap::show(true, data);

}
void LLPanelAscent::onKeywordAllertButton(void * data)
{
//	MfdKeywordFloaterStart::show(true,data);
}
void LLPanelAscent::onAutoCorrectButton(void * data)
{
//	lggAutoCorrectFloaterStart::show(TRUE,data);
}
void LLPanelAscent::onCustomBeamColor(void* data)
{
//	LggBeamColorMap::show(true,data);
}
void LLPanelAscent::onSpellAdd(void* data)
{

}
void LLPanelAscent::onSpellRemove(void* data)
{

}
void LLPanelAscent::onSpellGetMore(void* data)
{
}
void LLPanelAscent::onSpellEditCustom(void* data)
{
}

void LLPanelAscent::beamUpdateCall(LLUICtrl* crtl, void* userdata)
{
}
void LLPanelAscent::onTagComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{	
}
void LLPanelAscent::onComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{

	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		gSavedSettings.setString(box->getControlName(),box->getValue().asString());
	}	
}
void LLPanelAscent::onTagsBoxCommit(LLUICtrl* ctrl, void* userdata)
{
	AscentViewerLink::getInstance()->downloadClientTags();
}
void LLPanelAscent::onSpellBaseComboBoxCommit(LLUICtrl* ctrl, void* userdata)
{

	LLComboBox* box = (LLComboBox*)ctrl;
	if(box)
	{
		//glggHunSpell->newDictSelection(box->getValue().asString());
		//LLPanelAscent* panel = (LLPanelAscent*)userdata;//box->getParent();
		if(sInstance)sInstance->refresh();

	}
	//LLPanelAscent* panel = (LLPanelAscent*)userdata;
	//if(panel)panel->refresh();
}
void LLPanelAscent::onTexturePickerCommit(LLUICtrl* ctrl, void* userdata)
{
	LLTextureCtrl*	image_ctrl = (LLTextureCtrl*)ctrl;
	if(image_ctrl)
	{
		gSavedSettings.setString("AscentBuildPrefs_Texture", image_ctrl->getImageAssetID().asString());
	}
}

void LLPanelAscent::onClickOtrHelp(void* data)           // [$PLOTR$]
{
    LLWeb::loadURL("http://www.cypherpunks.ca/otr/");
}                                                         // [/$PLOTR$]

void LLPanelAscent::onRefresh(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;
	self->refresh();
	
	
}
void LLPanelAscent::onClickVoiceRevertDebug(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;
	gSavedSettings.setString("vivoxDebugServerName", "bhd.vivox.com");
	self->getChild<LLLineEditor>("debug_voice_field")->setValue("bhd.vivox.com");

 
 
}
void LLPanelAscent::onBeamDelete(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;
	
	LLComboBox* comboBox = self->getChild<LLComboBox>("AscentBeamShape_combo");

	if(comboBox != NULL) 
	{
		std::string filename = comboBox->getValue().asString()+".xml";
		std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "beams", filename));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beams", filename));
		
		if(gDirUtilp->fileExists(path_name1))
		{
			LLFile::remove(path_name1);
			gSavedSettings.setString("AscentBeamShape","===OFF===");
		}
		if(gDirUtilp->fileExists(path_name2))
		{
			LLFile::remove(path_name2);
			gSavedSettings.setString("AscentBeamShape","===OFF===");
		}
	}
	self->refresh();
}
void LLPanelAscent::onBeamColorDelete(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;

	LLComboBox* comboBox = self->getChild<LLComboBox>("BeamColor_combo");

	if(comboBox != NULL) 
	{
		std::string filename = comboBox->getValue().asString()+".xml";
		std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "beamsColors", filename));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beamsColors", filename));

		if(gDirUtilp->fileExists(path_name1))
		{
			LLFile::remove(path_name1);
			gSavedSettings.setString("AscentBeamColorFile","===OFF===");
		}
		if(gDirUtilp->fileExists(path_name2))
		{
			LLFile::remove(path_name2);
			gSavedSettings.setString("AscentBeamColorFile","===OFF===");
		}
	}
	self->refresh();
}



//workaround for lineeditor dumbness in regards to control_name
void LLPanelAscent::onCommitApplyControl(LLUICtrl* caller, void* user_data)
{
	LLLineEditor* line = (LLLineEditor*)caller;
	if(line)
	{
		LLControlVariable *var = line->findControl(line->getControlName());
		if(var)var->setValue(line->getValue());
	}
}

void LLPanelAscent::onCommitAvatarModifier(LLUICtrl* ctrl, void* userdata)
{
	gSavedPerAccountSettings.setF32("AscentAvatarXModifier", sInstance->getChild<LLSpinCtrl>("X Modifier")->get());
	gSavedPerAccountSettings.setF32("AscentAvatarYModifier", sInstance->getChild<LLSpinCtrl>("Y Modifier")->get());
	gSavedPerAccountSettings.setF32("AscentAvatarZModifier", sInstance->getChild<LLSpinCtrl>("Z Modifier")->get());
	gAgent.sendAgentSetAppearance();
}

void LLPanelAscent::onClickSetMirror(void* user_data)
{
	LLPanelAscent* self = (LLPanelAscent*)user_data;

	std::string cur_name(gSavedSettings.getString("AscentInvMirrorLocation"));
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
		gSavedSettings.setString("AscentInvMirrorLocation", dir_name);
	}
}

void LLPanelAscent::onClickSetHDDInclude(void* user_data)
{
	LLPanelAscent* self = (LLPanelAscent*)user_data;

	std::string cur_name(gSavedSettings.getString("AscentHDDIncludeLocation"));
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
		gSavedSettings.setString("AscentHDDIncludeLocation", dir_name);
	}
}
void LLPanelAscent::onClickSetXed(void* user_data)
{
	LLPanelAscent* self = (LLPanelAscent*)user_data;

	std::string cur_name(gSavedSettings.getString("AscentLSLExternalEditor"));
	
	LLFilePicker& picker = LLFilePicker::instance();
	if (! picker.getOpenFile(LLFilePicker::FFLOAD_APP) )
	{
		return; //Canceled!
	}
	std::string file_name = picker.getFirstFile();
	if (!file_name.empty() && file_name != cur_name)
	{
		self->childSetText("xed_location", file_name);
		gSavedSettings.setString("AscentLSLExternalEditor", file_name);
		
	} else {
		//self->childSetText("xed_location", " ");
	  gSavedSettings.setString("AscentLSLExternalEditor", " ");
	}
}

//Kadah - Force IMShowOnTyping to resolve a bug were if AnnounceIncoming was on, autoreply would not be sent
void LLPanelAscent::onAscentInstantMessageAnnounceIncoming(LLUICtrl* ctrl, void* userdata)
{
	LLPanelAscent* self = (LLPanelAscent*)ctrl->getParent();
	const bool AnnounceIncoming = self->childGetValue("AscentInstantMessageAnnounceIncoming").asBoolean();
	LLCheckBoxCtrl* ShowOnTyping = self->getChild<LLCheckBoxCtrl>("AscentInstantMessageShowOnTyping");
	ShowOnTyping->setEnabled(!AnnounceIncoming);
	if (AnnounceIncoming)
	{
		ShowOnTyping->setValue(true);
	} else {
		ShowOnTyping->setValue(gSavedPerAccountSettings.getBOOL("AscentInstantMessageShowOnTyping")); //restore to last value
	}
}

void LLPanelAscent::onConditionalPreferencesChanged(LLUICtrl* ctrl, void* userdata)
{
	/*LLPanelAscent* self = (LLPanelAscent*)ctrl->getParent();
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
		gSavedSettings.setBOOL("AscentRequestLocalTeleports", true);
		teleport->setEnabled(false);
	}*/
}

/*
//static 
void LLPanelAscent::onClickSilver(void* data)
{
	LLPanelAscent* self = (LLPanelAscent*)data;
	gSavedSettings.setString("SkinCurrent", "silver");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("silver");
}*/
