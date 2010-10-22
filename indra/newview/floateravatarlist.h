//
// C++ Interface: llfloateravatarlist
//
// Description: 
//
//
// Author: Dale Glass <dale@daleglass.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "llfloater.h"
#include "llfloaterreporter.h"
#include "lluuid.h"
#include "lltimer.h"
#include "llchat.h"
#include "llscrolllistctrl.h"
#include "llmemberlistener.h"
//#include "viewer.h"

#include <time.h>
#include <map>
#include <set>

class LLFloaterAvatarList;

/**
 * @brief Account type
 */
enum ACCOUNT_TYPE
{
	ACCOUNT_RESIDENT,         /** Normal resident */
	ACCOUNT_TRIAL,            /** Trial account */
	ACCOUNT_CHARTER_MEMBER,   /** Lifetime account obtained during beta */
	ACCOUNT_EMPLOYEE,         /** Linden Lab employee */
	ACCOUNT_CUSTOM            /** Custom account title specified. Seems to apply to Philip Linden */
};

/**
 * @brief Payment data
 */
enum PAYMENT_TYPE
{
	PAYMENT_NONE,             /** No payment data on file */
	PAYMENT_ON_FILE,          /** Payment data filled, but not used */
	PAYMENT_USED,             /** Payment data used */
	PAYMENT_LINDEN            /** Payment info doesn't apply (Linden, etc) */
};


/**
 * @brief Activity
 *
 * This enum is ordered by priority, higher values have higher priority.
 * Since we only have one column to show activity, priority decides what
 * to show in case of conflict.
 */
enum ACTIVITY_TYPE
{
	ACTIVITY_NONE,           /** Avatar not doing anything */ 
	ACTIVITY_MOVING,         /** Changing position */
	ACTIVITY_GESTURING,	 /** Playing a gesture */
	ACTIVITY_SOUND,          /** Playing a sound */
	ACTIVITY_REZZING,        /** Rezzing objects */
	ACTIVITY_PARTICLES,      /** Creating particles */
	ACTIVITY_TYPING,         /** Typing */
	ACTIVITY_NEW,            /** Avatar just appeared */
	ACTIVITY_DEAD            /** Avatar isn't around anymore, and will be removed soon from the list */
};

enum DATA_STATUS
{
	DATA_UNKNOWN,
	DATA_REQUESTING,
	DATA_ERROR,
	DATA_RETRIEVED
};

/**
 * @brief How to color the user list
 */
enum e_coloring_type
{
	CT_NONE,
	CT_DISTANCE,
	CT_AGE,
	CT_SCORE,
	CT_PAYMENT
};

/**
 * @brief Template class for a piece of data that must be retrieved
 *
 * This class handles the storage, retries and delays required to obtain a piece
 * of data..
 */
template <class T> 
class LLAvatarListDatum
{
public:
	LLAvatarListDatum()
	{
		mMaxPending = 32;

		mRetryDelay = 0.0f;
		mRequestDelay = 1.0f;
		mFirstRequestTimeout = 16.0f;
		mMaxRequestTimeout = 8192.0f;

		mRequestTimer.start();
		mRequestDelayTimer.start();
		mStatus = DATA_UNKNOWN;
		
	}

	/**
	 * @brief Determines whether a data re-request is needed
	 * @returns TRUE if a re-request is needed
	 *
	 * This function takes the decision based on whether the data has
	 * been already retrieved, the last data request was long enough ago,
	 * and the request rate being low enough.
	 *
	 * A FALSE return value doesn't mean that everything is OK, only that
	 * a retry shouldn't be made at this time.
	 */
	BOOL retryNeeded()
	{
		switch(mStatus)
		{
			case DATA_UNKNOWN:
			case DATA_REQUESTING:
			case DATA_ERROR:
				// Don't request too many at once
				if ( mPending >= mMaxPending )
				{
					return FALSE;
				}

				// Don't re-request if the requests would be made too fast
				if ( mRequestDelayTimer.getElapsedTimeF32() < mRequestDelay )
				{
					return FALSE;
				}

				// Re-request if retry timeout was reached
				return ( mRequestTimer.getElapsedTimeF32() > mRetryDelay );
			case DATA_RETRIEVED:
				return FALSE;
		}

		return FALSE;
	}

