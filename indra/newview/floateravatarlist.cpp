/** 
 * @author Dale Glass <dale@daleglass.net>, (C) 2007
 */

/**
 * Rewritten by jcool410
 * Altered it to work in 1.22.8
 * Removed usage of globals
 * Removed TrustNET
 * Added utilization of "minimap" data
 * Added bridge info -lgg
 */

// rewritten again by skills

#include "llviewerprecompiledheaders.h"

#include "llavatarconstants.h"
#include "llappviewer.h"
#include "floateravatarlist.h"
#include "lluictrlfactory.h"
#include "llviewerwindow.h"
#include "llscrolllistctrl.h"
#include "llviewercontrol.h"
#include "llvoavatar.h"
#include "llimview.h"
#include "llfloateravatarinfo.h"
#include "llregionflags.h"
#include "llfloaterreporter.h"
#include "llagent.h"
#include "llselectmgr.h"
#include "llviewerregion.h"
#include "lltracker.h"
#include "llviewercontrol.h"
#include "llviewerstats.h"
#include "llerror.h"
#include "llviewermessage.h"
#include "llweb.h"
#include "llviewerobjectlist.h"
#include "llmutelist.h"
#include "llchat.h"
#include "llchatbar.h"
#include "llfloaterchat.h"
#include "chatbar_as_cmdline.h"
#include "llcallbacklist.h"

// [RLVa:KB] - Phoenix specific
#include "rlvhandler.h"
// [/RLVa:KB]

#include <time.h>
#include <string.h>
#include <map>

#include "llworld.h"
#include "llsdutil.h"
#include "jc_lslviewerbridge.h"
#include "v3dmath.h"
#include "scriptcounter.h"
#include "llfloaterregioninfo.h"
#include "llfocusmgr.h"

#include "llmenugl.h"
#include "llviewermenu.h"
#include "llcombobox.h"
#include "llfloateractivespeakers.h"
#include "llimpanel.h" // for LLVoiceChannel

void cmdline_printchat(std::string message);
void send_chat_from_viewer(std::string utf8_out_text, EChatType type, S32 channel);

const F32 MIN_REQUEST_INTERVAL   = 1.0f; //How many seconds to wait between data requests
const F32 FIRST_REQUEST_TIMEOUT  = 16.0f; //How long to wait for a request to arrive during the first try in seconds
const F32 MAX_REQUEST_TIMEOUT    = 2048.0f; //Delay is doubled on each attempt. This is as high as it'll go
const F32 REQUEST_FAIL_TIMEOUT   = 15.0f; //How long to wait for a request to arrive before assuming failure
const F32 DEAD_KEEP_TIME		 = 10.0f; //How long to keep people who are gone in the list.
const F32 CLEANUP_TIMEOUT		 = 3600.0f; //How long to keep entries around before removing them.
extern U32 gFrameCount;

LLAvatarInfo::LLAvatarInfo()
{

}

LLAvatarInfo::LLAvatarInfo(PAYMENT_TYPE payment, ACCOUNT_TYPE account, struct tm birth)
{
	Payment = payment;
	Account = account;
	BirthDate = birth;
}

S32 LLAvatarInfo::getAge()
{
	time_t birth = mktime(&BirthDate);
	time_t now = time(NULL);
	return(S32)(difftime(now,birth) / (60*60*24));
}
typedef enum e_radar_alert_type
{
	ALERT_TYPE_SIM = 0,
	ALERT_TYPE_DRAW = 1,
	ALERT_TYPE_CHATRANGE = 2,
	ALERT_TYPE_AGE = 3
} ERadarAlertType;
void chat_avatar_status(std::string name, LLUUID key, ERadarAlertType type, bool entering)
{
	if(gSavedSettings.getBOOL("PhoenixRadarChatAlerts"))
	{
// [RLVa:KB] - Alternate: Phoenix-370
		if (gRlvHandler.hasBehaviour(RLV_BHVR_SHOWNAMES))
		{
			name = RlvStrings::getAnonym(name);
		}
// [/RLVa:KB]

		LLChat chat;
		switch(type)
		{
		case ALERT_TYPE_SIM:
			if(gSavedSettings.getBOOL("PhoenixRadarAlertSim"))
		{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());
				chat.mText = name+" has "+(entering ? "entered" : "left")+" the sim.";// ("+key.asString()+")";
		}
			break;
		case ALERT_TYPE_DRAW:
			if(gSavedSettings.getBOOL("PhoenixRadarAlertDraw"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());
				chat.mText = name+" has "+(entering ? "entered" : "left")+" draw distance.";// ("+key.asString()+")";
			}
			break;
		case ALERT_TYPE_CHATRANGE:
			if(gSavedSettings.getBOOL("PhoenixRadarAlertChatRange"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());
				chat.mText = name+" has "+(entering ? "entered" : "left")+" chat range.";// ("+key.asString()+")";
			}
			break;
		case ALERT_TYPE_AGE:
			if(gSavedSettings.getBOOL("PhoenixAvatarAgeAlert"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());

				make_ui_sound("PhoenixAvatarAgeAlertSoundUUID");
				chat.mText = name+" has triggered your avatar age alert.";
			}
			break;
		}
		if(chat.mText != "")
		{
				chat.mSourceType = CHAT_SOURCE_SYSTEM;
				LLFloaterChat::addChat(chat);
		}
	}
}

LLAvatarListEntry::LLAvatarListEntry(const LLUUID& id, const std::string &name, const LLVector3d &position, BOOL isLinden, BOOL isVoice) :
		mID(id), mName(name), mTime(time(NULL)), mPosition(position), mDrawPosition(), mAlert(FALSE), mMarked(FALSE), mFocused(FALSE), mIsLinden(isLinden), mIsVoice(isVoice), mAccountTitle(""),
			mUpdateTimer(), mFrame(gFrameCount), mInSimFrame(U32_MAX), mInDrawFrame(U32_MAX), mInChatFrame(U32_MAX)
{
}

void LLAvatarListEntry::setPosition(LLVector3d position, bool this_sim, bool drawn, bool chatrange)
{
	if ( drawn )
	{
		mDrawPosition = position;
	} 
	else if( mInDrawFrame == U32_MAX ) 
	{
		mDrawPosition.setZero();
	}

	//lgg if we already got a Z value from the bridge, dont over write it here
	if(position.mdV[VZ] == 0.0)
	{
		mPosition.mdV[VX] = position.mdV[VX];
		mPosition.mdV[VY] = position.mdV[VY];
		mNeedBridgeAssist = true;
	}else
	{
		//all normal
		mNeedBridgeAssist = false;
		mPosition = position;
	}

	mFrame = gFrameCount;
	if(this_sim)
	{
		if(mInSimFrame == U32_MAX)
		{
			chat_avatar_status(mName,mID,ALERT_TYPE_SIM, true);
		}
		mInSimFrame = mFrame;
	}
	if(drawn)
	{
		if(mInDrawFrame == U32_MAX)chat_avatar_status(mName,mID,ALERT_TYPE_DRAW, true);
		mInDrawFrame = mFrame;
	}
	if(chatrange)
	{
		if(mInChatFrame == U32_MAX)chat_avatar_status(mName,mID,ALERT_TYPE_CHATRANGE, true);
		mInChatFrame = mFrame;
	}
	mUpdateTimer.start();
}

LLVector3d LLAvatarListEntry::getPosition()
{
	return mPosition;
}

BOOL LLAvatarListEntry::getNeedsBridgeAssist()
{
	return mNeedBridgeAssist;
}

bool LLAvatarListEntry::getAlive()
{
	U32 current = gFrameCount;
	if(mInSimFrame != U32_MAX && (current - mInSimFrame) >= 2)
	{
		mInSimFrame = U32_MAX;
		chat_avatar_status(mName,mID,ALERT_TYPE_SIM, false);
		if(gSavedSettings.getBOOL("PhoenixRadarChatKeys"))
		{
			gMessageSystem->newMessage("ScriptDialogReply");
			 gMessageSystem->nextBlock("AgentData");
			 gMessageSystem->addUUID("AgentID", gAgent.getID());
			 gMessageSystem->addUUID("SessionID", gAgent.getSessionID());
			 gMessageSystem->nextBlock("Data");
			 gMessageSystem->addUUID("ObjectID", gAgent.getID());
			 gMessageSystem->addS32("ChatChannel", gSavedSettings.getS32("PhoenixRadarChatKeysChannel"));
			 gMessageSystem->addS32("ButtonIndex", 1);
			gMessageSystem->addString("ButtonLabel",llformat("%d,%d,", gFrameCount, 0) + mID.asString());
			gAgent.sendReliableMessage();
		}
	}
    if(mInDrawFrame != U32_MAX && (current - mInDrawFrame) >= 2)
    {
            mInDrawFrame = U32_MAX;
            chat_avatar_status(mName,mID,ALERT_TYPE_DRAW, false);
    }
    if(mInChatFrame != U32_MAX && (current - mInChatFrame) >= 2)
    {
            mInChatFrame = U32_MAX;
            chat_avatar_status(mName,mID,ALERT_TYPE_CHATRANGE, false);
    }
	return ((current - mFrame) <= 2);
}

F32 LLAvatarListEntry::getEntryAgeSeconds()
{
	return mUpdateTimer.getElapsedTimeF32();
}

void LLAvatarListEntry::setName(std::string name)
{
	if ( name.empty() || (name.compare(" ") == 0))
	{
		//llwarns << "Trying to set empty name" << llendl;
	}
	mName = name;
}

std::string LLAvatarListEntry::getName()
{
	return mName;
}

time_t LLAvatarListEntry::getTime()
{
	return mTime;
}

void LLAvatarListEntry::resetTime()
{
	mTime = time(NULL);
}

LLUUID LLAvatarListEntry::getID()
{
	return mID;
}

