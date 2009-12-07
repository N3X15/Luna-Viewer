/** 
 * @file llpanelskins.cpp
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
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
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

#include "llpanelskins.h"

// linden library includes
#include "llradiogroup.h"
#include "llbutton.h"
#include "lluictrlfactory.h"

// project includes
#include "llviewercontrol.h"
#include "llviewerwindow.h"

LLPanelSkins::LLPanelSkins()
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_skins.xml");
}

LLPanelSkins::~LLPanelSkins()
{
}

BOOL LLPanelSkins::postBuild()
{
	LLRadioGroup* skin_select = getChild<LLRadioGroup>("skin_selection");
	skin_select->setCommitCallback(onSelectSkin);
	skin_select->setCallbackUserData(this);

	getChild<LLButton>("classic_preview")->setClickedCallback(onClickClassic, this);
	getChild<LLButton>("silver_preview")->setClickedCallback(onClickSilver, this);
	getChild<LLButton>("emerald_preview")->setClickedCallback(onClickEmerald, this);
	getChild<LLButton>("ruby_preview")->setClickedCallback(onClickRuby, this);
	getChild<LLButton>("dark_preview")->setClickedCallback(onClickDark, this);
	getChild<LLButton>("pslpurple_preview")->setClickedCallback(onClickPslpurple, this);
	getChild<LLButton>("pslgreen_preview")->setClickedCallback(onClickPslgreen, this);
	getChild<LLButton>("gred_preview")->setClickedCallback(onClickGred, this);
	getChild<LLButton>("sapphire_preview")->setClickedCallback(onClickSapphire, this);
	getChild<LLButton>("white_emerald_preview")->setClickedCallback(onClickWhite_emerald, this);
	getChild<LLButton>("darkgred_preview")->setClickedCallback(onClickDarkgred, this);
	getChild<LLButton>("darkorange_preview")->setClickedCallback(onClickDarkorange, this);
	getChild<LLButton>("gemini_preview")->setClickedCallback(onClickGemini, this);
	getChild<LLButton>("Pony-Purple_preview")->setClickedCallback(onClickPony_Purple, this);
	getChild<LLButton>("Pony-Aquablue_preview")->setClickedCallback(onClickPony_Aquablue, this);

	refresh();
	return TRUE;
}

void LLPanelSkins::refresh()
{
	mSkin = gSavedSettings.getString("SkinCurrent");
	getChild<LLRadioGroup>("skin_selection")->setValue(mSkin);
}

void LLPanelSkins::apply()
{
	if (mSkin != gSavedSettings.getString("SkinCurrent"))
	{
		  LLNotifications::instance().add("ChangeSkin");
		  refresh();
	}
}

void LLPanelSkins::cancel()
{
	// reverts any changes to current skin
	gSavedSettings.setString("SkinCurrent", mSkin);
}

//static
void LLPanelSkins::onSelectSkin(LLUICtrl* ctrl, void* data)
{
	std::string skin_selection = ctrl->getValue().asString();
	gSavedSettings.setString("SkinCurrent", skin_selection);
}

//static 
void LLPanelSkins::onClickClassic(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "default");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("default");
}

//static 
void LLPanelSkins::onClickSilver(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "silver");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("silver");
}

//static 
void LLPanelSkins::onClickEmerald(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "emerald");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("emerald");
}

//static 
void LLPanelSkins::onClickRuby(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "ruby");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("ruby");
}
//static 
void LLPanelSkins::onClickDark(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "dark");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("dark");
}

//static 
void LLPanelSkins::onClickPslpurple(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "pslpurple");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("pslpurple");
}
//static 
void LLPanelSkins::onClickPslgreen(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "pslgreen");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("pslgreen");
}

//static 
void LLPanelSkins::onClickGred(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "gred");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("gred");
}
//static 
void LLPanelSkins::onClickSapphire(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "sapphire");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("sapphire");
}

//static 
void LLPanelSkins::onClickWhite_emerald(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "white_emerald");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("white_emerald");
}
//static 
void LLPanelSkins::onClickDarkgred(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "darkgred");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("darkgred");
}

//static 
void LLPanelSkins::onClickDarkorange(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "darkorange");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("darkorange");
}
//static 
void LLPanelSkins::onClickGemini(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "gemini");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("gemini");
}

//static 
void LLPanelSkins::onClickPony_Purple(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "Pony-Purple");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("Pony-Purple");
}

//static 
void LLPanelSkins::onClickPony_Aquablue(void* data)
{
	LLPanelSkins* self = (LLPanelSkins*)data;
	gSavedSettings.setString("SkinCurrent", "Pony-Aquablue");
	self->getChild<LLRadioGroup>("skin_selection")->setValue("Pony-Aquablue");
}
