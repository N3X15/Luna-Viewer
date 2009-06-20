//
// C++ Interface: llfloatereventlog
//
// Description: 
//
//
// Author: Dale Glass <dale@daleglass.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LL_LLFLOATERVENTLOG_H
#define LL_LLFLOATERVENTLOG_H

#include "llviewerprecompiledheaders.h"
#include "llfloater.h"
#include "llappviewer.h"
#include "llviewerobject.h"
#include "llviewercontrol.h"
#include "lltexturectrl.h"
#include "llpartdata.h"

#include <map>

using std::map;

class LLFloaterEventLog;
class LLEventEmitter;
class TLScrollListCtrl;

/*************************************************************************
* Events
*************************************************************************/

const S32 MAX_INTERESTING_TEXTURES = 3;


/**
 * @brief Event
 */
class LLWorldEvent : public LLRefCount
{
public:
	LLWorldEvent(const LLUUID &id = LLUUID::null ) : mDuration(0), mCount(0) { mLastFrame = gFrameCount; mGlobalID.generate();}

	F32 getAge() const { return mAgeTimer.getElapsedTimeF32(); }
	F32 getDuration() const { return mDuration; }
	S32 getCount() const { return mCount; }

	/**
	 * @brief Annoyance score
	 * Score that estimates how noticeable this event is
	 */
	virtual S32 getScore() const = 0;

	/**
	 * @brief Event identifier
	 * This is an UUID that identifies this event.
	 */
	virtual LLUUID getID() const = 0;

	/**
	 * @brief Unique identifier for this event
	 * Different from getID() in that getID() returns an ID related to the event somehow
	 * as it's the ID of a sound, texture, etc, and may not be globally unique.
	 *
	 * This one, however, is randomly generated and won't match anything else.
	 */
	LLUUID getGlobalID() const { return mGlobalID; }

	/**
	 * @brief Check duration timer, and update if needed
	 * Duration is defined as the amount of time spent on this event. This is calculated
	 * by adding up the times of the frames during which it happened. If during a frame,
	 * the count wasn't updated, then the event wasn't happening, and we reset the timer.
	 */
	void checkDuration() { if ( mLastFrame != gFrameCount ) mDurationTimer.reset(); }

	/**
	 * @brief Increment event count
	 */
	void incCount(S32 count = 1);

	bool operator+=(const LLWorldEvent &ev) { incCount(ev.mCount); return true; }
	bool operator<(const LLWorldEvent &ev) { return getScore() > ev.getScore(); }

	virtual std::string getIcon() = 0;

	void setParent(LLPointer<LLEventEmitter> parent) { mParent = parent; }
	LLPointer<LLEventEmitter> getParent() { return mParent; }
private:
	F32 mDuration;
	S32 mCount;
	LLTimer mAgeTimer;
	LLTimer mDurationTimer;
	U32 mLastFrame;

	LLUUID mGlobalID;
	LLPointer<LLEventEmitter> mParent;
};

/**
 * @brief Particle event
 * Represents a particle emission
 */
class LLParticleEvent : public LLWorldEvent
{
public:
	LLParticleEvent(const LLUUID &texture = LLUUID::null) : mTexture(texture) { }
	LLParticleEvent(const LLPartSysData &data);

	LLUUID getTexture() const { return mTexture; }
	virtual LLUUID getID() const { return mTexture; }

	virtual std::string getIcon() { return "P"; }

	virtual S32 getScore() const;

 	void setParticleData(const LLPartSysData &data) { mPartSysData = data; }
private:
	friend class LLFloaterEventLog;

	LLUUID mTexture;
	LLPartSysData mPartSysData;
	
};


class LLSoundEvent : public LLWorldEvent
{
public:
	LLSoundEvent(const LLUUID &sound = LLUUID::null) : mSound(sound) {}
	
	LLUUID getSound() const { return mSound; }
	virtual LLUUID getID() const { return mSound; }
	virtual S32 getScore() const { return 0; }
	virtual std::string getIcon() { return "S"; }
private:
	LLUUID mSound;
};


/*************************************************************************
* Emitters
*************************************************************************/

/**
 * @brief Emitter of events
 * This object represents something that emits events,
 * such as an avatar or an object
 */