void LLAvatarListEntry::setID(LLUUID id)
{
	if (!id.isNull())
		mID = id;
}

BOOL LLAvatarListEntry::getIsVoice()
{
	return mIsVoice;
}

void LLAvatarListEntry::setIsVoice(BOOL voice)
{
	mIsVoice = voice;
}

BOOL LLAvatarListEntry::getIsLinden()
{
	return ( mIsLinden || ( mAvatarInfo.getValue().Account == ACCOUNT_EMPLOYEE ) );
}

void LLAvatarListEntry::setAccountCustomTitle(std::string &title)
{
	mAccountTitle = title;
	mAvatarInfo.getValue().Account = ACCOUNT_CUSTOM;
}

std::string LLAvatarListEntry::getAccountCustomTitle()
{
	return mAccountTitle;
}
		
void LLAvatarListEntry::setAlert()
{
	mAlert = TRUE;
}
				
BOOL LLAvatarListEntry::getAlert()
{
	return mAlert;
}

void LLAvatarListEntry::toggleMark()
{
	mMarked = !mMarked;
}

BOOL LLAvatarListEntry::isMarked()
{
	return mMarked;
}

class LggPosCallback : public JCBridgeCallback
{
	public:
	LggPosCallback(std::vector<LLUUID> inavatars)
	{
		avatars = inavatars;
	}

	void fire(LLSD data)
	{
		//printchat("lol, \n"+std::string(LLSD::dumpXML(data)));
		LLFloaterAvatarList::processBridgeReply(avatars,data);
	}

	private:
	std::vector<LLUUID> avatars;
};


BOOL LLAvatarListEntry::isDead()
{
	return getEntryAgeSeconds() > DEAD_KEEP_TIME;
}

LLFloaterAvatarList* LLFloaterAvatarList::sInstance = NULL;
					
					
LLFloaterAvatarList::LLFloaterAvatarList() :  LLFloater(std::string("avatar list"))
{
	llassert_always(sInstance == NULL);
	sInstance = this;

	(new LLAgentProfile())->registerListener(this, "Avatarlist.Profile");
	(new LLAgentChatcmd())->registerListener(this, "Avatarlist.Chatcmd");
	(new LLAgentGetkey())->registerListener(this, "Avatarlist.Getkey");
	(new LLAgentTrack())->registerListener(this, "Avatarlist.Track");
	(new LLAgentMark())->registerListener(this, "Avatarlist.Mark");
	(new LLAgentIM())->registerListener(this, "Avatarlist.IM");
	(new LLAgentScripts())->registerListener(this, "Avatarlist.Scripts");
	(new LLAgentTpto())->registerListener(this, "Avatarlist.Tpto");
	(new LLAgentOffertp())->registerListener(this, "Avatarlist.Offertp");

	(new LLAgentMute())->registerListener(this, "Avatarlist.Mute");
	(new LLAgentUnmute())->registerListener(this, "Avatarlist.Unmute");
	(new LLAgentAr())->registerListener(this, "Avatarlist.Ar");
	(new LLAgentEject())->registerListener(this, "Avatarlist.Eject");
	(new LLAgentBan())->registerListener(this, "Avatarlist.Ban");
	(new LLAgentFreeze())->registerListener(this, "Avatarlist.Freeze");
	(new LLAgentUnfreeze())->registerListener(this, "Avatarlist.Unfreeze");

	(new LLAgentEstatekick())->registerListener(this, "Avatarlist.Estatekick");
	(new LLAgentEstateban())->registerListener(this, "Avatarlist.Estateban");
	(new LLAgentEstatetphome())->registerListener(this, "Avatarlist.Estatetphome");
	(new LLAgentEstategtfo())->registerListener(this, "Avatarlist.Estategtfo");

	LLMenuGL* menu = LLUICtrlFactory::getInstance()->buildMenu("menu_avatarlist.xml", this);
	if (!menu)
	{
		menu = new LLMenuGL(LLStringUtil::null);
	}
	menu->setVisible(FALSE);
	mPopupMenuHandle = menu->getHandle();
}

LLFloaterAvatarList::~LLFloaterAvatarList()
{
	gIdleCallbacks.deleteFunction( LLFloaterAvatarList::callbackIdle );
	sInstance = NULL;
}

BOOL LLFloaterAvatarList::handleRightMouseDown(S32 x, S32 y, MASK mask)
{
	LLFloaterAvatarList* self = getInstance();
	self->setFocus( TRUE );
	mAvatarList->handleRightMouseDown(x-7,y-30,mask);

	LLMenuGL* menu = (LLMenuGL*)mPopupMenuHandle.get();
	if (menu)
	{
		menu->buildDrawLabels();
		menu->updateParent(LLMenuGL::sMenuContainer);
		LLMenuGL::showPopup(this, menu, x, y);
	}
	return TRUE;
}

BOOL LLFloaterAvatarList::handleKeyHere(KEY key, MASK mask)
{
	if (( KEY_RETURN == key ) && (MASK_NONE == mask))
	{
		LLFloaterAvatarList* self = getInstance();
 		LLScrollListItem *item =   self->mAvatarList->getFirstSelected();
		LLUUID agent_id = item->getUUID();
		lookAtAvatar(agent_id);
		return TRUE;
	}

	if (( KEY_RETURN == key ) && (MASK_CONTROL == mask))
	{
		LLFloaterAvatarList* avlist = getInstance();
		LLScrollListItem *item = avlist->mAvatarList->getFirstSelected();

		if ( item )
		{
			LLUUID agent_id = item->getUUID();
			LLAvatarListEntry *ent = avlist->getAvatarEntry(agent_id);
			if ( ent )
			{
				gAgent.teleportViaLocation( ent->getPosition() );
			}
		}

		return TRUE;
	}
	return LLPanel::handleKeyHere(key, mask);
}

static void cmd_profile(const LLUUID& avatar, const std::string &name)
{
	LLFloaterAvatarInfo::showFromDirectory(avatar);
}
bool LLFloaterAvatarList::LLAgentProfile::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* self = getInstance();
	self->doCommand(cmd_profile);
	return true;
}

static void cmd_saycmd(const LLUUID& avatar, const std::string &avname)
{
	LLWString text = utf8str_to_wstring(gSavedPerAccountSettings.getString("PhoenixAvatarlistCustomAction"));
	if (!text.empty())
	{
		LLChatBar* chatp = new LLChatBar();

		EChatType nType = CHAT_TYPE_NORMAL;
		S32 channel;
		text = chatp->stripChannelNumber(text, &channel);
		
		std::string utf8text = wstring_to_utf8str(text);

		//---------- placeholder stuff
		std::string key_wildcard = "#k";
		std::string name_wildcard = "#n";
		//Extract Name
		std::string key = avatar.asString().c_str();
		std::string name = std::string(avname);

		//Handle Replacements
		size_t found = utf8text.find(key_wildcard);
		while(found != std::string::npos)
		{
			utf8text.replace(found, 2, key);
			found = utf8text.find(key_wildcard);
		}

		found = utf8text.find(name_wildcard);
		while(found != std::string::npos)
		{
			utf8text.replace(found, 2, name);
			found = utf8text.find(name_wildcard);
		}
		//----------

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
		if (!utf8_revised_text.empty() && cmd_line_chat(utf8_revised_text, nType))
		{
			send_chat_from_viewer(utf8_revised_text, nType, channel);
		}
	}
}

bool LLFloaterAvatarList::LLAgentChatcmd::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* self = getInstance();
	self->doCommand(cmd_saycmd);
	return true;
}

bool LLFloaterAvatarList::LLAgentGetkey::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* self = getInstance();
 	LLScrollListItem *item = self->mAvatarList->getFirstSelected();
	if (!item ) return true;

	LLUUID agent_id = item->getUUID();
	char buffer[UUID_STR_LENGTH];		/*Flawfinder: ignore*/
	agent_id.toString(buffer);

	gViewerWindow->mWindow->copyTextToClipboard(utf8str_to_wstring(buffer));
	return true;
}