	/**
	 * @brief Notifies the object that a request was started
	 */
	void requestStarted()
	{

		if ( mStatus != DATA_REQUESTING )
		{
			mPending++;
			//llinfos << "Pe
		}

		mStatus = DATA_REQUESTING;

		if ( mRetryDelay == 0 )
		{
			mRetryDelay = mFirstRequestTimeout;
		}
		else
		{
			if ( mRetryDelay < mMaxRequestTimeout )
			{
				mRetryDelay *= 2;
			}
		}

		mRequestTimer.start();
		mRequestDelayTimer.start();
	}

	/**
	 * @brief retryNeeded and requestStarted combined
	 * @returns TRUE if a re-request is needed
	 */
	BOOL requestIfNeeded()
	{
		BOOL ret = retryNeeded();
		if ( ret )
		{
			requestStarted();
		}

		return ret;
	}

	/**
	 * @brief Returns the request status
	 */
	DATA_STATUS getStatus()
	{
		if ( mStatus == DATA_REQUESTING 
		     && mRequestTimer.getElapsedTimeF32() > mRetryDelay )
		{
			mStatus = DATA_ERROR;

			// Consider this request failed, so the slot is free.
			mPending--;
		}

		return mStatus;
	}

	T& getValue()
{
		return mValue;
	}

	void setValue(T val)
	{
		if ( mStatus != DATA_RETRIEVED )
		{
			mPending--;
		}

		mValue = val;
		mStatus = DATA_RETRIEVED;
	}

	void setRequestDelay(F32 delay)
	{
		mRequestDelay = delay;
	}

	void setMaxPending(U32 count)
	{
		mMaxPending = count;
	}
private:
	friend class LLFloaterAvatarList;
	T mValue;

	DATA_STATUS mStatus;

	
	/**
	 * @brief How long to wait before trying to re-request the data
	 */
	F32 mRetryDelay;

	/**
	 * @brief How long to wait on the first attempt to get data
	 */
	F32 mFirstRequestTimeout;

	/**
	 * @brief Maximum delay between retries
	 *
	 * On failure, timeout gets doubled until it reaches this value
	 */
	F32 mMaxRequestTimeout;

	/**
	 * @brief Timer for the current request
	 */
	LLTimer mRequestTimer;



	/**
	 * @brief Timer used to separate requests, to avoid flooding the server
	 */
	static LLTimer mRequestDelayTimer;

	/**
	 * @brief Amount of pending requests
	 */
	static U32 mPending;


	/**
	 * @brief Time between requests of the same type
	 */
	F32 mRequestDelay;

	/**
	 * @brief Maximum amount of pending requests
	 */
	U32 mMaxPending;
};

template <class T> LLTimer LLAvatarListDatum<T>::mRequestDelayTimer;
template <class T> U32 LLAvatarListDatum<T>::mPending = 0;

//template <class T> F32 LLAvatarListDatum<T>::mRequestDelay;

/**
 * @brief Avatar payment information
 */
struct LLAvatarInfo
{
	PAYMENT_TYPE Payment;
	ACCOUNT_TYPE Account;
	struct tm    BirthDate;

	LLAvatarInfo();
	LLAvatarInfo(PAYMENT_TYPE payment, ACCOUNT_TYPE account, struct tm birth);
	S32 getAge();
};

/**
 * @brief Misc data about the avatar
 * This class holds replies from third party databases. There's no fixed format,
 * and interpretation is left to the end user.
 */
struct LLMiscDBInfo
{
	std::string data;

	LLMiscDBInfo(std::string d)
	{
		data = d;
	}

	LLMiscDBInfo()
	{
		data = "";
	}
};


/**
 * @brief This class is used to hold data about avatars.
 * We cache data about avatars to avoid repeating requests in this class.
 * Instances are kept in a map<LLAvatarListEntry>. We keep track of the
 * frame where the avatar was last seen. Avatars that are old enough are
 * not added to the visible list, but still kept around to cache the account
 * info data (like the birth date and the payment data)
 */
class LLAvatarListEntry {
public:


	/**
	 * @brief Initializes a list entry
	 * @param id Avatar's key
	 * @param name Avatar's name
	 * @param position Avatar's current position
	 * @param isLinden TRUE if the avatar is a Linden
	 */
	LLAvatarListEntry(const LLUUID& id = LLUUID::null, const std::string &name = "", const LLVector3d &position = LLVector3d::zero, BOOL isLinden = FALSE, BOOL isVoice = FALSE);

	/**
	 * Update world position.
	 * Affects age.
	 */	
	void setPosition(LLVector3d position, bool this_sim, bool drawn, bool chatrange);

	LLVector3d getPosition();

	BOOL getNeedsBridgeAssist();

	/**
	 * @brief Returns the age of this entry in frames
	 *
	 * This is only used for determining whether the avatar is still around.
	 * @see getEntryAgeSeconds
	 */
	bool getAlive();

	//for properly firing chat alerts based on prox
	U32 getOffSimFrames();
	U32 getOffDrawFrames();
	U32 getOutsideChatRangeFrames();

	/**
	 * @brief Returns the age of this entry in seconds
	 */
	F32 getEntryAgeSeconds();

	/**
	 * @brief Returns the name of the avatar
	 */
	std::string getName();
	time_t getTime();
	void resetTime();

	void setName(std::string name);

	LLUUID getID();

	void setID(LLUUID id);

	/**
	 * @brief Whether the avatar is a Linden
	 */
	BOOL getIsLinden();

	BOOL getIsVoice();
	void setIsVoice(BOOL voice);

	/**
	 * @brief Sets a custom title for the account
	 * @note Changes account type to ACCOUNT_CUSTOM
	 */
	void setAccountCustomTitle(std::string &title);

	/**
	 * @brief Gets the custom title for the account
	 */
	std::string getAccountCustomTitle();

	void setAlert();

	BOOL getAlert();

	/**
	 * @brief Returns whether the item is dead and shouldn't appear in the list
	 * @returns TRUE if dead
	 */
	BOOL isDead();

	void setFocus(BOOL value) { mFocused = value; }
	BOOL isMarked();
	void toggleMark();

private:
	friend class LLFloaterAvatarList;

	LLUUID mID;
	std::string mName;
	time_t mTime;
	LLVector3d mPosition;
	BOOL mNeedBridgeAssist;
	F32 mLastBridgeRequestTime;
	LLVector3d mDrawPosition;
	BOOL mAlert;
	BOOL mMarked;
	BOOL mFocused;
	BOOL mIsLinden;
	BOOL mIsVoice;

	ACTIVITY_TYPE mActivityType;

	std::string mAccountTitle;

	LLAvatarListDatum<LLAvatarInfo>   mAvatarInfo;
	LLAvatarListDatum<LLMiscDBInfo> mMiscInfo;

	LLTimer mUpdateTimer; //Timer to keep track of whether avatars are still there
	LLTimer mActivityTimer; //Timer for avatar activities
	U32 mFrame; //Last frame when this avatar was updated

	//last frame when this avatar was in sim
	U32 mInSimFrame;
	//last frame when this avatar was in draw
	U32 mInDrawFrame;
	//last frame when this avatar was in chat range
	U32 mInChatFrame;
};

//class LLIMSpeakerMgr;
class LLFloaterAvatarList : public LLFloater
{
private:
	LLFloaterAvatarList();
	LLHandle<LLView>	mPopupMenuHandle;
	
