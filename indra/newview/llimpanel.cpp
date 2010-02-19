/** 
 * @file llimpanel.cpp
 * @brief LLIMPanel class definition
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

#include "llimpanel.h"
#include "lggircgrouphandler.h"

#include "indra_constants.h"
#include "llfocusmgr.h"
#include "llfontgl.h"
#include "llrect.h"
#include "llerror.h"
#include "llstring.h"
#include "message.h"
#include "lltextbox.h"

#include "llagent.h"
#include "llbutton.h"
#include "llcallingcard.h"
#include "llchat.h"
#include "llconsole.h"
#include "llfloater.h"
#include "llfloatergroupinfo.h"
#include "llimview.h"
#include "llinventory.h"
#include "llinventorymodel.h"
#include "llinventoryview.h"
#include "llfloateractivespeakers.h"
#include "llfloateravatarinfo.h"
#include "llfloaterchat.h"
#include "llkeyboard.h"
#include "lllineeditor.h"
#include "llnotify.h"
#include "llresmgr.h"
#include "lltabcontainer.h"
#include "llviewertexteditor.h"
#include "llviewermenu.h"		// *FIX: for is_agent_friend()
#include "llviewermessage.h"
#include "llviewerstats.h"
#include "llviewercontrol.h"
#include "lluictrlfactory.h"
#include "llviewerwindow.h"
#include "lllogchat.h"
#include "llfloaterhtml.h"
#include "llweb.h"
#include "llhttpclient.h"
#include "llmutelist.h"
#include "llstylemap.h"

#include "emerald.h"

#if USE_OTR       // [$PLOTR$]
#include "context.h"
#include "llcombobox.h"
#include "otr_wrapper.h"
#include "otr_floater_smp_dialog.h"
#include "otr_floater_smp_progress.h"
#include "mfdkeywordfloater.h"
#endif // USE_OTR // [/$PLOTR$]

// [RLVa:KB]
#include "rlvhandler.h"
// [/RLVa:KB]

//
// Constants
//
const S32 LINE_HEIGHT = 16;
const S32 MIN_WIDTH = 200;
const S32 MIN_HEIGHT = 130;
const U32 DEFAULT_RETRIES_COUNT = 3;

//
// Statics
//
//
static std::string sTitleString = "Instant Message with [NAME]";
static std::string sTypingStartString = "[NAME]: ...";
static std::string sSessionStartString = "Starting session with [NAME] please wait.";

LLVoiceChannel::voice_channel_map_t LLVoiceChannel::sVoiceChannelMap;
LLVoiceChannel::voice_channel_map_uri_t LLVoiceChannel::sVoiceChannelURIMap;
LLVoiceChannel* LLVoiceChannel::sCurrentVoiceChannel = NULL;
LLVoiceChannel* LLVoiceChannel::sSuspendedVoiceChannel = NULL;

BOOL LLVoiceChannel::sSuspended = FALSE;

void session_starter_helper(
	const LLUUID& temp_session_id,
	const LLUUID& other_participant_id,
	EInstantMessage im_type)
{
	LLMessageSystem *msg = gMessageSystem;

	msg->newMessageFast(_PREHASH_ImprovedInstantMessage);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());

	msg->nextBlockFast(_PREHASH_MessageBlock);
	msg->addBOOLFast(_PREHASH_FromGroup, FALSE);
	msg->addUUIDFast(_PREHASH_ToAgentID, other_participant_id);
	msg->addU8Fast(_PREHASH_Offline, IM_ONLINE);
	msg->addU8Fast(_PREHASH_Dialog, im_type);
	msg->addUUIDFast(_PREHASH_ID, temp_session_id);
	msg->addU32Fast(_PREHASH_Timestamp, NO_TIMESTAMP); // no timestamp necessary

	std::string name;
	gAgent.buildFullname(name);

	msg->addStringFast(_PREHASH_FromAgentName, name);
	msg->addStringFast(_PREHASH_Message, LLStringUtil::null);
	msg->addU32Fast(_PREHASH_ParentEstateID, 0);
	msg->addUUIDFast(_PREHASH_RegionID, LLUUID::null);
	msg->addVector3Fast(_PREHASH_Position, gAgent.getPositionAgent());
}

void start_deprecated_conference_chat(
	const LLUUID& temp_session_id,
	const LLUUID& creator_id,
	const LLUUID& other_participant_id,
	const LLSD& agents_to_invite)
{
	U8* bucket;
	U8* pos;
	S32 count;
	S32 bucket_size;

	// *FIX: this could suffer from endian issues
	count = agents_to_invite.size();
	bucket_size = UUID_BYTES * count;
	bucket = new U8[bucket_size];
	pos = bucket;

	for(S32 i = 0; i < count; ++i)
	{
		LLUUID agent_id = agents_to_invite[i].asUUID();
		
		memcpy(pos, &agent_id, UUID_BYTES);
		pos += UUID_BYTES;
	}

	session_starter_helper(
		temp_session_id,
		other_participant_id,
		IM_SESSION_CONFERENCE_START);

	gMessageSystem->addBinaryDataFast(
		_PREHASH_BinaryBucket,
		bucket,
		bucket_size);

	gAgent.sendReliableMessage();
 
	delete[] bucket;
}

class LLStartConferenceChatResponder : public LLHTTPClient::Responder
{
public:
	LLStartConferenceChatResponder(
		const LLUUID& temp_session_id,
		const LLUUID& creator_id,
		const LLUUID& other_participant_id,
		const LLSD& agents_to_invite)
	{
		mTempSessionID = temp_session_id;
		mCreatorID = creator_id;
		mOtherParticipantID = other_participant_id;
		mAgents = agents_to_invite;
	}

	virtual void error(U32 statusNum, const std::string& reason)
	{
		//try an "old school" way.
		if ( statusNum == 400 )
		{
			start_deprecated_conference_chat(
				mTempSessionID,
				mCreatorID,
				mOtherParticipantID,
				mAgents);
		}

		//else throw an error back to the client?
		//in theory we should have just have these error strings
		//etc. set up in this file as opposed to the IMMgr,
		//but the error string were unneeded here previously
		//and it is not worth the effort switching over all
		//the possible different language translations
	}

private:
	LLUUID mTempSessionID;
	LLUUID mCreatorID;
	LLUUID mOtherParticipantID;

	LLSD mAgents;
};

// Returns true if any messages were sent, false otherwise.
// Is sort of equivalent to "does the server need to do anything?"
bool send_start_session_messages(
	const LLUUID& temp_session_id,
	const LLUUID& other_participant_id,
	const LLDynamicArray<LLUUID>& ids,
	EInstantMessage dialog)
{
	if ( dialog == IM_SESSION_IRC_START )
	{
		return false;
	}
	if ( dialog == IM_SESSION_GROUP_START )
	{
		session_starter_helper(
			temp_session_id,
			other_participant_id,
			dialog);

		switch(dialog)
		{
		case IM_SESSION_GROUP_START:
			gMessageSystem->addBinaryDataFast(
				_PREHASH_BinaryBucket,
				EMPTY_BINARY_BUCKET,
				EMPTY_BINARY_BUCKET_SIZE);
			break;
		default:
			break;
		}
		gAgent.sendReliableMessage();

		return true;
	}
	else if ( dialog == IM_SESSION_CONFERENCE_START )
	{
		LLSD agents;
		for (int i = 0; i < (S32) ids.size(); i++)
		{
			agents.append(ids.get(i));
		}

		//we have a new way of starting conference calls now
		LLViewerRegion* region = gAgent.getRegion();
		if (region)
		{
			std::string url = region->getCapability(
				"ChatSessionRequest");
			LLSD data;
			data["method"] = "start conference";
			data["session-id"] = temp_session_id;

			data["params"] = agents;

			LLHTTPClient::post(
				url,
				data,
				new LLStartConferenceChatResponder(
					temp_session_id,
					gAgent.getID(),
					other_participant_id,
					data["params"]));
		}
		else
		{
			start_deprecated_conference_chat(
				temp_session_id,
				gAgent.getID(),
				other_participant_id,
				agents);
		}
	}

	return false;
}

class LLVoiceCallCapResponder : public LLHTTPClient::Responder
{
public:
	LLVoiceCallCapResponder(const LLUUID& session_id) : mSessionID(session_id) {};

	virtual void error(U32 status, const std::string& reason);	// called with bad status codes
	virtual void result(const LLSD& content);

private:
	LLUUID mSessionID;
};


void LLVoiceCallCapResponder::error(U32 status, const std::string& reason)
{
	llwarns << "LLVoiceCallCapResponder::error("
		<< status << ": " << reason << ")"
		<< llendl;
	LLVoiceChannel* channelp = LLVoiceChannel::getChannelByID(mSessionID);
	if ( channelp )
	{
		if ( 403 == status )
		{
			//403 == no ability
			LLNotifications::instance().add(
				"VoiceNotAllowed",
				channelp->getNotifyArgs());
		}
		else
		{
			LLNotifications::instance().add(
				"VoiceCallGenericError",
				channelp->getNotifyArgs());
		}
		channelp->deactivate();
	}
}

void LLVoiceCallCapResponder::result(const LLSD& content)
{
	LLVoiceChannel* channelp = LLVoiceChannel::getChannelByID(mSessionID);
	if (channelp)
	{
		//*TODO: DEBUG SPAM
		LLSD::map_const_iterator iter;
		for(iter = content.beginMap(); iter != content.endMap(); ++iter)
		{
			llinfos << "LLVoiceCallCapResponder::result got " 
				<< iter->first << llendl;
		}

		channelp->setChannelInfo(
			content["voice_credentials"]["channel_uri"].asString(),
			content["voice_credentials"]["channel_credentials"].asString());
	}
}

//
// LLVoiceChannel
//
LLVoiceChannel::LLVoiceChannel(const LLUUID& session_id, const std::string& session_name) : 
	mSessionID(session_id), 
	mState(STATE_NO_CHANNEL_INFO), 
	mSessionName(session_name),
	mIgnoreNextSessionLeave(FALSE)
{
	mNotifyArgs["VOICE_CHANNEL_NAME"] = mSessionName;

	if (!sVoiceChannelMap.insert(std::make_pair(session_id, this)).second)
	{
		// a voice channel already exists for this session id, so this instance will be orphaned
		// the end result should simply be the failure to make voice calls
		llwarns << "Duplicate voice channels registered for session_id " << session_id << llendl;
	}

	LLVoiceClient::getInstance()->addObserver(this);
}

LLVoiceChannel::~LLVoiceChannel()
{
	// Don't use LLVoiceClient::getInstance() here -- this can get called during atexit() time and that singleton MAY have already been destroyed.
	if(gVoiceClient)
	{
		gVoiceClient->removeObserver(this);
	}
	
	sVoiceChannelMap.erase(mSessionID);
	sVoiceChannelURIMap.erase(mURI);
}

void LLVoiceChannel::setChannelInfo(
	const std::string& uri,
	const std::string& credentials)
{
	setURI(uri);

	mCredentials = credentials;

	if (mState == STATE_NO_CHANNEL_INFO)
	{
		if (mURI.empty())
		{
			LLNotifications::instance().add("VoiceChannelJoinFailed", mNotifyArgs);
			llwarns << "Received empty URI for channel " << mSessionName << llendl;
			deactivate();
		}
		else if (mCredentials.empty())
		{
			LLNotifications::instance().add("VoiceChannelJoinFailed", mNotifyArgs);
			llwarns << "Received empty credentials for channel " << mSessionName << llendl;
			deactivate();
		}
		else
		{
			setState(STATE_READY);

			// if we are supposed to be active, reconnect
			// this will happen on initial connect, as we request credentials on first use
			if (sCurrentVoiceChannel == this)
			{
				// just in case we got new channel info while active
				// should move over to new channel
				activate();
			}
		}
	}
}

void LLVoiceChannel::onChange(EStatusType type, const std::string &channelURI, bool proximal)
{
	if (channelURI != mURI)
	{
		return;
	}

	if (type < BEGIN_ERROR_STATUS)
	{
		handleStatusChange(type);
	}
	else
	{
		handleError(type);
	}
}

void LLVoiceChannel::handleStatusChange(EStatusType type)
{
	// status updates
	switch(type)
	{
	case STATUS_LOGIN_RETRY:
		//mLoginNotificationHandle = LLNotifyBox::showXml("VoiceLoginRetry")->getHandle();
		LLNotifications::instance().add("VoiceLoginRetry");
		break;
	case STATUS_LOGGED_IN:
		//if (!mLoginNotificationHandle.isDead())
		//{
		//	LLNotifyBox* notifyp = (LLNotifyBox*)mLoginNotificationHandle.get();
		//	if (notifyp)
		//	{
		//		notifyp->close();
		//	}
		//	mLoginNotificationHandle.markDead();
		//}
		break;
	case STATUS_LEFT_CHANNEL:
		if (callStarted() && !mIgnoreNextSessionLeave && !sSuspended)
		{
			// if forceably removed from channel
			// update the UI and revert to default channel
			LLNotifications::instance().add("VoiceChannelDisconnected", mNotifyArgs);
			deactivate();
		}
		mIgnoreNextSessionLeave = FALSE;
		break;
	case STATUS_JOINING:
		if (callStarted())
		{
			setState(STATE_RINGING);
		}
		break;
	case STATUS_JOINED:
		if (callStarted())
		{
			setState(STATE_CONNECTED);
		}
	default:
		break;
	}
}

// default behavior is to just deactivate channel
// derived classes provide specific error messages
void LLVoiceChannel::handleError(EStatusType type)
{
	deactivate();
	setState(STATE_ERROR);
}

BOOL LLVoiceChannel::isActive()
{ 
	// only considered active when currently bound channel matches what our channel
	return callStarted() && LLVoiceClient::getInstance()->getCurrentChannel() == mURI; 
}

BOOL LLVoiceChannel::callStarted()
{
	return mState >= STATE_CALL_STARTED;
}

void LLVoiceChannel::deactivate()
{
	if (mState >= STATE_RINGING)
	{
		// ignore session leave event
		mIgnoreNextSessionLeave = TRUE;
	}

	if (callStarted())
	{
		setState(STATE_HUNG_UP);
		// mute the microphone if required when returning to the proximal channel
		if (gSavedSettings.getBOOL("AutoDisengageMic") && sCurrentVoiceChannel == this)
		{
			gSavedSettings.setBOOL("PTTCurrentlyEnabled", true);
		}
	}

	if (sCurrentVoiceChannel == this)
	{
		// default channel is proximal channel
		sCurrentVoiceChannel = LLVoiceChannelProximal::getInstance();
		sCurrentVoiceChannel->activate();
	}
}

void LLVoiceChannel::activate()
{
	if (callStarted())
	{
		return;
	}

	// deactivate old channel and mark ourselves as the active one
	if (sCurrentVoiceChannel != this)
	{
		// mark as current before deactivating the old channel to prevent
		// activating the proximal channel between IM calls
		LLVoiceChannel* old_channel = sCurrentVoiceChannel;
		sCurrentVoiceChannel = this;
		if (old_channel)
		{
			old_channel->deactivate();
		}
	}

	if (mState == STATE_NO_CHANNEL_INFO)
	{
		// responsible for setting status to active
		getChannelInfo();
	}
	else
	{
		setState(STATE_CALL_STARTED);
	}
}

void LLVoiceChannel::getChannelInfo()
{
	// pretend we have everything we need
	if (sCurrentVoiceChannel == this)
	{
		setState(STATE_CALL_STARTED);
	}
}

//static 
LLVoiceChannel* LLVoiceChannel::getChannelByID(const LLUUID& session_id)
{
	voice_channel_map_t::iterator found_it = sVoiceChannelMap.find(session_id);
	if (found_it == sVoiceChannelMap.end())
	{
		return NULL;
	}
	else
	{
		return found_it->second;
	}
}

//static 
LLVoiceChannel* LLVoiceChannel::getChannelByURI(std::string uri)
{
	voice_channel_map_uri_t::iterator found_it = sVoiceChannelURIMap.find(uri);
	if (found_it == sVoiceChannelURIMap.end())
	{
		return NULL;
	}
	else
	{
		return found_it->second;
	}
}


void LLVoiceChannel::updateSessionID(const LLUUID& new_session_id)
{
	sVoiceChannelMap.erase(sVoiceChannelMap.find(mSessionID));
	mSessionID = new_session_id;
	sVoiceChannelMap.insert(std::make_pair(mSessionID, this));
}

void LLVoiceChannel::setURI(std::string uri)
{
	sVoiceChannelURIMap.erase(mURI);
	mURI = uri;
	sVoiceChannelURIMap.insert(std::make_pair(mURI, this));
}

void LLVoiceChannel::setState(EState state)
{
	switch(state)
	{
	case STATE_RINGING:
		gIMMgr->addSystemMessage(mSessionID, "ringing", mNotifyArgs);
		break;
	case STATE_CONNECTED:
		gIMMgr->addSystemMessage(mSessionID, "connected", mNotifyArgs);
		break;
	case STATE_HUNG_UP:
		gIMMgr->addSystemMessage(mSessionID, "hang_up", mNotifyArgs);
		break;
	default:
		break;
	}

	mState = state;
}


//static
void LLVoiceChannel::initClass()
{
	sCurrentVoiceChannel = LLVoiceChannelProximal::getInstance();
}


//static 
void LLVoiceChannel::suspend()
{
	if (!sSuspended)
	{
		sSuspendedVoiceChannel = sCurrentVoiceChannel;
		sSuspended = TRUE;
	}
}

//static 
void LLVoiceChannel::resume()
{
	if (sSuspended)
	{
		if (gVoiceClient->voiceEnabled())
		{
			if (sSuspendedVoiceChannel)
			{
				sSuspendedVoiceChannel->activate();
			}
			else
			{
				LLVoiceChannelProximal::getInstance()->activate();
			}
		}
		sSuspended = FALSE;
	}
}


//
// LLVoiceChannelGroup
//

LLVoiceChannelGroup::LLVoiceChannelGroup(const LLUUID& session_id, const std::string& session_name) : 
	LLVoiceChannel(session_id, session_name)
{
	mRetries = DEFAULT_RETRIES_COUNT;
	mIsRetrying = FALSE;
}

void LLVoiceChannelGroup::deactivate()
{
	if (callStarted())
	{
		LLVoiceClient::getInstance()->leaveNonSpatialChannel();
	}
	LLVoiceChannel::deactivate();
}

void LLVoiceChannelGroup::activate()
{
	if (callStarted()) return;

	LLVoiceChannel::activate();

	if (callStarted())
	{
		// we have the channel info, just need to use it now
		LLVoiceClient::getInstance()->setNonSpatialChannel(
			mURI,
			mCredentials);
	}
}

void LLVoiceChannelGroup::getChannelInfo()
{
	LLViewerRegion* region = gAgent.getRegion();
	if (region)
	{
		std::string url = region->getCapability("ChatSessionRequest");
		LLSD data;
		data["method"] = "call";
		data["session-id"] = mSessionID;
		LLHTTPClient::post(url,
						   data,
						   new LLVoiceCallCapResponder(mSessionID));
	}
}

void LLVoiceChannelGroup::setChannelInfo(
	const std::string& uri,
	const std::string& credentials)
{
	setURI(uri);

	mCredentials = credentials;

	if (mState == STATE_NO_CHANNEL_INFO)
	{
		if(!mURI.empty() && !mCredentials.empty())
		{
			setState(STATE_READY);

			// if we are supposed to be active, reconnect
			// this will happen on initial connect, as we request credentials on first use
			if (sCurrentVoiceChannel == this)
			{
				// just in case we got new channel info while active
				// should move over to new channel
				activate();
			}
		}
		else
		{
			//*TODO: notify user
			llwarns << "Received invalid credentials for channel " << mSessionName << llendl;
			deactivate();
		}
	}
	else if ( mIsRetrying )
	{
		// we have the channel info, just need to use it now
		LLVoiceClient::getInstance()->setNonSpatialChannel(
			mURI,
			mCredentials);
	}
}

void LLVoiceChannelGroup::handleStatusChange(EStatusType type)
{
	// status updates
	switch(type)
	{
	case STATUS_JOINED:
		mRetries = 3;
		mIsRetrying = FALSE;
	default:
		break;
	}

	LLVoiceChannel::handleStatusChange(type);
}

void LLVoiceChannelGroup::handleError(EStatusType status)
{
	std::string notify;
	switch(status)
	{
	case ERROR_CHANNEL_LOCKED:
	case ERROR_CHANNEL_FULL:
		notify = "VoiceChannelFull";
		break;
	case ERROR_NOT_AVAILABLE:
		//clear URI and credentials
		//set the state to be no info
		//and activate
		if ( mRetries > 0 )
		{
			mRetries--;
			mIsRetrying = TRUE;
			mIgnoreNextSessionLeave = TRUE;

			getChannelInfo();
			return;
		}
		else
		{
			notify = "VoiceChannelJoinFailed";
			mRetries = DEFAULT_RETRIES_COUNT;
			mIsRetrying = FALSE;
		}

		break;

	case ERROR_UNKNOWN:
	default:
		break;
	}

	// notification
	if (!notify.empty())
	{
		LLNotificationPtr notification = LLNotifications::instance().add(notify, mNotifyArgs);
		// echo to im window
		gIMMgr->addMessage(mSessionID, LLUUID::null, SYSTEM_FROM, notification->getMessage());
	}

	LLVoiceChannel::handleError(status);
}

void LLVoiceChannelGroup::setState(EState state)
{
	switch(state)
	{
	case STATE_RINGING:
		if ( !mIsRetrying )
		{
			gIMMgr->addSystemMessage(mSessionID, "ringing", mNotifyArgs);
		}

		mState = state;
		break;
	default:
		LLVoiceChannel::setState(state);
	}
}

//
// LLVoiceChannelProximal
//
LLVoiceChannelProximal::LLVoiceChannelProximal() : 
	LLVoiceChannel(LLUUID::null, LLStringUtil::null)
{
	activate();
}

BOOL LLVoiceChannelProximal::isActive()
{
	return callStarted() && LLVoiceClient::getInstance()->inProximalChannel(); 
}

void LLVoiceChannelProximal::activate()
{
	if (callStarted()) return;

	LLVoiceChannel::activate();

	if (callStarted())
	{
		// this implicitly puts you back in the spatial channel
		LLVoiceClient::getInstance()->leaveNonSpatialChannel();
	}
}

void LLVoiceChannelProximal::onChange(EStatusType type, const std::string &channelURI, bool proximal)
{
	if (!proximal)
	{
		return;
	}

	if (type < BEGIN_ERROR_STATUS)
	{
		handleStatusChange(type);
	}
	else
	{
		handleError(type);
	}
}

void LLVoiceChannelProximal::handleStatusChange(EStatusType status)
{
	// status updates
	switch(status)
	{
	case STATUS_LEFT_CHANNEL:
		// do not notify user when leaving proximal channel
		return;
	case STATUS_VOICE_DISABLED:
		 gIMMgr->addSystemMessage(LLUUID::null, "unavailable", mNotifyArgs);
		return;
	default:
		break;
	}
	LLVoiceChannel::handleStatusChange(status);
}


void LLVoiceChannelProximal::handleError(EStatusType status)
{
	std::string notify;
	switch(status)
	{
	  case ERROR_CHANNEL_LOCKED:
	  case ERROR_CHANNEL_FULL:
		notify = "ProximalVoiceChannelFull";
		break;
	  default:
		 break;
	}

	// notification
	if (!notify.empty())
	{
		LLNotifications::instance().add(notify, mNotifyArgs);
	}

	LLVoiceChannel::handleError(status);
}

void LLVoiceChannelProximal::deactivate()
{
	if (callStarted())
	{
		setState(STATE_HUNG_UP);
	}
}


//
// LLVoiceChannelP2P
//
LLVoiceChannelP2P::LLVoiceChannelP2P(const LLUUID& session_id, const std::string& session_name, const LLUUID& other_user_id) : 
		LLVoiceChannelGroup(session_id, session_name), 
		mOtherUserID(other_user_id),
		mReceivedCall(FALSE)
{
	// make sure URI reflects encoded version of other user's agent id
	setURI(LLVoiceClient::getInstance()->sipURIFromID(other_user_id));
}

void LLVoiceChannelP2P::handleStatusChange(EStatusType type)
{
	// status updates
	switch(type)
	{
	case STATUS_LEFT_CHANNEL:
		if (callStarted() && !mIgnoreNextSessionLeave && !sSuspended)
		{
			if (mState == STATE_RINGING)
			{
				// other user declined call
				LLNotifications::instance().add("P2PCallDeclined", mNotifyArgs);
			}
			else
			{
				// other user hung up
				LLNotifications::instance().add("VoiceChannelDisconnectedP2P", mNotifyArgs);
			}
			deactivate();
		}
		mIgnoreNextSessionLeave = FALSE;
		return;
	default:
		break;
	}

	LLVoiceChannel::handleStatusChange(type);
}

void LLVoiceChannelP2P::handleError(EStatusType type)
{
	switch(type)
	{
	case ERROR_NOT_AVAILABLE:
		LLNotifications::instance().add("P2PCallNoAnswer", mNotifyArgs);
		break;
	default:
		break;
	}

	LLVoiceChannel::handleError(type);
}

void LLVoiceChannelP2P::activate()
{
	if (callStarted()) return;

	LLVoiceChannel::activate();

	if (callStarted())
	{
		// no session handle yet, we're starting the call
		if (mSessionHandle.empty())
		{
			mReceivedCall = FALSE;
			LLVoiceClient::getInstance()->callUser(mOtherUserID);
		}
		// otherwise answering the call
		else
		{
			LLVoiceClient::getInstance()->answerInvite(mSessionHandle);
			
			// using the session handle invalidates it.  Clear it out here so we can't reuse it by accident.
			mSessionHandle.clear();
		}
	}
}

void LLVoiceChannelP2P::getChannelInfo()
{
	// pretend we have everything we need, since P2P doesn't use channel info
	if (sCurrentVoiceChannel == this)
	{
		setState(STATE_CALL_STARTED);
	}
}

// receiving session from other user who initiated call
void LLVoiceChannelP2P::setSessionHandle(const std::string& handle, const std::string &inURI)
{ 
	BOOL needs_activate = FALSE;
	if (callStarted())
	{
		// defer to lower agent id when already active
		if (mOtherUserID < gAgent.getID())
		{
			// pretend we haven't started the call yet, so we can connect to this session instead
			deactivate();
			needs_activate = TRUE;
		}
		else
		{
			// we are active and have priority, invite the other user again
			// under the assumption they will join this new session
			mSessionHandle.clear();
			LLVoiceClient::getInstance()->callUser(mOtherUserID);
			return;
		}
	}

	mSessionHandle = handle;

	// The URI of a p2p session should always be the other end's SIP URI.
	if(!inURI.empty())
	{
		setURI(inURI);
	}
	else
	{
		setURI(LLVoiceClient::getInstance()->sipURIFromID(mOtherUserID));
	}
	
	mReceivedCall = TRUE;

	if (needs_activate)
	{
		activate();
	}
}

void LLVoiceChannelP2P::setState(EState state)
{
	// you only "answer" voice invites in p2p mode
	// so provide a special purpose message here
	if (mReceivedCall && state == STATE_RINGING)
	{
		gIMMgr->addSystemMessage(mSessionID, "answering", mNotifyArgs);
		mState = state;
		return;
	}
	LLVoiceChannel::setState(state);
}


//
// LLFloaterIMPanel
//
LLFloaterIMPanel::LLFloaterIMPanel(
	const std::string& session_label,
	const LLUUID& session_id,
	const LLUUID& other_participant_id,
	EInstantMessage dialog) :
	LLFloater(session_label, LLRect(), session_label),
	mInputEditor(NULL),
	mHistoryEditor(NULL),
	mSessionUUID(session_id),
	mVoiceChannel(NULL),
	mSessionInitialized(FALSE),
	mSessionStartMsgPos(0),
	mOtherParticipantUUID(other_participant_id),
	mDialog(dialog),
	mTyping(FALSE),
	mOtherTyping(FALSE),
	mTypingLineStartIndex(0),
	mSentTypingState(TRUE),
	mNumUnreadMessages(0),
	mShowSpeakersOnConnect(TRUE),
	mAutoConnect(FALSE),
	mTextIMPossible(TRUE),
	mProfileButtonEnabled(TRUE),
	mCallBackEnabled(TRUE),
	mSpeakers(NULL),
	mSpeakerPanel(NULL),
#if USE_OTR       // [$PLOTR$]
    mOtrSmpDialog(NULL),
    mOtrSmpProgress(NULL),
#endif // USE_OTR // [/$PLOTR$]
	mFirstKeystrokeTimer(),
	mLastKeystrokeTimer()
{
	init(session_label);
}

LLFloaterIMPanel::LLFloaterIMPanel(
	const std::string& session_label,
	const LLUUID& session_id,
	const LLUUID& other_participant_id,
	const LLDynamicArray<LLUUID>& ids,
	EInstantMessage dialog) :
	LLFloater(session_label, LLRect(), session_label),
	mInputEditor(NULL),
	mHistoryEditor(NULL),
	mSessionUUID(session_id),
	mVoiceChannel(NULL),
	mSessionInitialized(FALSE),
	mSessionStartMsgPos(0),
	mOtherParticipantUUID(other_participant_id),
	mDialog(dialog),
	mTyping(FALSE),
	mOtherTyping(FALSE),
	mTypingLineStartIndex(0),
	mSentTypingState(TRUE),
	mShowSpeakersOnConnect(TRUE),
	mAutoConnect(FALSE),
	mTextIMPossible(TRUE),
	mProfileButtonEnabled(TRUE),
	mCallBackEnabled(TRUE),
	mSpeakers(NULL),
	mSpeakerPanel(NULL),
#if USE_OTR       // [$PLOTR$]
    mOtrSmpDialog(NULL),
    mOtrSmpProgress(NULL),
#endif // USE_OTR // [/$PLOTR$]
	mFirstKeystrokeTimer(),
	mLastKeystrokeTimer()
{
	mSessionInitialTargetIDs = ids;
	init(session_label);
}


void LLFloaterIMPanel::init(const std::string& session_label)
{
	mSessionLabel = session_label;

	std::string xml_filename;
	switch(mDialog)
	{
	case IM_SESSION_GROUP_START:
		mFactoryMap["active_speakers_panel"] = LLCallbackMap(createSpeakersPanel, this);
		xml_filename = "floater_instant_message_group.xml";
		mVoiceChannel = new LLVoiceChannelGroup(mSessionUUID, mSessionLabel);
		break;
	case IM_SESSION_IRC_START:
		mSessionLabel = "<"+mSessionLabel+">";
		setShortTitle(mSessionLabel);
		mFactoryMap["active_speakers_panel"] = LLCallbackMap(createSpeakersPanel, this);
		xml_filename =  "floater_instant_message_ad_hoc.xml";
		mVoiceChannel = new LLVoiceChannelGroup(mSessionUUID, mSessionLabel);
		break;
	case IM_SESSION_INVITE:
		mFactoryMap["active_speakers_panel"] = LLCallbackMap(createSpeakersPanel, this);
		if (gAgent.isInGroup(mSessionUUID))
		{
			xml_filename = "floater_instant_message_group.xml";
		}
		else // must be invite to ad hoc IM
		{
			xml_filename = "floater_instant_message_ad_hoc.xml";
		}
		mVoiceChannel = new LLVoiceChannelGroup(mSessionUUID, mSessionLabel);
		break;
	case IM_SESSION_P2P_INVITE:
		xml_filename = "floater_instant_message.xml";
		mVoiceChannel = new LLVoiceChannelP2P(mSessionUUID, mSessionLabel, mOtherParticipantUUID);
		break;
	case IM_SESSION_CONFERENCE_START:
		mFactoryMap["active_speakers_panel"] = LLCallbackMap(createSpeakersPanel, this);
		xml_filename = "floater_instant_message_ad_hoc.xml";
		mVoiceChannel = new LLVoiceChannelGroup(mSessionUUID, mSessionLabel);
		break;
	// just received text from another user
	case IM_NOTHING_SPECIAL:

		xml_filename = "floater_instant_message.xml";
		
		mTextIMPossible = LLVoiceClient::getInstance()->isSessionTextIMPossible(mSessionUUID);
		mProfileButtonEnabled = LLVoiceClient::getInstance()->isParticipantAvatar(mSessionUUID);
		mCallBackEnabled = LLVoiceClient::getInstance()->isSessionCallBackPossible(mSessionUUID);
		
		mVoiceChannel = new LLVoiceChannelP2P(mSessionUUID, mSessionLabel, mOtherParticipantUUID);
		break;
	case IM_PRIVATE_IRC:
		mSessionLabel = mSessionLabel;
		setShortTitle(mSessionLabel);
		xml_filename = "floater_instant_message.xml";

		mTextIMPossible = TRUE;
		mProfileButtonEnabled = TRUE;
		mCallBackEnabled = FALSE;

		mVoiceChannel = new LLVoiceChannelP2P(mSessionUUID, mSessionLabel, mOtherParticipantUUID);
		break;

	default:
		llwarns << "Unknown session type" << llendl;
		xml_filename = "floater_instant_message.xml";
		break;
	}

	mSpeakers = new LLIMSpeakerMgr(mVoiceChannel);

	LLUICtrlFactory::getInstance()->buildFloater(this,
								xml_filename,
								&getFactoryMap(),
								FALSE);

	setTitle(mSessionLabel);
	mInputEditor->setMaxTextLength(S32_MAX);
	// enable line history support for instant message bar
	mInputEditor->setEnableLineHistory(TRUE);

	if ( gSavedPerAccountSettings.getBOOL("LogShowHistory") )
	{
		LLLogChat::loadHistory(mSessionLabel,
				       &chatFromLogFile,
				       (void *)this);
	}

	if ( !mSessionInitialized )
	{
		if ( !send_start_session_messages(
				 mSessionUUID,
				 mOtherParticipantUUID,
				 mSessionInitialTargetIDs,
				 mDialog) )
		{
			//we don't need to need to wait for any responses
			//so we're already initialized
			mSessionInitialized = TRUE;
			mSessionStartMsgPos = 0;
		}
		else
		{
			//locally echo a little "starting session" message
			LLUIString session_start = sSessionStartString;

			session_start.setArg("[NAME]", getTitle());
			mSessionStartMsgPos = 
				mHistoryEditor->getWText().length();

			addHistoryLine(
				session_start,
				gSavedSettings.getColor4("SystemChatColor"),
				false);
		}
	}
}


LLFloaterIMPanel::~LLFloaterIMPanel()
{
	delete mSpeakers;
	mSpeakers = NULL;
	
	// End the text IM session if necessary
	if(gVoiceClient && mOtherParticipantUUID.notNull())
	{
		switch(mDialog)
		{
			case IM_NOTHING_SPECIAL:
			case IM_SESSION_P2P_INVITE:
				gVoiceClient->endUserIMSession(mOtherParticipantUUID);
			break;
			
			default:
				// Appease the compiler
			break;
		}
	}
	
	//kicks you out of the voice channel if it is currently active

	// HAVE to do this here -- if it happens in the LLVoiceChannel destructor it will call the wrong version (since the object's partially deconstructed at that point).
	mVoiceChannel->deactivate();
	
	delete mVoiceChannel;
	mVoiceChannel = NULL;

	//delete focus lost callback
	if(mInputEditor)
	{
		mInputEditor->setFocusLostCallback( NULL );
	}
	if(mDialog == IM_SESSION_IRC_START)
	{
		glggIrcGroupHandler.endDownIRCListener(mSessionUUID);
	}

#if USE_OTR       // [$PLOTR$]
    if (mOtrSmpDialog)   delete mOtrSmpDialog;
    if (mOtrSmpProgress) delete mOtrSmpProgress;
#endif // USE_OTR // [/$PLOTR$]
}

static void passwordFocusChanged(LLFocusableElement *element, void *userdata)
{
	LLLineEditor *password = dynamic_cast<LLLineEditor*>(element);

	if(password)
	{
		password->setDrawAsterixes(!password->hasFocus());
	}
}

BOOL LLFloaterIMPanel::postBuild() 
{
	requires<LLLineEditor>("chat_editor");
	requires<LLTextEditor>("im_history");

	if (checkRequirements())
	{
		mInputEditor = getChild<LLLineEditor>("chat_editor");
		mInputEditor->setFocusReceivedCallback( onInputEditorFocusReceived, this );
		mInputEditor->setFocusLostCallback( onInputEditorFocusLost, this );
		mInputEditor->setKeystrokeCallback( onInputEditorKeystroke );
		mInputEditor->setCommitCallback( onCommitChat );
		mInputEditor->setCallbackUserData(this);
		mInputEditor->setCommitOnFocusLost( FALSE );
		mInputEditor->setRevertOnEsc( FALSE );
		mInputEditor->setReplaceNewlinesWithSpaces( FALSE );

		childSetAction("profile_callee_btn", onClickProfile, this);
		childSetAction("profile_tele_btn", onClickTeleport, this);
		childSetAction("group_info_btn", onClickGroupInfo, this);
		childSetAction("history_btn", onClickHistory, this);

		childSetAction("start_call_btn", onClickStartCall, this);
		childSetAction("end_call_btn", onClickEndCall, this);
		childSetAction("send_btn", onClickSend, this);
		childSetAction("toggle_active_speakers_btn", onClickToggleActiveSpeakers, this);

		childSetAction("moderator_kick_speaker", onKickSpeaker, this);
		//LLButton* close_btn = getChild<LLButton>("close_btn");
		//close_btn->setClickedCallback(&LLFloaterIMPanel::onClickClose, this);

		mHistoryEditor = getChild<LLViewerTextEditor>("im_history");
		mHistoryEditor->setParseHTML(TRUE);
		mHistoryEditor->setParseHighlights(TRUE);

		
		if(IM_SESSION_IRC_START == mDialog || IM_PRIVATE_IRC == mDialog)
		{
// 			childSetVisible("profile_btn", FALSE);
// 			childSetVisible("profile_callee_btn", FALSE);
// 			childSetVisible("start_call_btn",FALSE);
// 			childSetVisible("profile_tele_btn",FALSE);
// 			childSetVisible("password",FALSE);
// 			childSetVisible("otr_combo",FALSE);
		
			childSetEnabled("profile_callee_btn", FALSE);
			childSetEnabled("start_call_btn",FALSE);
			childSetEnabled("profile_tele_btn",FALSE);
			childSetVisible("password",FALSE);
			childSetVisible("otr_combo",FALSE);
			if ( IM_SESSION_GROUP_START == mDialog )
			{
				childSetEnabled("profile_callee_btn", FALSE);
			}
			else if(IM_PRIVATE_IRC == mDialog)
			{
				childSetEnabled("profile_callee_btn",TRUE);

			}
		}
		
		if(!mProfileButtonEnabled)
		{
			childSetEnabled("profile_callee_btn", FALSE);
		}

		sTitleString = getString("title_string");
		sTypingStartString = getString("typing_start_string");
		sSessionStartString = getString("session_start_string");

		if (mSpeakerPanel)
		{
			mSpeakerPanel->refreshSpeakers();
		}

		if (mDialog == IM_NOTHING_SPECIAL)
		{
			childSetAction("mute_btn", onClickMuteVoice, this);
			childSetCommitCallback("speaker_volume", onVolumeChange, this);
		}

#if USE_OTR       // [$PLOTR$]
        if (!gOTR) OTR_Wrapper::init();
		if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
        {
            LLComboBox *combo = getChild<LLComboBox>("otr_combo");
            if (!combo)
            {
                llwarns << "$PLOTR$ Can't find OTR control/status" << llendl;
            }
            else
            {
                llinfos << "$PLOTR$ found OTR control/status" << llendl;
                combo->setCommitCallback(onClickOtr);
                combo->setCallbackUserData(this);
                combo->setAllowTextEntry(FALSE, 0, FALSE);
                showOtrStatus();
            }
        }
#endif // USE_OTR // [/$PLOTR$]

		LLLineEditor *password_editor = getChild<LLLineEditor>("password");

		if(password_editor)
		{
			password_editor->setDrawAsterixes(TRUE);
			password_editor->setFocusChangedCallback(passwordFocusChanged);
		}

		if(mDialog == IM_NOTHING_SPECIAL && is_agent_friend(mOtherParticipantUUID))
		{
			if(!LLAvatarTracker::instance().isBuddyOnline(mOtherParticipantUUID))
			{
				setOffline();
			}
		}

		setDefaultBtn("send_btn");
		return TRUE;
	}

	return FALSE;
}

void* LLFloaterIMPanel::createSpeakersPanel(void* data)
{
	LLFloaterIMPanel* floaterp = (LLFloaterIMPanel*)data;
	floaterp->mSpeakerPanel = new LLPanelActiveSpeakers(floaterp->mSpeakers, TRUE);
	return floaterp->mSpeakerPanel;
}

//static 
void LLFloaterIMPanel::onClickMuteVoice(void* user_data)
{
	LLFloaterIMPanel* floaterp = (LLFloaterIMPanel*)user_data;
	if (floaterp)
	{
		BOOL is_muted = LLMuteList::getInstance()->isMuted(floaterp->mOtherParticipantUUID, LLMute::flagVoiceChat);

		LLMute mute(floaterp->mOtherParticipantUUID, floaterp->getTitle(), LLMute::AGENT);
		if (!is_muted)
		{
			LLMuteList::getInstance()->add(mute, LLMute::flagVoiceChat);
		}
		else
		{
			LLMuteList::getInstance()->remove(mute, LLMute::flagVoiceChat);
		}
	}
}

//static 
void LLFloaterIMPanel::onVolumeChange(LLUICtrl* source, void* user_data)
{
	LLFloaterIMPanel* floaterp = (LLFloaterIMPanel*)user_data;
	if (floaterp)
	{
		gVoiceClient->setUserVolume(floaterp->mOtherParticipantUUID, (F32)source->getValue().asReal());
	}
}


// virtual
void LLFloaterIMPanel::draw()
{	
	LLViewerRegion* region = gAgent.getRegion();
	
	BOOL enable_connect = (region && region->getCapability("ChatSessionRequest") != "")
					  && mSessionInitialized
					  && LLVoiceClient::voiceEnabled()
					  && mCallBackEnabled;

	// hide/show start call and end call buttons
	if(mDialog!=IM_SESSION_IRC_START && mDialog!=IM_PRIVATE_IRC)
	{
		childSetVisible("end_call_btn", LLVoiceClient::voiceEnabled() && mVoiceChannel->getState() >= LLVoiceChannel::STATE_CALL_STARTED);
		childSetVisible("start_call_btn", LLVoiceClient::voiceEnabled() && mVoiceChannel->getState() < LLVoiceChannel::STATE_CALL_STARTED);
		childSetEnabled("start_call_btn", enable_connect);
		childSetEnabled("send_btn", !childGetValue("chat_editor").asString().empty());
	}
	
	LLPointer<LLSpeaker> self_speaker = mSpeakers->findSpeaker(gAgent.getID());
	if(!mTextIMPossible)
	{
		mInputEditor->setEnabled(FALSE);
		mInputEditor->setLabel(getString("unavailable_text_label"));
	}
	else if (self_speaker.notNull() && self_speaker->mModeratorMutedText)
	{
		mInputEditor->setEnabled(FALSE);
		mInputEditor->setLabel(getString("muted_text_label"));
	}
	else
	{
		mInputEditor->setEnabled(TRUE);
		mInputEditor->setLabel(getString("default_text_label"));
	}

	if (mAutoConnect && enable_connect)
	{
		onClickStartCall(this);
		mAutoConnect = FALSE;
	}

	// show speakers window when voice first connects
	if (mShowSpeakersOnConnect && mVoiceChannel->isActive())
	{
		childSetVisible("active_speakers_panel", TRUE);
		mShowSpeakersOnConnect = FALSE;
	}
	childSetValue("toggle_active_speakers_btn", childIsVisible("active_speakers_panel"));

	if (mTyping)
	{
		// Time out if user hasn't typed for a while.
		if (mLastKeystrokeTimer.getElapsedTimeF32() > LLAgent::TYPING_TIMEOUT_SECS)
		{
			setTyping(FALSE);
		}

		// If we are typing, and it's been a little while, send the
		// typing indicator
		if (!mSentTypingState
			&& mFirstKeystrokeTimer.getElapsedTimeF32() > 1.f)
		{
			sendTypingState(TRUE);
			mSentTypingState = TRUE;
		}
	}

	// use embedded panel if available
	if (mSpeakerPanel)
	{
		if (mSpeakerPanel->getVisible())
		{
			mSpeakerPanel->refreshSpeakers();
		}
	}
	else
	{
		// refresh volume and mute checkbox
		childSetVisible("speaker_volume", LLVoiceClient::voiceEnabled() && mVoiceChannel->isActive());
		childSetValue("speaker_volume", gVoiceClient->getUserVolume(mOtherParticipantUUID));

		childSetValue("mute_btn", LLMuteList::getInstance()->isMuted(mOtherParticipantUUID, LLMute::flagVoiceChat));
		childSetVisible("mute_btn", LLVoiceClient::voiceEnabled() && mVoiceChannel->isActive());
	}
	LLFloater::draw();
}

class LLSessionInviteResponder : public LLHTTPClient::Responder
{
public:
	LLSessionInviteResponder(const LLUUID& session_id)
	{
		mSessionID = session_id;
	}

	void error(U32 statusNum, const std::string& reason)
	{
		llinfos << "Error inviting all agents to session" << llendl;
		//throw something back to the viewer here?
	}

private:
	LLUUID mSessionID;
};

BOOL LLFloaterIMPanel::inviteToSession(const LLDynamicArray<LLUUID>& ids)
{
	LLViewerRegion* region = gAgent.getRegion();
	if (!region)
	{
		return FALSE;
	}
	
	S32 count = ids.count();

	if( isInviteAllowed() && (count > 0) )
	{
		llinfos << "LLFloaterIMPanel::inviteToSession() - inviting participants" << llendl;

		std::string url = region->getCapability("ChatSessionRequest");

		LLSD data;

		data["params"] = LLSD::emptyArray();
		for (int i = 0; i < count; i++)
		{
			data["params"].append(ids.get(i));
		}

		data["method"] = "invite";
		data["session-id"] = mSessionUUID;
		LLHTTPClient::post(
			url,
			data,
			new LLSessionInviteResponder(
				mSessionUUID));		
	}
	else
	{
		llinfos << "LLFloaterIMPanel::inviteToSession -"
				<< " no need to invite agents for "
				<< mDialog << llendl;
		// successful add, because everyone that needed to get added
		// was added.
	}

	return TRUE;
}

void LLFloaterIMPanel::addHistoryLine(const std::string &utf8msg, LLColor4 incolor, bool log_to_file, const LLUUID& source, const std::string& name)
{
	//mfd key word alert
	if(gAgent.getID() != source)
	{
		if(MfdKeywordFloaterStart::hasKeyword(utf8msg,2))
		{
			if(gSavedPerAccountSettings.getBOOL("EmeraldKeywordChangeColor"))
				incolor= gSavedPerAccountSettings.getColor4("EmeraldKeywordColor");
		}
	}
	
	const LLColor4& color = incolor;
	// start tab flashing when receiving im for background session from user
	if (source != LLUUID::null)
	{
		LLMultiFloater* hostp = getHost();
		if( !isInVisibleChain() 
			&& hostp 
			&& source != gAgent.getID())
		{
			hostp->setFloaterFlashing(this, TRUE);
		}
	}

	// Now we're adding the actual line of text, so erase the 
	// "Foo is typing..." text segment, and the optional timestamp
	// if it was present. JC
	removeTypingIndicator(NULL);

	// Actually add the line
	std::string timestring;
	bool prepend_newline = true;
	if (gSavedSettings.getBOOL("IMShowTimestamps"))
	{
		timestring = mHistoryEditor->appendTime(prepend_newline);
		prepend_newline = false;
	}

	// 'name' is a sender name that we want to hotlink so that clicking on it opens a profile.
	if (!name.empty()) // If name exists, then add it to the front of the message.
	{
		// Don't hotlink any messages from the system (e.g. "Second Life:"), so just add those in plain text.
		if (name == SYSTEM_FROM)
		{
			mHistoryEditor->appendColoredText(name,false,prepend_newline,color);
		}
		else
		{
			// Convert the name to a hotlink and add to message.
			const LLStyleSP &source_style = LLStyleMap::instance().lookupAgent(source);
			mHistoryEditor->appendStyledText(name,false,prepend_newline,source_style);
		}
		prepend_newline = false;
	}
	mHistoryEditor->appendColoredText(utf8msg, false, prepend_newline, color);
	
	if (log_to_file
		&& gSavedPerAccountSettings.getBOOL("LogInstantMessages") ) 
	{
		std::string histstr;
		if (gSavedPerAccountSettings.getBOOL("IMLogTimestamp"))
			histstr = LLLogChat::timestamp(gSavedPerAccountSettings.getBOOL("LogTimestampDate")) + name + utf8msg;
		else
			histstr = name + utf8msg;

		LLLogChat::saveHistory(getTitle(),histstr);
	}

	if (!isInVisibleChain())
	{
		mNumUnreadMessages++;
	}

	if (source != LLUUID::null)
	{
		mSpeakers->speakerChatted(source);
		mSpeakers->setSpeakerTyping(source, FALSE);
	}
}


void LLFloaterIMPanel::setVisible(BOOL b)
{
	LLPanel::setVisible(b);

	LLMultiFloater* hostp = getHost();
	if( b && hostp )
	{
		hostp->setFloaterFlashing(this, FALSE);

		/* Don't change containing floater title - leave it "Instant Message" JC
		LLUIString title = sTitleString;
		title.setArg("[NAME]", mSessionLabel);
		hostp->setTitle( title );
		*/
	}
}


