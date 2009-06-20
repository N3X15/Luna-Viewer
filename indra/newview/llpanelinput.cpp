/** 
 * @file llpanelinput.cpp
 * @brief Input preferences panel
 *
 * $LicenseInfo:firstyear=2004&license=viewergpl$
 * 
 * Copyright (c) 2004-2008, Linden Research, Inc.
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

#include "llpanelinput.h"

// linden library includes
#include "llerror.h"
#include "llrect.h"
#include "llfontgl.h"
#include "message.h"
#include "lluictrlfactory.h"

// project includes
#include "llviewerwindow.h"
#include "llcheckboxctrl.h"
#include "llfloaterjoystick.h"
#include "llradiogroup.h"
#include "llresmgr.h"
#include "llspinctrl.h"
#include "llslider.h"
#include "llsliderctrl.h"
#include "lltextbox.h"
#include "llui.h"
#include "llappviewer.h"
#include "llviewercontrol.h"

//Ventrella
#include "llagent.h"
//end Ventrella


//
// Imported globals
//

//
// Globals
//

//
// Static functions
//


LLPanelInput::LLPanelInput() 
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_input.xml");
}

BOOL LLPanelInput::postBuild()
{
	childSetAction("joystic_setup_button", onClickJoystickSetup, (void*)this);
	
	refresh();

	return TRUE;
}


LLPanelInput::~LLPanelInput()
{
	// Children all cleaned up by default view destructor.
}

void LLPanelInput::refresh()
{
	LLPanel::refresh();

	mMouseSensitivityVal = gSavedSettings.getF32("MouseSensitivity");
	mAutomaticFly =gSavedSettings.getBOOL("AutomaticFly");
	mInvertMouse = gSavedSettings.getBOOL("InvertMouse");
	mEditCameraMovement = gSavedSettings.getBOOL("EditCameraMovement");
	mAppearanceCameraMovement = gSavedSettings.getBOOL("AppearanceCameraMovement");
	mDynamicCameraStrengthVal = gSavedSettings.getF32("DynamicCameraStrength");
	mNumpadControlVal = gSavedSettings.getS32("NumpadControl");
	
	// First Person Visibility
	mFirstPersonAvatarVisible = gSavedSettings.getBOOL("FirstPersonAvatarVisible");
}

void LLPanelInput::apply()
{

}

void LLPanelInput::cancel()
{
	gSavedSettings.setF32("MouseSensitivity", mMouseSensitivityVal);
	gSavedSettings.setBOOL("AutomaticFly", mAutomaticFly);
	gSavedSettings.setBOOL("InvertMouse", mInvertMouse);
	gSavedSettings.setBOOL("EditCameraMovement", mEditCameraMovement);
	gSavedSettings.setBOOL("AppearanceCameraMovement", mAppearanceCameraMovement);
	gSavedSettings.setF32("DynamicCameraStrength", mDynamicCameraStrengthVal);
	gSavedSettings.setS32("NumpadControl", mNumpadControlVal);
	gSavedSettings.setBOOL("FirstPersonAvatarVisible", mFirstPersonAvatarVisible);
}

//static
void LLPanelInput::onClickJoystickSetup(void* user_data)
{
	LLPanelInput* prefs = (LLPanelInput*)user_data;
	LLFloaterJoystick* floaterp = LLFloaterJoystick::showInstance();
	LLFloater* parent_floater = gFloaterView->getParentFloater(prefs);
	if (parent_floater)
	{
		parent_floater->addDependentFloater(floaterp, FALSE);
	}
}