	class LLAgentProfile : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentChatcmd : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentGetkey : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentTrack : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentMark : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentScripts : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentIM : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentTpto : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentOffertp : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};

	class LLAgentMute : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentUnmute : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentAr : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentEject : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentBan : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentFreeze : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentUnfreeze : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};

	class LLAgentEstatekick : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentEstateban : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentEstatetphome : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};
	class LLAgentEstategtfo : public LLMemberListener<LLFloaterAvatarList>	{ public: /*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);	};


public:
	~LLFloaterAvatarList();

	virtual BOOL	handleRightMouseDown( S32 x, S32 y, MASK mask );
	virtual BOOL	handleKeyHere(KEY key, MASK mask);

	/*virtual*/ void onClose(bool app_quitting);
	/*virtual*/ void onOpen();
	/*virtual*/ BOOL postBuild();
	/*virtual*/ void draw();

	static void toggle(void*); //Toggles interface visibility
	static void showInstance();
	void updateAvatarList(); //Updates the internal avatar list with the currently present avatars.
	void updateAvatarListVoice(); //adds voice participants
	void refreshAvatarList(); //Refresh avatar list (display)
	static void processAvatarPropertiesReply(LLMessageSystem *msg, void**); //Process the reply to a request for avatar properties
	static void processBridgeReply(std::vector<LLUUID> avatars, LLSD bridgeResponce); //Process the update pos based on bridge info
	static void processSoundTrigger(LLMessageSystem *msg, void**); //Process the reply to a request for avatar properties
	static void callbackPhoenixChat(const LLSD &notification, const LLSD &response);

	BOOL avatarIsInList(LLUUID avatar); //Returns TRUE if the avatar is in the list of known avatars
	LLAvatarListEntry* getAvatarEntry(LLUUID avatar); //Returns the entry for an avatar, if preset
	std::string getSelectedNames(const std::string& separator = ", "); //Returns a string with the selected names in the list
	std::string getSelectedName();
	LLUUID getSelectedID();

	static LLFloaterAvatarList* getInstance(){ return sInstance; }
	static void lookAtAvatar(LLUUID &uuid);

private:
	static LLFloaterAvatarList* sInstance;
	enum AVATARS_COLUMN_ORDER
	{
		LIST_AVATAR_ICON,
		LIST_AVATAR_NAME,
		LIST_AVATAR_VOICE,
		LIST_DISTANCE,
		LIST_AGE,
		LIST_SIM,
		LIST_PAYMENT,
		LIST_ACTIVITY,
		LIST_TIME,
		LIST_CLIENT
	};

	typedef void (*avlist_command_t)(const LLUUID &avatar, const std::string &name);

	void speakText(S32 channel, EChatType type, std::string text);
	
	void removeFocusFromAll();
	void focusOnPrev(BOOL marked_only);
	void focusOnNext(BOOL marked_only);

	static void onClickProfile(void *userdata);
	static void onClickIM(void *userdata);
	static void onClickTeleportOffer(void *userdata);
	static void onClickTrack(void *userdata);
	static void onClickAgeAlert(LLUICtrl* ctrl,void *userdata);
	static void onClickAgeAlertDays(LLUICtrl* ctrl,void *userdata);
	static void onCommandCommit(LLUICtrl* ctrl,void *userdata);
	static void onClickMark(void *userdata);
	static void	onClickSayCmd(void *userdata);

	static void onClickPrevMarked(void *userdata);
	static void onClickNextMarked(void *userdata);
	
	static void	onClickTP(void *userdata);
	static void onDoubleClick(void *userdata);
	//static void lookAtAvatar(LLUUID &uuid);

	static void callbackFreeze(const LLSD& notification, const LLSD& response);
	static void callbackEject(const LLSD& notification, const LLSD& response);
	static void callbackAR(void *userdata);
	static void callbackEjectFromEstate(const LLSD& notification, const LLSD& response);
	static void callbackBanFromEstate(const LLSD& notification, const LLSD& response);

	static void callbackIdle(void *userdata);
	void doCommand(avlist_command_t cmd);
	void expireAvatarList();//Cleanup avatar list, removing dead entries from it.

private:
	LLLineEditor*	mInputEditor; //Pointer to the avatar scroll list
	LLScrollListCtrl*			mAvatarList;
	std::map<LLUUID, LLAvatarListEntry>	mAvatars;

	F32 mlastBridgeCallTime;

	struct LLAreaAlertEntry
	{
		LLUUID key;
		U32 area;
	};

	std::map<LLUUID, LLAreaAlertEntry>	mAreaAlertList;

	void sendAvatarPropertiesRequest(LLUUID avid);//Request information about the specified avatar
	void checkTrackingStatus();

	LLColor4 getAvatarColor(LLAvatarListEntry *ent, F32 distance, e_coloring_type type);

	BOOL mTracking;             // tracking?
	BOOL mTrackByLocation;      // TRUE if tracking by known position, FALSE for tracking a friend
	LLUUID mTrackedAvatar;     // who we're tracking

	LLTimer mDataRequestTimer;
	LLTimer mUpdateThrottleTimer;
	LLUUID mFocusedAvatar;

	std::set<LLUUID> mAnnouncedAvatars;

	void checkAnnouncements();
	void clearAnnouncements();
};