void LLFloaterIMPanel::setInputFocus( BOOL b )
{
	mInputEditor->setFocus( b );
}


void LLFloaterIMPanel::selectAll()
{
	mInputEditor->selectAll();
}


void LLFloaterIMPanel::selectNone()
{
	mInputEditor->deselect();
}


BOOL LLFloaterIMPanel::handleKeyHere( KEY key, MASK mask )
{
	BOOL handled = FALSE;
	if( KEY_RETURN == key && mask == MASK_NONE)
	{
		sendMsg(0);
		handled = TRUE;

		// Close talk panels on hitting return
		// but not shift-return or control-return
		if ( !gSavedSettings.getBOOL("PinTalkViewOpen") && !(mask & MASK_CONTROL) && !(mask & MASK_SHIFT) )
		{
			gIMMgr->toggle(NULL);
		}
	}
	else if ( KEY_RETURN == key && mask == MASK_ALT)
	{
		// ooc chat
		sendMsg(1);
		handled = TRUE;
	}
	else if (KEY_RETURN == key && (mask == (MASK_ALT | MASK_SHIFT | MASK_CONTROL)))
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
		handled = TRUE;
	}

	else if ( KEY_ESCAPE == key )
	{
		handled = TRUE;
		gFocusMgr.setKeyboardFocus(NULL);

		// Close talk panel with escape
		if( !gSavedSettings.getBOOL("PinTalkViewOpen") )
		{
			gIMMgr->toggle(NULL);
		}
	}

	// May need to call base class LLPanel::handleKeyHere if not handled
	// in order to tab between buttons.  JNC 1.2.2002
	return handled;
}