bool LLFloaterAvatarList::LLAgentTrack::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
 	LLScrollListItem *item =   avlist->mAvatarList->getFirstSelected();
	if (!item) return true;

	LLUUID agent_id = item->getUUID();

	if ( avlist->mTracking && avlist->mTrackedAvatar == agent_id )
	{
		LLTracker::stopTracking(NULL);
		avlist->mTracking = FALSE;
	}
	else
	{
		avlist->mTracking = TRUE;
		avlist->mTrackByLocation = FALSE;
		avlist->mTrackedAvatar = agent_id;
		LLTracker::trackAvatar(agent_id, avlist->mAvatars[agent_id].getName());
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentMark::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	LLDynamicArray<LLUUID> ids = avlist->mAvatarList->getSelectedIDs();

	for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
	{
		LLUUID avid = *itr;
		LLAvatarListEntry *ent = avlist->getAvatarEntry(avid);
		if ( ent != NULL )
		{
			ent->toggleMark();
		}
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentIM::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	LLDynamicArray<LLUUID> ids = avlist->mAvatarList->getSelectedIDs();
	if(ids.size() > 0)
	{
		if(ids.size() == 1)
		{
			// Single avatar
			LLUUID agent_id = ids[0];
			char buffer[MAX_STRING];
			snprintf(buffer, MAX_STRING, "%s", avlist->mAvatars[agent_id].getName().c_str());
			gIMMgr->setFloaterOpen(TRUE);
			gIMMgr->addSession(buffer,IM_NOTHING_SPECIAL,agent_id);
		}
		else
		{
			// Group IM
			LLUUID session_id;
			session_id.generate();
			gIMMgr->setFloaterOpen(TRUE);
			gIMMgr->addSession("Avatars Conference", IM_SESSION_CONFERENCE_START, ids[0], ids);
		}
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentOffertp::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	LLDynamicArray<LLUUID> ids = avlist->mAvatarList->getSelectedIDs();
	if(ids.size() > 0)
	{
		handle_lure(ids);
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentTpto::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	LLScrollListItem *item = avlist->mAvatarList->getFirstSelected();

	if ( item )
	{
		LLUUID agent_id = item->getUUID();
		LLAvatarListEntry *ent = avlist->getAvatarEntry(agent_id);
		if ( ent )
		{
			llinfos << "Trying to teleport to " << ent->getName() << " at " << ent->getPosition() << llendl;
			gAgent.teleportViaLocation( ent->getPosition() );
		}
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentMute::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* self = getInstance();
	LLDynamicArray<LLUUID> ids = self->mAvatarList->getSelectedIDs();
	if(ids.size() > 0)
	{
		for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
		{
			LLUUID agent_id = *itr;
			std::string agent_name;
			if(gCacheName->getFullName(agent_id, agent_name))
			{
				// *NOTE: Users may click on Mute multiple times to ensure a person is muted
				// there is no visual feedback given in Avatar List as of now
				if (LLMuteList::getInstance()->isMuted(agent_id))
				{
					//LLMute mute(agent_id, agent_name, LLMute::AGENT);
					//LLMuteList::getInstance()->remove(mute);	
					//LLFloaterMute::getInstance()->selectMute(agent_id);
				}
				else
				{
					LLMute mute(agent_id, agent_name, LLMute::AGENT);
					LLMuteList::getInstance()->add(mute);
				}
			}
		}
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentUnmute::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* self = getInstance();
	LLDynamicArray<LLUUID> ids = self->mAvatarList->getSelectedIDs();
	if(ids.size() > 0)
	{
		for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
		{
			LLUUID agent_id = *itr;
			std::string agent_name;
			if(gCacheName->getFullName(agent_id, agent_name))
			{
				if (LLMuteList::getInstance()->isMuted(agent_id))
				{
					LLMute mute(agent_id, agent_name, LLMute::AGENT);
					LLMuteList::getInstance()->remove(mute);	
				}
			}
		}
	}
	return true;
}

bool LLFloaterAvatarList::LLAgentAr::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
 	LLScrollListItem *item = avlist->mAvatarList->getFirstSelected();
	if ( item )
	{
		LLUUID agent_id = item->getUUID();
		LLAvatarListEntry *ent = avlist->getAvatarEntry(agent_id);

		if ( ent )
		{
			LLFloaterReporter::showFromObject(ent->getID());
		}
	}
	return true;
}

std::string keyasname(LLUUID id)
{
	std::string ret;
	gCacheName->getFullName(id,ret);
	return ret;
}
static void send_freeze(const LLUUID& avatar_id, bool freeze)
{
	U32 flags = 0x0;
	if (!freeze)
	{
		// unfreeze
		flags |= 0x1;
	}

	LLMessageSystem* msg = gMessageSystem;
	LLViewerObject* avatar = gObjectList.findObject(avatar_id);

	if (avatar)
	{
		msg->newMessage("FreezeUser");
		msg->nextBlock("AgentData");
		msg->addUUID("AgentID", gAgent.getID());
		msg->addUUID("SessionID", gAgent.getSessionID());
		msg->nextBlock("Data");
		msg->addUUID("TargetID", avatar_id );
		msg->addU32("Flags", flags );
		msg->sendReliable( avatar->getRegion()->getHost() );
	}
}

static void cmd_freeze(const LLUUID& avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Freezing "+ keyasname(avatar)+".");
	send_freeze(avatar, true);
}

static void cmd_unfreeze(const LLUUID& avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Unfreezing "+ keyasname(avatar)+".");
	send_freeze(avatar, false);
}

bool LLFloaterAvatarList::LLAgentFreeze::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	avlist->doCommand(cmd_freeze);
	return true;
}

bool LLFloaterAvatarList::LLAgentUnfreeze::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	avlist->doCommand(cmd_unfreeze);
	return true;
}


static void send_eject(const LLUUID& avatar_id, bool ban)
{	
	LLMessageSystem* msg = gMessageSystem;
	LLViewerObject* avatar = gObjectList.findObject(avatar_id);

	if (avatar)
	{
		U32 flags = 0x0;
		if ( ban )
		{
			// eject and add to ban list
			flags |= 0x1;
		}

		msg->newMessage("EjectUser");
		msg->nextBlock("AgentData");
		msg->addUUID("AgentID", gAgent.getID() );
		msg->addUUID("SessionID", gAgent.getSessionID() );
		msg->nextBlock("Data");
		msg->addUUID("TargetID", avatar_id );
		msg->addU32("Flags", flags );
		msg->sendReliable( avatar->getRegion()->getHost() );
	}
}


static void cmd_eject(const LLUUID& avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Ejecting "+ keyasname(avatar)+".");
	send_eject(avatar, false);
}

bool LLFloaterAvatarList::LLAgentEject::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	if(gSavedSettings.getBOOL("PhoenixModerateConfirm"))
	{
		LLSD args;
		args["AVATAR_NAME"] = avlist->getSelectedNames();
		LLNotifications::instance().add("EjectAvatarFullnameNoBan", args, LLSD(), callbackEject);
	}
	else
	{
		avlist->doCommand(cmd_eject);
	}
	return true;
}

static void cmd_ban(const LLUUID& avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Ejecting and banning "+ keyasname(avatar)+".");
	send_eject(avatar, true);
}

bool LLFloaterAvatarList::LLAgentBan::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	avlist->doCommand(cmd_ban);
	return true;
}

bool LLFloaterAvatarList::LLAgentScripts::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
 	LLScrollListItem *item =   avlist->mAvatarList->getFirstSelected();
	if(!item)return true;
	LLViewerObject *obj=gObjectList.findObject(item->getUUID());
	if(obj)
	{
		LLSelectMgr::getInstance()->selectObjectOnly(obj);
		ScriptCounter::serializeSelection(false);
		LLSelectMgr::getInstance()->deselectAll();
	}
	return true;
}

typedef std::vector<std::string> strings_t;
static void sendEstateOwnerMessage( LLMessageSystem* msg,const std::string& request,const LLUUID& invoice,const strings_t& strings)
{
	llinfos << "Sending estate request '" << request << "'" << llendl;
	msg->newMessage("EstateOwnerMessage");
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->addUUIDFast(_PREHASH_TransactionID, LLUUID::null); //not used
	msg->nextBlock("MethodData");
	msg->addString("Method", request);
	msg->addUUID("Invoice", invoice);
	if(strings.empty())
	{
		msg->nextBlock("ParamList");
		msg->addString("Parameter", NULL);
	}
	else
	{
		strings_t::const_iterator it = strings.begin();
		strings_t::const_iterator end = strings.end();
		for(; it != end; ++it)
		{
			msg->nextBlock("ParamList");
			msg->addString("Parameter", *it);
		}
	}
	msg->sendReliable(gAgent.getRegion()->getHost());
}

static void cmd_estate_eject(const LLUUID &avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Kicking "+ keyasname(avatar)+" from estate.");
	strings_t strings;
	strings.push_back(avatar.asString());
	sendEstateOwnerMessage(gMessageSystem, "kickestate", LLFloaterRegionInfo::getLastInvoice(), strings);
}

static void cmd_estate_ban(const LLUUID &avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Banning "+ keyasname(avatar)+" from estate.");
	//send_estate_message("teleporthomeuser", avatar); // Kick first, just to be sure
	LLPanelEstateInfo::sendEstateAccessDelta(ESTATE_ACCESS_BANNED_AGENT_ADD | ESTATE_ACCESS_ALLOWED_AGENT_REMOVE | ESTATE_ACCESS_NO_REPLY, avatar);
}

static void cmd_tph_estate(const LLUUID &avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Teleporting "+ keyasname(avatar)+" home.");
	//send_estate_message("teleporthomeuser", avatar); // Kick first, just to be sure
	//sendEstateAccessDelta(ESTATE_ACCESS_BANNED_AGENT_ADD | ESTATE_ACCESS_ALLOWED_AGENT_REMOVE, avatar);
	strings_t strings;
	// [0] = our agent id
	// [1] = target agent id
	std::string buffer;
	gAgent.getID().toString(buffer);
	strings.push_back(buffer);
	avatar.toString(buffer);
	strings.push_back(strings_t::value_type(buffer));
	LLUUID invoice(LLFloaterRegionInfo::getLastInvoice());
	sendEstateOwnerMessage(gMessageSystem, "teleporthomeuser", invoice, strings);
}


bool LLFloaterAvatarList::LLAgentEstatekick::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	LLSD args;
	LLSD payload;
	args["EVIL_USER"] = avlist->getSelectedNames();
	LLNotifications::instance().add("EstateKickUser", args, payload, callbackEjectFromEstate);
	return true;
}

bool LLFloaterAvatarList::LLAgentEstateban::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	avlist->doCommand(cmd_estate_ban);
	/*
	LLSD args;
	args["ALL_ESTATES"] = "all estates";
	LLNotifications::instance().add("EstateBannedAgentAdd", args, LLSD(), callbackBanFromEstate);*/
	return true;
}
	
bool LLFloaterAvatarList::LLAgentEstatetphome::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	avlist->doCommand(cmd_tph_estate);
	return true;
}

bool LLFloaterAvatarList::LLAgentEstategtfo::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	LLFloaterAvatarList* avlist = getInstance();
	avlist->doCommand(cmd_estate_ban);
	avlist->doCommand(cmd_estate_eject);
	//LLFloaterAvatarList* avlist = getInstance();
	/*
	LLAgentEstateban(userdata);
	LLAgentEstatekick(userdata);
	LLAgentEstatetphome(userdata);
	*/
	return true;
}

