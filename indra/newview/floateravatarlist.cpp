/** 
 * @file llfloatermute.cpp
 * @brief Container for mute list
 *
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
#include "llchat.h"
#include "llviewermessage.h"
#include "llweb.h"
#include "llviewerobjectlist.h"
#include "llmutelist.h"
#include "llchat.h"
#include "llfloaterchat.h"
#include "llcallbacklist.h"

// [RLVa:KB] - Emerald specific
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
void cmdline_printchat(std::string message);

// Timeouts
/**
 * @brief How long to keep showing an activity, in seconds
 */
const F32 ACTIVITY_TIMEOUT = 1.0f;
const F32 ACTIVITY_TIMEOUT_TYPING = 30.0f;


/**
 * @brief How many seconds to wait between data requests
 *
 * This is intended to avoid flooding the server with requests
 */
const F32 MIN_REQUEST_INTERVAL   = 1.0f;

/**
 * @brief How long to wait for a request to arrive during the first try in seconds
 */
const F32 FIRST_REQUEST_TIMEOUT  = 16.0f;

/**
 * @brief Delay is doubled on each attempt. This is as high as it'll go
 */
const F32 MAX_REQUEST_TIMEOUT    = 2048.0f;
 
/**
 * How long to wait for a request to arrive before assuming failure
 * and showing the failure icon in the list. This is just for the user's
 * information, if a reply arrives after this interval we'll accept it anyway.
 */
const F32 REQUEST_FAIL_TIMEOUT   = 15.0f;

/**
 * How long to keep people who are gone in the list. After this time is reached,
 * they're not shown in the list anymore, but still kept in memory until
 * CLEANUP_TIMEOUT is reached.
 */
const F32 DEAD_KEEP_TIME = 10.0f;

/**
 * @brief How long to keep entries around before removing them.
 *
 * @note Longer term, data like birth and payment info should be cached on disk.
 */