BOOL LLFloaterIMPanel::handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
								  EDragAndDropType cargo_type,
								  void* cargo_data,
								  EAcceptance* accept,
								  std::string& tooltip_msg)
{

	if (mDialog == IM_NOTHING_SPECIAL)
	{
		LLToolDragAndDrop::handleGiveDragAndDrop(mOtherParticipantUUID, mSessionUUID, drop,
												 cargo_type, cargo_data, accept);
	}
	
	// handle case for dropping calling cards (and folders of calling cards) onto invitation panel for invites
	else if (isInviteAllowed())
	{
		*accept = ACCEPT_NO;
		
		if (cargo_type == DAD_CALLINGCARD)
		{
			if (dropCallingCard((LLInventoryItem*)cargo_data, drop))
			{
				*accept = ACCEPT_YES_MULTI;
			}
		}
		else if (cargo_type == DAD_CATEGORY)
		{
			if (dropCategory((LLInventoryCategory*)cargo_data, drop))
			{
				*accept = ACCEPT_YES_MULTI;
			}
		}
	}
	return TRUE;
} 

BOOL LLFloaterIMPanel::dropCallingCard(LLInventoryItem* item, BOOL drop)
{
	BOOL rv = isInviteAllowed();
	if(rv && item && item->getCreatorUUID().notNull())
	{
		if(drop)
		{
			LLDynamicArray<LLUUID> ids;
			ids.put(item->getCreatorUUID());
			inviteToSession(ids);
		}
	}
	else
	{
		// set to false if creator uuid is null.
		rv = FALSE;
	}
	return rv;
}