//static
void LLFloaterAvatarList::toggle(void*)
{
// [RLVa:KB] - Alternate: Phoenix-370
	if (gRlvHandler.hasBehaviour(RLV_BHVR_SHOWNAMES))
	{
		return;
	}
// [/RLVa:KB]

	if (sInstance)
	{
		if(sInstance->getVisible())
		{
			sInstance->close(false);
		}
		else
		{
			sInstance->open();
		}
	}
	else
	{
		showInstance();
	}
}

//static
void LLFloaterAvatarList::showInstance()
{
	if (sInstance)
						{
		if(!sInstance->getVisible())
		{
			sInstance->open();
		}
	}
	else
	{
		sInstance = new LLFloaterAvatarList();
		LLUICtrlFactory::getInstance()->buildFloater(sInstance, "floater_avatar_scanner.xml");
	}
}
								
void LLFloaterAvatarList::draw()
{
	LLFloater::draw();
}

void LLFloaterAvatarList::onOpen()
{
	gSavedSettings.setBOOL("ShowAvatarList", TRUE);
	sInstance->setVisible(TRUE);
}

void LLFloaterAvatarList::onClose(bool app_quitting)
{
	sInstance->setVisible(FALSE);
	if( !app_quitting )
	{
		gSavedSettings.setBOOL("ShowAvatarList", FALSE);
	}
	if ( !gSavedSettings.getBOOL("PhoenixAvatarListKeepOpen") || app_quitting )
	{
		destroy();
	}
}

BOOL LLFloaterAvatarList::postBuild()
{
	LLTabContainer* tab = getChild<LLTabContainer>("actions_tab_container");

	if(tab)
	{
		LLPanel* panel = tab->getPanelByName("custom_tab");

		if(panel)
		{
			tab->enableTabButton(tab->getIndexForPanel(panel), FALSE);
		}
	}

	mTracking = FALSE;
	mTrackByLocation = FALSE;
	mlastBridgeCallTime = gFrameTimeSeconds;

	childSetCommitCallback("agealert", onClickAgeAlert,this);
	childSetValue("agealert",gSavedSettings.getBOOL("PhoenixAvatarAgeAlert"));

	childSetCommitCallback("AgeAlertDays",onClickAgeAlertDays,this);
	childSetValue("AgeAlertDays",gSavedSettings.getF32("PhoenixAvatarAgeAlertDays"));

	childSetAction("say_btn", onClickSayCmd,this);
	childSetAction("tp_btn", onClickTP,this);
	childSetAction("profile_btn", onClickProfile, this);
	childSetAction("im_btn", onClickIM, this);
	childSetAction("offer_btn", onClickTeleportOffer, this);
	childSetAction("track_btn", onClickTrack, this);

	childSetAction("mark_btn", onClickMark, this);
	childSetAction("prev_marked_btn", onClickPrevMarked, this);
	childSetAction("next_marked_btn", onClickNextMarked, this);


	mInputEditor = getChild<LLLineEditor>("customcommand");
	childSetCommitCallback("customcommand", onCommandCommit,this);

	if (mInputEditor)
	{
		mInputEditor->setText(gSavedPerAccountSettings.getString("PhoenixAvatarlistCustomAction"));
		mInputEditor->setCommitOnFocusLost( TRUE );
		mInputEditor->setRevertOnEsc( FALSE );
		mInputEditor->setEnableLineHistory(TRUE);
	}

	// Get a pointer to the scroll list from the interface
	mAvatarList = getChild<LLScrollListCtrl>("avatar_list");

	mAvatarList->setCallbackUserData(this);
	mAvatarList->setDoubleClickCallback(onDoubleClick);
	mAvatarList->sortByColumn("distance", TRUE);
	mDataRequestTimer.start();
	mUpdateThrottleTimer.start();
	refreshAvatarList();

	gIdleCallbacks.addFunction( LLFloaterAvatarList::callbackIdle );
	return TRUE;
}

void LLFloaterAvatarList::updateAvatarList()
{
	if (gDisconnected) return;
	if ( sInstance != this ) return;
	if ( mUpdateThrottleTimer.getElapsedTimeF32() < 0.25) return;

	mUpdateThrottleTimer.reset();

	LLCheckboxCtrl* check;
	check = getChild<LLCheckboxCtrl>("update_enabled_cb");

	if ( !check->getValue() )return;

	LLVector3d mypos = gAgent.getPositionGlobal();

	std::vector<LLUUID> avatar_ids;
	std::vector<LLUUID> sorted_avatar_ids;
	std::vector<LLVector3d> positions;
	LLWorld::instance().getAvatars(&avatar_ids, &positions, mypos, F32_MAX);
	sorted_avatar_ids = avatar_ids;
	std::sort(sorted_avatar_ids.begin(), sorted_avatar_ids.end());

	for(std::vector<LLCharacter*>::const_iterator iter = LLCharacter::sInstances.begin(); iter != LLCharacter::sInstances.end(); ++iter)
	{
		LLUUID avid = (*iter)->getID();
		if(!std::binary_search(sorted_avatar_ids.begin(), sorted_avatar_ids.end(), avid))
		{
			avatar_ids.push_back(avid);
		}
	}

	std::string name;
	std::string first;
	std::string last;

	size_t i;
	size_t count = avatar_ids.size();
	for(i = 0; i < count; ++i)
	{
		const LLUUID &avid = avatar_ids[i];

		LLVector3d position;
		LLViewerObject *obj = gObjectList.findObject(avid);

		if(obj)
		{
			LLVOAvatar* avatarp = dynamic_cast<LLVOAvatar*>(obj);
			if (avatarp == NULL) continue;
			if (avatarp->isDead() || avatarp->isSelf() || avatarp->mIsDummy) continue;

			// Get avatar data
			position = gAgent.getPosGlobalFromAgent(avatarp->getCharacterPosition());
			name = avatarp->getFullname();

			// Apparently, sometimes the name comes out empty, with a " " name. This is because
			// getFullname concatenates first and last name with a " " in the middle.
			// This code will avoid adding a nameless entry to the list until it acquires a name.

			//duped for lower section
			if (name.empty() || (name.compare(" ") == 0))// || (name.compare(gCacheName->getDefaultName()) == 0))
			{
				if(gCacheName->getName(avid, first, last)) name = first + " " + last;
				else continue;
			}
			if (avid.isNull()) continue;
			if ( mAvatars.count( avid ) > 0 )
			{
				// Avatar already in list, update position
				mAvatars[avid].setPosition(position, (avatarp->getRegion() == gAgent.getRegion()), true, (position - mypos).magVec() < 20.0);
			}
			else
			{
				// Avatar not there yet, add it
				BOOL isLinden = ( strcmp(last.c_str(), "Linden") == 0 || last == "Linden" );
				LLAvatarListEntry entry(avid, name, position, isLinden, 0);
				mAvatars[avid] = entry;
			}
		}
		else
		{
			if( i < positions.size())
			{
				position = positions[i];
			}
			else continue;

			if(gCacheName->getName(avid, first, last)) name = first + " " + last;
			else continue; //prevent (Loading...)

			if ( mAvatars.count( avid ) > 0 )
			{
				// Avatar already in list, update position
				mAvatars[avid].setPosition(position, gAgent.getRegion()->pointInRegionGlobal(position), false, (position - mypos).magVec() < 20.0);
			}
			else
			{
				// Avatar not there yet, add it
				BOOL isLinden = last == "Linden";
				LLAvatarListEntry entry(avid, name, position, isLinden, 0);
				mAvatars[avid] = entry;
			}
		}
	}

	updateAvatarListVoice();
	expireAvatarList();
	refreshAvatarList();
	checkTrackingStatus();
	checkAnnouncements();
	//processARQueue();
}

void LLFloaterAvatarList::updateAvatarListVoice() 
{
	if (gDisconnected) return;
	if ( sInstance != this ) return;

	std::string name;
	std::string first;
	std::string last;

	LLVoiceClient::participantMap* participants = gVoiceClient->getParticipantList();
	if(participants)
	{
		LLVoiceClient::participantMap::iterator participant_it;

		for (participant_it = participants->begin(); participant_it != participants->end(); ++participant_it)
		{
			LLVoiceClient::participantState* participantp = participant_it->second;
			LLUUID participant_id = participantp->mAvatarID;

			if (participant_id.isNull()) continue;
			if (participant_id == gAgent.getID()) continue;

			if (mAvatars.count(participant_id) > 0)
			{
				mAvatars[participant_id].mIsVoice = TRUE; //if already in list, just give them voice
				continue;
			}

			name = std::string(participantp->mAccountName); //participantp->mDisplayName;

			llinfos << "voice participant not in list: " << participant_id.asString().c_str() << " - " << name << llendl;

			// The check against "==" is to check that we don't get a voice ID (which is in the form of base64 data)
			// Voice IDs are of constant length, and thus always have "==" padding at the end. "=" is otherwise illegal in
			// a name (for now).
			if (name.empty() || (name.compare(" ") == 0) || name.substr(name.length() - 2, 2) == "==")// || (name.compare(gCacheName->getDefaultName()) == 0))
			{
				if(gCacheName->getName(participant_id, first, last)) name = first + " " + last;
				else continue;
			}
			BOOL isLinden = ( strcmp(last.c_str(), "Linden") == 0 || last == "Linden" );
			LLAvatarListEntry entry(participant_id, name, LLVector3d::zero, isLinden, TRUE);
			mAvatars[participant_id] = entry;
			//setSpeaker(participantp->mAvatarID, participantp->mDisplayName, LLSpeaker::STATUS_VOICE_ACTIVE, (participantp->isAvatar()?LLSpeaker::SPEAKER_AGENT:LLSpeaker::SPEAKER_EXTERNAL));
		}
	}
}