class LLEventEmitter : public LLRefCount
{
public:
	LLEventEmitter() {}
	virtual void update() {}

	virtual std::string getOwnerName() const { return mOwnerName; }
	virtual LLUUID   getOwnerID() const { return mOwnerID; }

	void setOwner(const LLUUID &id, const std::string &name = "")
						{ 	mOwnerID = id; mOwnerName = name; }

	virtual std::string getName() const { return mObjectName; }
	virtual void setName(const std::string &name) { mObjectName = name; }

	virtual LLUUID getID() const { return mObjectID; }

	LLUUID getObjectID() const { return mObjectID; }
	std::string getObjectName() const { return mObjectName; }

	void setObject( const LLUUID& id, const std::string &name = "" )
						{ mObjectID = id; mObjectName = name;  }

	LLVector3d getPosition() const { return mPosition; }
	void setPosition(const LLVector3d &pos) { mPosition = pos; }

	void addEvent(LLPointer<LLWorldEvent> ev) { mEvents[ev->getID()] = ev; ev->setParent(this); }
	LLPointer<LLWorldEvent> getEvent(const LLUUID &id) { if ( mEvents.count( id ) > 0 )	{ return mEvents[id]; } return NULL; }

	LLUUID mObjectID;
	std::string mObjectName;

protected:
	friend class LLFloaterEventLog;

	LLUUID mOwnerID;
	std::string mOwnerName;

	LLVector3d mPosition;
	map <LLUUID, LLPointer<LLWorldEvent> > mEvents;

private:

};

/**
 * @brief Base class for objects owned by an avatar
 * This handles the part of looking up the avatar's name if needed.
 */
class LLAvatarOwnedEmitter: public LLEventEmitter
{
public:
	LLAvatarOwnedEmitter(const LLUUID& id = LLUUID::null, const std::string& name = "");
	~LLAvatarOwnedEmitter();

	virtual void update();

protected:

private:
	bool mNamePending;
	static void nameLookupCallback(const LLUUID&id, const std::string&first, const std::string&last, BOOL group, void *userdata);
};


/**
 * @brief Avatar emitter
 * Emitter for avatars -- the avatar itself is producing the event
 */
class LLAvatarEmitter : public LLAvatarOwnedEmitter
{
public:
	LLAvatarEmitter(const LLUUID& id = LLUUID::null, const std::string& name = "") { setOwner(id, name); }
	
	virtual LLUUID   getID() const { return mOwnerID; }
	virtual std::string getName() const { return "<avatar>"; }
private:
};

/**
 * @brief Object emitter
 * An object, owned by an avatar, is producing this event
 */
class LLObjectEmitter : public LLAvatarOwnedEmitter
{
public:
	LLObjectEmitter(const LLUUID& id = LLUUID::null, const LLUUID &owner = LLUUID::null);
	LLObjectEmitter(LLPointer <LLViewerObject> obj);
	/**
	 * @brief Request object info on the object
	 * There's a little complication here: We can't decide to request this at any time,
	 * as by then the object might have vanished, and the region as well.
	 *
	 * For this reason, this function only gets called from the constructor with the 
	 * LLViewerObject argument, or when the event is being updated, when the object is
	 * still obviously there.
	 */
	void requestObjectInfo(const LLViewerRegion *reg);

private:
	bool mRequestedProperties;
};



/*************************************************************************
* Event Log
*************************************************************************/

class LLFloaterEventLog : public LLFloater
{
public:
	enum QUICK_BUTTON_FUNCTION
	{
		QB_AVATAR_PROFILE,
		QB_AVATAR_KEY,
		QB_AVATAR_TRACK,
		QB_AVATAR_MUTE,
		QB_AVATAR_EJECT,
		QB_AVATAR_LW_GOHOME,
		QB_OBJECT_KEY,
		QB_OBJECT_TRACK,
		QB_OBJECT_MUTE,
		QB_OBJECT_RETURN,
		QB_UUID_BLACKLIST,
		QB_UUID_WHITELIST,
		QB_UUID_MUTELIST,
		QB_COUNT /* must be the last one */
	} quick_button_function_t;