BOOL LLFloaterIMPanel::dropCategory(LLInventoryCategory* category, BOOL drop)
{
	BOOL rv = isInviteAllowed();
	if(rv && category)
	{
		LLInventoryModel::cat_array_t cats;
		LLInventoryModel::item_array_t items;
		LLUniqueBuddyCollector buddies;
		gInventory.collectDescendentsIf(category->getUUID(),
										cats,
										items,
										LLInventoryModel::EXCLUDE_TRASH,
										buddies);
		S32 count = items.count();
		if(count == 0)
		{
			rv = FALSE;
		}
		else if(drop)
		{
			LLDynamicArray<LLUUID> ids;
			for(S32 i = 0; i < count; ++i)
			{
				ids.put(items.get(i)->getCreatorUUID());
			}
			inviteToSession(ids);
		}
	}
	return rv;
}

BOOL LLFloaterIMPanel::isInviteAllowed() const
{

	return ( (IM_SESSION_CONFERENCE_START == mDialog) 
			 || (IM_SESSION_INVITE == mDialog) );
}


// static
void LLFloaterIMPanel::onTabClick(void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
	self->setInputFocus(TRUE);
}


// static
void LLFloaterIMPanel::onClickProfile( void* userdata )
{
	//  Bring up the Profile window
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
	
	if (self->mOtherParticipantUUID.notNull())
	{
		LLFloaterAvatarInfo::showFromDirectory(self->getOtherParticipantID());
	}
}

//static
void LLFloaterIMPanel::onClickTeleport( void* userdata )
{
	//  Bring up the Profile window
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
	
	if (self->mOtherParticipantUUID.notNull())
	{
		handle_lure(self->getOtherParticipantID());
		//do a teleport to other part id
		//LLFloaterAvatarInfo::showFromDirectory(self->getOtherParticipantID());
	}
}