void LLFloaterAvatarList::expireAvatarList()
{
	if(gDisconnected)return;

//	llinfos << "avatar list: expiring" << llendl;
	std::map<LLUUID, LLAvatarListEntry>::iterator iter;
	std::queue<LLUUID> delete_queue;

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		LLAvatarListEntry *ent = &iter->second;
		LLUUID av_id = ent->getID();

		if (ent->getIsVoice() && (gVoiceClient->findParticipantByID(av_id)==NULL))
		{
			ent->setIsVoice(FALSE);
			//delete_queue.push(av_id);
		}

        ent->getAlive();
		if ( ent->getEntryAgeSeconds() > CLEANUP_TIMEOUT )
		{
			//llinfos << "avatar list: expiring avatar " << ent->getName() << llendl;
			delete_queue.push(av_id);
		}
	}

	while(!delete_queue.empty())
	{
		mAvatars.erase(delete_queue.front());
		delete_queue.pop();
	}
}

void LLFloaterAvatarList::refreshAvatarList() 
{
	if(gDisconnected)return;

	// Don't update list when interface is hidden
	if (!sInstance->getVisible())return;

	LLCheckboxCtrl* fetch_data;
	fetch_data = getChild<LLCheckboxCtrl>("fetch_avdata_enabled_cb");

	// We rebuild the list fully each time it's refreshed
	// The assumption is that it's faster to refill it and sort than
	// to rebuild the whole list.
	LLDynamicArray<LLUUID> selected = mAvatarList->getSelectedIDs();
	S32 scrollpos = mAvatarList->getScrollPos();

	mAvatarList->deleteAllItems();
	LLVector3d mypos = gAgent.getPositionGlobal();
	std::map<LLUUID, LLAvatarListEntry>::iterator iter;
	std::string toSendToBridge("");
	std::vector<LLUUID> avatarsToSendToBridge;

	static BOOL *sPhoenixAvatarListColorEntries = rebind_llcontrol<BOOL>("PhoenixAvatarListColorEntries", &gSavedSettings, true);
	LLColor4 friend_color;
	LLColor4 muted_color;
	LLColor4 marked_color = LLColor4::cyan.getValue();
	if(*sPhoenixAvatarListColorEntries)
	{
		static LLColor4* sMapFriend = rebind_llcontrol<LLColor4>("MapFriend", &gColors, true);
		friend_color = (*sMapFriend).getValue();
		static LLColor4* sMapMuted = rebind_llcontrol<LLColor4>("MapMuted", &gColors, true);
		muted_color = (*sMapMuted).getValue();
	}

	LLColor4 av_color;
	static LLColor4* sDefaultListText = rebind_llcontrol<LLColor4>("DefaultListText", &gColors, true);

	const std::string icon_image_0 = "icn_active-speakers-dot-lvl0.tga";
	const std::string icon_image_1 = "icn_active-speakers-dot-lvl1.tga";
	const std::string icon_image_2 = "icn_active-speakers-dot-lvl2.tga";

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		LLSD element;
		LLUUID av_id;
		LLAvatarListEntry *ent = &iter->second;

		av_id = ent->getID();
		if ( av_id.isNull() ) continue;

		BOOL av_isvoice = ent->getIsVoice();
		LLVOAvatar *av = (LLVOAvatar*)gObjectList.findObject(av_id);
		BOOL flagForFedUpDistance = false;
		LLVector3d position = LLVector3d::zero;
		F32 distance = 0.0f;
		std::string icon = "";

		if (!av_isvoice)
		{
			// Skip if avatar hasn't been around
			if ( ent->isDead() ) continue;
		}

		position = ent->getPosition();
		//lgg stuff.. ok.. uhmm..
		
		LLVector3d delta = position - mypos;
		distance = (F32)delta.magVec();
		if(position.mdV[VZ] == 0.0)
		{
			flagForFedUpDistance = true;
			distance = 9000;
		}

		if(ent->getNeedsBridgeAssist() )
		{
			toSendToBridge += std::string("|") +av_id.asString();
			avatarsToSendToBridge.push_back(av_id);
			//we might not send this depending on the last time we did
		}
		
		delta.mdV[2] = 0.0f;
		F32 side_distance = (F32)delta.magVec();

		// HACK: Workaround for an apparent bug:
		// sometimes avatar entries get stuck, and are registered
		// by the client as perpetually moving in the same direction.
		// this makes sure they get removed from the visible list eventually
		if ( side_distance > 2048.0f) continue;



		LLAvatarInfo avinfo = ent->mAvatarInfo.getValue();
		DATA_STATUS avinfo_status = ent->mAvatarInfo.getStatus();

		element["id"] = av_id;
		if(*sPhoenixAvatarListColorEntries)
		{
			if ( is_agent_friend(av_id)) av_color = friend_color;
			else if (LLMuteList::getInstance()->isMuted(av_id)) av_color = muted_color;
			else av_color = (*sDefaultListText).getValue();
		}
		else
		{
			av_color = (*sDefaultListText).getValue();
		}
		
		// Marked avatars should always show up coloured
		if ( ent->isMarked() )
		{
			av_color = marked_color;
		}

		element["columns"][LIST_AVATAR_NAME]["color"] = av_color.getValue();

		if ( ent->getIsLinden() ) element["columns"][LIST_AVATAR_NAME]["font-style"] = "BOLD";

		element["columns"][LIST_AVATAR_NAME]["column"] = "avatar_name";
		element["columns"][LIST_AVATAR_NAME]["type"] = "text";
		element["columns"][LIST_AVATAR_NAME]["value"] = ent->getName().c_str();

		if (av_isvoice)
		{
			F32 power = gVoiceClient->getCurrentPower(av_id);
			S32 icon_image_idx = llmin(2, llfloor((power / LLVoiceClient::OVERDRIVEN_POWER_LEVEL) * 3.f));

			std::string icon_image_id;
			switch(icon_image_idx)
			{
			case 0:
				icon_image_id = icon_image_0;
				break;
			case 1:
				icon_image_id = icon_image_1;
				break;
			case 2:
				icon_image_id = icon_image_2;
				break;
			}

			LLColor4 icon_color;
			if (power >= LLVoiceClient::OVERDRIVEN_POWER_LEVEL)
			{
				icon_color.setVec(1.f, 0.f, 0.f, 1.f);
			}
			else
			{
				icon_color.setVec(0.f, 1.f, 0.f, 1.f);
			}
			//icon_cell->setValue(icon_image_id);

			element["columns"][LIST_AVATAR_VOICE]["column"] = "voice";
			element["columns"][LIST_AVATAR_VOICE]["type"] = "icon";
			element["columns"][LIST_AVATAR_VOICE]["value"] = icon_image_id; //"icn_active-speakers-dot-lvl0.tga";
			element["columns"][LIST_AVATAR_VOICE]["color"] = icon_color.getValue();
		}

		char temp[32];
		snprintf(temp, sizeof(temp), "%.2f", distance);

		element["columns"][LIST_DISTANCE]["column"] = "distance";
		element["columns"][LIST_DISTANCE]["type"] = "text";
		element["columns"][LIST_DISTANCE]["value"] = temp;
		if(flagForFedUpDistance)
		{
			//lgg fix for out of draw distance
			//element["columns"][LIST_DISTANCE]["value"] = std::string("(> "+llformat("%d", gSavedSettings.getF32("RenderFarClip") )+")");
			static F32 *sRenderFarClip = rebind_llcontrol<F32>("RenderFarClip", &gSavedSettings, true);
			element["columns"][LIST_DISTANCE]["value"] = llformat("> %d", (S32)(*sRenderFarClip) );
		}
		element["columns"][LIST_DISTANCE]["color"] = getAvatarColor(ent, distance, CT_DISTANCE).getValue();
		
		if ( avinfo_status == DATA_RETRIEVED )
		{
			static F32 *sPhoenixAvatarAgeAlertDays = rebind_llcontrol<F32>("PhoenixAvatarAgeAlertDays", &gSavedSettings, true);
			if ((avinfo.getAge() < (*sPhoenixAvatarAgeAlertDays)) && !ent->getAlert())
			{
				ent->setAlert();
				chat_avatar_status(ent->getName().c_str(),av_id,ALERT_TYPE_AGE, true);
			}

			element["columns"][LIST_AGE]["column"] = "age";
			element["columns"][LIST_AGE]["type"] = "text";
			element["columns"][LIST_AGE]["value"] = avinfo.getAge();
			element["columns"][LIST_AGE]["color"] = getAvatarColor(ent, distance, CT_AGE).getValue();
		}

		element["columns"][LIST_SIM]["column"] = "samesim";
		element["columns"][LIST_SIM]["type"] = "text";

		icon = "";
		if(av && av->isAvatar() && av->isTyping())
		{
			icon = "avatar_typing.tga";
		}
		else if(gAgent.getRegion()->pointInRegionGlobal(position))
		{
			icon = "account_id_green.tga";
		}
		if (!icon.empty() )
		{
			element["columns"][LIST_SIM].erase("color");
			element["columns"][LIST_SIM]["type"] = "icon";
			element["columns"][LIST_SIM]["value"] = icon;
		}

		icon = "";

		switch(avinfo_status)
		{				
			case DATA_UNKNOWN:
				icon = /*gViewerArt.getString(*/"info_unknown.tga"/*)*/;
				break;
			case DATA_REQUESTING:
				icon = /*gViewerArt.getString(*/"info_fetching.tga"/*)*/;
				break;
			case DATA_ERROR:
				icon =  /*gViewerArt.getString(*/"info_error.tga"/*)*/;
				break;
			case DATA_RETRIEVED:
				switch(avinfo.Payment)
				{
					case PAYMENT_NONE:
						break;
					case PAYMENT_ON_FILE:
						icon =  /*gViewerArt.getString(*/"payment_info_filled.tga"/*)*/;
						break;
					case PAYMENT_USED:
						icon =  /*gViewerArt.getString(*/"payment_info_used.tga"/*)*/;
						break;
					case PAYMENT_LINDEN:
						// confusingly named icon, maybe use something else
						icon =  /*gViewerArt.getString(*/"icon_top_pick.tga"/*)*/;
						break;
				}
				break;
		}

		element["columns"][LIST_PAYMENT]["column"] = "payment_data";
		element["columns"][LIST_PAYMENT]["type"] = "text";
	
		if ( !icon.empty() )
		{
			element["columns"][LIST_PAYMENT]["type"] = "icon";
			element["columns"][LIST_PAYMENT]["value"] =  icon;
		}

		S32 seentime = (S32)difftime( time(NULL) , ent->getTime() );
		S32 hours = (S32)(seentime / (60*60));
		S32 mins = (S32)((seentime - hours*(60*60)) / 60);
		S32 secs = (S32)((seentime - hours*(60*60) - mins*60));

		element["columns"][LIST_TIME]["column"] = "time";
		element["columns"][LIST_TIME]["type"] = "text";
		element["columns"][LIST_TIME]["color"] = (*sDefaultListText).getValue();
		element["columns"][LIST_TIME]["value"] = llformat("%d:%02d:%02d", hours,mins,secs);

		element["columns"][LIST_CLIENT]["column"] = "client";
		element["columns"][LIST_CLIENT]["type"] = "text";

		static LLColor4* sAvatarNameColor = rebind_llcontrol<LLColor4>("AvatarNameColor", &gColors, true);
	
		LLColor4 avatar_name_color = (*sAvatarNameColor);
		std::string client;

		static LLColor4 *sScrollUnselectedColor = rebind_llcontrol<LLColor4>("ScrollUnselectedColor", LLUI::sColorsGroup, true);

		if(av)
		{
			LLVOAvatar::resolveClient(avatar_name_color, client, av);
			if(client == "")
			{
				avatar_name_color = *sScrollUnselectedColor;
				client = "?";
			}
			element["columns"][LIST_CLIENT]["value"] = client.c_str();
			//element["columns"][LIST_CLIENT]["color"] = avatar_name_color.getValue();
		}
		else
		{
			element["columns"][LIST_CLIENT]["value"] = "Out Of Range";
			avatar_name_color = *sScrollUnselectedColor;
		}

		avatar_name_color = avatar_name_color * 0.5 + *sScrollUnselectedColor * 0.5;

		element["columns"][LIST_CLIENT]["color"] = avatar_name_color.getValue();
		

		// Add to list
		mAvatarList->addElement(element, ADD_BOTTOM);

		// Request data only if fetching avatar data is enabled
		if ( fetch_data->getValue() && ent->mAvatarInfo.requestIfNeeded() )
		{
			sendAvatarPropertiesRequest(av_id);
			//llinfos << "Data for avatar " << ent->getName() << " didn't arrive yet, retrying" << llendl;
		}
	}
	
	//lgg send batch of names to bridge
	if((toSendToBridge != "" ) && gSavedSettings.getBOOL("PhoenixUseBridgeRadar"))
	{
		F32 timeNow = gFrameTimeSeconds;
		if( (timeNow - mlastBridgeCallTime) > 20)
		{
			mlastBridgeCallTime = timeNow;
			//llinfos << "Sending data to the bridge: " << toSendToBridge << llendl;
			JCLSLBridge::bridgetolsl("pos"+toSendToBridge, new LggPosCallback(avatarsToSendToBridge));

		}
		
	}
	// finish
	mAvatarList->sortItems();
	mAvatarList->selectMultiple(selected);
	mAvatarList->setScrollPos(scrollpos);

	childSetEnabled("Say", mInputEditor->getText().size() > 0);

