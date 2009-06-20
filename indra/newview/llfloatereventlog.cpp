#include "llviewerprecompiledheaders.h"
#include "llfloatereventlog.h"

// #include "llvieweruictrlfactory.h" // builds floaters from XML
#include "lluictrlfactory.h"
#include "llviewerwindow.h"
#include "tlscrolllistctrl.h"
#include "llappviewer.h"
#include "llagent.h"
#include "llpartdata.h"
#include "llfloateravatarinfo.h"
#include "llmutelist.h"
#include "lltracker.h"
#include "llviewerregion.h"
#include "llviewerobject.h"
#include "llviewerpartsource.h"
#include "lltextbox.h"
#include "audioengine.h"
#include <iostream>
#include <vector>
#include <algorithm>


// TODO
// Angel Fluffy:
// 	1) cols need to be narrower
// 	2) score needs to be the 2nd col
// 	3) you need to be able to right click the key and copy it
// 	5) you need headers to make numbers readable (what does 'distance' mean, for example?
// 	8) perhaps a toggle to list all objects owned by avatars under the avatar's name, without doing individual objects? 
// 	9) I know that but I think you need to color-code it so that people don't go "is >10 a griefer? >100? >1000? >10,000?" the numbers are... less good.
// 	10) Those boxes need to be clearly marked so that you know what they are
//          e.g. "Textures by score" with "#1" and "#2" and "#3" below them
//
// Michi Lumin:
//	1) gohome button
//
// DONE:
// Thomas Shikami: 
//	1) add a bonus at that z-bouncing types
//
// NOT POSSIBLE/INCORRECT
// 	6) the blue button needs a purpose 
// 	7) if the particle texture has a name, then it would be great to show the name too



LLFloaterEventLog *gFloaterEventLog = NULL;

/*************************************************************************
* Events
*************************************************************************/

void LLWorldEvent::incCount(S32 count)
{
	if ( mLastFrame != gFrameCount )
	{
		mAgeTimer.reset();
		mDuration += mDurationTimer.getElapsedTimeAndResetF32();
		mLastFrame = gFrameCount;
	}

	mCount += count;
}

LLParticleEvent::LLParticleEvent(const LLPartSysData &data)
{
	mTexture = data.mPartImageID;
}

S32 LLParticleEvent::getScore() const
{

	// Extra score for being bouncy
	F32 bounce_factor = 1.0f;
	if ( mPartSysData.mPartData.mFlags & LLPartData::LL_PART_BOUNCE_MASK )
		bounce_factor = 1.5f;

	// TODO: Add bonus for bouncing
	return (S32)(mPartSysData.mBurstPartCount *
	       ( 1 + mPartSysData.mPartData.mMaxAge ) *
	       ( 1 / mPartSysData.mBurstRate ) *
	       ( 1 + mPartSysData.mPartData.mStartScale.magVec() + 
	             mPartSysData.mPartData.mEndScale.magVec() ) *
	             bounce_factor );
}


/*************************************************************************
* Emitters
*************************************************************************/

LLAvatarOwnedEmitter::LLAvatarOwnedEmitter(const LLUUID& id, const std::string& name)
{
	mNamePending = false;
	setOwner(id, name);

}

void LLAvatarOwnedEmitter::update()
{
	if ( mOwnerName.empty() && !mNamePending )
	{
		// llinfos << "Requesting name for id " << mOwnerID << llendl;
		mNamePending = true;
		gCacheName->get( mOwnerID, FALSE, (LLCacheNameCallback)nameLookupCallback, this );
	}
}

// static
void LLAvatarOwnedEmitter::nameLookupCallback(const LLUUID&id, const std::string&first, const std::string&last, BOOL group, void *userdata)
{
	if ( !first.empty() && !last.empty() && !id.isNull())
	{
		LLAvatarOwnedEmitter *em = (LLAvatarOwnedEmitter*)userdata;

		std::string name;

		name += first;
		name += " ";
		name += last;
	
		// llinfos << "Setting name for id " << em->mOwnerID << " to " << name << llendl;
//		em->mOwnerID = id;
		em->mOwnerName = name;
		em->mNamePending = false;
	}
}

LLAvatarOwnedEmitter::~LLAvatarOwnedEmitter()
{
	if ( mNamePending && gCacheName )
	{
		gCacheName->cancelCallback( mOwnerID, (LLCacheNameCallback)nameLookupCallback, this );
	}
}

LLObjectEmitter::LLObjectEmitter(const LLUUID& id, const LLUUID &owner)
{
	mRequestedProperties = false;

	setOwner(owner);
	setObject(id);
}

LLObjectEmitter::LLObjectEmitter(LLPointer<LLViewerObject> vo)
{
	mRequestedProperties = false;
	setObject(vo->getID());
	setPosition(vo->getPositionGlobal());

	if ( vo->isParticleSource() )
	{
		setOwner(vo->mPartSourcep->getOwnerUUID());
	}

	requestObjectInfo(vo->getRegion());
}