// static
void LLFloaterIMPanel::onClickHistory( void* userdata )
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
	
	if (self->mOtherParticipantUUID.notNull())
	{
		char command[256];
		std::string fullname(gDirUtilp->getScrubbedFileName(self->getTitle()));
		sprintf(command, "\"%s\\%s.txt\"", gDirUtilp->getPerAccountChatLogsDir().c_str(),fullname.c_str());
		gViewerWindow->getWindow()->ShellEx(command);

		llinfos << command << llendl;
		}
	}

// static
void LLFloaterIMPanel::onClickGroupInfo( void* userdata )
{
	//  Bring up the Profile window
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;

	LLFloaterGroupInfo::showFromUUID(self->mSessionUUID);
}

// static
void LLFloaterIMPanel::onClickClose( void* userdata )
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
	if(self)
	{
		self->close();
	}
}

// static
void LLFloaterIMPanel::onClickStartCall(void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;

	self->mVoiceChannel->activate();
}

// static
void LLFloaterIMPanel::onClickEndCall(void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;

	self->getVoiceChannel()->deactivate();
}

// static
void LLFloaterIMPanel::onClickSend(void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*)userdata;
	self->sendMsg(0);
}

// static
void LLFloaterIMPanel::onClickToggleActiveSpeakers(void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*)userdata;

	self->childSetVisible("active_speakers_panel", !self->childIsVisible("active_speakers_panel"));
}

// static
void LLFloaterIMPanel::onCommitChat(LLUICtrl* caller, void* userdata)
{
	LLFloaterIMPanel* self= (LLFloaterIMPanel*) userdata;
	self->sendMsg(0);
}

// static
void LLFloaterIMPanel::onInputEditorFocusReceived( LLFocusableElement* caller, void* userdata )
{
	LLFloaterIMPanel* self= (LLFloaterIMPanel*) userdata;
	self->mHistoryEditor->setCursorAndScrollToEnd();
}

// static
void LLFloaterIMPanel::onInputEditorFocusLost(LLFocusableElement* caller, void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
	self->setTyping(FALSE);
}

// static
void LLFloaterIMPanel::onInputEditorKeystroke(LLLineEditor* caller, void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*)userdata;
	std::string text = self->mInputEditor->getText();
	if (!text.empty())
	{
		self->setTyping(TRUE);
	}
	else
	{
		// Deleting all text counts as stopping typing.
		self->setTyping(FALSE);
	}
}

void LLFloaterIMPanel::onClose(bool app_quitting)
{
	setTyping(FALSE);

	if(mSessionUUID.notNull())
	{
		std::string name;
		gAgent.buildFullname(name);
		pack_instant_message(
			gMessageSystem,
			gAgent.getID(),
			FALSE,
			gAgent.getSessionID(),
			mOtherParticipantUUID,
			name, 
			LLStringUtil::null,
			IM_ONLINE,
			IM_SESSION_LEAVE,
			mSessionUUID);
		gAgent.sendReliableMessage();
	}
	gIMMgr->removeSession(mSessionUUID);

	destroy();
}

void LLFloaterIMPanel::onVisibilityChange(BOOL new_visibility)
{
	if (new_visibility)
	{
		mNumUnreadMessages = 0;
	}
}

void deliver_message(const std::string& utf8_text,
					 const LLUUID& im_session_id,
					 const LLUUID& other_participant_id,
					 EInstantMessage dialog)
{
	std::string name;
	bool sent = false;
	gAgent.buildFullname(name);

	const LLRelationship* info = NULL;
	info = LLAvatarTracker::instance().getBuddyInfo(other_participant_id);
	
	U8 offline = (!info || info->isOnline()) ? IM_ONLINE : IM_OFFLINE;
	
	if((offline == IM_OFFLINE) && (LLVoiceClient::getInstance()->isOnlineSIP(other_participant_id)))
	{
		// User is online through the OOW connector, but not with a regular viewer.  Try to send the message via SLVoice.
		sent = gVoiceClient->sendTextMessage(other_participant_id, utf8_text);
	}
	
	if(!sent)
	{
		// Send message normally.

		// default to IM_SESSION_SEND unless it's nothing special - in
		// which case it's probably an IM to everyone.
		U8 new_dialog = dialog;
		if ( dialog == IM_SESSION_IRC_START)
		{
			glggIrcGroupHandler.sendIrcChatByID(im_session_id,utf8_text);
			return;
		}
		if ( dialog != IM_NOTHING_SPECIAL )
		{
			new_dialog = IM_SESSION_SEND;
		}
		pack_instant_message(
			gMessageSystem,
			gAgent.getID(),
			FALSE,
			gAgent.getSessionID(),
			other_participant_id,
			name.c_str(),
			utf8_text.c_str(),
			offline,
			(EInstantMessage)new_dialog,
			im_session_id);
		gAgent.sendReliableMessage();
	}

	// If there is a mute list and this is not a group chat...
	if ( LLMuteList::getInstance() )
	{
		// ... the target should not be in our mute list for some message types.
		// Auto-remove them if present.
		switch( dialog )
		{
		case IM_NOTHING_SPECIAL:
		case IM_GROUP_INVITATION:
		case IM_INVENTORY_OFFERED:
		case IM_SESSION_INVITE:
		case IM_SESSION_P2P_INVITE:
		case IM_SESSION_CONFERENCE_START:
		case IM_SESSION_SEND: // This one is marginal - erring on the side of hearing.
		case IM_LURE_USER:
		case IM_GODLIKE_LURE_USER:
		case IM_FRIENDSHIP_OFFERED:
			LLMuteList::getInstance()->autoRemove(other_participant_id, LLMuteList::AR_IM);
			break;
		default: ; // do nothing
		}
	}
}

#if USE_OTR       // [$PLOTR$]
static bool g_otr_force_typing_stop = false; // ugly hack...
// sometimes we must send messages, but don't know if they are offline

void otr_deliver_message(const std::string& utf8_text,
                         const LLUUID& im_session_id,
                         const LLUUID& other_participant_id,
                         EInstantMessage dialog)
{
//    llinfos
//        << "$PLOTR$ message length:" << utf8_text.length()
//        << " ["    << utf8_text.substr(0, 24)
//        << "]...[" << utf8_text.substr(utf8_text.length()-10, utf8_text.length()-1)
//        << "]"     << llendl;
    std::string name;
    gAgent.buildFullname(name);

	const LLRelationship* info = NULL;
	info = LLAvatarTracker::instance().getBuddyInfo(other_participant_id);
	
	U8 offline = (!info || info->isOnline()) ? IM_ONLINE : IM_OFFLINE;

    // default to IM_SESSION_SEND unless it's nothing special - in
    // which case it's probably an IM to everyone.
    U8 new_dialog = dialog;

    if ( dialog != IM_NOTHING_SPECIAL )
    {
        new_dialog = IM_SESSION_SEND;
    }
    if ((new_dialog == IM_NOTHING_SPECIAL) &&
        (g_otr_force_typing_stop ||
         (gSavedSettings.getBOOL("EmeraldOTRInTypingStop"))))
    {
        OtrlMessageType mtype = otrl_proto_message_type(utf8_text.c_str());
        switch (mtype)
        {
        case OTRL_MSGTYPE_UNKNOWN:
            llwarns << "Sending unknown type of OTR message" << llendl;
            // fall through
        case OTRL_MSGTYPE_QUERY:
        case OTRL_MSGTYPE_DH_COMMIT:
        case OTRL_MSGTYPE_DH_KEY:
        case OTRL_MSGTYPE_REVEALSIG:
        case OTRL_MSGTYPE_SIGNATURE:
        case OTRL_MSGTYPE_V1_KEYEXCH:
        case OTRL_MSGTYPE_DATA:
        case OTRL_MSGTYPE_TAGGEDPLAINTEXT:
            new_dialog = IM_TYPING_STOP;
            break;
        case OTRL_MSGTYPE_NOTOTR:
        case OTRL_MSGTYPE_ERROR:
        default:
            /* new_dialog = IM_NOTHING_SPECIAL */ ;
        }
    }
    pack_instant_message(
        gMessageSystem,
        gAgent.getID(),
        FALSE,
        gAgent.getSessionID(),
        other_participant_id,
        name.c_str(),
        utf8_text.c_str(),
        offline,
        (EInstantMessage)new_dialog,
        im_session_id);
    gAgent.sendReliableMessage();

    // If there is a mute list and this is not a group chat...
    if ( LLMuteList::getInstance() )
    {
        // ... the target should not be in our mute list for some message types.
        // Auto-remove them if present.
        switch( dialog )
        {
        case IM_NOTHING_SPECIAL:
        case IM_GROUP_INVITATION:
        case IM_INVENTORY_OFFERED:
        case IM_SESSION_INVITE:
        case IM_SESSION_P2P_INVITE:
        case IM_SESSION_CONFERENCE_START:
        case IM_SESSION_SEND: // This one is marginal - erring on the side of hearing.
        case IM_LURE_USER:
        case IM_GODLIKE_LURE_USER:
        case IM_FRIENDSHIP_OFFERED:
            LLMuteList::getInstance()->autoRemove(other_participant_id, LLMuteList::AR_IM);
            break;
        default: ; // do nothing
        }
    }
}

// static
void LLFloaterIMPanel::onClickOtr(LLUICtrl* source, void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*) userdata;
    if (self)
    {
        self->doOtrMenu();
        self->showOtrStatus();
    }
    else
    {
        llwarns << "$PLOTR$ onClickOtr() can't find floater." << llendl;
    }
}

void LLFloaterIMPanel::doOtrStart()
{
    U32 otrpref = gSavedSettings.getU32("EmeraldUseOTR");
    // otrpref: 0 == Require use of OTR in IMs, 1 == Request OTR if available, 2 == Accept OTR requests, 3 == Decline use of OTR
    if (3 == otrpref)
    {
        //otrLogMessageGetstring("otr_err_deacivated");
        showOtrStatus();
        return;
    }
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        llinfos << "$PLOTR$ otr menu start/restart/refresh" << llendl;
        gcry_error_t err = 0;
        char *newmessage = NULL;
        char my_uuid[UUID_STR_SIZE];
        char their_uuid[UUID_STR_SIZE];
        gAgent.getID().toString(&(my_uuid[0]));
        mOtherParticipantUUID.toString(&(their_uuid[0]));

        const LLRelationship* info = NULL;
        info = LLAvatarTracker::instance().getBuddyInfo(mOtherParticipantUUID);
        if (info && (!info->isOnline()))
        {
            otrLogMessageGetstringName("otr_err_offline_start");
            return;
        }

        if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
        {
            // only try OTR for 1 on 1 IM's
            err = otrl_message_sending(
                gOTR->get_userstate(), 
                gOTR->get_uistate(), 
                &mSessionUUID,
                my_uuid,
                gOTR->get_protocolid(),
                their_uuid,
                "?OTRv2?", NULL, &newmessage,
                NULL, NULL);
        }
        if (err)
        {
            llwarns << "$PLOTR$ OTR failed to encrypt start message" << llendl;
            otrLogMessageGetstring("otr_err_failed_starting");
            return;
        }
        else if (newmessage)
        {
            // OTR encrypted the message.  Handle fragmentation of the message
            int context_added = 0;
            ConnContext *context = getOtrContext(1, &context_added);
            if (context_added)
            {
                llwarns << "$PLOTR$ context added *after* send start but before fragmentation." << llendl;
            }
            if (! context)
            {
                otrLogMessageGetstring("otr_err_failed_starting");
                llwarns << "$PLOTR$ can't find context, not sending start message." << llendl;
                return;
            }
            else
            {
                char *extrafragment = NULL;
                err = otrl_message_fragment_and_send(
                    gOTR->get_uistate(), 
                    &mSessionUUID,
                    context,
                    newmessage,
                    OTRL_FRAGMENT_SEND_ALL,
                    &extrafragment);
            }
            if (newmessage) otrl_message_free(newmessage);
            //otrLogMessageGetstringName("otr_prog_I_start");
        }
        else
        {
            llwarns << "$PLOTR$ can't start OTR for some reason." << llendl;
            otrLogMessageGetstring("otr_err_failed_starting");
            return;
        }
    }
}

void LLFloaterIMPanel::doOtrStop(bool pretend_they_did)
{
    llinfos << "$PLOTR$ otr menu stop 1" << llendl;
    // do not disable this bassed on gSavedSettings.getU32("EmeraldUseOTR");
    // when the user disables OTR we may still need to stop currently encrypted conversations
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        char my_uuid[UUID_STR_SIZE];
        char their_uuid[UUID_STR_SIZE];
        gAgent.getID().toString(&(my_uuid[0]));
        mOtherParticipantUUID.toString(&(their_uuid[0]));
        llinfos << "$PLOTR$ otr menu stop 2 their_uuid:" << mOtherParticipantUUID << llendl;
        g_otr_force_typing_stop = true; // ugly hack
        otrl_message_disconnect(
            gOTR->get_userstate(), 
            gOTR->get_uistate(), 
            &mSessionUUID,
            my_uuid,
            gOTR->get_protocolid(),
            their_uuid);
        g_otr_force_typing_stop = false;
        if (pretend_they_did)
        {
            otrLogMessageGetstringName("otr_prog_they_stop");
        }
        else
        {
            //otrLogMessageGetstringName("otr_prog_I_stop");
        }
        showOtrStatus();
    }
}