//	llinfos << "avatar list refresh: done" << llendl;

}

//static
void LLFloaterAvatarList::onClickMark(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	LLDynamicArray<LLUUID> ids = avlist->mAvatarList->getSelectedIDs();

	for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
	{
		LLUUID avid = *itr;
		LLAvatarListEntry *ent = avlist->getAvatarEntry(avid);
		if ( ent != NULL )
		{
			ent->toggleMark();
		}
	}
}

void LLFloaterAvatarList::removeFocusFromAll()
{
	std::map<LLUUID, LLAvatarListEntry>::iterator iter;

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		LLAvatarListEntry *ent = &iter->second;
		ent->setFocus(FALSE);
	}
}

void LLFloaterAvatarList::focusOnPrev(BOOL marked_only)
{
	std::map<LLUUID, LLAvatarListEntry>::iterator iter;
	LLAvatarListEntry *prev = NULL;
	LLAvatarListEntry *ent;

	if ( mAvatars.size() == 0 ) return;
	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		ent = &iter->second;
		if ( ent->isDead() ) continue;
		if ( (ent->getID() == mFocusedAvatar) && (prev != NULL)  )
		{
			removeFocusFromAll();
			prev->setFocus(TRUE);
			mFocusedAvatar = prev->getID();
			gAgent.lookAtObject(mFocusedAvatar, CAMERA_POSITION_OBJECT);
			return;
		}
		if ( (!marked_only) || ent->isMarked() ) prev = ent;
	}
	if (prev != NULL && ((!marked_only) || prev->isMarked()) )
	{
		removeFocusFromAll();
		prev->setFocus(TRUE);
		mFocusedAvatar = prev->getID();
		gAgent.lookAtObject(mFocusedAvatar, CAMERA_POSITION_OBJECT);
	}
}

void LLFloaterAvatarList::focusOnNext(BOOL marked_only)
{
	std::map<LLUUID, LLAvatarListEntry>::iterator iter;
	BOOL found = FALSE;
	LLAvatarListEntry *first = NULL;
	LLAvatarListEntry *ent;

	if ( mAvatars.size() == 0 ) return;

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		ent = &iter->second;
		if ( ent->isDead() ) continue;
		if ( NULL == first && ((!marked_only) || ent->isMarked())) first = ent;
		if ( found && ((!marked_only) || ent->isMarked()) )
		{
			removeFocusFromAll();
			ent->setFocus(TRUE);
			mFocusedAvatar = ent->getID();
			gAgent.lookAtObject(mFocusedAvatar, CAMERA_POSITION_OBJECT);
			return;
		}
		if ( ent->getID() == mFocusedAvatar ) found = TRUE;
	}

	if (first != NULL && ((!marked_only) || first->isMarked()))
	{
		removeFocusFromAll();
		first->setFocus(TRUE);
		mFocusedAvatar = first->getID();
		gAgent.lookAtObject(mFocusedAvatar, CAMERA_POSITION_OBJECT);
	}
}

//static
void LLFloaterAvatarList::onClickPrevMarked(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;
	self->focusOnPrev(TRUE);
}

//static
void LLFloaterAvatarList::onClickNextMarked(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;
	self->focusOnNext(TRUE);
}


// static
void LLFloaterAvatarList::onClickIM(void* userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	LLDynamicArray<LLUUID> ids = avlist->mAvatarList->getSelectedIDs();
	if(ids.size() > 0)
	{
		if(ids.size() == 1)
		{
			// Single avatar
			LLUUID agent_id = ids[0];
			char buffer[MAX_STRING];
			snprintf(buffer, MAX_STRING, "%s", avlist->mAvatars[agent_id].getName().c_str());
			gIMMgr->setFloaterOpen(TRUE);
			gIMMgr->addSession(
				buffer,
				IM_NOTHING_SPECIAL,
				agent_id);
		}
		else
		{
			// Group IM
			LLUUID session_id;
			session_id.generate();
			gIMMgr->setFloaterOpen(TRUE);
			gIMMgr->addSession("Avatars Conference", IM_SESSION_CONFERENCE_START, ids[0], ids);
		}
	}
}

void LLFloaterAvatarList::onClickTeleportOffer(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;

	LLDynamicArray<LLUUID> ids = avlist->mAvatarList->getSelectedIDs();
	if(ids.size() > 0)
	{
		handle_lure(ids);
	}
}

void LLFloaterAvatarList::onClickTrack(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	
 	LLScrollListItem *item =   avlist->mAvatarList->getFirstSelected();
	if (!item) return;

	LLUUID agent_id = item->getUUID();

	if ( avlist->mTracking && avlist->mTrackedAvatar == agent_id ) {
		LLTracker::stopTracking(NULL);
		avlist->mTracking = FALSE;
	}
	else
	{
		avlist->mTracking = TRUE;
		avlist->mTrackByLocation = FALSE;
		avlist->mTrackedAvatar = agent_id;
		LLTracker::trackAvatar(agent_id, avlist->mAvatars[agent_id].getName());
	}
}

void LLFloaterAvatarList::sendAvatarPropertiesRequest(LLUUID avid)
{
	

	lldebugs << "LLPanelAvatar::sendAvatarPropertiesRequest()" << llendl; 
	LLMessageSystem *msg = gMessageSystem;

	msg->newMessageFast(_PREHASH_AvatarPropertiesRequest);
	msg->nextBlockFast( _PREHASH_AgentData);
	msg->addUUIDFast(   _PREHASH_AgentID, gAgent.getID() );
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->addUUIDFast(   _PREHASH_AvatarID, avid);
	gAgent.sendReliableMessage();

	mAvatars[avid].mAvatarInfo.requestStarted();
}