const F32 CLEANUP_TIMEOUT = 3600.0f;

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
//	if(gSavedSettings.getBOOL("EmeraldRadarChatAlerts") && (strcmp(name.c_str(),"(???) (???)") == 1))
	if(gSavedSettings.getBOOL("EmeraldRadarChatAlerts"))
	{
// [RLVa:KB] - Alternate: Emerald-370
		if (gRlvHandler.hasBehaviour(RLV_BHVR_SHOWNAMES))
		{
			name = RlvStrings::getAnonym(name);
		}
// [/RLVa:KB]

		LLChat chat;
		switch(type)
		{
		case ALERT_TYPE_SIM:
			if(gSavedSettings.getBOOL("EmeraldRadarAlertSim"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());
				chat.mText = name+" has "+(entering ? "entered" : "left")+" the sim.";// ("+key.asString()+")";
			}
			if(gSavedSettings.getBOOL("EmeraldRadarChatKeys"))
			if(!entering)
			{
				gMessageSystem->newMessage("ScriptDialogReply");
				 gMessageSystem->nextBlock("AgentData");
				 gMessageSystem->addUUID("AgentID", gAgent.getID());
				 gMessageSystem->addUUID("SessionID", gAgent.getSessionID());
				 gMessageSystem->nextBlock("Data");
				 gMessageSystem->addUUID("ObjectID", gAgent.getID());
				 gMessageSystem->addS32("ChatChannel", gSavedSettings.getS32("EmeraldRadarChatKeysChannel"));
				 gMessageSystem->addS32("ButtonIndex", 1);
				gMessageSystem->addString("ButtonLabel",llformat("%d,%d,", gFrameCount, 0) + key.asString());
				gAgent.sendReliableMessage();
			}
			break;
		case ALERT_TYPE_DRAW:
			if(gSavedSettings.getBOOL("EmeraldRadarAlertDraw"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());
				chat.mText = name+" has "+(entering ? "entered" : "left")+" draw distance.";// ("+key.asString()+")";
			}
			break;
		case ALERT_TYPE_CHATRANGE:
			if(gSavedSettings.getBOOL("EmeraldRadarAlertChatRange"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());
				chat.mText = name+" has "+(entering ? "entered" : "left")+" chat range.";// ("+key.asString()+")";
			}
			break;
		case ALERT_TYPE_AGE:
			if(gSavedSettings.getBOOL("EmeraldAvatarAgeAlert"))
			{
				chat.mFromName = name;
				chat.mURL = llformat("secondlife:///app/agent/%s/about",key.asString().c_str());

				make_ui_sound("EmeraldAvatarAgeAlertSoundUUID");
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

LLAvatarListEntry::LLAvatarListEntry(const LLUUID& id, const std::string &name, const LLVector3d &position, BOOL isLinden) :
		mID(id), mName(name), mTime(time(NULL)), mPosition(position), mDrawPosition(), mAlert(FALSE), mMarked(FALSE), mFocused(FALSE), mIsLinden(isLinden), mActivityType(ACTIVITY_NEW), mAccountTitle(""),
			mUpdateTimer(), mActivityTimer(), mFrame(gFrameCount), mInSimFrame(U32_MAX), mInDrawFrame(U32_MAX), mInChatFrame(U32_MAX)
{
}

void LLAvatarListEntry::setPosition(LLVector3d position, bool this_sim, bool drawn, bool chatrange)
{
	if ( mActivityType == ACTIVITY_DEAD )
	{
		resetTime();
		setActivity(ACTIVITY_NEW);
	}

	if ( drawn )
	{
		if ( mDrawPosition != position && !mDrawPosition.isExactlyZero() )
		{
			setActivity(ACTIVITY_MOVING);
		}

		mDrawPosition = position;
	} else if( mInDrawFrame == U32_MAX ) {
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
//U32 LLAvatarListEntry::getOffSimFrames()
//{
//	return (gFrameCount - mInSimFrame);
//}
//U32 LLAvatarListEntry::getOffDrawFrames()
//{
//	return (gFrameCount - mInDrawFrame);
//}
//U32 LLAvatarListEntry::getOutsideChatRangeFrames();
//{
//	return (gFrameCount - mInChatFrame);
//}

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
	if ( id.isNull() )
	{
		llwarns << "Trying to set null id" << llendl;
	}
	mID = id;
}

BOOL LLAvatarListEntry::getIsLinden()
{
	// Are there any employees that are not a Linden?
	// I suppose this is a bit redundant.
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



void LLAvatarListEntry::setActivity(ACTIVITY_TYPE activity)
{
	if ( activity >= mActivityType || mActivityTimer.getElapsedTimeF32() > ACTIVITY_TIMEOUT )
	{
		mActivityType = activity;
		mActivityTimer.start();
	}
}

ACTIVITY_TYPE LLAvatarListEntry::getActivity()
{
	if (mActivityType == ACTIVITY_TYPING)
	{
		if ( mActivityTimer.getElapsedTimeF32() > ACTIVITY_TIMEOUT_TYPING )
		{
			mActivityType = ACTIVITY_NONE;
		}
	}
	else
	{
		if ( mActivityTimer.getElapsedTimeF32() > ACTIVITY_TIMEOUT )
		{
			mActivityType = ACTIVITY_NONE;
		}
	}
	return mActivityType;
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
}

LLFloaterAvatarList::~LLFloaterAvatarList()
{
	gIdleCallbacks.deleteFunction( LLFloaterAvatarList::callbackIdle );
	sInstance = NULL;
}

//static
void LLFloaterAvatarList::toggle(void*)
{
// [RLVa:KB] - Alternate: Emerald-370
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
	if ( !gSavedSettings.getBOOL("EmeraldAvatarListKeepOpen") || app_quitting )
	{
		destroy();
	}
}

BOOL LLFloaterAvatarList::postBuild()
{
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
	}

	// Default values
	mTracking = FALSE;
	mTrackByLocation = FALSE;
	mARLastFrame = 0;
	mlastBridgeCallTime = gFrameTimeSeconds;

	// Create interface from XML
	//gUICtrlFactory->buildFloater(this, "floater_avatar_scanner.xml");

	// Floater starts hidden	
	//setVisible(FALSE);

	// Hide them until some other way is found
	// Users may not expect to find a Ban feature on the Eject button
	//childSetVisible("refresh_btn", false);
	//childSetVisible("ban_btn", false);
	//childSetVisible("unban_btn", false);
	//childSetVisible("unmute_btn", false);
	//childSetVisible("estate_ban_btn", false);

	// Set callbacks
	//childSetAction("refresh_btn", onClickRefresh, this);
	childSetAction("profile_btn", onClickProfile, this);
	childSetAction("im_btn", onClickIM, this);
	childSetAction("offer_btn", onClickTeleportOffer, this);
	childSetAction("track_btn", onClickTrack, this);
	childSetAction("mark_btn", onClickMark, this);

	childSetAction("prev_in_list_btn", onClickPrevInList, this);
	childSetAction("next_in_list_btn", onClickNextInList, this);
	childSetAction("prev_marked_btn", onClickPrevMarked, this);
	childSetAction("next_marked_btn", onClickNextMarked, this);
	
	childSetAction("get_key_btn", onClickGetKey, this);

	childSetAction("freeze_btn", onClickFreeze, this);
	childSetAction("eject_btn", onClickEject, this);
//	childSetAction("ban_btn", onClickBan, this);
//	childSetAction("unban_btn", onClickUnban, this);
	childSetAction("mute_btn", onClickMute, this);
	childSetAction("unmute_btn", onClickUnmute, this);
	childSetAction("ar_btn", onClickAR, this);
	childSetAction("teleport_btn", onClickTeleport, this);
	childSetAction("estate_kick_btn", onClickKickFromEstate, this);
	childSetAction("estate_ban_btn", onClickBanFromEstate, this);
	childSetAction("estate_tph_btn", onClickTPHFromEstate, this);
	childSetAction("estate_gtfo_btn", onClickGTFOFromEstate, this);
	childSetAction("script_count_btn", onClickScriptCount, this);

	childSetCommitCallback("agealert", onClickAgeAlert,this);
	childSetValue("agealert",gSavedSettings.getBOOL("EmeraldAvatarAgeAlert"));

	childSetCommitCallback("AgeAlertDays",onClickAgeAlertDays,this);
	childSetValue("AgeAlertDays",gSavedSettings.getF32("EmeraldAvatarAgeAlertDays"));


	// *FIXME: Uncomment once onClickRefresh has been restored
	//setDefaultBtn("refresh_btn");

	// Get a pointer to the scroll list from the interface
	mAvatarList = getChild<LLScrollListCtrl>("avatar_list");

	mAvatarList->setCallbackUserData(this);
	mAvatarList->setDoubleClickCallback(onDoubleClick);
	mAvatarList->sortByColumn("distance", TRUE);
	mDataRequestTimer.start();
	mUpdateThrottleTimer.start();
	refreshAvatarList();

	//LLMessageSystem *msg = gMessageSystem;
	//msg->addHandlerFunc("AvatarPropertiesReply", processAvatarPropertiesReply);

	gIdleCallbacks.addFunction( LLFloaterAvatarList::callbackIdle );

	return TRUE;
}

void LLFloaterAvatarList::updateAvatarList()
{
	if(gDisconnected)return;
//	LLVOAvatar *avatarp;

	if( sInstance != this ) return;

	if ( mUpdateThrottleTimer.getElapsedTimeF32() < 0.25)
		return;
	mUpdateThrottleTimer.reset();

	//llinfos << "avatar list refresh: updating map" << llendl;

	// Check whether updates are enabled
	LLCheckboxCtrl* check;
	check = getChild<LLCheckboxCtrl>("update_enabled_cb");

	if ( !check->getValue() )
	{
		return;
	}

	LLVector3d mypos = gAgent.getPositionGlobal();

	{
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

		size_t i;
		size_t count = avatar_ids.size();

		for(i = 0; i < count; ++i)
		{
			std::string name;
			std::string first;
			std::string last;
			const LLUUID &avid = avatar_ids[i];

			LLVector3d position;
			LLViewerObject *obj = gObjectList.findObject(avid);

			if(obj)
			{
				LLVOAvatar* avatarp = dynamic_cast<LLVOAvatar*>(obj);

				if (avatarp == NULL)
				{
					continue;
				}

				// Skip if avatar is dead(what's that?)
				// or if the avatar is ourselves.
				if (avatarp->isDead() || avatarp->isSelf())
				{
					continue;
				}

				// Get avatar data
				position = gAgent.getPosGlobalFromAgent(avatarp->getCharacterPosition());
				name = avatarp->getFullname();

				// Apparently, sometimes the name comes out empty, with a " " name. This is because
				// getFullname concatenates first and last name with a " " in the middle.
				// This code will avoid adding a nameless entry to the list until it acquires a name.

				//duped for lower section
				if (name.empty() || (name.compare(" ") == 0))// || (name.compare(gCacheName->getDefaultName()) == 0))
				{
					if(gCacheName->getName(avid, first, last))
					{
						name = first + " " + last;
					}
					else
					{
						continue;
					}
				}

				if (avid.isNull())
				{
					//llinfos << "Key empty for avatar " << name << llendl;
					continue;
				}

				if ( mAvatars.count( avid ) > 0 )
				{
					// Avatar already in list, update position
					mAvatars[avid].setPosition(position, (avatarp->getRegion() == gAgent.getRegion()), true, (position - mypos).magVec() < 20.0);
				}
				else
				{
					// Avatar not there yet, add it
					BOOL isLinden = ( strcmp(last.c_str(), "Linden") == 0 || last == "Linden" );

					LLAvatarListEntry entry(avid, name, position, isLinden);
					mAvatars[avid] = entry;

					//sendAvatarPropertiesRequest(avid);
					//llinfos << "avatar list refresh: adding " << name << llendl;

				}
			}
			else
			{
				if( i < positions.size())
				{
					position = positions[i];
				}
				else
				{
					continue;
				}

				if(gCacheName->getName(avid, first, last))
				{
					name = first + " " + last;
				}
				else
				{
					//name = gCacheName->getDefaultName();
					continue; //prevent (Loading...)
				}

				if ( mAvatars.count( avid ) > 0 )
				{
					// Avatar already in list, update position
					mAvatars[avid].setPosition(position, gAgent.getRegion()->pointInRegionGlobal(position), false, (position - mypos).magVec() < 20.0);
				}
				else
				{
					// Avatar not there yet, add it
					BOOL isLinden = last == "Linden";

					LLAvatarListEntry entry(avid, name, position, isLinden);
					mAvatars[avid] = entry;
				}
			}
		}
	}
	
//	llinfos << "avatar list refresh: done" << llendl;

	expireAvatarList();
	refreshAvatarList();
	checkTrackingStatus();
	checkAnnouncements();
	//processARQueue();
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
		
		if ( !ent->getAlive())
		{
			ent->setActivity(ACTIVITY_DEAD);
		}


		if ( ent->getEntryAgeSeconds() > CLEANUP_TIMEOUT )
		{
			//llinfos << "avatar list: expiring avatar " << ent->getName() << llendl;
			LLUUID av_id = ent->getID();
			delete_queue.push(av_id);
		}
	}

	while(!delete_queue.empty())
	{
		mAvatars.erase(delete_queue.front());
		delete_queue.pop();
	}
}

/**
 * Redraws the avatar list
 * Only does anything if the avatar list is visible.
 * @author Dale Glass
 */
void LLFloaterAvatarList::refreshAvatarList() 
{
	if(gDisconnected)return;

	// Don't update list when interface is hidden
	if (!sInstance->getVisible())return;

	LLCheckboxCtrl* fetch_data;
	fetch_data = getChild<LLCheckboxCtrl>("fetch_avdata_enabled_cb");

	//BOOL db_enabled = gSavedSettings.getBOOL("DBEnabled");
	//std::string db_avatar = gSavedPerAccountSettings.getString("DBAvatarName");
	//if ( db_avatar.empty() )
	//{
	//	db_enabled = FALSE;
	//}



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

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		LLSD element;
		LLUUID av_id;

		
		LLAvatarListEntry *ent = &iter->second;

		// Skip if avatar hasn't been around
		if ( ent->isDead() )
		{
			continue;
		}

		av_id = ent->getID();

		// Get avatar name, position
		LLAvatarInfo avinfo = ent->mAvatarInfo.getValue();

		DATA_STATUS avinfo_status = ent->mAvatarInfo.getStatus();

		LLVector3d position = ent->getPosition();
		//lgg stuff.. ok.. uhmm..
		BOOL flagForFedUpDistance = false;

		
		LLVector3d delta = position - mypos;
		F32 distance = (F32)delta.magVec();
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

		std::string icon = "";

		// HACK: Workaround for an apparent bug:
		// sometimes avatar entries get stuck, and are registered
		// by the client as perpetually moving in the same direction.
		// this makes sure they get removed from the visible list eventually

		//jcool410 -- this fucks up seeing dueds thru minimap data > 1024m away, so, lets just say > 2048m to the side is bad
		//aka 8 sims
		if ( side_distance > 2048.0f)
		{
			continue;
		}

		if ( av_id.isNull() )
		{
			//llwarns << "Avatar with null key somehow got into the list!" << llendl;
			continue;
		}





		element["id"] = av_id;

		element["columns"][LIST_AVATAR_ICON]["column"] = "avatar_icon";
		element["columns"][LIST_AVATAR_ICON]["type"] = "text";
		element["columns"][LIST_AVATAR_NAME]["color"] = gColors.getColor("DefaultListText").getValue();
		if ( ent->isMarked() )
		{
			element["columns"][LIST_AVATAR_ICON]["type"] = "icon";
			element["columns"][LIST_AVATAR_ICON]["value"] = /*gViewerArt.getString(*/"flag_blue.tga"/*)*/;
			element["columns"][LIST_AVATAR_ICON]["color"] = gColors.getColor("DefaultListIcon").getValue();
		}


		if ( ent->getIsLinden() )
		{
			element["columns"][LIST_AVATAR_NAME]["font-style"] = "BOLD";
		}

		if ( ent->isFocused() )
		{
			element["columns"][LIST_AVATAR_NAME]["color"] = LLColor4::cyan.getValue();
		}

		//element["columns"][LIST_AVATAR_NAME]["font-color"] = getAvatarColor(ent, distance).getValue();
		element["columns"][LIST_AVATAR_NAME]["column"] = "avatar_name";
		element["columns"][LIST_AVATAR_NAME]["type"] = "text";
		element["columns"][LIST_AVATAR_NAME]["value"] = ent->getName().c_str();

		char temp[32];
		snprintf(temp, sizeof(temp), "%.2f", distance);

		element["columns"][LIST_DISTANCE]["column"] = "distance";
		element["columns"][LIST_DISTANCE]["type"] = "text";
		
		element["columns"][LIST_DISTANCE]["value"] = temp;
		if(flagForFedUpDistance)
		{
			//lgg fix for out of draw distance
			//element["columns"][LIST_DISTANCE]["value"] = std::string("(> "+llformat("%d", gSavedSettings.getF32("RenderFarClip") )+")");
			element["columns"][LIST_DISTANCE]["value"] = llformat("> %d", (S32)gSavedSettings.getF32("RenderFarClip") );
		}
		element["columns"][LIST_DISTANCE]["color"] = getAvatarColor(ent, distance, CT_DISTANCE).getValue();

		
		if ( avinfo_status == DATA_RETRIEVED )
		{
			if ((avinfo.getAge() < gSavedSettings.getF32("EmeraldAvatarAgeAlertDays")) && !ent->getAlert())
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
		if(gAgent.getRegion()->pointInRegionGlobal(position))
		{
			icon = "account_id_green.tga";
		}
		
		if (!icon.empty() )
		{	
			element["columns"][LIST_SIM].erase("color");
			element["columns"][LIST_SIM]["type"] = "icon";
			element["columns"][LIST_SIM]["value"] = icon;
		}
	

		// Get an icon for the payment data
		// These should be replaced with something proper instead of reusing whatever
		// LL-provided images happened to fit
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

		// TODO: Add icon for "unknown" status
		//if ( PAYMENT_NONE != avinfo.Payment && DATA_UNKNOWN != avinfo_status )
		if ( !icon.empty() )
		{
			element["columns"][LIST_PAYMENT]["color"] = gColors.getColor("DefaultListIcon").getValue();
			element["columns"][LIST_PAYMENT]["type"] = "icon";
			element["columns"][LIST_PAYMENT]["value"] =  icon;
			//llinfos << "Payment icon: " << payment_icon << llendl;
		}

		
		ACTIVITY_TYPE activity = ent->getActivity();
		icon = "";
		switch( activity )
		{
			case ACTIVITY_NONE:
				break;
			case ACTIVITY_MOVING:
				icon = /*gViewerArt.getString(*/"inv_item_animation.tga"/*)*/;
				break;
			case ACTIVITY_GESTURING:
				icon = /*gViewerArt.getString(*/"inv_item_gesture.tga"/*)*/;
				break;
			case ACTIVITY_SOUND:
				icon = /*gViewerArt.getString(*/"inv_item_sound.tga"/*)*/;
				break;
			case ACTIVITY_REZZING:
				icon = /*gViewerArt.getString(*/"ff_edit_theirs.tga"/*)*/;
				break;
			case ACTIVITY_PARTICLES:
				// TODO: Replace with something better
				icon = /*gViewerArt.getString(*/"particles.tga"/*)*/;
				break;
			case ACTIVITY_NEW:
				icon = /*gViewerArt.getString(*/"avatar_new.tga"/*)*/;
				break;
			case ACTIVITY_TYPING:
				icon = /*gViewerArt.getString(*/"avatar_typing.tga"/*)*/;
				break;
			case ACTIVITY_DEAD:
				// TODO: Replace, icon is quite inappropiate
				icon = /*gViewerArt.getString(*/"avatar_gone.tga"/*)*/;
				break;
		}

		element["columns"][LIST_ACTIVITY]["column"] = "activity";
		element["columns"][LIST_ACTIVITY]["type"] = "text";

		if (!icon.empty() )
		{	
			element["columns"][LIST_ACTIVITY]["type"] = "icon";
			element["columns"][LIST_ACTIVITY]["value"] = icon;
			element["columns"][LIST_ACTIVITY]["color"] = gColors.getColor("DefaultListIcon").getValue();
			//llinfos << "Activity icon: " << activity_icon << llendl;
		}

		//element["columns"][LIST_PAYMENT]["column"] = "payment_data";
		//element["columns"][LIST_PAYMENT]["type"] = "text";

		S32 seentime = (S32)difftime( time(NULL) , ent->getTime() );
		S32 hours = (S32)(seentime / (60*60));
		S32 mins = (S32)((seentime - hours*(60*60)) / 60);
		S32 secs = (S32)((seentime - hours*(60*60) - mins*60));

		element["columns"][LIST_TIME]["column"] = "time";
		element["columns"][LIST_TIME]["type"] = "text";
		element["columns"][LIST_TIME]["color"] = gColors.getColor("DefaultListText").getValue();
		element["columns"][LIST_TIME]["value"] = llformat("%d:%02d:%02d", hours,mins,secs);

		element["columns"][LIST_CLIENT]["column"] = "client";
		element["columns"][LIST_CLIENT]["type"] = "text";
		LLColor4 avatar_name_color = gColors.getColor( "AvatarNameColor" );
		std::string client;
		LLVOAvatar *av = (LLVOAvatar*)gObjectList.findObject(av_id);
		if(av)
		{
			LLVOAvatar::resolveClient(avatar_name_color, client, av);
			if(client == "")
			{
				avatar_name_color = gColors.getColor( "ScrollUnselectedColor" );
				client = "?";
			}
			element["columns"][LIST_CLIENT]["value"] = client.c_str();
			//element["columns"][LIST_CLIENT]["color"] = avatar_name_color.getValue();
		}
		else
		{
			element["columns"][LIST_CLIENT]["value"] = "Out Of Range";
			avatar_name_color = gColors.getColor( "ScrollUnselectedColor" );
		}

		avatar_name_color = avatar_name_color * 0.5 + gColors.getColor( "ScrollUnselectedColor" ) * 0.5;

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
	if((toSendToBridge != "" ) && gSavedSettings.getBOOL("EmeraldUseBridgeRadar"))
	{
		if(gFocusMgr.getAppHasFocus()) // Phox: This interaction seems to cause a lockup if it happens too much while the window is out of focus.
		{
		F32 timeNow = gFrameTimeSeconds;
		if( (timeNow - mlastBridgeCallTime) > 20)
		{
			mlastBridgeCallTime = timeNow;
			//llinfos << "Sending data to the bridge: " << toSendToBridge << llendl;
			JCLSLBridge::bridgetolsl("pos"+toSendToBridge, new LggPosCallback(avatarsToSendToBridge));

		}
		}
		
	}
	// finish
	mAvatarList->sortItems();
	mAvatarList->selectMultiple(selected);
	mAvatarList->setScrollPos(scrollpos);

//	llinfos << "avatar list refresh: done" << llendl;

}

// static
void LLFloaterAvatarList::onClickIM(void* userdata)
{
	//llinfos << "LLFloaterFriends::onClickIM()" << llendl;
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
		if(gSavedSettings.getBOOL("EmeraldRadarChatKeys"))
		{
			LLFloaterAvatarList* self = getInstance();
			if(self) self->clearAnnouncements();
		}else
		{
			if(gSavedSettings.getWarning("EmeraldRadarChat"))
				LLNotifications::instance().add("EmeraldRadarChat", LLSD(),LLSD(), callbackEmeraldChat);
	
		}
	}
}
void LLFloaterAvatarList::callbackEmeraldChat(const LLSD &notification, const LLSD &response)
{
	//gSavedSettings.setWarning("EmeraldOTR", FALSE);
	S32 option = LLNotification::getSelectedOption(notification, response);
	if ( option == 0 )
	{
		gSavedSettings.setWarning("EmeraldRadarChat",FALSE);
	}
	else if ( option == 1 )
	{
		gSavedSettings.setBOOL("EmeraldRadarChatKeys",true);
		gSavedSettings.setBOOL("EmeraldRadarChatAlerts",true);
		gSavedSettings.setBOOL("EmeraldAvatarListKeepOpen",true);
		LLFloaterAvatarList* self = getInstance();
		if(self) self->clearAnnouncements();
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
		LLTracker::trackLocationUpdate(mAvatars[mTrackedAvatar].getPosition(),name, tooltip);
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
	
	//if ( mAvatars.count( avatar ) < 0 )
	//{
		//return NULL;
	//}

	//return &mAvatars[avatar];
}

void LLFloaterAvatarList::onClickAgeAlert(LLUICtrl* ctrl,void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	gSavedSettings.setBOOL("EmeraldAvatarAgeAlert", avlist->childGetValue("agealert"));
}

void LLFloaterAvatarList::onClickAgeAlertDays(LLUICtrl* ctrl,void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	gSavedSettings.setF32("EmeraldAvatarAgeAlertDays", avlist->childGetValue("AgeAlertDays").asInteger());
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

LLColor4 LLFloaterAvatarList::getAvatarColor(LLAvatarListEntry *ent, F32 distance, e_coloring_type type)
{
// 	F32 r = 0.0f, g = 0.0f, b = 0.0f, a = 0.5f;
	LLColor4 av_color = gColors.getColor("DefaultListText").getValue();

	switch(type)
	{
		case CT_NONE:
			av_color = gColors.getColor("DefaultListText").getValue();
			break;
		case CT_DISTANCE:
			if ( distance <= 20.0f )
			{
				av_color = gColors.getColor("AvatarListTextDistNormalRange");
			}
			else if ( distance > 20.0f && distance <= 96.0f )
			{
				av_color = gColors.getColor("AvatarListTextDistShoutRange");
			}
			else
			{
				av_color = gColors.getColor("AvatarListTextDistOver");
			}
			break;
		case CT_AGE:
			if ( ent->mAvatarInfo.getStatus() == DATA_RETRIEVED )
			{
				S32 age = ent->mAvatarInfo.getValue().getAge();
				if ( age <= 7 )
				{
					av_color = gColors.getColor("AvatarListTextAgeYoung");
				}
				else
				{
					av_color = gColors.getColor("AvatarListTextAgeNormal");
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

	if ( mAvatars.size() == 0 )
	{
		return;
	}

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		ent = &iter->second;

		if ( ent->isDead() )
			continue;

		if ( (ent->getID() == mFocusedAvatar) && (prev != NULL)  )
		{
			removeFocusFromAll();
			prev->setFocus(TRUE);
			mFocusedAvatar = prev->getID();
			gAgent.lookAtObject(mFocusedAvatar, CAMERA_POSITION_OBJECT);
			return;
		}

		if ( (!marked_only) || ent->isMarked() )
		{
			prev = ent;
		}
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

	if ( mAvatars.size() == 0 )
	{
		return;
	}

	for(iter = mAvatars.begin(); iter != mAvatars.end(); iter++)
	{
		ent = &iter->second;

		if ( ent->isDead() )
			continue;

		if ( NULL == first && ((!marked_only) || ent->isMarked()))
		{
			first = ent;
		}

		if ( found && ((!marked_only) || ent->isMarked()) )
		{
			removeFocusFromAll();
			ent->setFocus(TRUE);
			mFocusedAvatar = ent->getID();
			gAgent.lookAtObject(mFocusedAvatar, CAMERA_POSITION_OBJECT);
			return;
		}

		if ( ent->getID() == mFocusedAvatar )
		{
			found = TRUE;
		} 
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
void LLFloaterAvatarList::onClickPrevInList(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;
	self->focusOnPrev(FALSE);
}

//static
void LLFloaterAvatarList::onClickNextInList(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;
	self->focusOnNext(FALSE);
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

//static
void LLFloaterAvatarList::onClickGetKey(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;
 	LLScrollListItem *item = self->mAvatarList->getFirstSelected();

	if ( NULL == item ) return;

	LLUUID agent_id = item->getUUID();

	char buffer[UUID_STR_LENGTH];		/*Flawfinder: ignore*/
	agent_id.toString(buffer);

	gViewerWindow->mWindow->copyTextToClipboard(utf8str_to_wstring(buffer));
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
/* ?? Deprecated?
static void send_estate_message(
	const char* request,
	const LLUUID &target)
{

	LLMessageSystem* msg = gMessageSystem;
	LLUUID invoice;

	// This seems to provide an ID so that the sim can say which request it's
	// replying to. I think this can be ignored for now.
	invoice.generate();

	llinfos << "Sending estate request '" << request << "'" << llendl;
	msg->newMessage("EstateOwnerMessage");
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->addUUIDFast(_PREHASH_TransactionID, LLUUID::null); //not used
	msg->nextBlock("MethodData");
	msg->addString("Method", request);
	msg->addUUID("Invoice", invoice);

	// Agent id
	msg->nextBlock("ParamList");
	msg->addString("Parameter", gAgent.getID().asString().c_str());

	// Target
	msg->nextBlock("ParamList");
	msg->addString("Parameter", target.asString().c_str());

	msg->sendReliable(gAgent.getRegion()->getHost());
}
*/
/* ?? Depcrecated? Apparently not in use
static void send_estate_ban(const LLUUID& agent)
{
	LLUUID invoice;
	U32 flags = ESTATE_ACCESS_BANNED_AGENT_ADD;

	invoice.generate();

	LLMessageSystem* msg = gMessageSystem;
	msg->newMessage("EstateOwnerMessage");
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->addUUIDFast(_PREHASH_TransactionID, LLUUID::null); //not used

	msg->nextBlock("MethodData");
	msg->addString("Method", "estateaccessdelta");
	msg->addUUID("Invoice", invoice);

	char buf[MAX_STRING];		// Flawfinder: ignore
	gAgent.getID().toString(buf);
	msg->nextBlock("ParamList");
	msg->addString("Parameter", buf);

	snprintf(buf, MAX_STRING, "%u", flags);			// Flawfinder: ignore
	msg->nextBlock("ParamList");
	msg->addString("Parameter", buf);

	agent.toString(buf);
	msg->nextBlock("ParamList");
	msg->addString("Parameter", buf);

	gAgent.sendReliableMessage();
}
*/
std::string keyasname(LLUUID id)
{
	std::string ret;
	gCacheName->getFullName(id,ret);
	return ret;
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
static void cmd_eject(const LLUUID& avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Ejecting "+ keyasname(avatar)+".");
	send_eject(avatar, false);
}
static void cmd_ban(const LLUUID& avatar, const std::string &name)
{
	cmdline_printchat("Moderation: Ejecting and banning "+ keyasname(avatar)+".");
	send_eject(avatar, true);
}
static void cmd_profile(const LLUUID& avatar, const std::string &name)
{
	LLFloaterAvatarInfo::showFromDirectory(avatar);
}
//static void cmd_mute(const LLUUID&avatar, const std::string &name)         { LLMuteList::getInstance()->add(LLMute(avatar, name, LLMute::AGENT)); }
//static void cmd_unmute(const LLUUID&avatar, const std::string &name)       { LLMuteList::getInstance()->remove(LLMute(avatar, name, LLMute::AGENT)); }



typedef std::vector<std::string> strings_t;
static void sendEstateOwnerMessage(
	LLMessageSystem* msg,
	const std::string& request,
	const LLUUID& invoice,
	const strings_t& strings)
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

void LLFloaterAvatarList::onClickFreeze(void *userdata)
{
	LLSD args;
	args["AVATAR_NAME"] = ((LLFloaterAvatarList*)userdata)->getSelectedNames();

	LLNotifications::instance().add("FreezeAvatarFullname", args, LLSD(), callbackFreeze);
}

void LLFloaterAvatarList::onClickEject(void *userdata)
{
	if(gSavedSettings.getBOOL("EmeraldModerateConfirm"))
	{
		LLSD args;
		args["AVATAR_NAME"] = ((LLFloaterAvatarList*)userdata)->getSelectedNames();
		LLNotifications::instance().add("EjectAvatarFullnameNoBan", args, LLSD(), callbackEject);
	}else
	{
		LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
		avlist->doCommand(cmd_eject);
	}
}

void LLFloaterAvatarList::onClickBan(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	avlist->doCommand(cmd_ban);
}

//static

void LLFloaterAvatarList::onClickScriptCount(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
 	LLScrollListItem *item =   avlist->mAvatarList->getFirstSelected();
	LLViewerObject *obj=gObjectList.findObject(item->getUUID());
	if(obj)
	{
		LLSelectMgr::getInstance()->selectObjectOnly(obj);
		ScriptCounter::serializeSelection(false);
		LLSelectMgr::getInstance()->deselectAll();
	}
}
//static
void LLFloaterAvatarList::onClickMute(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;

	//LLFloaterMute::showInstance();

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
}

//static
void LLFloaterAvatarList::onClickUnmute(void *userdata)
{
	LLFloaterAvatarList *self = (LLFloaterAvatarList*)userdata;

	//LLFloaterMute::showInstance();

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
					//LLFloaterMute::getInstance()->selectMute(agent_id);
				}
				else
				{
					//LLMute mute(agent_id, agent_name, LLMute::AGENT);
					//LLMuteList::getInstance()->add(mute);
				}
			}
		}
	}
}

//static
void LLFloaterAvatarList::onClickKickFromEstate(void *userdata)
{
	LLSD args;
	LLSD payload;
	args["EVIL_USER"] = ((LLFloaterAvatarList*)userdata)->getSelectedNames();
	LLNotifications::instance().add("EstateKickUser", args, payload, callbackEjectFromEstate);
}

void LLFloaterAvatarList::onClickBanFromEstate(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	avlist->doCommand(cmd_estate_ban);
	/*
	LLSD args;
	args["ALL_ESTATES"] = "all estates";
	LLNotifications::instance().add("EstateBannedAgentAdd", args, LLSD(), callbackBanFromEstate);*/
}

void LLFloaterAvatarList::onClickTPHFromEstate(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	avlist->doCommand(cmd_tph_estate);
}
void LLFloaterAvatarList::onClickGTFOFromEstate(void *userdata)
{
	onClickBanFromEstate(userdata);
	onClickKickFromEstate(userdata);
	onClickTPHFromEstate(userdata);
}
//static
void LLFloaterAvatarList::onClickAR(void *userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
 	LLScrollListItem *item =   avlist->mAvatarList->getFirstSelected();

	if ( item )
	{
		LLUUID agent_id = item->getUUID();
		LLAvatarListEntry *ent = avlist->getAvatarEntry(agent_id);
		
		if ( ent )
		{
			LLFloaterReporter::showFromObject(ent->getID());
		}
	}
}

// static
void LLFloaterAvatarList::onClickProfile(void* userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
	avlist->doCommand(cmd_profile);
}

//static
void LLFloaterAvatarList::onClickTeleport(void* userdata)
{
	LLFloaterAvatarList *avlist = (LLFloaterAvatarList*)userdata;
 	LLScrollListItem *item =   avlist->mAvatarList->getFirstSelected();

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

void LLFloaterAvatarList::checkAnnouncements()
{
	LLViewerRegion* regionp = gAgent.getRegion();
	if(!regionp)return;//ALWAYS VALIDATE DATA
	std::ostringstream ids;
	std::set<LLUUID> new_set;
	static int last_transact_num = 0;
	int transact_num = (int)gFrameCount;
	int num_ids = 0;

	if(!gSavedSettings.getBOOL("EmeraldRadarChatKeys"))
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
		 		gMessageSystem->addS32("ChatChannel", gSavedSettings.getS32("EmeraldRadarChatKeysChannel"));
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
	 	gMessageSystem->addS32("ChatChannel", gSavedSettings.getS32("EmeraldRadarChatKeysChannel"));
	 	gMessageSystem->addS32("ButtonIndex", 1);
	 	gMessageSystem->addString("ButtonLabel",llformat("%d,%d", transact_num, num_ids) + ids.str());
		gAgent.sendReliableMessage();
	}
}

void LLFloaterAvatarList::clearAnnouncements()
{
	mAnnouncedAvatars.clear();
}