void LLFloaterIMPanel::doOtrAuth()
{
    if (mOtrSmpDialog)
    {
        llinfos << "$PLOTR$ mOtrSmpDialog SMP already in progress, ignoring request to start it" << llendl;
        return;
    }
    if (mOtrSmpDialog || mOtrSmpProgress)
    {
        llinfos << "$PLOTR$ SMP already in progress, ignoring request to start it" << llendl;
        // $TODO$ Tell the user nicely to cancel the one in progress.
        // $TODO$ better yet, cancel it/them for the user
        return;
    }
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        llinfos << "$PLOTR$ otr menu auth" << llendl;
    
        ConnContext *context = getOtrContext();
        if (!context)
        {
            llwarns << "$PLOTR$ doOtrAuth can't find context." << llendl;
            return;
        }
        char my_uuid[UUID_STR_SIZE];
        gAgent.getID().toString(&(my_uuid[0]));
        char my_fingerprint[45];
        otrl_privkey_fingerprint(gOTR->get_userstate(),
                                 my_fingerprint,
                                 my_uuid,
                                 gOTR->get_protocolid());
        char other_fingerprint[45];
        otrl_privkey_hash_to_human(other_fingerprint, context->active_fingerprint->fingerprint);
        startSmpDialog(mSessionUUID, mOtherParticipantUUID,
                       &(my_fingerprint[0]), &(other_fingerprint[0]));
    }
}

void LLFloaterIMPanel::doOtrMenu()
{
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        LLComboBox *combo = getChild<LLComboBox>("otr_combo");
        if (!combo)
        {
            llwarns << "$PLOTR$ Can't find OTR control/status" << llendl;
        }
        else
        {
            std::string choice = combo->getSimple();
            if ((getString("otr_start") == choice) ||
                (getString("otr_restart") == choice) ||
                (getString("otr_refresh") == choice))
            {
                doOtrStart();
            }
            else if (getString("otr_stop") == choice)
            {
                doOtrStop();
            }
            else if (getString("otr_auth") == choice)
            {
                doOtrAuth();
            }
            else if (getString("otr_help") == choice)
            {
                llinfos << "$PLOTR$ otr help" << llendl;
                LLWeb::loadURL("http://www.cypherpunks.ca/otr/");
            }
            else if (getString("otr_levels") == choice)
            {
                llinfos << "$PLOTR$ otr levels help" << llendl;
                LLWeb::loadURL("http://www.cypherpunks.ca/otr/help/3.2.0/levels.php");
            }
            else
            {
                llwarns << "$PLOTR$ unknown menu item" << llendl;
            }
        }
    }
}
    
ConnContext *LLFloaterIMPanel::getOtrContext(int create_if_not_found, int *context_added)
{
    ConnContext *context = NULL;
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        char my_uuid[UUID_STR_SIZE];
        char their_uuid[UUID_STR_SIZE];
        gAgent.getID().toString(&(my_uuid[0]));
        mOtherParticipantUUID.toString(&(their_uuid[0]));
        context = otrl_context_find(
            gOTR->get_userstate(), 
            their_uuid,
            my_uuid,
            gOTR->get_protocolid(),
            create_if_not_found, context_added, NULL, NULL);
    }
    return context;
}

bool LLFloaterIMPanel::otherIsOtrAuthenticated()
{
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        ConnContext *context = getOtrContext();
        if (context && context->active_fingerprint &&
            context->active_fingerprint->trust &&
            *(context->active_fingerprint->trust))
        {
            llinfos << "$PLOTR$ they are authenticated -- trust level is "
                    << (context->active_fingerprint->trust) << llendl;
            return true;
        }
    }
    llinfos << "$PLOTR$ they are NOT authenticated" << llendl;
    return false;
}