// static
void LLFloaterAvatarList::processBridgeReply(std::vector<LLUUID> avatars, LLSD bridgeResponce)
{
	if(!sInstance)return;

	LLFloaterAvatarList* self = sInstance;


	//lgg
	for(int i=0;i<(int)bridgeResponce.size();i++)
	{

		// Verify that the avatar is in the list, if not, ignore.
		if ( self->mAvatarList->getItemIndex(avatars[i]) < 0 )
		{
			return;
		}

		LLAvatarListEntry *entry = &self->mAvatars[avatars[i]];
 
		std::string toParse = bridgeResponce[i].asString();
		
		//llinfos << "Recieved Bridge Responce to radar: " << toParse.c_str() << llendl;
		
		LLVector3d v;
		char * pch = strtok ((char *)toParse.c_str()," ,<>");
		std::string toParseOut("");
		while (pch != NULL)
		{
			toParseOut += std::string(" ") + std::string(pch);
			pch = strtok (NULL, " ,<>");
		}
		if(toParseOut.length() > 1)
			toParseOut = toParseOut.substr(1);

		//llinfos << "ok now trying to parse " << toParseOut.c_str() << llendl;


		S32 count = sscanf( toParseOut.c_str(), "%lf %lf %lf", v.mdV + 0, v.mdV + 1, v.mdV + 2 );
		if( 3 == count )
		{
//			llinfos << "setting new height" << llendl;
			entry->mPosition =v;
		}
	}
		

}

// static
void LLFloaterAvatarList::processAvatarPropertiesReply(LLMessageSystem *msg, void**)
{

	if(!sInstance)return;

	LLFloaterAvatarList* self = NULL;
	LLAvatarInfo avinfo;

	BOOL	identified = FALSE;
	BOOL	transacted = FALSE;

	LLUUID	agent_id;	// your id
	LLUUID	avatar_id;	// target of this panel
	U32	flags = 0x0;
	char	born_on[DB_BORN_BUF_SIZE];
	S32	charter_member_size = 0;

	msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_AgentID, agent_id);
	msg->getUUIDFast(_PREHASH_AgentData, _PREHASH_AvatarID, avatar_id );

	
	self = sInstance;

	// Verify that the avatar is in the list, if not, ignore.
	if ( self->mAvatarList->getItemIndex(avatar_id) < 0 )
	{
		return;
	}

	LLAvatarListEntry *entry = &self->mAvatars[avatar_id];

	msg->getStringFast(_PREHASH_PropertiesData, _PREHASH_BornOn, DB_BORN_BUF_SIZE, born_on);
	msg->getU32Fast(_PREHASH_PropertiesData, _PREHASH_Flags, flags);

	identified = (flags & AVATAR_IDENTIFIED);
	transacted = (flags & AVATAR_TRANSACTED);

	// What's this?
	// Let's see if I understand correctly: CharterMember property is dual purpose:
	// it either contains a number indicating an account type (usual value), or 
	// it contains a string with a custom title. Probably that's where Philip Linden's
	// "El Presidente" title comes from. Heh.
	U8 caption_index = 0;
	std::string caption_text;
	charter_member_size = msg->getSize("PropertiesData", "CharterMember");

	if(1 == charter_member_size)
	{
		msg->getBinaryData("PropertiesData", "CharterMember", &caption_index, 1);
	}
	else if(1 < charter_member_size)
	{
		char caption[MAX_STRING];
		msg->getString("PropertiesData", "CharterMember", MAX_STRING, caption);

		caption_text = caption;
		entry->setAccountCustomTitle(caption_text);
	}
		

	if(caption_text.empty())
	{
		
		const enum ACCOUNT_TYPE ACCT_TYPE[] = {
			ACCOUNT_RESIDENT,
			ACCOUNT_TRIAL,
			ACCOUNT_CHARTER_MEMBER,
			ACCOUNT_EMPLOYEE
		};

		//enum ACCOUNT_TYPE acct =
		avinfo.Account =  ACCT_TYPE[llclamp(caption_index, (U8)0, (U8)(sizeof(ACCT_TYPE)/sizeof(ACCT_TYPE[0])-1))];
		//entry->setAccountType(acct);

		
		if ( avinfo.Account != ACCOUNT_EMPLOYEE )
		{
			if ( transacted )
			{
				avinfo.Payment = PAYMENT_USED;
			}
			else if ( identified )
			{
				avinfo.Payment = PAYMENT_ON_FILE;
			}
			else
			{
				avinfo.Payment = PAYMENT_NONE;
			}
		}
		else
		{
			avinfo.Payment = PAYMENT_LINDEN;
		}
	}
	
	// Structure must be zeroed to have sane results, as we
	// have an incomplete string for input
	memset(&avinfo.BirthDate, 0, sizeof(avinfo.BirthDate));

	int num_read = sscanf(born_on, "%d/%d/%d", &avinfo.BirthDate.tm_mon,
	                                           &avinfo.BirthDate.tm_mday,
	                                           &avinfo.BirthDate.tm_year);

	if ( num_read == 3 && avinfo.BirthDate.tm_mon <= 12 )
	{
		avinfo.BirthDate.tm_year -= 1900;
		avinfo.BirthDate.tm_mon--;
	}
	else
	{
		// Zero again to remove any partially read data
		memset(&avinfo.BirthDate, 0, sizeof(avinfo.BirthDate));
		llwarns << "Error parsing birth date: " << born_on << llendl;
	}

	entry->mAvatarInfo.setValue(avinfo);
}
void LLFloaterAvatarList::processSoundTrigger(LLMessageSystem* msg,void**)
{
	LLUUID	sound_id,owner_id;
	msg->getUUIDFast(_PREHASH_SoundData, _PREHASH_SoundID, sound_id);
	msg->getUUIDFast(_PREHASH_SoundData, _PREHASH_OwnerID, owner_id);
	if(owner_id == gAgent.getID() && sound_id == LLUUID("76c78607-93f9-f55a-5238-e19b1a181389"))
	{
		//lgg we need to auto turn on settings for ppl now that we know they has the thingy
		if(gSavedSettings.getBOOL("PhoenixRadarChatKeys"))
		{
			LLFloaterAvatarList* self = getInstance();
			if(self) self->clearAnnouncements();
		}else
		{
			if(gSavedSettings.getWarning("PhoenixRadarChat"))
				LLNotifications::instance().add("PhoenixRadarChat", LLSD(),LLSD(), callbackPhoenixChat);
	
		}
	}
}
void LLFloaterAvatarList::callbackPhoenixChat(const LLSD &notification, const LLSD &response)
{
	//gSavedSettings.setWarning("PhoenixOTR", FALSE);
	S32 option = LLNotification::getSelectedOption(notification, response);
	if ( option == 0 )
	{
		gSavedSettings.setWarning("PhoenixRadarChat",FALSE);
	}
	else if ( option == 1 )
	{
		gSavedSettings.setBOOL("PhoenixRadarChatKeys",true);
		gSavedSettings.setBOOL("PhoenixRadarChatAlerts",true);
		gSavedSettings.setBOOL("PhoenixAvatarListKeepOpen",true);
		LLFloaterAvatarList* self = getInstance();
		if(self) self->clearAnnouncements();
	}
}

// static
void LLFloaterAvatarList::onClickSayCmd( void* userdata )
{
	LLFloaterAvatarList* self = getInstance();
	if (self->mInputEditor)
	{
		gSavedPerAccountSettings.setString("PhoenixAvatarlistCustomAction",std::string(self->mInputEditor->getText()));
		self->doCommand(cmd_saycmd);
	}
}

void LLFloaterAvatarList::onCommandCommit(LLUICtrl* ctrl,void *userdata)
{
	LLFloaterAvatarList* self = getInstance();
	if (self->mInputEditor)
	{
		gSavedPerAccountSettings.setString("PhoenixAvatarlistCustomAction",std::string(self->mInputEditor->getText()));
	}
}

// static
void LLFloaterAvatarList::onClickTP( void* userdata )
{
	LLFloaterAvatarList* avlist = getInstance();
	LLScrollListItem *item = avlist->mAvatarList->getFirstSelected();

	if ( item )
	{
		LLUUID agent_id = item->getUUID();
		LLAvatarListEntry *ent = avlist->getAvatarEntry(agent_id);
		if ( ent )
		{
			llinfos << "Trying to teleport to " << ent->getName() << " at " << ent->getPosition() << llendl;
			gAgent.teleportViaLocation( ent->getPosition() );
		}
	}
}

void LLFloaterAvatarList::checkTrackingStatus()
{

	if ( mTracking && LLTracker::getTrackedPositionGlobal().isExactlyZero() )
	{
		// trying to track an avatar, but tracker stopped tracking		
		if ( mAvatars.count( mTrackedAvatar ) > 0 && !mTrackByLocation )
		{
			llinfos << "Switching to location-based tracking" << llendl;
			mTrackByLocation = TRUE;
		}
		else
		{
			// not found
			llinfos << "Stopping tracking avatar, server-side didn't work, and not in list anymore." << llendl;
			LLTracker::stopTracking(NULL);
			mTracking = FALSE;
		}
	}

	if ( mTracking && mTrackByLocation )
	{
		std::string name = mAvatars[mTrackedAvatar].getName();
		std::string tooltip = "Tracking last known position";
		name += " (near)";
		LLTracker::trackLocation(mAvatars[mTrackedAvatar].getPosition(),name, tooltip);
	}

	//llinfos << "Tracking position: " << LLTracker::getTrackedPositionGlobal() << llendl;
	
}


BOOL  LLFloaterAvatarList::avatarIsInList(LLUUID avatar)
{
	return ( mAvatars.count( avatar ) > 0 );
}

LLAvatarListEntry * LLFloaterAvatarList::getAvatarEntry(LLUUID avatar)
{
	if ( avatar.isNull() )
	{
		return NULL;
	}

	std::map<LLUUID, LLAvatarListEntry>::iterator iter;

	iter = mAvatars.find(avatar);
	if ( iter == mAvatars.end() )
	{
		return NULL;
	}
	return &iter->second;	
}

