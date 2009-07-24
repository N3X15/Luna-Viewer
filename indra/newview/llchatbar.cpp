/** 
 * @file llchatbar.cpp
 * @brief LLChatBar class implementation
 *
 * $LicenseInfo:firstyear=2002&license=viewergpl$
 * 
 * Copyright (c) 2002-2009, Linden Research, Inc.
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

#include "llchatbar.h"

#include "imageids.h"
#include "llfontgl.h"
#include "llrect.h"
#include "llerror.h"
#include "llparcel.h"
#include "llstring.h"
#include "message.h"
#include "llfocusmgr.h"

#include "llagent.h"
#include "llbutton.h"
#include "llcombobox.h"
#include "llcommandhandler.h"	// secondlife:///app/chat/ support
#include "llviewercontrol.h"
#include "llfloaterchat.h"
#include "llgesturemgr.h"
#include "llkeyboard.h"
#include "lllineeditor.h"
#include "llstatusbar.h"
#include "llspinctrl.h"
#include "lltextbox.h"
#include "lluiconstants.h"
#include "llviewergesture.h"			// for triggering gestures
#include "llviewermenu.h"		// for deleting object with DEL key
#include "llviewerstats.h"
#include "llviewerwindow.h"
#include "llframetimer.h"
#include "llresmgr.h"
#include "llworld.h"
#include "llinventorymodel.h"
#include "llmultigesture.h"
#include "llui.h"
#include "llviewermenu.h"
#include "lluictrlfactory.h"
#include "chatbar_as_cmdline.h"


//
// Globals
//
const F32 AGENT_TYPING_TIMEOUT = 5.f;	// seconds

LLChatBar *gChatBar = NULL;

// legacy calllback glue
void toggleChatHistory(void* user_data);
void toggleChanSelect(void* user_data);
//void send_chat_from_viewer(const std::string& utf8_out_text, EChatType type, S32 channel);
// [RLVa:KB] - Alternate: Emerald-370 | Checked: 2009-07-07 (RLVa-1.0.0d) | Modified: RLVa-0.2.2a
void send_chat_from_viewer(std::string utf8_out_text, EChatType type, S32 channel);
// [/RLVa:KB]


class LLChatBarGestureObserver : public LLGestureManagerObserver
{
public:
	LLChatBarGestureObserver(LLChatBar* chat_barp) : mChatBar(chat_barp){}
	virtual ~LLChatBarGestureObserver() {}
	virtual void changed() { mChatBar->refreshGestures(); }
private:
	LLChatBar* mChatBar;
};


//
// Functions
//

LLChatBar::LLChatBar() 
:	LLPanel(LLStringUtil::null, LLRect(), BORDER_NO),
	mInputEditor(NULL),
	mGestureLabelTimer(),
	mLastSpecialChatChannel(0),
	mIsBuilt(FALSE),
	mChanSelectorExpanded(FALSE),
	mGestureCombo(NULL),
	mObserver(NULL)
{
	setIsChrome(TRUE);
	
	#if !LL_RELEASE_FOR_DOWNLOAD
	childDisplayNotFound();
#endif
}


LLChatBar::~LLChatBar()
{
	gGestureManager.removeObserver(mObserver);
	delete mObserver;
	mObserver = NULL;
	// LLView destructor cleans up children
}

BOOL LLChatBar::postBuild()
{
	childSetAction("History", toggleChatHistory, this);
	//lgg
	childSetAction("Expand",this->toggleChanSelect,this);
	childSetCommitCallback("Say", onClickSay, this);

	// attempt to bind to an existing combo box named gesture
	setGestureCombo(getChild<LLComboBox>( "Gesture"));

	LLButton * sayp = getChild<LLButton>("Say");
	if(sayp)
	{
		setDefaultBtn(sayp);
	}

	mInputEditor = getChild<LLLineEditor>("Chat Editor");
	if (mInputEditor)
	{
		mInputEditor->setCallbackUserData(this);
		mInputEditor->setKeystrokeCallback(&onInputEditorKeystroke);
		mInputEditor->setFocusLostCallback(&onInputEditorFocusLost, this);
		mInputEditor->setFocusReceivedCallback( &onInputEditorGainFocus, this );
		mInputEditor->setCommitOnFocusLost( FALSE );
		mInputEditor->setRevertOnEsc( FALSE );
		mInputEditor->setIgnoreTab(TRUE);
		mInputEditor->setPassDelete(TRUE);
		mInputEditor->setReplaceNewlinesWithSpaces(FALSE);

		mInputEditor->setMaxTextLength(S32_MAX);
		mInputEditor->setEnableLineHistory(TRUE);
	}

	mIsBuilt = TRUE;

	return TRUE;
}

//-----------------------------------------------------------------------
// Overrides
//-----------------------------------------------------------------------

// virtual
BOOL LLChatBar::handleKeyHere( KEY key, MASK mask )
{
	BOOL handled = FALSE;

	// ALT-RETURN is reserved for windowed/fullscreen toggle

	// not any more :p


	if( KEY_RETURN == key )
	{
		if (mask == MASK_CONTROL)
		{
			// shout
			sendChat(CHAT_TYPE_SHOUT);
			handled = TRUE;
		}
		else if (mask == MASK_SHIFT)
		{
			// whisper
			sendChat( CHAT_TYPE_WHISPER );
			handled = TRUE;
		}
		else if (mask == MASK_ALT)
		{
			// ooc chat

			if (mInputEditor)
			{
				std::string msg;
				std::string text = mInputEditor->getText();
				msg.assign( gSavedSettings.getString("EmeraldOOCPrefix") + " " + text + " " + gSavedSettings.getString("EmeraldOOCPostfix") );
				mInputEditor->setText(msg);
			}

			sendChat( CHAT_TYPE_NORMAL );
			handled = TRUE;
		}
		else if (mask == MASK_NONE)
		{
			// say
			sendChat( CHAT_TYPE_NORMAL );
			handled = TRUE;
		}
		else if (mask == (MASK_ALT | MASK_SHIFT | MASK_CONTROL))
		{
			//Insert new line symbol after the current cursor pos, then increment the curser by 1.
			if (mInputEditor)
			{
				std::string msg = mInputEditor->getText();
				if (mInputEditor->getCursor() > 0)
				{
					if (msg[mInputEditor->getCursor() - 1] != '\n')
					{
						//For some reason you have to use a newline character, the ¶ wont show up in chat.
						msg = msg.insert(mInputEditor->getCursor(), "\n");
						mInputEditor->setText(msg);
						mInputEditor->setCursor(mInputEditor->getCursor() + 1);
					}
				}
			}
		}
	}
	// only do this in main chatbar
	else if ( KEY_ESCAPE == key && gChatBar == this)
	{
		stopChat();

		handled = TRUE;
	}

	return handled;
}

void LLChatBar::refresh()
{
	// HACK: Leave the name of the gesture in place for a few seconds.
	const F32 SHOW_GESTURE_NAME_TIME = 2.f;
	if (mGestureLabelTimer.getStarted() && mGestureLabelTimer.getElapsedTimeF32() > SHOW_GESTURE_NAME_TIME)
	{
		LLCtrlListInterface* gestures = mGestureCombo ? mGestureCombo->getListInterface() : NULL;
		if (gestures) gestures->selectFirstItem();
		mGestureLabelTimer.stop();
	}

	if ((gAgent.getTypingTime() > AGENT_TYPING_TIMEOUT) && (gAgent.getRenderState() & AGENT_STATE_TYPING))
	{
		gAgent.stopTyping();
	}

	childSetValue("History", LLFloaterChat::instanceVisible(LLSD()));

	childSetValue("ChatChannel",( 1.f * ((S32)(getChild<LLSpinCtrl>("ChatChannel")->get()))) );
	childSetEnabled("Say", mInputEditor->getText().size() > 0);
	childSetEnabled("Shout", mInputEditor->getText().size() > 0);

}

void LLChatBar::refreshGestures()
{
	if (mGestureCombo)
	{
		//store current selection so we can maintain it
		std::string cur_gesture = mGestureCombo->getValue().asString();
		mGestureCombo->selectFirstItem();
		std::string label = mGestureCombo->getValue().asString();;
		// clear
		mGestureCombo->clearRows();

		// collect list of unique gestures
		std::map <std::string, BOOL> unique;
		LLGestureManager::item_map_t::iterator it;
		for (it = gGestureManager.mActive.begin(); it != gGestureManager.mActive.end(); ++it)
		{
			LLMultiGesture* gesture = (*it).second;
			if (gesture)
			{
				if (!gesture->mTrigger.empty())
				{
					unique[gesture->mTrigger] = TRUE;
				}
			}
		}

		// add unique gestures
		std::map <std::string, BOOL>::iterator it2;
		for (it2 = unique.begin(); it2 != unique.end(); ++it2)
		{
			mGestureCombo->addSimpleElement((*it2).first);
		}
		
		mGestureCombo->sortByName();
		// Insert label after sorting, at top, with separator below it
		mGestureCombo->addSeparator(ADD_TOP);
		mGestureCombo->addSimpleElement(getString("gesture_label"), ADD_TOP);
		
		if (!cur_gesture.empty())
		{ 
			mGestureCombo->selectByValue(LLSD(cur_gesture));
		}
		else
		{
			mGestureCombo->selectFirstItem();
		}
	}
}

// Move the cursor to the correct input field.
void LLChatBar::setKeyboardFocus(BOOL focus)
{
	if (focus)
	{
		if (mInputEditor)
		{
			mInputEditor->setFocus(TRUE);
			mInputEditor->selectAll();
		}
	}
	else if (gFocusMgr.childHasKeyboardFocus(this))
	{
		if (mInputEditor)
		{
			mInputEditor->deselect();
		}
		setFocus(FALSE);
	}
}


// Ignore arrow keys in chat bar
void LLChatBar::setIgnoreArrowKeys(BOOL b)
{
	if (mInputEditor)
	{
		mInputEditor->setIgnoreArrowKeys(b);
	}
}

BOOL LLChatBar::inputEditorHasFocus()
{
	return mInputEditor && mInputEditor->hasFocus();
}

std::string LLChatBar::getCurrentChat()
{
	return mInputEditor ? mInputEditor->getText() : LLStringUtil::null;
}

void LLChatBar::setGestureCombo(LLComboBox* combo)
{
	mGestureCombo = combo;
	if (mGestureCombo)
	{
		mGestureCombo->setCommitCallback(onCommitGesture);
		mGestureCombo->setCallbackUserData(this);

		// now register observer since we have a place to put the results
		mObserver = new LLChatBarGestureObserver(this);
		gGestureManager.addObserver(mObserver);

		// refresh list from current active gestures
		refreshGestures();
	}
}

//-----------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------

// If input of the form "/20foo" or "/20 foo", returns "foo" and channel 20.
// Otherwise returns input and channel 0.
LLWString LLChatBar::stripChannelNumber(const LLWString &mesg, S32* channel)
{
	if (mesg[0] == '/'
		&& mesg[1] == '/')
	{
		// This is a "repeat channel send"
		*channel = mLastSpecialChatChannel;
		return mesg.substr(2, mesg.length() - 2);
	}
	else if (mesg[0] == '/'
			 && mesg[1]
			 && LLStringOps::isDigit(mesg[1]))
	{
		// This a special "/20" speak on a channel
		S32 pos = 0;

		// Copy the channel number into a string
		LLWString channel_string;
		llwchar c;
		do
		{
			c = mesg[pos+1];
			channel_string.push_back(c);
			pos++;
		}
		while(c && pos < 64 && LLStringOps::isDigit(c));
		
		// Move the pointer forward to the first non-whitespace char
		// Check isspace before looping, so we can handle "/33foo"
		// as well as "/33 foo"
		while(c && iswspace(c))
		{
			c = mesg[pos+1];
			pos++;
		}
		
		mLastSpecialChatChannel = strtol(wstring_to_utf8str(channel_string).c_str(), NULL, 10);
		*channel = mLastSpecialChatChannel;
		return mesg.substr(pos, mesg.length() - pos);
	}
	else
	{
		// This is normal chat.
		//NO WHAT ARE YOU DOING!!! :(*channel = 0;
		return mesg;
	}
}


void LLChatBar::sendChat( EChatType type )
{
	if (mInputEditor)
	{
		LLWString text = mInputEditor->getConvertedText();
		if (!text.empty())
		{
			//TODO: chat as commandline?
			if(mInputEditor->getText()=="/luareload")
			{
				FLLua::init();
				childSetValue("Chat Editor", LLStringUtil::null);
				return;
			}
			if(FLLua::isMacro(mInputEditor->getText()))
				FLLua::getInstance()->RunMacro(mInputEditor->getText());

			// store sent line in history, duplicates will get filtered
			if (mInputEditor) mInputEditor->updateHistory();
			// Check if this is destined for another channel
			//greg changed channel here
			//F32 readChan= getChild<LLSpinCtrl>("ChatChannel")->get();
			//S32 undoneChan = (S32)readChan;
			//S32 channel = undoneChan;
			S32 channel = (S32)(getChild<LLSpinCtrl>("ChatChannel")->get());
			stripChannelNumber(text, &channel);
			
			std::string utf8text = wstring_to_utf8str(text);//+" and read is "+llformat("%f",readChan)+" and undone is "+llformat("%d",undoneChan)+" but actualy channel is "+llformat("%d",channel);
			// Try to trigger a gesture, if not chat to a script.
			std::string utf8_revised_text;
			if (0 == channel)
			{
				// discard returned "found" boolean
				gGestureManager.triggerAndReviseString(utf8text, &utf8_revised_text);
			}
			else
			{
				utf8_revised_text = utf8text;
			}

			utf8_revised_text = utf8str_trim(utf8_revised_text);

			if (!utf8_revised_text.empty() && cmd_line_chat(utf8_revised_text, type))
			{
				// Chat with animation
				sendChatFromViewer(utf8_revised_text, type, TRUE);
			}
		}
	}

	childSetValue("Chat Editor", LLStringUtil::null);

	gAgent.stopTyping();

	// If the user wants to stop chatting on hitting return, lose focus
	// and go out of chat mode.
	if (gChatBar == this && gSavedSettings.getBOOL("CloseChatOnReturn"))
	{
		stopChat();
	}
}


//-----------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------

// static 
void LLChatBar::startChat(const char* line)
{
	if (gSavedSettings.getBOOL("EmeraldUseChatBar"))
	{
		gChatBar->setVisible(TRUE);
		gChatBar->setKeyboardFocus(TRUE);
		gSavedSettings.setBOOL("ChatVisible", TRUE);

		if (line && gChatBar->mInputEditor)
		{
			std::string line_string(line);
			gChatBar->mInputEditor->setText(line_string);
		}
		// always move cursor to end so users don't obliterate chat when accidentally hitting WASD
		gChatBar->mInputEditor->setCursorToEnd();
	}
}


// Exit "chat mode" and do the appropriate focus changes
// static
void LLChatBar::stopChat()
{
	// In simple UI mode, we never release focus from the chat bar
	gChatBar->setKeyboardFocus(FALSE);

	// If we typed a movement key and pressed return during the
	// same frame, the keyboard handlers will see the key as having
	// gone down this frame and try to move the avatar.
	gKeyboard->resetKeys();
	gKeyboard->resetMaskKeys();

	// stop typing animation
	gAgent.stopTyping();

	// hide chat bar so it doesn't grab focus back
	gChatBar->setVisible(FALSE);
	gSavedSettings.setBOOL("ChatVisible", FALSE);
}

// static
void LLChatBar::onInputEditorKeystroke( LLLineEditor* caller, void* userdata )
{
	LLChatBar* self = (LLChatBar *)userdata;

	LLWString raw_text;
	if (self->mInputEditor) raw_text = self->mInputEditor->getWText();

	// Can't trim the end, because that will cause autocompletion
	// to eat trailing spaces that might be part of a gesture.
	LLWStringUtil::trimHead(raw_text);

	S32 length = raw_text.length();

	//if( (length > 0) && (raw_text[0] != '/') )  // forward slash is used for escape (eg. emote) sequences
// [RLVa:KB] - Checked: 2009-06-07 (RLVa-0.2.1c)
	if ( (length > 0) && (raw_text[0] != '/') && (!gRlvHandler.hasBehaviour(RLV_BHVR_REDIRCHAT)) )
// [/RLVa:KB]
	{
		gAgent.startTyping();
	}
	else
	{
		gAgent.stopTyping();
	}

	/* Doesn't work -- can't tell the difference between a backspace
	   that killed the selection vs. backspace at the end of line.
	if (length > 1 
		&& text[0] == '/'
		&& key == KEY_BACKSPACE)
	{
		// the selection will already be deleted, but we need to trim
		// off the character before
		std::string new_text = raw_text.substr(0, length-1);
		self->mInputEditor->setText( new_text );
		self->mInputEditor->setCursorToEnd();
		length = length - 1;
	}
	*/

	KEY key = gKeyboard->currentKey();

	// Ignore "special" keys, like backspace, arrows, etc.
	if (length > 1 
		&& raw_text[0] == '/'
		&& key < KEY_SPECIAL)
	{
		// we're starting a gesture, attempt to autocomplete

		std::string utf8_trigger = wstring_to_utf8str(raw_text);
		std::string utf8_out_str(utf8_trigger);

		if (gGestureManager.matchPrefix(utf8_trigger, &utf8_out_str))
		{
			if (self->mInputEditor)
			{
				std::string rest_of_match = utf8_out_str.substr(utf8_trigger.size());
				self->mInputEditor->setText(utf8_trigger + rest_of_match); // keep original capitalization for user-entered part
				S32 outlength = self->mInputEditor->getLength(); // in characters
			
				// Select to end of line, starting from the character
				// after the last one the user typed.
				self->mInputEditor->setSelection(length, outlength);
			}
		}

		//llinfos << "GESTUREDEBUG " << trigger 
		//	<< " len " << length
		//	<< " outlen " << out_str.getLength()
		//	<< llendl;
	}
}