void LLFloaterIMPanel::showOtrStatus()
{
    if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
    {
        LLComboBox *combo = getChild<LLComboBox>("otr_combo");
        if (!combo)
        {
            llwarns << "$PLOTR$ Can't find OTR control/status" << llendl;
        }
        else
        {
            ConnContext *context = getOtrContext();
            U32 otrpref = gSavedSettings.getU32("EmeraldUseOTR");
            // otrpref: 0 == Require OTR, 1 == Request OTR, 2 == Accept OTR, 3 == Decline OTR
            if (3 == otrpref)
            {
                if (context && (OTRL_MSGSTATE_ENCRYPTED == context->msgstate))
                {
                    doOtrStop();
                }
                combo->removeall();
                combo->add(getString("otr_start"),   ADD_BOTTOM, TRUE); // to tell them where to turn it back on
                combo->add(getString("otr_stop"),    ADD_BOTTOM, FALSE);
                combo->add(getString("otr_auth"),    ADD_BOTTOM, FALSE);
                combo->add(getString("otr_help"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_levels"),  ADD_BOTTOM, TRUE);
                combo->setLabel(getString("otr_not_private"));
            }
            else if (context && (OTRL_MSGSTATE_ENCRYPTED == context->msgstate))
            {
                combo->removeall();
                combo->add(getString("otr_refresh"), ADD_BOTTOM, TRUE);
                combo->add(getString("otr_stop"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_auth"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_help"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_levels"),  ADD_BOTTOM, TRUE);
                if (otherIsOtrAuthenticated())
                    combo->setLabel(getString("otr_private"));
                else
                    combo->setLabel(getString("otr_unverified"));
            }
            else if (context && (OTRL_MSGSTATE_FINISHED == context->msgstate))
            {
                if (OTRL_MSGSTATE_ENCRYPTED == mOtrLastStatus)
                {
                    if (otherIsOtrAuthenticated())
                        otrLogMessageGetstringName("otr_prog_they_stop_private");
                    else
                        otrLogMessageGetstringName("otr_prog_they_stop_unverified");
                }
                combo->removeall();
                combo->add(getString("otr_restart"), ADD_BOTTOM, TRUE);
                combo->add(getString("otr_stop"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_auth"),    ADD_BOTTOM, FALSE);
                combo->add(getString("otr_help"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_levels"),  ADD_BOTTOM, TRUE);
                combo->setLabel(getString("otr_finished"));
            }
            else // OTRL_MSGSTATE_PLAINTEXT, or no context yet
            {
                combo->removeall();
                combo->add(getString("otr_start"),   ADD_BOTTOM, TRUE);
                combo->add(getString("otr_stop"),    ADD_BOTTOM, FALSE);
                combo->add(getString("otr_auth"),    ADD_BOTTOM, FALSE);
                combo->add(getString("otr_help"),    ADD_BOTTOM, TRUE);
                combo->add(getString("otr_levels"),  ADD_BOTTOM, TRUE);
                combo->setLabel(getString("otr_not_private"));
            }
            if (context)
            {
                mOtrLastStatus = context->msgstate;
            }
        }
    }    
}

void LLFloaterIMPanel::otrLogMessage(std::string message)
{
    addHistoryLine(message, gSavedSettings.getColor("SystemChatColor"), true, mOtherParticipantUUID);
}

void LLFloaterIMPanel::otrLogMessageGetstring(const char *message_name)
{
    LLUIString msg = getString(message_name);
    otrLogMessage(msg);
}

void LLFloaterIMPanel::otrLogMessageGetstringName(const char *message_name)
{
    LLUIString msg = getString(message_name);
    std::string them;
    if (!gCacheName->getFullName(mOtherParticipantUUID, them)) them = getString("otr_generic_name");
    msg.setArg("[NAME]", them);
    otrLogMessage(msg);
}

void otr_log_message(LLUUID session_id, const char *message)
{
	LLFloaterIMPanel* floater = gIMMgr->findFloaterBySession(session_id);
    if (floater) floater->otrLogMessage(message);
    else
    {
        llinfos << "$PLOTR$ otr_log_message(" << message << ") failed to find floater." << llendl;
    }
}

void otr_log_message_getstring(LLUUID session_id, const char *message_name)
{
	LLFloaterIMPanel* floater = gIMMgr->findFloaterBySession(session_id);
    if (floater) floater->otrLogMessageGetstring(message_name);
    else
    {
        llinfos << "$PLOTR$ otr_log_message_getstring(" << message_name << ") failed to find floater." << llendl;
    }
}

void otr_log_message_getstring_name(LLUUID session_id, const char *message_name)
{
	LLFloaterIMPanel* floater = gIMMgr->findFloaterBySession(session_id);
    if (floater) floater->otrLogMessageGetstringName(message_name);
    else
    {
        llinfos << "$PLOTR$ otr_log_message_getstring_name(" << message_name << ") failed to find floater." << llendl;
    }
}

void LLFloaterIMPanel::otrAuthenticateKey(const char *trust)
{
    int context_added = 0;
    ConnContext *context = getOtrContext(0, &context_added);
    if (gOTR && context)
    {
        otrl_context_set_trust(context->active_fingerprint, trust);
        otrLogMessageGetstringName("otr_log_authenticated");
        otrLogMessageGetstringName("otr_log_start_private");
        std::string pubpath =
            gDirUtilp->getExpandedFilename(
                LL_PATH_PER_SL_ACCOUNT, OTR_PUBLIC_KEYS_FILE);
        otrl_privkey_write_fingerprints(gOTR->get_userstate(), pubpath.c_str());
        showOtrStatus();
    }
}

void otr_authenticate_key(LLUUID session_id, const char *trust)
{
	LLFloaterIMPanel* floater = gIMMgr->findFloaterBySession(session_id);
    if (floater) floater->otrAuthenticateKey(trust);
    else
    {
        llinfos << "$PLOTR$ otr_authenticate_key(" << session_id << ", " << trust << ") failed to find floater." << llendl;
    }
}

void otr_show_status(LLUUID session_id)
{
	LLFloaterIMPanel* floater = gIMMgr->findFloaterBySession(session_id);
    if (floater) floater->showOtrStatus();
    else
    {
        llinfos << "$PLOTR$ can't find floater." << llendl;
    }
}

void LLFloaterIMPanel::pretendTheyOtrStop()
{
    llinfos << "$PLOTR$ pretending they did doOtrStop()" << llendl;
    // we really stop our end, but...
    doOtrStop(true); // ... pretend that they did it
    ConnContext *context = getOtrContext();
    if (context) context->msgstate = OTRL_MSGSTATE_FINISHED;
    else
    {
        llwarns << "$PLOTR$ can't find context." << llendl;
    }
    showOtrStatus();
}

void LLFloaterIMPanel::startSmpDialog(
    LLUUID session_id, LLUUID other_id,
    std::string my_fingerprint, std::string other_fingerprint)
{
    if (mOtrSmpDialog)
    {
        mOtrSmpDialog->close();
        delete mOtrSmpDialog;
    }
    mOtrSmpDialog = new OtrFloaterSmpDialog(
        this, mSessionUUID, mOtherParticipantUUID,
        my_fingerprint, other_fingerprint);
    if (mOtrSmpDialog) mOtrSmpDialog->show();
    else
    {
        llwarns << "$PLOTR$ couldn't new OtrFloaterSmpDialog" << llendl;
    }
}

void LLFloaterIMPanel::startSmpDialogQA(
    LLUUID session_id, LLUUID other_id, std::string question, OtrlTLV *tlv)
{
    if (mOtrSmpDialog)
    {
        mOtrSmpDialog->close();
        delete mOtrSmpDialog;
    }
    mOtrSmpDialog = new OtrFloaterSmpDialog(
        this, mSessionUUID, mOtherParticipantUUID, question, tlv);
    if (mOtrSmpDialog) mOtrSmpDialog->show();
    else
    {
        llwarns << "$PLOTR$ couldn't new OtrFloaterSmpDialog" << llendl;
    }
}

void LLFloaterIMPanel::startSmpDialogSS(
    LLUUID session_id, LLUUID other_id, OtrlTLV *tlv)
{
    if (mOtrSmpDialog)
    {
        mOtrSmpDialog->close();
        delete mOtrSmpDialog;
    }
    mOtrSmpDialog = new OtrFloaterSmpDialog(
        this, mSessionUUID, mOtherParticipantUUID, tlv);
    if (mOtrSmpProgress) mOtrSmpDialog->show();
    else
    {
        llwarns << "$PLOTR$ couldn't new OtrFloaterSmpDialog" << llendl;
    }
}

void LLFloaterIMPanel::endSmpDialog()
{
    if (!mOtrSmpDialog)
    {
        llwarns << "$PLOTR$ couldn't find OtrFloaterSmpDialog" << llendl;
    }
    else 
    {
        delete mOtrSmpDialog;
        mOtrSmpDialog = NULL;
    }
}

void LLFloaterIMPanel::startSmpProgress(
    LLUUID session_id, LLUUID other_id,
    std::string a_question, std::string a_secret_answer, bool is_reply)
{
    if (mOtrSmpProgress)
    {
        mOtrSmpProgress->close();
        delete mOtrSmpProgress;
    }
    mOtrSmpProgress =
        new OtrFloaterSmpProgress(this, mSessionUUID, mOtherParticipantUUID,
                                  a_question, a_secret_answer, is_reply);
    if (mOtrSmpProgress) mOtrSmpProgress->show();
    else
    {
        llwarns << "$PLOTR$ couldn't new OtrFloaterSmpProgress" << llendl;
    }
}

void LLFloaterIMPanel::startSmpProgress(
    LLUUID session_id, LLUUID other_id,
    std::string a_secret, bool is_reply)
{
    if (mOtrSmpProgress)
    {
        mOtrSmpProgress->close();
        delete mOtrSmpProgress;
    }
    mOtrSmpProgress =
        new OtrFloaterSmpProgress(this, mSessionUUID, mOtherParticipantUUID,
                                  a_secret, is_reply);
    if (mOtrSmpProgress) mOtrSmpProgress->show();
    else
    {
        llwarns << "$PLOTR$ couldn't new OtrFloaterSmpProgress" << llendl;
    }
}

void LLFloaterIMPanel::endSmpProgress()
{
    if (!mOtrSmpProgress)
    {
        llwarns << "$PLOTR$ couldn't find OtrFloaterSmpProgress" << llendl;
    }
    else 
    {
        delete mOtrSmpProgress;
        mOtrSmpProgress = NULL;
    }
}

void LLFloaterIMPanel::handleOtrTlvs(OtrlTLV *tlvs)
{
    ConnContext *context = getOtrContext();
    if (! context)
    {
        llwarns << "$PLOTR$ Can't find otr context" << llendl;
        return;
    }
    if (! context->smstate)
    {
        llwarns << "$PLOTR$ OTR context doesn't have smstate" << llendl;
        return;
    }
    if (context->smstate->sm_prog_state == OTRL_SMP_PROG_CHEATED)
    {
        if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
        context->smstate->nextExpected = OTRL_SMP_EXPECT1;
        context->smstate->sm_prog_state = OTRL_SMP_PROG_OK;
        return;
    }
    NextExpectedSMP nextMsg = context->smstate->nextExpected;
    OtrlTLV *tlv = NULL;
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP1Q);
	if (tlv)
    {
	    if (nextMsg != OTRL_SMP_EXPECT1)
        {
            if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
            return;
        }
        // Start a challenge SMP dialog
        char *question = (char *)tlv->data;
        char *eoq = (char *)memchr(question, '\0', tlv->len);
        if (!eoq)
        {
            llwarns << "$PLOTR$ bad format in OTRL_TLV_SMP1Q, no end to question." << llendl;
            if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
            return;
        }
        startSmpDialogQA(mSessionUUID, mOtherParticipantUUID, question, tlv);
        if (mOtrSmpProgress) mOtrSmpProgress->setPercent(25);
        return;
    }
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP1);
	if (tlv)
    {
	    if (nextMsg != OTRL_SMP_EXPECT1)
        {
            if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
            return;
        }
        // Start a challenge SMP dialog
        startSmpDialogSS(mSessionUUID, mOtherParticipantUUID, tlv);
        if (mOtrSmpProgress) mOtrSmpProgress->setPercent(25);
        return;
    }
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP2);
	if (tlv)
    {
	    if (nextMsg != OTRL_SMP_EXPECT2)
        {
            if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
            return;
        }
	    else
        {
            // If we received TLV2, we will send TLV3 and expect TLV4
            context->smstate->nextExpected = OTRL_SMP_EXPECT4;
            if (mOtrSmpProgress) mOtrSmpProgress->setPercent(75);
	    }
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP3);
	if (tlv)
    {
	    if (nextMsg != OTRL_SMP_EXPECT3)
        {
            if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
            return;
        }
	    else
        {
            // If we received TLV3, we will send TLV4
            // We will not expect more messages, so prepare for next SMP
            context->smstate->nextExpected = OTRL_SMP_EXPECT1;
            // Report result to user
            if (context->smstate->sm_prog_state == OTRL_SMP_PROG_SUCCEEDED)
            {
                if (context->active_fingerprint &&
                    context->active_fingerprint->trust &&
                    *(context->active_fingerprint->trust))
                {
                    // they authed me OK, and I already authed them in the past
                    if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_ok");
                }
                else
                {
                    // they authed me OK, but I haven't authed them yet
                    if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_ok_name_next");
                }
            }
            else
            {
                if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_failed");
            }
	    }
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP4);
	if (tlv)
    {
	    if (nextMsg != OTRL_SMP_EXPECT4)
        {
            if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_errored");
            return;
        }
	    else {
            // We will not expect more messages, so prepare for next SMP
            context->smstate->nextExpected = OTRL_SMP_EXPECT1;
            // Report result to user
            if (context->active_fingerprint &&
                context->active_fingerprint->trust &&
                *(context->active_fingerprint->trust))
            {
                if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_ok");
            }
            else
            {
                if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_failed");
            }
	    }
	}
	tlv = otrl_tlv_find(tlvs, OTRL_TLV_SMP_ABORT);
	if (tlv)
    {
	    // The message we are waiting for will not arrive, so reset
	    // and prepare for the next SMP
	    context->smstate->nextExpected = OTRL_SMP_EXPECT1;
        if (mOtrSmpProgress) mOtrSmpProgress->setFinalStatus("otr_smp_prog_auth_aborted");
	}
}
#endif // USE_OTR // [/$PLOTR$]

void LLFloaterIMPanel::sendMsg(bool ooc)
{
	if (!gAgent.isGodlike() 
		&& (mDialog == IM_NOTHING_SPECIAL)
		&& mOtherParticipantUUID.isNull())
	{
		llinfos << "Cannot send IM to everyone unless you're a god." << llendl;
		return;
	}

	if (mInputEditor)
	{
		LLWString text = mInputEditor->getConvertedText();
		if(!text.empty())
		{
			if(ooc)
			{
				std::string tempText=mInputEditor->getText();
				tempText = gSavedSettings.getString("EmeraldOOCPrefix") + " " + tempText + " " + gSavedSettings.getString("EmeraldOOCPostfix");
				mInputEditor->setText(tempText);
				text = utf8str_to_wstring(tempText);
			}
			// Truncate and convert to UTF8 for transport
			std::string utf8_text = wstring_to_utf8str(text);
			if (gSavedSettings.getBOOL("EmeraldAutoCloseOOC"))
			{
				if(utf8_text.length() > 2)
				{
					// Chalice - OOC autoclosing patch based on code by Henri Beauchamp
					int needsClosingType=0;
					if (utf8_text.find("((") == 0 && utf8_text.find("))") == -1)
						needsClosingType=1;
					else if(utf8_text.find("[[") == 0 && utf8_text.find("]]") == -1)
						needsClosingType=2;
					if(needsClosingType==1)
					{
						if(utf8_text.at(utf8_text.length() - 1) == ')')
							utf8_text+=" ";
						utf8_text+="))";
					}
					else if(needsClosingType==2)
					{
						if(utf8_text.at(utf8_text.length() - 1) == ']')
							utf8_text+=" ";
						utf8_text+="]]";
					}
					needsClosingType=0;
					if (utf8_text.find("((") == -1 && utf8_text.find("))") == (utf8_text.length() - 2))
						needsClosingType=1;
					else if (utf8_text.find("[[") == -1 && utf8_text.find("]]") == (utf8_text.length() - 2))
						needsClosingType=2;
					if(needsClosingType==1)
					{
						if(utf8_text.at(0) == '(')
							utf8_text.insert(0," ");
						utf8_text.insert(0,"((");
					}
					else if(needsClosingType==2)
					{
						if(utf8_text.at(0) == '[')
							utf8_text.insert(0," ");
						utf8_text.insert(0,"[[");
					}
				}
			}
			// Convert MU*s style poses into IRC emotes here.
			if (gSavedSettings.getBOOL("EmeraldAllowMUpose") && utf8_text.find(":") == 0 && utf8_text.length() > 3)
			{
				if (utf8_text.find(":'") == 0)
				{
					utf8_text.replace(0, 1, "/me");
 				}
				else if (isalpha(utf8_text.at(1)))	// Do not prevent smileys and such.
				{
					utf8_text.replace(0, 1, "/me ");
				}
			}
			//utf8_text = utf8str_truncate(utf8_text, MAX_MSG_BUF_SIZE - 1);

// [RLVa:KB] - Alternate: Snowglobe-1.2.4 | Checked: 2009-07-10 (RLVa-1.0.0g) | Modified: RLVa-1.0.0g
			if (gRlvHandler.hasBehaviour(RLV_BHVR_SENDIM))
			{
				if (IM_NOTHING_SPECIAL == mDialog)			// One-on-one IM: allow if recipient is a sendim exception
				{
					if (!gRlvHandler.isException(RLV_BHVR_SENDIM, mOtherParticipantUUID))
						utf8_text = RlvStrings::getString(RLV_STRING_BLOCKED_SENDIM);
				}
				else if (gAgent.isInGroup(mSessionUUID))	// Group chat: allow if recipient is a sendim exception
				{
					if (!gRlvHandler.isException(RLV_BHVR_SENDIM, mSessionUUID))
						utf8_text = RlvStrings::getString(RLV_STRING_BLOCKED_SENDIM);
				}
				else if (mSpeakers)							// Conference chat: allow if all participants are sendim exceptions
				{
					LLSpeakerMgr::speaker_list_t speakers;
					mSpeakers->getSpeakerList(&speakers, TRUE);

					for (LLSpeakerMgr::speaker_list_t::const_iterator itSpeaker = speakers.begin(); 
							itSpeaker != speakers.end(); ++itSpeaker)
					{
						LLSpeaker* pSpeaker = *itSpeaker;
						if ( (gAgent.getID() != pSpeaker->mID) && (!gRlvHandler.isException(RLV_BHVR_SENDIM, pSpeaker->mID)) )
						{
							utf8_text = RlvStrings::getString(RLV_STRING_BLOCKED_SENDIM);
							break;
						}
					}
				}
				else										// Catch all fall-through
				{
					utf8_text = RlvStrings::getString(RLV_STRING_BLOCKED_SENDIM);
				}
			}
// [/RLVa:KB]
			
			if ( mSessionInitialized )
			{
#if USE_OTR // [$PLOTR$]
                const LLRelationship* info = NULL;
                info = LLAvatarTracker::instance().getBuddyInfo(mOtherParticipantUUID);
                ConnContext *context = getOtrContext(1);
                if (info && (!info->isOnline()) && context &&
                    (   (OTRL_MSGSTATE_ENCRYPTED == context->msgstate)
                     || (OTRL_MSGSTATE_FINISHED == context->msgstate)))
                {
                    // we can't continue this encrypted session but we
                    // can't let the user accidentally send unencrypted
                    if (OTRL_MSGSTATE_ENCRYPTED == context->msgstate)
                        pretendTheyOtrStop();
                    otrLogMessageGetstringName("otr_err_offline_send");
                    return; // leave the unsent message in the edit box
                }

                gcry_error_t err = 0;
                char *newmessage = NULL;
                char my_uuid[UUID_STR_SIZE];
                char their_uuid[UUID_STR_SIZE];
                gAgent.getID().toString(&(my_uuid[0]));
                mOtherParticipantUUID.toString(&(their_uuid[0]));

                bool was_finished = false;
                if (gOTR && context && (context->msgstate == OTRL_MSGSTATE_FINISHED))
                {
                    was_finished = true;
                }
                else if (gOTR && (IM_NOTHING_SPECIAL == mDialog))
                {
                    // only try OTR for 1 on 1 IM's
                    err = otrl_message_sending(
                        gOTR->get_userstate(), 
                        gOTR->get_uistate(), 
                        &mSessionUUID,
                        my_uuid,
                        gOTR->get_protocolid(),
                        their_uuid,
                        &(utf8_text[0]), NULL, &newmessage,
                        NULL, NULL);
                }
                context = getOtrContext();
                if (err)
                {
                    otrLogMessageGetstring("otr_err_failed_sending");
                    return; // leave the unsent message in the edit box
                }
                if (was_finished)
                {
                    llinfos << "$PLOTR$ OTR tried to send into finished conv, not sending message!" << llendl;
                    //otrLogMessageGetstringName("otr_err_send_in_finished"); //Don't error and tell the user to restart, just restart instead!
					doOtrStart();
                    return; // leave the unsent message in the edit box
                }
                OtrlMessageType msgtype = OTRL_MSGTYPE_NOTOTR;
                if (newmessage) msgtype = otrl_proto_message_type(newmessage);
                if (newmessage && (OTRL_MSGTYPE_TAGGEDPLAINTEXT == msgtype))
                {
                    // OTR just added the whitespace tag.
                    otrl_message_free(newmessage); // don't send the message with whitespace tag
                    err = otrl_message_sending(
                        gOTR->get_userstate(), 
                        gOTR->get_uistate(), 
                        &mSessionUUID,
                        my_uuid,
                        gOTR->get_protocolid(),
                        their_uuid,
                        "typing", NULL, &newmessage,
                        NULL, NULL);
                    if (!newmessage)
                    {
                        llwarns << "$PLOTR$ shouldn't happen, OTR should keep adding whitespace tags till we get a reply from them." << llendl;
                    }
                    else
                    {
                        // deliver a whitespace tagged "typing" in a IM_TYPING_STOP packet
                        std::string my_name;
                        gAgent.buildFullname(my_name);
                        const LLRelationship* info = NULL;
                        info = LLAvatarTracker::instance().getBuddyInfo(mOtherParticipantUUID);
                        U8 offline = (!info || info->isOnline()) ? IM_ONLINE : IM_OFFLINE;
                        pack_instant_message(
                            gMessageSystem,
                            gAgent.getID(),
                            FALSE,
                            gAgent.getSessionID(),
                            mOtherParticipantUUID,
                            my_name,
                            newmessage,
                            offline,
                            IM_TYPING_STOP,
                            mSessionUUID);
                        gAgent.sendReliableMessage();
                        otrl_message_free(newmessage);
                        newmessage = NULL;
                    }
                }
                if (newmessage)
                {
                    // OTR encrypted the message
                    if (! context)
                    {
                        llwarns << "$PLOTR$ can't find context, not sending message." << llendl;
                        otrLogMessageGetstring("otr_err_failed_sending");
                        return; // leave the unsent message in the edit box
                    }

                    {
                        // Handle fragmentation of the message
                        char *extrafragment = NULL;
                        err = otrl_message_fragment_and_send(
                            gOTR->get_uistate(), 
                            &mSessionUUID,
                            context,
                            newmessage,
                            OTRL_FRAGMENT_SEND_ALL,
                            &extrafragment);
                    }
                    if (newmessage) otrl_message_free(newmessage);
                    showOtrStatus();
                }
                else
                {   // OTR didn't encrypt, or we didn't try cause it's not 1:1 IM
#endif // USE_OTR // [/$PLOTR$]
                    // same code like in llchatbar.cpp
                    U32 split = MAX_MSG_BUF_SIZE - 1;

                    if ( isEncrypted() ) split = 799; // Length left for message if encrypted
                    U32 pos = 0;
                    U32 total = utf8_text.length();
				
                    while(pos < total)
                    {
                        U32 next_split = split;

                        if(pos + next_split > total) next_split = total - pos;

                        // don't split utf-8 bytes
                        while(U8(utf8_text[pos + next_split]) >= 0x80 && U8(utf8_text[pos + next_split]) < 0xC0
                              && next_split > 0)
                        {
                            --next_split;
                        }

                        if(next_split == 0)
                        {
                            next_split = split;
                            LL_WARNS("Splitting") << "utf-8 couldn't be split correctly" << LL_ENDL;
                        }

                        std::string send = utf8_text.substr(pos, pos + next_split);
                        pos += next_split;
						
						
						
						if( mDialog == IM_PRIVATE_IRC)
						{
							glggIrcGroupHandler.trySendPrivateImToID(utf8_text,mOtherParticipantUUID,false);
						}
						else
                        // *FIXME: Queue messages if IM is not IM_NOTHING_SPECIAL
                        deliver_message(encrypt(send),
                                        mSessionUUID,
                                        mOtherParticipantUUID,
                                        mDialog);
                    }
                }
#if USE_OTR       // [$PLOTR$]
            }
#endif // USE_OTR // [/$PLOTR$]
            // local echo
            if((mDialog == IM_NOTHING_SPECIAL) && 
               (mOtherParticipantUUID.notNull()))
            {
                std::string history_echo;
                gAgent.buildFullname(history_echo);

                // Look for IRC-style emotes here.
                std::string prefix = utf8_text.substr(0, 4);
                if (prefix == "/me " || prefix == "/me'")
                {
                    if(isEncrypted())
                    {
                        utf8_text.replace(0,3,"\xe2\x80\xa7");
                    }
                    else
                    {
                        utf8_text.replace(0,3,"");
                    }
                }
                else
                {
                    if(isEncrypted())
                    {
                        history_echo += "\xe2\x80\xa7: ";
                    }
                    else
                    {
                        history_echo += ": ";
                    }
                }
                history_echo += utf8_text;

                BOOL other_was_typing = mOtherTyping;
                if(isEncrypted())
                {
                    addHistoryLine(history_echo, gSavedSettings.getColor("IMEncryptedChatColor"), true, gAgent.getID());
                }
                else
                {
                    addHistoryLine(history_echo, gSavedSettings.getColor("IMChatColor"), true, gAgent.getID());
                }

                if (other_was_typing) 
                {
                    addTypingIndicator(mOtherTypingName);
                }

            }
			else
			{
				//queue up the message to send once the session is
				//initialized
				mQueuedMsgsForInit.append(utf8_text);
			}
		}

		LLViewerStats::getInstance()->incStat(LLViewerStats::ST_IM_COUNT);

	}
	mInputEditor->setText(LLStringUtil::null);

	// Don't need to actually send the typing stop message, the other
	// client will infer it from receiving the message.
	mTyping = FALSE;
	mSentTypingState = TRUE;
}

void LLFloaterIMPanel::updateSpeakersList(const LLSD& speaker_updates)
{
	mSpeakers->updateSpeakers(speaker_updates); 
}

void LLFloaterIMPanel::processSessionUpdate(const LLSD& session_update)
{
	if (
		session_update.has("moderated_mode") &&
		session_update["moderated_mode"].has("voice") )
	{
		BOOL voice_moderated = session_update["moderated_mode"]["voice"];

		if (voice_moderated)
		{
			setTitle(mSessionLabel + std::string(" ") + getString("moderated_chat_label"));
		}
		else
		{
			setTitle(mSessionLabel);
		}


		//update the speakers dropdown too
		mSpeakerPanel->setVoiceModerationCtrlMode(voice_moderated);
	}
}

void LLFloaterIMPanel::setSpeakers(const LLSD& speaker_list)
{
	mSpeakers->setSpeakers(speaker_list);
}
void LLFloaterIMPanel::setIRCSpeakers(const LLSD& speaker_list)
{
	mSpeakers->setIrcSpeakers(speaker_list);
}

void LLFloaterIMPanel::sessionInitReplyReceived(const LLUUID& session_id)
{
	mSessionUUID = session_id;
	mVoiceChannel->updateSessionID(session_id);
	mSessionInitialized = TRUE;

	//we assume the history editor hasn't moved at all since
	//we added the starting session message
	//so, we count how many characters to remove
	S32 chars_to_remove = mHistoryEditor->getWText().length() -
		mSessionStartMsgPos;
	mHistoryEditor->removeTextFromEnd(chars_to_remove);

	//and now, send the queued msg
	LLSD::array_iterator iter;
	for ( iter = mQueuedMsgsForInit.beginArray();
		  iter != mQueuedMsgsForInit.endArray();
		  ++iter)
	{
		deliver_message(
			iter->asString(),
			mSessionUUID,
			mOtherParticipantUUID,
			mDialog);
	}
}

void LLFloaterIMPanel::requestAutoConnect()
{
	mAutoConnect = TRUE;
}

void LLFloaterIMPanel::setTyping(BOOL typing)
{
	if (typing)
	{
		// Every time you type something, reset this timer
		mLastKeystrokeTimer.reset();

		if (!mTyping)
		{
			// You just started typing.
			mFirstKeystrokeTimer.reset();

			// Will send typing state after a short delay.
			mSentTypingState = FALSE;
		}

		mSpeakers->setSpeakerTyping(gAgent.getID(), TRUE);
	}
	else
	{
		if (mTyping)
		{
			// you just stopped typing, send state immediately
			sendTypingState(FALSE);
			mSentTypingState = TRUE;
		}
		mSpeakers->setSpeakerTyping(gAgent.getID(), FALSE);
	}

	mTyping = typing;
}

void LLFloaterIMPanel::sendTypingState(BOOL typing)
{
	// Don't want to send typing indicators to multiple people, potentially too
	// much network traffic.  Only send in person-to-person IMs.
	if (mDialog != IM_NOTHING_SPECIAL) return;
	std::string name;
	gAgent.buildFullname(name);

	pack_instant_message(
		gMessageSystem,
		gAgent.getID(),
		FALSE,
		gAgent.getSessionID(),
		mOtherParticipantUUID,
		name,
		std::string("typing"),
		IM_ONLINE,
		(typing ? IM_TYPING_START : IM_TYPING_STOP),
		mSessionUUID);
	gAgent.sendReliableMessage();
}

void LLFloaterIMPanel::processIMTyping(const LLIMInfo* im_info, BOOL typing)
{
	if (typing)
	{
		// other user started typing
		addTypingIndicator(im_info->mName);
	}
	else
	{
		// other user stopped typing
		removeTypingIndicator(im_info);
	}

	// user is online, enable teleport button
	if(mDialog == IM_NOTHING_SPECIAL)
	{
		childSetEnabled("profile_tele_btn", true);
	}
}


void LLFloaterIMPanel::addTypingIndicator(const std::string &name)
{
	// we may have lost a "stop-typing" packet, don't add it twice
	if (!mOtherTyping)
	{
		mTypingLineStartIndex = mHistoryEditor->getWText().length();
		LLUIString typing_start = sTypingStartString;
		typing_start.setArg("[NAME]", name);
		addHistoryLine(typing_start, gSavedSettings.getColor4("SystemChatColor"), false);
		mOtherTypingName = name;
		mOtherTyping = TRUE;
	}
	// MBW -- XXX -- merge from release broke this (argument to this function changed from an LLIMInfo to a name)
	// Richard will fix.
//	mSpeakers->setSpeakerTyping(im_info->mFromID, TRUE);
}


void LLFloaterIMPanel::removeTypingIndicator(const LLIMInfo* im_info)
{
	if (mOtherTyping)
	{
		// Must do this first, otherwise addHistoryLine calls us again.
		mOtherTyping = FALSE;

		S32 chars_to_remove = mHistoryEditor->getWText().length() - mTypingLineStartIndex;
		mHistoryEditor->removeTextFromEnd(chars_to_remove);
		if (im_info)
		{
			mSpeakers->setSpeakerTyping(im_info->mFromID, FALSE);
		}
	}
}

//static
void LLFloaterIMPanel::chatFromLogFile(LLLogChat::ELogLineType type, std::string line, void* userdata)
{
	LLFloaterIMPanel* self = (LLFloaterIMPanel*)userdata;
	std::string message = line;

	switch (type)
	{
	case LLLogChat::LOG_EMPTY:
		// add warning log enabled message
		if (gSavedPerAccountSettings.getBOOL("LogInstantMessages"))
		{
			message = LLFloaterChat::getInstance()->getString("IM_logging_string");
		}
		break;
	case LLLogChat::LOG_END:
		// add log end message
		if (gSavedPerAccountSettings.getBOOL("LogInstantMessages"))
		{
			message = LLFloaterChat::getInstance()->getString("IM_logging_string");
		}
		break;
	case LLLogChat::LOG_LINE:
		// just add normal lines from file
		break;
	default:
		// nothing
		break;
	}

	//self->addHistoryLine(line, LLColor4::grey, FALSE);
	self->mHistoryEditor->appendColoredText(message, false, true, LLColor4::grey);
}

// user is known to be offline when we receive this
void LLFloaterIMPanel::setOffline()
{
	if(!gAgent.isGodlike())
	{
		childSetEnabled("profile_tele_btn", false);
	}
#if USE_OTR       // [$PLOTR$]
    llinfos << "$PLOTR$ friend went offline" << llendl;
    if (gOTR)
    {
        ConnContext *context = getOtrContext();
        if (context && (context->msgstate == OTRL_MSGSTATE_ENCRYPTED))
        {
            pretendTheyOtrStop();
        }
    }
#endif // USE_OTR // [/$PLOTR$]
}

void LLFloaterIMPanel::showSessionStartError(
	const std::string& error_string)
{
	//the error strings etc. should be really be static and local
	//to this file instead of in the LLFloaterIM
	//but they were in llimview.cpp first and unfortunately
	//some translations into non English languages already occurred
	//thus making it a tad harder to change over to a
	//"correct" solution.  The best solution
	//would be to store all of the misc. strings into
	//their own XML file which would be read in by any LLIMPanel
	//post build function instead of repeating the same info
	//in the group, adhoc and normal IM xml files.
	LLSD args;
	args["REASON"] =
		LLFloaterIM::sErrorStringsMap[error_string];
	args["RECIPIENT"] = getTitle();

	LLSD payload;
	payload["session_id"] = mSessionUUID;

	LLNotifications::instance().add(
		"ChatterBoxSessionStartError",
		args,
		payload,
		onConfirmForceCloseError);
}

void LLFloaterIMPanel::showSessionEventError(
	const std::string& event_string,
	const std::string& error_string)
{
	LLSD args;
	args["REASON"] =
		LLFloaterIM::sErrorStringsMap[error_string];
	args["EVENT"] =
		LLFloaterIM::sEventStringsMap[event_string];
	args["RECIPIENT"] = getTitle();

	LLNotifications::instance().add(
		"ChatterBoxSessionEventError",
		args);
}

void LLFloaterIMPanel::showSessionForceClose(
	const std::string& reason_string)
{
	LLSD args;

	args["NAME"] = getTitle();
	args["REASON"] = LLFloaterIM::sForceCloseSessionMap[reason_string];

	LLSD payload;
	payload["session_id"] = mSessionUUID;

	LLNotifications::instance().add(
		"ForceCloseChatterBoxSession",
		args,
		payload,
		LLFloaterIMPanel::onConfirmForceCloseError);

}

//static 
void LLFloaterIMPanel::onKickSpeaker(void* user_data)
{

}

bool LLFloaterIMPanel::onConfirmForceCloseError(const LLSD& notification, const LLSD& response)
{
	//only 1 option really
	LLUUID session_id = notification["payload"]["session_id"];

	if ( gIMMgr )
	{
		LLFloaterIMPanel* floaterp = gIMMgr->findFloaterBySession(
			session_id);

		if ( floaterp ) floaterp->close(FALSE);
	}
	return false;
}

bool LLFloaterIMPanel::isEncrypted()
{
#if USE_OTR       // [$PLOTR$]
    if (gOTR)
    {
        ConnContext *context = getOtrContext();
        if (context && (context->msgstate == OTRL_MSGSTATE_ENCRYPTED)) return true;
    }    
#endif // USE_OTR // [/$PLOTR$]
	LLLineEditor *password_editor = getChild<LLLineEditor>("password");

	if(password_editor)
	{
		std::string password = password_editor->getText();
		if(!password.empty())
		{
			return true;
		}
	}

	return false;
}

std::string LLFloaterIMPanel::encrypt(const std::string &msg)
{
	LLLineEditor *password_editor = getChild<LLLineEditor>("password");

	if(password_editor)
	{
		std::string password = password_editor->getText();
		if(!password.empty())
		{
			std::string salt("01234567");

			for(int i = 0; i < 4; i++)
			{
				U16 x = U16(ll_rand(RAND_MAX));
				salt[i*2] = x % 94 + 33;
				x /= 94;
				salt[i*2+1] = x % 94 + 33;
			}

			EGenKey key(password, reinterpret_cast<const U8*>(salt.c_str()));
			EAESEncrypt enc(key.key(), key.iv());
			std::vector<U8> crypted = enc.encrypt(msg);
			return std::string("\xdf\xbf") + salt + EAscii85::encode(crypted);
		}
	}
	return msg;
}

bool LLFloaterIMPanel::decryptMsg(const std::string& msg, std::string& decrypted_msg)
{
	LLLineEditor *password_editor = getChild<LLLineEditor>("password");

	if(password_editor)
	{
		if(msg.length() < 13)
			return false;
		if(msg[0] != '\xdf' || msg[1] != '\xbf' || msg[10] != '<' || msg[11] != '~')
			return false;

		const unsigned char *salt = reinterpret_cast<const U8*>(msg.c_str()) + 2;
		std::string password = password_editor->getText();
		
		if(!password.empty())
		{
			EGenKey key(password, salt);
			EAESDecrypt dec(key.key(), key.iv());
			std::vector<U8> crypted = EAscii85::decode(msg.substr(10));
			if(crypted.size() < 16)
				return false;
			decrypted_msg = dec.decrypt(crypted);
			return true;
		}
	}

	return false;
}