void LLObjectEmitter::requestObjectInfo(const LLViewerRegion *reg)
{
	LLAvatarOwnedEmitter::update();

	if ( !mObjectID.isNull() && mObjectName.empty() && !mRequestedProperties )
	{
		// llinfos << " --EM-- id: " << mObjectID << llendl;

		mRequestedProperties = true;
		
		LLMessageSystem* msg = gMessageSystem;
 
		msg->newMessageFast(_PREHASH_RequestObjectPropertiesFamily);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU32Fast(_PREHASH_RequestFlags, 0x0 );
		msg->addUUIDFast(_PREHASH_ObjectID, mObjectID );
		
		//LLViewerRegion* regionp = gAgent.getRegion();
		msg->sendReliable( reg->getHost() );
	}
}


/*************************************************************************
* Event Log
*************************************************************************/


LLFloaterEventLog::LLFloaterEventLog() :  LLFloater("event log")
{
	// Create interface from XML
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_event_log.xml");

	// Floater starts hidden	
	setVisible(FALSE);

	// Get a pointer to the scroll list from the interface
	mEventList = (TLScrollListCtrl *)getChild<LLScrollListCtrl>("event_log");
	// mEventList = (TLScrollListCtrl *)LLUICtrlFactory::getScrollListByName(this, "event_log");
	mSelectedTexture = getChild<LLTextureCtrl>("selected_texture");
	// mSelectedTexture = LLUICtrlFactory::getTexturePickerByName(this, "selected_texture");

	// List of all the names of the action buttons
	// This is used to help with translation issues: We copy the button's name
	// to the configurable button.
	mActionButtons[QB_AVATAR_PROFILE]   = getChild<LLButton>("av_profile_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "av_profile_btn");
	mActionButtons[QB_AVATAR_KEY]       = getChild<LLButton>("av_get_key_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "av_get_key_btn");
	mActionButtons[QB_AVATAR_TRACK]     = getChild<LLButton>("av_track_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "av_track_btn");
	mActionButtons[QB_AVATAR_MUTE]      = getChild<LLButton>("av_mute_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "av_mute_btn");
	mActionButtons[QB_AVATAR_EJECT]     = getChild<LLButton>("av_eject_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "av_eject_btn");
	mActionButtons[QB_AVATAR_LW_GOHOME] = getChild<LLButton>("av_luskwood_gohome_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "av_luskwood_gohome_btn");
	mActionButtons[QB_OBJECT_KEY]       = getChild<LLButton>("obj_get_key_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "obj_get_key_btn");
	mActionButtons[QB_OBJECT_TRACK]     = getChild<LLButton>("obj_track_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "obj_track_btn");
	mActionButtons[QB_OBJECT_MUTE]      = getChild<LLButton>("obj_mute_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "obj_mute_btn");
	mActionButtons[QB_OBJECT_RETURN]    = getChild<LLButton>("obj_return_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "obj_return_btn");
	mActionButtons[QB_UUID_BLACKLIST]   = getChild<LLButton>("uuid_blacklist_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "uuid_blacklist_btn");
	mActionButtons[QB_UUID_WHITELIST]   = getChild<LLButton>("uuid_whitelist_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "uuid_whitelist_btn");
	mActionButtons[QB_UUID_MUTELIST]    = getChild<LLButton>("uuid_mutelist_btn"); 
	// LLUICtrlFactory::getButtonByName(this, "uuid_mutelist_btn");


	mActionButtonsType[QB_AVATAR_PROFILE]   = QBT_AVATAR;
	mActionButtonsType[QB_AVATAR_KEY]       = QBT_AVATAR;
	mActionButtonsType[QB_AVATAR_TRACK]     = QBT_AVATAR;
	mActionButtonsType[QB_AVATAR_MUTE]      = QBT_AVATAR;
	mActionButtonsType[QB_AVATAR_EJECT]     = QBT_AVATAR;
	mActionButtonsType[QB_AVATAR_LW_GOHOME] = QBT_AVATAR;
	mActionButtonsType[QB_OBJECT_KEY]       = QBT_OBJECT;
	mActionButtonsType[QB_OBJECT_TRACK]     = QBT_OBJECT;
	mActionButtonsType[QB_OBJECT_MUTE]      = QBT_OBJECT;
	mActionButtonsType[QB_OBJECT_RETURN]    = QBT_OBJECT;
	mActionButtonsType[QB_UUID_BLACKLIST]   = QBT_UUID;
	mActionButtonsType[QB_UUID_WHITELIST]   = QBT_UUID;
	mActionButtonsType[QB_UUID_MUTELIST]    = QBT_UUID;


	// Make list of texture selectors
	for(int i=0;i<MAX_INTERESTING_TEXTURES;i++)
	{
		std::ostringstream o;

		o << "hiscore_texture_" << i;
		mHiscoreTextures[i] = getChild<LLTextureCtrl>(o.str()); 
		// LLUICtrlFactory::getTexturePickerByName(this, o.str());

		o.str(""); o << "hiscore_name_" << i;
		mHiscoreNames[i] = getChild<LLTextBox>(o.str()); 
		// LLUICtrlFactory::getTextBoxByName(this, o.str());

		o.str(""); o << "hiscore_owner_" << i;
		mHiscoreOwners[i] = getChild<LLTextBox>(o.str()); 
		// LLUICtrlFactory::getTextBoxByName(this, o.str());

		o.str(""); o << "hiscore_action_" << i;
		mHiscoreButtons[i] = getChild<LLButton>(o.str()); 
		// LLUICtrlFactory::getButtonByName(this, o.str());

		childSetAction(o.str(), onClickQuickButton, mHiscoreButtons[i]);
	}
	
	mEventList->setCallbackUserData(this);
	mEventList->sortByColumn("score", FALSE);
	mEventList->setDoubleClickCallback(onDoubleClick);

	childSetAction("av_profile_btn",         onClickAvatarProfile, this);
	childSetAction("av_get_key_btn",         onClickAvatarGetKey, this);
	childSetAction("av_track_btn",           onClickAvatarTrack, this);
	childSetAction("av_mute_btn",            onClickAvatarMute, this);
	childSetAction("av_eject_btn",           onClickAvatarEject, this);
	//childSetAction("av_luskwood_gohome_btn", onClickAvatarGohomerSendHome, this);

	childSetAction("obj_get_key_btn",        onClickObjectGetKey, this);
	childSetAction("obj_track_btn",          onClickObjectTrack, this);
	childSetAction("obj_mute_btn",           onClickObjectMute, this);
	childSetAction("obj_return_btn",         onClickObjectReturn, this);


	childSetAction("uuid_blacklist_btn",     onClickUUIDBlacklist, this);
	childSetAction("uuid_whitelist_btn",     onClickUUIDWhitelist, this);
	childSetAction("uuid_mutelist_btn",      onClickUUIDMutelist, this);

#ifdef DGMSGHANDLER
	LLMessageSystem *msg = gMessageSystem;
	msg->addHandlerFuncFast(_PREHASH_ObjectPropertiesFamily, processObjectPropertiesFamily);
	msg->addHandlerFuncFast(_PREHASH_ObjectPropertiesFamily, processObjectProperties);
#endif

	updateControlsState();
	
}

LLFloaterEventLog::~LLFloaterEventLog()
{
#ifdef DGMSGHANDLER
	LLMessageSystem *msg = gMessageSystem;
	if ( msg )
	{
		msg->delHandlerFuncFast(_PREHASH_ObjectPropertiesFamily, processObjectPropertiesFamily);
		msg->delHandlerFuncFast(_PREHASH_ObjectPropertiesFamily, processObjectProperties);
	}
#endif
}

void LLFloaterEventLog::show()
{
	// Make sure we make a noise.
	open();
}

void LLFloaterEventLog::updateControlsState()
{
	BOOL row_selected = FALSE;
	EVENT_TYPE type = LLFloaterEventLog::EVENT_PARTICLE; // TODO: fix

	if ( mEventList->getFirstSelected() ) row_selected = TRUE;


	childSetVisible("particle_info_container", row_selected && type == LLFloaterEventLog::EVENT_PARTICLE);
	childSetEnabled("av_profile_btn", row_selected);
	childSetEnabled("av_get_key_btn", row_selected);
	childSetEnabled("av_track_btn", row_selected);
	childSetEnabled("av_mute_btn", row_selected);
	childSetEnabled("av_eject_btn", row_selected);
	childSetEnabled("av_luskwood_gohome_btn", row_selected);

	childSetEnabled("obj_get_key_btn", row_selected);
	childSetEnabled("obj_track_btn", row_selected);
	childSetEnabled("obj_mute_btn", row_selected);
	childSetEnabled("obj_return_btn", row_selected);

	childSetEnabled("uuid_blacklist_btn", row_selected);
	childSetEnabled("uuid_whitelist_btn", row_selected);
	childSetEnabled("uuid_mutelist_btn", row_selected);
}

//static
void LLFloaterEventLog::toggle(void*) {
	if (!gFloaterEventLog) {
		llinfos << "No event log!" << llendl;
		return;
	}

	if (gFloaterEventLog->getVisible())
	{
		gFloaterEventLog->close();
	}
	else
	{
		gFloaterEventLog->show();
	}
}

//static
BOOL LLFloaterEventLog::visible(void*)
{
	return (gFloaterEventLog && gFloaterEventLog->getVisible());
}

void LLFloaterEventLog::logAvatarEvent( const LLUUID &avatar, const std::string &name, LLPointer <LLWorldEvent> ev)
{
	logEvent( new LLAvatarEmitter(avatar, name), ev);
}

void LLFloaterEventLog::logObjectEvent( const LLUUID &object, const LLUUID &owner, LLPointer <LLWorldEvent> ev)
{
	LLObjectEmitter *em = new LLObjectEmitter(object, owner);
	logEvent( em, ev);
}

void LLFloaterEventLog::logEvent( LLPointer<LLEventEmitter> emitter, LLPointer <LLWorldEvent> event)
{
	LLUUID id = emitter->getID();

	LLPointer<LLEventEmitter> em = getEmitter(id);

	if ( !em )
	{
		em = emitter;
		mEmitters[id] = em;
	}

	LLPointer<LLWorldEvent> ev = em->getEvent(event->getID());
	if ( !ev )
	{
		em->addEvent(event);
		ev = event;
	}

	//ev->incCount();
	*ev += *event;
}

void LLFloaterEventLog::logObject(  LLPointer<LLViewerObject> vo )
{
	// Ignore things that aren't interesting.
	if ( !vo->isParticleSource()) return;

	LLUUID emitter_id = vo->getID();

	LLPointer<LLEventEmitter> em = getEmitter(emitter_id);
	LLPointer<LLWorldEvent> ev;

	if ( !em )
	{
		em = new LLObjectEmitter(vo);
		mEmitters[emitter_id] = em;
	}

	if ( vo->isParticleSource() )
	{
		ev = em->getEvent( vo->mPartSourcep->mPartSysData.mPartImageID );
		if ( ! ev )
		{
			ev = new LLParticleEvent( vo->mPartSourcep->mPartSysData );
			em->addEvent( ev );
		}

		ev->incCount( vo->mPartSourcep->mPartSysData.mBurstPartCount );

		LLParticleEvent *pev = dynamic_cast<LLParticleEvent*>( ev.get() );
		if ( pev )
		{
			pev->setParticleData(vo->mPartSourcep->mPartSysData);
		}
	}
	LLAudioSource *src = vo->getAudioSource( em->getOwnerID() );
	if ( src )
	{
		// If there wasn't a sound played, getAudioSource adds one, with gain = 0.02f as
		// some sort of placeholder. That's not very interesting, so we ignore very 
		// silent sounds.

		LLAudioData *data = src->getCurrentData();

		if ( data )
		{
			LLUUID sound_id = data->getID();
			ev = em->getEvent( sound_id );

			if ( !ev )
			{
				ev = new LLSoundEvent( sound_id );
				em->addEvent( ev );
			}
		}
	}

}


struct DescendingEventSort
{
     bool operator()(LLPointer<LLWorldEvent> a, LLPointer<LLWorldEvent> b)
     {
          return a->getScore() > b->getScore();
     }
};

void LLFloaterEventLog::updateList()
{
	BOOL show_null       = getChild<LLCheckBoxCtrl>("show_null_cb")->get(); 
	// LLUICtrlFactory::getCheckBoxByName(this, "show_null_cb")->getValue();
	BOOL show_zero_count = getChild<LLCheckBoxCtrl>("show_zero_count_cb")->get(); 
	//LLUICtrlFactory::getCheckBoxByName(this, "show_zero_count_cb")->getValue();

	LLDynamicArray<LLUUID> selected = mEventList->getSelectedIDs();
	S32 scrollpos = mEventList->getScrollPos();

	U32 max_age = 120; // gSavedSettings.getU32("EventLogMaxAge");
	U32 max_entries = 64; // gSavedSettings.getU32("EventLogMaxEntries");
	LLVector3d mypos = gAgent.getPositionGlobal();

	mEventList->deleteAllItems();


	std::vector< LLPointer<LLWorldEvent> > annoyances;

	
	std::map<LLUUID, LLPointer<LLEventEmitter> >::iterator iter;

	for(iter = mEmitters.begin(); iter != mEmitters.end(); iter++)
	{
		LLPointer<LLEventEmitter> em = iter->second;


		LLVector3d delta = em->getPosition() - mypos;
		F32 distance = (F32)delta.magVec();

		//llinfos << "Emitter: " << em->getID() << llendl;

		std::map<LLUUID, LLPointer<LLWorldEvent> >::iterator iter_events;
		for(iter_events = em->mEvents.begin(); iter_events != em->mEvents.end(); iter_events++)
		{
			LLPointer<LLWorldEvent> ev = iter_events->second;

			if ( ev->getAge() > max_age ) continue;
			if ( --max_entries == 0 ) goto too_many_entries;
			if ( !show_null && ev->getID().isNull() ) continue;
			if ( !show_zero_count && ev->getCount() == 0 ) continue;

			ev->checkDuration();

			if ( !ev->getID().isNull() && ev->getAge() < 1.0f )
			{
				annoyances.push_back( ev );
			}

			LLSD row;

			row["id"] = ev->getGlobalID();

			row["columns"][LIST_EVENT_ICON]["column"] = "type_icon";
			row["columns"][LIST_EVENT_ICON]["type"]   = "text";
			row["columns"][LIST_EVENT_ICON]["value"]  = ev->getIcon();

			row["columns"][LIST_OWNER_NAME]["column"] = "owner_name";
			row["columns"][LIST_OWNER_NAME]["type"]   = "text";
			row["columns"][LIST_OWNER_NAME]["value"]  = em->getOwnerName();

			row["columns"][LIST_OBJECT_NAME]["column"] = "object_name";
			row["columns"][LIST_OBJECT_NAME]["type"]   = "text";
			row["columns"][LIST_OBJECT_NAME]["value"]  = em->getName();

			row["columns"][LIST_ID]["column"] = "id";
			row["columns"][LIST_ID]["type"]   = "text";
			row["columns"][LIST_ID]["value"]  = ev->getID();

			row["columns"][LIST_DISTANCE]["column"] = "distance";
			row["columns"][LIST_DISTANCE]["type"]   = "text";
			row["columns"][LIST_DISTANCE]["value"]  = llformat("%.2f", distance);

			row["columns"][LIST_COUNT]["column"] = "count";
			row["columns"][LIST_COUNT]["type"]   = "text";
			row["columns"][LIST_COUNT]["value"]  = ev->getCount();

			row["columns"][LIST_DURATION]["column"] = "duration";
			row["columns"][LIST_DURATION]["type"]   = "text";
			row["columns"][LIST_DURATION]["value"]  = llformat("%.2f", ev->getDuration());
			 
			row["columns"][LIST_AGE]["column"] = "age";
			row["columns"][LIST_AGE]["type"]   = "text";
			row["columns"][LIST_AGE]["value"]  = llformat("%.2f", ev->getAge());

			row["columns"][LIST_SCORE]["column"] = "score";
			row["columns"][LIST_SCORE]["type"]   = "text";
			row["columns"][LIST_SCORE]["value"]  = ev->getScore();

			// Add to list
			//llinfos << "Event: " << ev->getID() << llendl;
			mEventList->addElement(row, ADD_BOTTOM);
		}

		em->update();
	}

too_many_entries:
	// finish
	mEventList->sort();
	mEventList->selectMultiple(selected);
	mEventList->setScrollPos(scrollpos);


	// Update list of major annoyances
	std::sort(annoyances.begin(), annoyances.end(), DescendingEventSort());
	int count = 0;
	while( annoyances.size() > (unsigned)count && count < MAX_INTERESTING_TEXTURES)
	{
		mHiscoreTextures[count]->setImageAssetID( annoyances[count]->getID() );
		mHiscoreNames[count]->setText( annoyances[count]->getParent()->getName() );
		mHiscoreOwners[count]->setText( annoyances[count]->getParent()->getOwnerName() );
		mHiscoreButtons[count]->setEnabled(TRUE);
		mHiscoreEvents[count] = annoyances[count];
		count++;
	}

	// Clear remaining ones if any
	while( count < MAX_INTERESTING_TEXTURES )
	{
		mHiscoreTextures[count]->setImageAssetID( LLUUID::null );
		mHiscoreNames[count]->setText(std::string(""));
		mHiscoreOwners[count]->setText(std::string(""));
		mHiscoreButtons[count]->setEnabled(FALSE);
		mHiscoreEvents[count] = NULL;
		count++;
	}

}


void LLFloaterEventLog::updateQuickButtons()
{
	S32 action = LLFloaterEventLog::QB_OBJECT_MUTE; // gSavedSettings.getS32("EventLogQuickAction");
	if ( action >= QB_COUNT )
	{
		llwarns << "Quick action button set to out of range value: " << action << llendl;
		return;
	}

	std::string text = mActionButtons[action]->getLabelSelected();

	switch( mActionButtonsType[action] )
	{
		case QBT_AVATAR: text = "Avatar: " + text; break;
		case QBT_OBJECT: text = "Object: " + text; break;
		case QBT_UUID:   text = "UUID: " + text; break;
	}

	for(S32 i = 0;i<MAX_INTERESTING_TEXTURES;i++)
	{
		mHiscoreButtons[i]->setLabel(text);
	}
}

void LLFloaterEventLog::updateSelectedEventInfo()
{


	// Update texture selectors
	LLScrollListItem *selected_item =   mEventList->getFirstSelected();
	if ( selected_item )
	{
		LLParticleEvent *pev = dynamic_cast<LLParticleEvent*>( findEventBySelectedID(selected_item->getUUID()).get() );
		if ( pev )
		{
			mSelectedTexture->setImageAssetID( pev->getID() );
			std::string pattern;

			switch( pev->mPartSysData.mPattern )
			{
				case LLPartSysData::LL_PART_SRC_PATTERN_DROP:             pattern = "DROP"; break;
				case LLPartSysData::LL_PART_SRC_PATTERN_EXPLODE:          pattern = "EXPLODE"; break;
				case LLPartSysData::LL_PART_SRC_PATTERN_ANGLE:            pattern = "ANGLE"; break;
				case LLPartSysData::LL_PART_SRC_PATTERN_ANGLE_CONE:       pattern = "ANGLE_CONE"; break;
				case LLPartSysData::LL_PART_SRC_PATTERN_ANGLE_CONE_EMPTY: pattern = "ANGLE_CONE_EMPTY"; break;
				default:                                   pattern = "UNKNOWN"; break;
			}

			childSetTextArg("part_pattern",    "[PATTERN]",        pattern);
			childSetTextArg("part_texture",    "[TEXTURE]",        pev->getID().asString());
			childSetTextArg("part_burst_rate", "[BURST_COUNT]",    llformat("%i", (U32)pev->mPartSysData.mBurstPartCount));
			childSetTextArg("part_burst_rate", "[BURST_RATE]",     llformat("%.3f", pev->mPartSysData.mBurstRate));
			childSetTextArg("part_burst_rate", "[PARTICLE_COUNT]", llformat("%.3f", pev->mPartSysData.mBurstPartCount *
			                                                                        (1 / pev->mPartSysData.mBurstRate)) );

			childSetTextArg("part_radius",     "[RADIUS]",         llformat("%.3f", pev->mPartSysData.mBurstRadius));
			childSetTextArg("part_angle",      "[ANGLE_BEGIN]",    llformat("%.3f", pev->mPartSysData.mInnerAngle));
			childSetTextArg("part_angle",      "[ANGLE_END]",      llformat("%.3f", pev->mPartSysData.mOuterAngle));

			childSetTextArg("part_speed",      "[SPEED_MIN]",      llformat("%.3f", pev->mPartSysData.mBurstSpeedMin));
			childSetTextArg("part_speed",      "[SPEED_MAX]",      llformat("%.3f", pev->mPartSysData.mBurstSpeedMax));

			childSetTextArg("part_size",       "[SCALE_BEGIN]",    llformat("<%.3f, %.3f>", pev->mPartSysData.mPartData.mStartScale.mV[VX],
			                                                                                pev->mPartSysData.mPartData.mStartScale.mV[VY]));
			childSetTextArg("part_size",       "[SCALE_END]",      llformat("<%.3f, %.3f>", pev->mPartSysData.mPartData.mEndScale.mV[VX],
			                                                                                pev->mPartSysData.mPartData.mEndScale.mV[VY]));
			childSetTextArg("part_color",      "[COLOR_BEGIN]",    llformat("<%.3f, %.3f, %.3f>", pev->mPartSysData.mPartData.mStartColor.mV[0],
			                                                                                      pev->mPartSysData.mPartData.mStartColor.mV[1],
			                                                                                      pev->mPartSysData.mPartData.mStartColor.mV[2]));
			childSetTextArg("part_color",      "[COLOR_END]",      llformat("<%.3f, %.3f, %.3f>", pev->mPartSysData.mPartData.mEndColor.mV[0],
			                                                                                      pev->mPartSysData.mPartData.mEndColor.mV[1],
			                                                                                      pev->mPartSysData.mPartData.mEndColor.mV[2]));
			childSetTextArg("part_alpha",      "[ALPHA_BEGIN]",    llformat("%.3f", pev->mPartSysData.mPartData.mStartColor.mV[3]));
			childSetTextArg("part_alpha",      "[ALPHA_END]",      llformat("%.3f", pev->mPartSysData.mPartData.mEndColor.mV[3]));

			
			std::string flags = "";
			U32 f = pev->mPartSysData.mPartData.mFlags;
			if ( f &  LLPartData::LL_PART_INTERP_COLOR_MASK )      flags += "interp_color ";
			if ( f &  LLPartData::LL_PART_INTERP_SCALE_MASK )      flags += "interp_size ";
			if ( f &  LLPartData::LL_PART_BOUNCE_MASK )            flags += "bounce ";
			if ( f &  LLPartData::LL_PART_WIND_MASK )              flags += "wind ";
			if ( f &  LLPartData::LL_PART_FOLLOW_SRC_MASK )        flags += "follow_src ";
			if ( f &  LLPartData::LL_PART_FOLLOW_VELOCITY_MASK )   flags += "follow_vel ";
			if ( f &  LLPartData::LL_PART_TARGET_POS_MASK )        flags += "target ";
			if ( f &  LLPartData::LL_PART_TARGET_LINEAR_MASK )     flags += "linear ";
			if ( f &  LLPartData::LL_PART_EMISSIVE_MASK )          flags += "glow ";
			if ( f &  LLPartData::LL_PART_BEAM_MASK )              flags += "beam ";
			if ( f &  LLPartData::LL_PART_DEAD_MASK )              flags += "dead ";

			childSetTextArg("part_flags",      "[FLAGS]", flags);
		}
	}

}

void LLFloaterEventLog::updateWindow()
{
	if (!LLFloaterEventLog::visible(NULL))
	{
		return;
	}


	BOOL enabled         = getChild<LLCheckBoxCtrl>("enabled_cb")->get(); 
	// LLUICtrlFactory::getCheckBoxByName(this, "enabled_cb")->getValue();
	if ( enabled ) updateList();


	updateSelectedEventInfo();
	updateControlsState();
	updateQuickButtons();
}

void LLFloaterEventLog::processObjectPropertiesData(
			LLUUID id,std::string name,
			LLUUID owner_id,std::string fullname,
			std::string desc)
{
		// llinfos << "Processing object data for " << id << " n:" << name << " o:" << fullname << llendl;
	if ( gFloaterEventLog->mEmitters.count( id ) > 0 )
	{
		/* llinfos << " --EM-- id: " << id << " n: " << name << " o: " << fullname 
			<< " d: " << desc << llendl;
		/**/
		// llinfos << "Setting data for " << id << llendl;
		LLPointer<LLEventEmitter> em = gFloaterEventLog->mEmitters[id];
		em->setName( name );
		em->setOwner(owner_id,fullname);
	}
}
LLPointer<LLEventEmitter> LLFloaterEventLog::findEmitterBySelectedID(const LLUUID &id)
{
	std::map<LLUUID, LLPointer<LLEventEmitter> >::iterator iter;
	for(iter = mEmitters.begin(); iter != mEmitters.end(); iter++)
	{
		LLPointer<LLEventEmitter> em = iter->second;

		std::map<LLUUID, LLPointer<LLWorldEvent> >::iterator iter_events;
		for(iter_events = em->mEvents.begin(); iter_events != em->mEvents.end(); iter_events++)
		{
			LLPointer<LLWorldEvent> ev = iter_events->second;

			if ( ev->getGlobalID() == id )
			{
				return em;
			}
		}
	}
	
	return NULL;
}

LLPointer<LLWorldEvent> LLFloaterEventLog::findEventBySelectedID(const LLUUID &id)
{
	std::map<LLUUID, LLPointer<LLEventEmitter> >::iterator iter;
	for(iter = mEmitters.begin(); iter != mEmitters.end(); iter++)
	{
		LLPointer<LLEventEmitter> em = iter->second;

		std::map<LLUUID, LLPointer<LLWorldEvent> >::iterator iter_events;
		for(iter_events = em->mEvents.begin(); iter_events != em->mEvents.end(); iter_events++)
		{
			LLPointer<LLWorldEvent> ev = iter_events->second;

			if ( ev->getGlobalID() == id )
			{
				return ev;
			}
		}
	}

	return NULL;
}

//static
void LLFloaterEventLog::onDoubleClick(void *userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
 	LLScrollListItem *item =   self->mEventList->getFirstSelected();
	LLUUID item_id = item->getUUID();


	// TODO: Ok, this is rather ugly and should be reworked.
	// We've assigned an arbitrary ID to each element, as there's no way to
	// uniquely identify a row otherwise -- as both emitters and events may not
	// be unique.
	//
	// Now we need to find the emitter for a key, so we scan through the whole
	// list looking for it

	LLPointer<LLEventEmitter> em = self->findEmitterBySelectedID(item_id);
	if ( em )
	{
		gAgent.TLlookAtObject(em->getID(), CAMERA_POSITION_OBJECT);
		
	}
	else
	{
		llwarns << "Searched the whole list for " << item_id << " but couldn't find it!" << llendl;
	}
}



void LLFloaterEventLog::doCommand(avlist_command_t func, bool use_object_id, bool first_only)
{
	LLDynamicArray<LLUUID> ids = mEventList->getSelectedIDs();

	std::map<LLUUID, S32> seen_ids;

	// Go through the list of selections, removing duplicated object or avatar IDs.
	for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
	{
		LLUUID id = *itr;
		LLPointer<LLEventEmitter> pev = findEmitterBySelectedID(id);

		if ( pev  )
		{
			LLUUID dest_id;
			std::string dest_name;

			if ( use_object_id )
			{
				dest_id   = pev->getID();
				dest_name = pev->getName();
			}
			else
			{
				dest_id   = pev->getOwnerID();
				dest_name = pev->getOwnerName();
			}

			
			if ( seen_ids.count( dest_id ) < 1 )
			{
				func(dest_id, dest_name);

				if ( first_only ) return;
			}

			seen_ids[dest_id] = 1;
		}
	}



}


static void cmd_eject(const LLUUID& avatar, const std::string &name)       { /*send_eject(avatar, false);*/ }
static void cmd_profile(const LLUUID& avatar, const std::string &name)     { LLFloaterAvatarInfo::showFromDirectory(avatar); }
static void cmd_mute(const LLUUID&avatar, const std::string &name)         
{
	LLMuteList::getInstance()->add(LLMute(avatar, name, LLMute::AGENT)); 
}
static void cmd_gohome(const LLUUID&avatar, const std::string &name)
{
	gAgent.TLsendChat("gohome " + name + " " + avatar.asString(), 0, CHAT_TYPE_SHOUT); 
}
static void cmd_track(const LLUUID&avatar, const std::string &name)        { LLTracker::trackAvatar(avatar, name); }
static void cmd_get_key(const LLUUID&avatar, const std::string &name)
{
	char buffer[UUID_STR_LENGTH];		/*Flawfinder: ignore*/
	avatar.toString(buffer);
	gViewerWindow->mWindow->copyTextToClipboard(utf8str_to_wstring(buffer));
}

//////////////////////////////////////////////////////////////////////////
// Button handlers for avatars
//////////////////////////////////////////////////////////////////////////

//static
void LLFloaterEventLog::onClickAvatarGohomerSendHome(void *userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_gohome, false);
}

//static
void LLFloaterEventLog::onClickAvatarGetKey(void *userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_get_key, false, true);
}

//static
void LLFloaterEventLog::onClickAvatarEject(void *userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_eject, false);
}

//static
void LLFloaterEventLog::onClickAvatarMute(void *userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_mute, false);
}

//static
void LLFloaterEventLog::onClickAvatarProfile(void* userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_profile, false);
}

//static
void LLFloaterEventLog::onClickAvatarTrack(void *userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_track, false, true);
}

//////////////////////////////////////////////////////////////////////////
// Button handlers for objects
//////////////////////////////////////////////////////////////////////////

//static
void LLFloaterEventLog::onClickObjectGetKey(void* userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_get_key, true, true);
}


//static
void LLFloaterEventLog::onClickObjectTrack(void* userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_track, true, true);
}

//static
void LLFloaterEventLog::onClickObjectMute(void* userdata)
{
	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
	self->doCommand(cmd_mute, true);
}

//static
void LLFloaterEventLog::onClickObjectReturn(void* userdata)
{
//	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
//	self->doCommand(cmd_mute, true);
	llinfos << "not implemented" << llendl;
}

//////////////////////////////////////////////////////////////////////////
// Button handlers for UUIDs
//////////////////////////////////////////////////////////////////////////

//static
void LLFloaterEventLog::onClickUUIDBlacklist(void* userdata)
{
//	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
//	self->doCommand(cmd_mute, true);
	llinfos << "not implemented" << llendl;
}

//static
void LLFloaterEventLog::onClickUUIDWhitelist(void* userdata)
{
//	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
//	self->doCommand(cmd_mute, true);
	llinfos << "not implemented" << llendl;
}

//static
void LLFloaterEventLog::onClickUUIDMutelist(void* userdata)
{
//	LLFloaterEventLog *self = (LLFloaterEventLog*)userdata;
//	self->doCommand(cmd_mute, true);
	llinfos << "not implemented" << llendl;
}


//////////////////////////////////////////////////////////////////////////
// Quick buttons
//////////////////////////////////////////////////////////////////////////
//static
void LLFloaterEventLog::onClickQuickButton(void *button)
{
	S32 pos = -1;
	
	// Find this button in the list of buttons to obtain its position
	for(S32 i=0;i<MAX_INTERESTING_TEXTURES;i++)
	{
		if ( gFloaterEventLog->mHiscoreButtons[i] == button )
		{
			pos = i;
			break;
		}
	}

	if ( pos < 0 )
	{
		llwarns << "Failed to find myself in the floater!" << llendl;
		return;
	}
	
	// Get the event associated with that position
	LLPointer<LLWorldEvent>   ev = gFloaterEventLog->mHiscoreEvents[pos];
	LLPointer<LLEventEmitter> em = ev->getParent();

	S32 action = LLFloaterEventLog::QB_OBJECT_MUTE; // gSavedSettings.getS32("EventLogQuickAction");
	switch(action)
	{
		case QB_AVATAR_PROFILE:   cmd_profile( em->getOwnerID(), em->getOwnerName() ); break;
		case QB_AVATAR_KEY:       cmd_get_key( em->getOwnerID(), em->getOwnerName() ); break;
		case QB_AVATAR_TRACK:     cmd_track( em->getOwnerID(), em->getOwnerName() ); break;
		case QB_AVATAR_MUTE:      cmd_mute( em->getOwnerID(), em->getOwnerName() ); break;
		case QB_AVATAR_EJECT:     cmd_eject( em->getOwnerID(), em->getOwnerName() ); break;
		case QB_AVATAR_LW_GOHOME: cmd_gohome( em->getOwnerID(), em->getOwnerName() ); break;
		case QB_OBJECT_KEY:       cmd_get_key( em->getID(), em->getName() ); break;
		case QB_OBJECT_TRACK:     cmd_track( em->getID(), em->getName() ); break;
		case QB_OBJECT_MUTE:      cmd_mute( em->getID(), em->getName() ); break;
		case QB_OBJECT_RETURN:    llwarns << "Not implemented" << llendl; break;
		case QB_UUID_BLACKLIST:   llwarns << "Not implemented" << llendl; break;
		case QB_UUID_WHITELIST:   llwarns << "Not implemented" << llendl; break;
		case QB_UUID_MUTELIST:    llwarns << "Not implemented" << llendl; break;
		default:                  llwarns << "Unknown action " << action << llendl; break;
	}
}