	enum QUICK_BUTTON_TYPE
	{
		QBT_AVATAR,
		QBT_OBJECT,
		QBT_UUID
	} quick_button_type_t;

	LLFloaterEventLog();
	~LLFloaterEventLog();

	void show();
	virtual void onClose(bool app_quitting) { setVisible(FALSE); }
	static void toggle(void*);
	static BOOL visible(void*);

	void logAvatarEvent( const LLUUID &avatar, const std::string &name = "", LLPointer <LLWorldEvent> ev = NULL);
	void logObjectEvent( const LLUUID &object, const LLUUID &owner = LLUUID::null, LLPointer <LLWorldEvent> ev = NULL);

	void logObject( LLPointer<LLViewerObject> vo);

	void logEvent( LLPointer<LLEventEmitter> em, LLPointer <LLWorldEvent> ev);

	void updateWindow();

	static void processObjectPropertiesData(LLUUID id,std::string name,LLUUID owner_id,std::string fullname,std::string desc);
	static void onDoubleClick(void *userdata);

private:
	enum EVENTS_COLUMN_ORDER
	{
		LIST_EVENT_ICON,
		LIST_OWNER_NAME,
		LIST_OBJECT_NAME,
		LIST_ID,
		LIST_DISTANCE,
		LIST_COUNT,
		LIST_DURATION,
		LIST_AGE,
		LIST_SCORE
	} event_type_t;

	enum EVENT_TYPE
	{
		EVENT_PARTICLE,
		EVENT_SOUND
	};

	/**
	 * @brief Return pointer to emitter if present
	 */
	LLPointer<LLEventEmitter> getEmitter(const LLUUID &id) { if ( mEmitters.count( id ) > 0 )	{ return mEmitters[id]; } return NULL; }

	LLPointer<LLEventEmitter> findEmitterBySelectedID(const LLUUID &id);
	LLPointer<LLWorldEvent> findEventBySelectedID(const LLUUID &id);
	

	void updateControlsState();

	TLScrollListCtrl *mEventList;

	LLTextureCtrl *mSelectedTexture;

	LLTextureCtrl*          mHiscoreTextures[MAX_INTERESTING_TEXTURES];
	LLTextBox*              mHiscoreNames[MAX_INTERESTING_TEXTURES];
	LLTextBox*              mHiscoreOwners[MAX_INTERESTING_TEXTURES];
	LLButton*               mHiscoreButtons[MAX_INTERESTING_TEXTURES];
	LLPointer<LLWorldEvent> mHiscoreEvents[MAX_INTERESTING_TEXTURES];

	LLButton*             mActionButtons[QB_COUNT]; /** List of the names of all the buttons that perform an action */
	QUICK_BUTTON_TYPE   mActionButtonsType[QB_COUNT];

	map<LLUUID, LLPointer<LLEventEmitter> > mEmitters;


	void updateList();
	void updateSelectedEventInfo();
	void updateQuickButtons();

	typedef void (*avlist_command_t)(const LLUUID &avatar, const std::string&name);

	/** 
	 * @brief Perform a command on all the selected entries in the list
	 * @param func Function to call
	 * @param use_object_id Use the object's ID instead of the avatar's (command applies to object)
	 * @param first_only Only do the command for the first selection
	 */
	void LLFloaterEventLog::doCommand(avlist_command_t func, bool use_object_id, bool first_only = false);

	static void onClickAvatarGohomerSendHome(void *userdata);
	static void onClickAvatarGetKey(void *userdata);
	static void onClickAvatarEject(void *userdata);
	static void onClickAvatarMute(void *userdata);
	static void onClickAvatarProfile(void* userdata);
	static void onClickAvatarTrack(void* userdata);

	static void onClickObjectGetKey(void* userdata);
	static void onClickObjectTrack(void* userdata);
	static void onClickObjectMute(void* userdata);
	static void onClickObjectReturn(void* userdata);

	static void onClickUUIDBlacklist(void* userdata);
	static void onClickUUIDWhitelist(void* userdata);
	static void onClickUUIDMutelist(void* userdata);

	static void onClickQuickButton(void *userdata);
};



extern LLFloaterEventLog *gFloaterEventLog;

#endif