// static
void LLChatBar::onInputEditorFocusLost( LLFocusableElement* caller, void* userdata)
{
	// stop typing animation
	gAgent.stopTyping();
}

// static
void LLChatBar::onInputEditorGainFocus( LLFocusableElement* caller, void* userdata )
{
	LLFloaterChat::setHistoryCursorAndScrollToEnd();
}

// static
void LLChatBar::onClickSay( LLUICtrl* ctrl, void* userdata )
{
	e_chat_type chat_type = CHAT_TYPE_NORMAL;
	if (ctrl->getValue().asString() == "shout")
	{
		chat_type = CHAT_TYPE_SHOUT;
	}
	else if (ctrl->getValue().asString() == "whisper")
	{
		chat_type = CHAT_TYPE_WHISPER;
	}
	LLChatBar* self = (LLChatBar*) userdata;
	self->sendChat(chat_type);
}

void LLChatBar::sendChatFromViewer(const std::string &utf8text, EChatType type, BOOL animate)
{
	sendChatFromViewer(utf8str_to_wstring(utf8text), type, animate);
}

void LLChatBar::sendChatFromViewer(const LLWString &wtext, EChatType type, BOOL animate)
{
	// Look for "/20 foo" channel chats.
	S32 channel = (S32)(getChild<LLSpinCtrl>("ChatChannel")->get());
			
	//todo 
	LLWString out_text = stripChannelNumber(wtext, &channel);
	std::string utf8_out_text = wstring_to_utf8str(out_text);
	std::string utf8_text = wstring_to_utf8str(wtext)+" and chan is "+llformat("%d",channel);

	utf8_text = utf8str_trim(utf8_text);
	if (!utf8_text.empty())
	{
		utf8_text = utf8str_truncate(utf8_text, MAX_STRING - 1);
	}

// [RLVa:KB] - Checked: 2009-06-07 (RLVa-0.2.1c) | Modified: RLVa-0.2.0b
	if ( (0 == channel) && (rlv_handler_t::isEnabled()) )
	{
		// Adjust the (public) chat "volume" on chat and gestures (also takes care of playing the proper animation)
		if ( ((CHAT_TYPE_SHOUT == type) || (CHAT_TYPE_NORMAL == type)) && (gRlvHandler.hasBehaviour("chatnormal")) )
			type = CHAT_TYPE_WHISPER;
		else if ( (CHAT_TYPE_SHOUT == type) && (gRlvHandler.hasBehaviour("chatshout")) )
			type = CHAT_TYPE_NORMAL;
		else if ( (CHAT_TYPE_WHISPER == type) && (gRlvHandler.hasBehaviour("chatwhisper")) )
			type = CHAT_TYPE_NORMAL;

		animate &= !gRlvHandler.hasBehaviour(RLV_BHVR_REDIRCHAT);
	}
// [/RLVa:KB]

	// Don't animate for chats people can't hear (chat to scripts)
	if (animate && (channel == 0))
	{
		if (type == CHAT_TYPE_WHISPER)
		{
			lldebugs << "You whisper " << utf8_text << llendl;
			gAgent.sendAnimationRequest(ANIM_AGENT_WHISPER, ANIM_REQUEST_START);
		}
		else if (type == CHAT_TYPE_NORMAL)
		{
			lldebugs << "You say " << utf8_text << llendl;
			gAgent.sendAnimationRequest(ANIM_AGENT_TALK, ANIM_REQUEST_START);
		}
		else if (type == CHAT_TYPE_SHOUT)
		{
			lldebugs << "You shout " << utf8_text << llendl;
			gAgent.sendAnimationRequest(ANIM_AGENT_SHOUT, ANIM_REQUEST_START);
		}
		else
		{
			llinfos << "send_chat_from_viewer() - invalid volume" << llendl;
			return;
		}
	}
	else
	{
		if (type != CHAT_TYPE_START && type != CHAT_TYPE_STOP)
		{
			lldebugs << "Channel chat: " << utf8_text << llendl;
		}
	}

	send_chat_from_viewer(utf8_out_text, type, channel);
}

