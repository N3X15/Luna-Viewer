/** 
 * @file llpanelgeneral.h
 * @author ppl
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

#ifndef LL_LLPanelEmerald_H
#define LL_LLPanelEmerald_H

#include "llpanel.h"


#include "llviewerinventory.h"

class JCInvDropTarget;

class LLPanelEmerald : public LLPanel
{
public:
	LLPanelEmerald();
	virtual ~LLPanelEmerald();

	virtual BOOL postBuild();
	void refresh();
	void apply();
	void cancel();

private:
	//static void onSelectSkin(LLUICtrl* ctrl, void* data);
	//static void onClickClassic(void* data);
	//static void onClickSilver(void* data);
	static void onClickVoiceRevertProd(void* data);
	static void onClickVoiceRevertDebug(void* data);
	static void onCommitApplyControl(LLUICtrl* caller, void* user_data);
	static void onCommitSendAppearance(LLUICtrl* ctrl, void* userdata);	
	static void onTexturePickerCommit(LLUICtrl* ctrl, void* userdata);
	static void onComboBoxCommit(LLUICtrl* ctrl, void* userdata);
	//static void onCommitVoiceDebugServerName(LLUICtrl* caller, void* user_data);
	//static void onCommitAvatarEffectsChange(LLUICtrl* caller, void* user_data);
	//static void onCommitAutoResponse(LLUICtrl* caller, void* user_data);

private:
	std::string mSkin;
	static LLPanelEmerald* sInstance;
	static JCInvDropTarget* mObjectDropTarget;
	static void IMAutoResponseItemDrop(LLViewerInventoryItem* item);
};

#endif // LL_LLPanelEmerald_H