void LLFloaterAvatarList::onClickAgeAlert(LLUICtrl* ctrl,void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	gSavedSettings.setBOOL("PhoenixAvatarAgeAlert", avlist->childGetValue("agealert"));
}

void LLFloaterAvatarList::onClickAgeAlertDays(LLUICtrl* ctrl,void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	gSavedSettings.setF32("PhoenixAvatarAgeAlertDays", avlist->childGetValue("AgeAlertDays").asInteger());
}

LLColor4 LLFloaterAvatarList::getAvatarColor(LLAvatarListEntry *ent, F32 distance, e_coloring_type type)
{
	static LLColor4* sDefaultListText = rebind_llcontrol<LLColor4>("DefaultListText", &gColors, true);
	static LLColor4* sAvatarListTextDistNormalRange = rebind_llcontrol<LLColor4>("AvatarListTextDistNormalRange", &gColors, true);
	static LLColor4* sAvatarListTextDistShoutRange = rebind_llcontrol<LLColor4>("AvatarListTextDistShoutRange", &gColors, true);
	static LLColor4* sAvatarListTextDistOver = rebind_llcontrol<LLColor4>("AvatarListTextDistOver", &gColors, true);
	static LLColor4* sAvatarListTextAgeNormal = rebind_llcontrol<LLColor4>("AvatarListTextAgeNormal", &gColors, true);
	static LLColor4* sAvatarListTextAgeYoung = rebind_llcontrol<LLColor4>("AvatarListTextAgeYoung", &gColors, true);
// 	F32 r = 0.0f, g = 0.0f, b = 0.0f, a = 0.5f;
	LLColor4 av_color = (*sDefaultListText).getValue();

	switch(type)
	{
		case CT_NONE:
			av_color = (*sDefaultListText).getValue();
			break;
		case CT_DISTANCE:
			if ( distance <= 20.0f )
			{
				av_color = (*sAvatarListTextDistNormalRange).getValue();
			}
			else if ( distance > 20.0f && distance <= 96.0f )
			{
				av_color = (*sAvatarListTextDistShoutRange).getValue();
			}
			else
			{
				av_color = (*sAvatarListTextDistOver).getValue();
			}
			break;
		case CT_AGE:
			if ( ent->mAvatarInfo.getStatus() == DATA_RETRIEVED )
			{
				S32 age = ent->mAvatarInfo.getValue().getAge();
				if ( age <= 7 )
				{
					av_color = (*sAvatarListTextAgeYoung).getValue();
				}
				else
				{
					av_color = (*sAvatarListTextAgeNormal).getValue();
				}
			}
			break;
		case CT_SCORE:
			break;
		case CT_PAYMENT:
			break;
	}
	return av_color;
}

void LLFloaterAvatarList::onDoubleClick(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;
 	LLScrollListItem *item =   self->mAvatarList->getFirstSelected();
	LLUUID agent_id = item->getUUID();

	//gAgent.lookAtObject(agent_id, CAMERA_POSITION_OBJECT);
	lookAtAvatar(agent_id);
}

void LLFloaterAvatarList::lookAtAvatar(LLUUID &uuid)
{ // twisted laws
    LLViewerObject* voavatar = gObjectList.findObject(uuid);
    if(voavatar && voavatar->isAvatar())
    {
        gAgent.setFocusOnAvatar(FALSE, FALSE);
        gAgent.changeCameraToThirdPerson();
        gAgent.setFocusGlobal(voavatar->getPositionGlobal(),uuid);
        gAgent.setCameraPosAndFocusGlobal(voavatar->getPositionGlobal() 
                + LLVector3d(3.5,1.35,0.75) * voavatar->getRotation(), 
                                                voavatar->getPositionGlobal(), 
                                                uuid );
    }
}

void LLFloaterAvatarList::doCommand(void (*func)(const LLUUID &avatar, const std::string &name))
{
	LLDynamicArray<LLUUID> ids = mAvatarList->getSelectedIDs();

	for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
	{
		LLUUID avid = *itr;
		LLAvatarListEntry *ent = getAvatarEntry(avid);
		if ( ent != NULL )
		{
			llinfos << "Executing command on " << ent->getName() << llendl;
			func(avid, ent->getName());
		}
	}
}

std::string LLFloaterAvatarList::getSelectedNames(const std::string& separator)
{
	std::string ret = "";

	LLDynamicArray<LLUUID> ids = mAvatarList->getSelectedIDs();
	for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
	{
		LLUUID avid = *itr;
		LLAvatarListEntry *ent = getAvatarEntry(avid);
		if ( ent != NULL )
	{
			if (!ret.empty()) ret += separator;
			ret += ent->getName();
		}
	}

	return ret;
	}

std::string LLFloaterAvatarList::getSelectedName()
{
	LLUUID id = getSelectedID();
	LLAvatarListEntry *ent = getAvatarEntry(id);
	if(ent)
	{
		return ent->getName();
	}
	return "";
}

LLUUID LLFloaterAvatarList::getSelectedID()
{
	LLScrollListItem *item = mAvatarList->getFirstSelected();
	if(item) return item->getUUID();
	return LLUUID::null;
}
	
//static 
void LLFloaterAvatarList::callbackFreeze(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);

	LLFloaterAvatarList *avlist = LLFloaterAvatarList::sInstance;

	if ( option == 0 )
	{
		avlist->doCommand(cmd_freeze);
	}
	else if ( option == 1 )
	{
		avlist->doCommand(cmd_unfreeze);
	}
}

//static 
void LLFloaterAvatarList::callbackEject(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);

	LLFloaterAvatarList *avlist = LLFloaterAvatarList::sInstance;
 
	if ( option == 0 )
{
		avlist->doCommand(cmd_eject);
}
	else if ( option == 1 )
    {
		//avlist->doCommand(cmd_ban);
    }
}

//static 
void LLFloaterAvatarList::callbackEjectFromEstate(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotification::getSelectedOption(notification, response);

	LLFloaterAvatarList *avlist = LLFloaterAvatarList::sInstance;

	if ( option == 0 )
	{
		avlist->doCommand(cmd_estate_eject);
	}
}

void LLFloaterAvatarList::callbackBanFromEstate(const LLSD& notification, const LLSD& response)
{/*
	S32 option = LLNotification::getSelectedOption(notification, response);

	LLFloaterAvatarList *avlist = LLFloaterAvatarList::sInstance;

	if ( option == 0 )
	{
		avlist->doCommand(cmd_estate_ban);
	}*/
	}

//static
void LLFloaterAvatarList::callbackIdle(void *userdata) {
	if ( LLFloaterAvatarList::sInstance != NULL ) {
		LLFloaterAvatarList::sInstance->updateAvatarList();
	}
}

// static
void LLFloaterAvatarList::onClickProfile(void* userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	avlist->doCommand(cmd_profile);
}
void LLFloaterAvatarList::checkAnnouncements()
{
	LLViewerRegion* regionp = gAgent.getRegion();
	if(!regionp)return;//ALWAYS VALIDATE DATA
	std::ostringstream ids;
	std::set<LLUUID> new_set;
	static int last_transact_num = 0;
	int transact_num = (int)gFrameCount;
	int num_ids = 0;

	if(!gSavedSettings.getBOOL("PhoenixRadarChatKeys"))
{
		mAnnouncedAvatars.clear();
		return;
}

	if(transact_num > last_transact_num)
{
		last_transact_num = transact_num;
}
	else
{
		//on purpose, avatar IDs on map don't change until the next frame.
		//no need to send more than once a frame.
		return;
}

	// Where the heck are iterators in LLDynamicArray?
	if (!regionp) return; // caused crash if logged out/connection lost
	for (int i = 0; i < regionp->mMapAvatarIDs.count(); i++)
{
		const LLUUID &id = regionp->mMapAvatarIDs.get(i);

		new_set.insert(id);

		if(mAnnouncedAvatars.count(id) == 0)
{
			ids << "," << id.asString();
			++num_ids;

			if(ids.tellp() > 200)
{
				gMessageSystem->newMessage("ScriptDialogReply");
		 		gMessageSystem->nextBlock("AgentData");
		 		gMessageSystem->addUUID("AgentID", gAgent.getID());
		 		gMessageSystem->addUUID("SessionID", gAgent.getSessionID());
		 		gMessageSystem->nextBlock("Data");
		 		gMessageSystem->addUUID("ObjectID", gAgent.getID());
		 		gMessageSystem->addS32("ChatChannel", gSavedSettings.getS32("PhoenixRadarChatKeysChannel"));
		 		gMessageSystem->addS32("ButtonIndex", 1);
				gMessageSystem->addString("ButtonLabel",llformat("%d,%d", transact_num, num_ids) + ids.str());
				gAgent.sendReliableMessage();

				num_ids = 0;
				ids.seekp(0);
				ids.str("");
	}
}
}
	mAnnouncedAvatars = new_set;
	if(num_ids > 0)
{
	 	gMessageSystem->newMessage("ScriptDialogReply");
	 	gMessageSystem->nextBlock("AgentData");
	 	gMessageSystem->addUUID("AgentID", gAgent.getID());
	 	gMessageSystem->addUUID("SessionID", gAgent.getSessionID());
	 	gMessageSystem->nextBlock("Data");
	 	gMessageSystem->addUUID("ObjectID", gAgent.getID());
	 	gMessageSystem->addS32("ChatChannel", gSavedSettings.getS32("PhoenixRadarChatKeysChannel"));
	 	gMessageSystem->addS32("ButtonIndex", 1);
	 	gMessageSystem->addString("ButtonLabel",llformat("%d,%d", transact_num, num_ids) + ids.str());
		gAgent.sendReliableMessage();
        }
}

void LLFloaterAvatarList::clearAnnouncements()
{
	mAnnouncedAvatars.clear();
}