// void send_chat_from_viewer(const std::string& utf8_out_text, EChatType type, S32 channel)
// [RLVa:KB] - Alternate: Emerald-206
void send_chat_from_viewer(std::string utf8_out_text, EChatType type, S32 channel)
// [/RLVa:KB]
{
// [RLVa:KB] - Alternate: Emerald-206 | Checked: 2009-07-02 (RLVa-0.2.2a) | Modified: RLVa-0.2.2a
	// Only process chat messages (ie not CHAT_TYPE_START, CHAT_TYPE_STOP, etc)
	if ( (rlv_handler_t::isEnabled()) && (!gRlvHandler.isReplyInProgress()) &&
		 ( (CHAT_TYPE_WHISPER == type) || (CHAT_TYPE_NORMAL == type) || (CHAT_TYPE_SHOUT == type) ) )
	{
		if (0 == channel)
		{
			// (We already did this before, but LLChatHandler::handle() calls this directly)
			if ( ((CHAT_TYPE_SHOUT == type) || (CHAT_TYPE_NORMAL == type)) && (gRlvHandler.hasBehaviour("chatnormal")) )
				type = CHAT_TYPE_WHISPER;
			else if ( (CHAT_TYPE_SHOUT == type) && (gRlvHandler.hasBehaviour("chatshout")) )
				type = CHAT_TYPE_NORMAL;
			else if ( (CHAT_TYPE_WHISPER == type) && (gRlvHandler.hasBehaviour("chatwhisper")) )
				type = CHAT_TYPE_NORMAL;

			// Redirect chat if needed
			if ( ( (gRlvHandler.hasBehaviour(RLV_BHVR_REDIRCHAT) || (gRlvHandler.hasBehaviour(RLV_BHVR_REDIREMOTE)) ) && 
				 (gRlvHandler.redirectChatOrEmote(utf8_out_text)) ) )
			{
				return;
			}

			// Filter public chat if sendchat restricted (and filter anything that redirchat didn't redirect)
			if ( (gRlvHandler.hasBehaviour("sendchat")) || (gRlvHandler.hasBehaviour(RLV_BHVR_REDIRCHAT)) )
				gRlvHandler.filterChat(utf8_out_text);
		}
		else
		{
			// Don't allow chat on a non-public channel if sendchannel restricted (unless the channel is an exception)
			if ( (gRlvHandler.hasBehaviour("sendchannel")) && (!gRlvHandler.hasBehaviour("sendchannel", llformat("%d", channel))) )
				return;

			// Don't allow chatting on the debug channel if @sendchat or @redirchat restricted (shows as public chat on viewers)
			if ( (channel >= CHAT_CHANNEL_DEBUG) && 
				 ((gRlvHandler.hasBehaviour("sendchat")) || (gRlvHandler.hasBehaviour(RLV_BHVR_REDIRCHAT))) )
			{
				return;
			}
		}
	}
// [/RLVa:KB]

	// same code like in llimpanel.cpp
	U32 split = MAX_MSG_BUF_SIZE - 1;
	U32 pos = 0;
	U32 total = utf8_out_text.length();
	while(pos < total)
	{
		U32 next_split = split;

		if(pos + next_split > total) next_split = total - pos;

		// don't split utf-8 bytes
		while(U8(utf8_out_text[pos + next_split]) >= 0x80 && U8(utf8_out_text[pos + next_split]) < 0xC0
			&& next_split > 0)
		{
			--next_split;
		}

		if(next_split == 0)
		{
			next_split = split;
			LL_WARNS("Splitting") << "utf-8 couldn't be split correctly" << LL_ENDL;
		}

		std::string send = utf8_out_text.substr(pos, pos + next_split);
		pos += next_split;

		// *FIXME: Queue messages and wait for server
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ChatFromViewer);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ChatData);
		msg->addStringFast(_PREHASH_Message, send);
	msg->addU8Fast(_PREHASH_Type, type);
	msg->addS32("Channel", channel);

	gAgent.sendReliableMessage();

	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CHAT_COUNT);
	}
}


