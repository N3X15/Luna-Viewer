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

// linden library includes
#include "llradiogroup.h"
#include "llbutton.h"
#include "lluictrlfactory.h"

// project includes
#include "llviewercontrol.h"
#include "llviewerwindow.h"


#include "lltabcontainer.h"

#include "llinventorymodel.h"

#include "llstartup.h"

#include "lltexteditor.h"

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
	mObjectDropTarget = NULL;
}

BOOL LLPanelEmerald::postBuild()
{
	//LLRadioGroup* skin_select = getChild<LLRadioGroup>("skin_selection");
	//skin_select->setCommitCallback(onSelectSkin);
	//skin_select->setCallbackUserData(this);
	//onCommitApplyControl
	getChild<LLButton>("revert_production_voice_btn")->setClickedCallback(onClickVoiceRevertProd, this);
	getChild<LLButton>("revert_debug_voice_btn")->setClickedCallback(onClickVoiceRevertDebug, this);

	childSetCommitCallback("production_voice_field", onCommitApplyControl);//onCommitVoiceProductionServerName);
	childSetCommitCallback("debug_voice_field", onCommitApplyControl);//onCommitVoiceDebugServerName);

	childSetCommitCallback("EmeraldCmdLinePos", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineGround", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineHeight", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineTeleportHome", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineRezPlatform", onCommitApplyControl);
	childSetCommitCallback("EmeraldCmdLineMapTo", onCommitApplyControl);

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
	}else
	{
		childSetValue("im_give_disp_rect_txt","Not logged in");
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

	refresh();
	return TRUE;
}

void LLPanelEmerald::refresh()
{
	//mSkin = gSavedSettings.getString("SkinCurrent");
	//getChild<LLRadioGroup>("skin_selection")->setValue(mSkin);
}

void LLPanelEmerald::IMAutoResponseItemDrop(LLViewerInventoryItem* item)
{
	gSavedPerAccountSettings.setString("EmeraldInstantMessageResponseItemData", item->getUUID().asString());
	sInstance->childSetValue("im_give_disp_rect_txt","Currently set to: "+item->getName());
}

void LLPanelEmerald::apply()
{
	LLTextEditor* im = sInstance->getChild<LLTextEditor>("im_response");
	LLWString im_response;
	if (im) im_response = im->getWText(); 
	LLWStringUtil::replaceTabsWithSpaces(im_response, 4);
	LLWStringUtil::replaceChar(im_response, '\n', '^');
	LLWStringUtil::replaceChar(im_response, ' ', '%');
	gSavedPerAccountSettings.setString("EmeraldInstantMessageResponse", std::string(wstring_to_utf8str(im_response)));

	//gSavedPerAccountSettings.setString(
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseFriends", childGetValue("EmeraldInstantMessageResponseFriends").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseMuted", childGetValue("EmeraldInstantMessageResponseMuted").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseAnyone", childGetValue("EmeraldInstantMessageResponseAnyone").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageShowResponded", childGetValue("EmeraldInstantMessageShowResponded").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageShowOnTyping", childGetValue("EmeraldInstantMessageShowOnTyping").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseRepeat", childGetValue("EmeraldInstantMessageResponseRepeat").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageResponseItem", childGetValue("EmeraldInstantMessageResponseItem").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageAnnounceIncoming", childGetValue("EmeraldInstantMessageAnnounceIncoming").asBoolean());
	gSavedPerAccountSettings.setBOOL("EmeraldInstantMessageAnnounceStealFocus", childGetValue("EmeraldInstantMessageAnnounceStealFocus").asBoolean());

}

void LLPanelEmerald::cancel()
{
	// reverts any changes to current skin
	//gSavedSettings.setString("SkinCurrent", mSkin);
}
//static 
void LLPanelEmerald::onClickVoiceRevertProd(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	gSavedSettings.setString("vivoxProductionServerName", "bhr.vivox.com");
	self->getChild<LLLineEditor>("production_voice_field")->setValue("bhr.vivox.com");
}

void LLPanelEmerald::onClickVoiceRevertDebug(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	gSavedSettings.setString("vivoxDebugServerName", "bhd.vivox.com");
	self->getChild<LLLineEditor>("debug_voice_field")->setValue("bhd.vivox.com");
}
/*
//LGGs avatar effects crap
void LLPanelEmerald::onCommitAvatarEffectsChange(LLUICtrl* ctrl, void* user_data)
{
	//std::string control_name = ctrl->getControlName();
	BOOL newValue = ctrl->getValue().asBoolean();
	std::string setting_name =ctrl->getName();
	gSavedSettings.setBOOL(setting_name, newValue);
	//if(control_name == "EmeraldAlwaysFly")
	//{
	//	gSavedSettings.setBOOL("allow_phantom_toggle", newValue);
	//}
	//if (control_name == "EmeraldAllowPhantomToggle")
	//{
	//		gSavedSettings.setBOOL("allow_phantom_toggle", FALSE);
	//}EmeraldAlwaysFly
}*/ //jcool410 - *burns* see comment in postbuild
//lgg - Cool, it works magicly instead, even better :) 

//void LLPanelEmerald::onCommitVoiceProductionServerName(LLUICtrl* caller, void* user_data)
//{
//	gSavedSettings.setString("vivoxProductionServerName", (std::string)(((LLLineEditor*)caller)->getValue()));
///	std::string cntrl = ((LLLineEditor*)caller)->getControlName()
//}

//void LLPanelEmerald::onCommitVoiceDebugServerName(LLUICtrl* caller, void* user_data)
//{
//	gSavedSettings.setString("vivoxDebugServerName", (std::string)(((LLLineEditor*)caller)->getValue()));
//}

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


/*
//static 
void LLPanelEmerald::onClickSilver(void* data)
{
	LLPanelEmerald* self = (LLPanelEmerald*)data;
	gSavedSettings.setString("SkinCurrent", "silver");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("silver");
}*/