// static
void LLChatBar::onCommitGesture(LLUICtrl* ctrl, void* data)
{
	LLChatBar* self = (LLChatBar*)data;
	LLCtrlListInterface* gestures = self->mGestureCombo ? self->mGestureCombo->getListInterface() : NULL;
	if (gestures)
	{
		S32 index = gestures->getFirstSelectedIndex();
		if (index == 0)
		{
			return;
		}
		const std::string& trigger = gestures->getSelectedValue().asString();

		// pretend the user chatted the trigger string, to invoke
		// substitution and logging.
		std::string text(trigger);
		std::string revised_text;
		gGestureManager.triggerAndReviseString(text, &revised_text);

		revised_text = utf8str_trim(revised_text);
		if (!revised_text.empty())
		{
			// Don't play nodding animation
			self->sendChatFromViewer(revised_text, CHAT_TYPE_NORMAL, FALSE);
		}
	}
	self->mGestureLabelTimer.start();
	if (self->mGestureCombo != NULL)
	{
		// free focus back to chat bar
		self->mGestureCombo->setFocus(FALSE);
	}
}

void toggleChatHistory(void* user_data)
{
	LLFloaterChat::toggleInstance(LLSD());
}

//static
void LLChatBar::toggleChanSelect(void* user_data)//lgg
{
	LLChatBar* self = (LLChatBar*) user_data;

	//Rect code by Cryogenic
	LLRect chatbar = self->mInputEditor->getRect();
	LLRect chanselect;
	LLRect expander;
	S32 chatdelta;
	if(self->childGetRect("ChatChannel",chanselect) && self->childGetRect("Expand",expander))
	{

		if(self->mChanSelectorExpanded)
		{
			self->mChanSelectorExpanded=false;
			chatdelta = chanselect.getWidth();
			
			//self->childSetLabelArg("Expand","[NOTHING]",std::string("<"));
			self->childSetToolTip("Expand",std::string("Show Channel Selector"));
		}else
		{
			self->mChanSelectorExpanded=true;
			
			//self->childSetText("Expand",std::string(">"));
			
			//self->childSetLabelArg("Expand","[NOTHING]",std::string(">"));
			self->childSetToolTip("Expand",std::string("Hide Channel Selector"));
			chatdelta = -chanselect.getWidth();
		}
		expander.setCenterAndSize(expander.getCenterX() + chatdelta,expander.getCenterY(),expander.getWidth(),expander.getHeight());
		chatbar.setCenterAndSize(chatbar.getCenterX() + chatdelta/2,chatbar.getCenterY(),chatbar.getWidth() + chatdelta,chatbar.getHeight());
		self->childSetVisible("ChatChannel",self->mChanSelectorExpanded);
		self->mInputEditor->setRect(chatbar);
		self->childSetRect("Expand",expander);
	}
	else
	{
		llwarns << "ChatChannel or Expand could not be found in panel_chat_bar.xml" << llendl;
	}
}

class LLChatHandler : public LLCommandHandler
{
public:
	// not allowed from outside the app
	LLChatHandler() : LLCommandHandler("chat", true) { }

    // Your code here
	bool handle(const LLSD& tokens, const LLSD& query_map,
				LLWebBrowserCtrl* web)
	{
		if (tokens.size() < 2) return false;
		S32 channel = tokens[0].asInteger();
		std::string mesg = tokens[1].asString();
		send_chat_from_viewer(mesg, CHAT_TYPE_NORMAL, channel);
		return true;
	}
};

// Creating the object registers with the dispatcher.
LLChatHandler gChatHandler;
