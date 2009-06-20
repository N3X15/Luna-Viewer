/** 
 * @file audioengine.h
 * @brief Definition of LLAudioEngine base class abstracting the audio support
 *
 * $LicenseInfo:firstyear=2000&license=viewergpl$
 * 
 * Copyright (c) 2000-2008, Linden Research, Inc.
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


#ifndef LL_AUDIOENGINE_H
#define LL_AUDIOENGINE_H

#include <list>
#include <map>

#include "listener.h"
#include "v3math.h"
#include "v3dmath.h"
#include "listener.h"
#include "lltimer.h"
#include "lluuid.h"
#include "llframetimer.h"
#include "llassettype.h"

const F32 LL_WIND_UPDATE_INTERVAL = 0.1f;
const F32 LL_ROLLOFF_MULTIPLIER_UNDER_WATER = 5.f;			//  How much sounds are weaker under water
const F32 LL_WIND_UNDERWATER_CENTER_FREQ = 20.f;

const F32 ATTACHED_OBJECT_TIMEOUT = 5.0f;
const F32 DEFAULT_MIN_DISTANCE = 2.0f;

#define MAX_CHANNELS 30
#define MAX_BUFFERS 40	// Some extra for preloading, maybe?

// This define is intended to allow us to switch from os based wav
// file loading to vfs based wav file loading. The problem is that I
// am unconvinced that the LLWaveFile works for loading sounds from
// memory. So, until that is fixed up, changed, whatever, this remains
// undefined.
//#define USE_WAV_VFILE

class LLVFS;

class LLAudioSource;
class LLAudioData;
class LLAudioChannel;
class LLAudioBuffer;



//
//  LLAudioEngine definition
//

class LLAudioEngine 
{
public:
	typedef enum LLAudioType
	{
		AUDIO_TYPE_NONE    = 0,
		AUDIO_TYPE_SFX     = 1,
		AUDIO_TYPE_UI      = 2,
		AUDIO_TYPE_AMBIENT = 3,
		AUDIO_TYPE_COUNT   = 4 // last
	};
	
	LLAudioEngine();
	virtual ~LLAudioEngine();

	// initialization/startup/shutdown
	//virtual BOOL init();

	virtual BOOL init(const S32 num_channels, void *userdata);
	virtual void shutdown();

	// Used by the mechanics of the engine
	//virtual void processQueue(const LLUUID &sound_guid);
	virtual void setListener(LLVector3 pos,LLVector3 vel,LLVector3 up,LLVector3 at);
	virtual void updateWind(LLVector3 direction, F32 camera_height_above_water) = 0;
	virtual void idle(F32 max_decode_time = 0.f);
	virtual void updateChannels();

	//
	// "End user" functionality
	//
	virtual BOOL isWindEnabled();
	virtual void enableWind(BOOL state_b);

	// Use these for temporarily muting the audio system.
	// Does not change buffers, initialization, etc. but
	// stops playing new sounds.
	virtual void setMuted(BOOL muted);
	virtual BOOL getMuted() const { return mMuted; }

	F32 getMasterGain();
	void setMasterGain(F32 gain);

	F32 getSecondaryGain(S32 type);
	void setSecondaryGain(S32 type, F32 gain);

	F32 getInternetStreamGain();

	virtual void setDopplerFactor(F32 factor);
	virtual F32 getDopplerFactor();
	virtual void setDistanceFactor(F32 factor);
	virtual F32 getDistanceFactor();
	virtual void setRolloffFactor(F32 factor);
	virtual F32 getRolloffFactor();
	virtual void setMaxWindGain(F32 gain);


	// Methods actually related to setting up and removing sounds
	// Owner ID is the owner of the object making the request
	void triggerSound(const LLUUID &sound_id, const LLUUID& owner_id, const F32 gain,
					  const S32 type = LLAudioEngine::AUDIO_TYPE_NONE,
					  const LLVector3d &pos_global = LLVector3d::zero);
	BOOL preloadSound(const LLUUID &id);

	void addAudioSource(LLAudioSource *asp);
	void cleanupAudioSource(LLAudioSource *asp);

	LLAudioSource *findAudioSource(const LLUUID &source_id);
	LLAudioData *getAudioData(const LLUUID &audio_uuid);


	virtual void startInternetStream(const std::string& url) = 0;
	virtual void stopInternetStream() = 0;
	virtual void pauseInternetStream(int pause) = 0;
	virtual int isInternetStreamPlaying() = 0;
	virtual void getInternetStreamInfo(char* artist, char* title) { artist[0] = 0; title[0] = 0; }
	// use a value from 0.0 to 1.0, inclusive
	virtual void setInternetStreamGain(F32 vol) { mInternetStreamGain = vol; }
	virtual const std::string& getInternetStreamURL() { return LLStringUtil::null; }

	// For debugging usage
	virtual LLVector3 getListenerPos();

	LLAudioBuffer *getFreeBuffer(); // Get a free buffer, or flush an existing one if you have to.
	LLAudioChannel *getFreeChannel(const F32 priority); // Get a free channel or flush an existing one if your priority is higher
	void cleanupBuffer(LLAudioBuffer *bufferp);

	BOOL hasDecodedFile(const LLUUID &uuid);
	BOOL hasLocalFile(const LLUUID &uuid);

	BOOL updateBufferForData(LLAudioData *adp, const LLUUID &audio_uuid = LLUUID::null);


	// Asset callback when we're retrieved a sound from the asset server.
	void startNextTransfer();
	static void assetCallback(LLVFS *vfs, const LLUUID &uuid, LLAssetType::EType type, void *user_data, S32 result_code, LLExtStat ext_status);


	friend class LLPipeline; // For debugging
public:
	F32 mMaxWindGain; // Hack.  Public to set before fade in?

protected:
	virtual LLAudioBuffer *createBuffer() = 0;
	virtual LLAudioChannel *createChannel() = 0;

	virtual void initWind() = 0;
	virtual void cleanupWind() = 0;
	virtual void setInternalGain(F32 gain) = 0;

	void commitDeferredChanges();

	virtual void allocateListener() = 0;


	// Internet stream methods
	virtual void initInternetStream() {}
	virtual void updateInternetStream() {}


	// listener methods
	virtual void setListenerPos(LLVector3 vec);
	virtual void setListenerVelocity(LLVector3 vec);
	virtual void orientListener(LLVector3 up, LLVector3 at);
	virtual void translateListener(LLVector3 vec);


	F64 mapWindVecToGain(LLVector3 wind_vec);
	F64 mapWindVecToPitch(LLVector3 wind_vec);
	F64 mapWindVecToPan(LLVector3 wind_vec);

protected:
	LLListener *mListenerp;

	BOOL mMuted;
	void* mUserData;

	S32 mLastStatus;
	
	S32 mNumChannels;
	BOOL mEnableWind;

	LLUUID mCurrentTransfer; // Audio file currently being transferred by the system
	LLFrameTimer mCurrentTransferTimer;

	// A list of all audio sources that are known to the viewer at this time.
	// This is most likely a superset of the ones that we actually have audio
	// data for, or are playing back.
	typedef std::map<LLUUID, LLAudioSource *> source_map;
	typedef std::map<LLUUID, LLAudioData *> data_map;

	source_map mAllSources;
	data_map mAllData;

	LLAudioChannel *mChannels[MAX_CHANNELS];

	// Buffers needs to change into a different data structure, as the number of buffers
	// that we have active should be limited by RAM usage, not count.
	LLAudioBuffer *mBuffers[MAX_BUFFERS];
	
	F32 mMasterGain;
	F32 mSecondaryGain[AUDIO_TYPE_COUNT];

	// Hack!  Internet streams are treated differently from other sources!
	F32 mInternetStreamGain;

	F32 mNextWindUpdate;

	LLFrameTimer mWindUpdateTimer;

private:
	void setDefaults();
};




//
// Standard audio source.  Can be derived from for special sources, such as those attached to objects.
//


class LLAudioSource
{
public:
	// owner_id is the id of the agent responsible for making this sound
	// play, for example, the owner of the object currently playing it
	LLAudioSource(const LLUUID &id, const LLUUID& owner_id, const F32 gain, const S32 type = LLAudioEngine::AUDIO_TYPE_NONE);
	virtual ~LLAudioSource();

	virtual void update();						// Update this audio source
	void updatePriority();

	void preload(const LLUUID &audio_id); // Only used for preloading UI sounds, now.

	void addAudioData(LLAudioData *adp, BOOL set_current = TRUE);

	void setAmbient(const BOOL ambient)						{ mAmbient = ambient; }
	BOOL isAmbient() const									{ return mAmbient; }

	void setLoop(const BOOL loop)							{ mLoop = loop; }
	BOOL isLoop() const										{ return mLoop; }

	void setSyncMaster(const BOOL master)					{ mSyncMaster = master; }
	BOOL isSyncMaster() const								{ return mSyncMaster; }

	void setSyncSlave(const BOOL slave)						{ mSyncSlave = slave; }
	BOOL isSyncSlave() const								{ return mSyncSlave; }

	void setQueueSounds(const BOOL queue)					{ mQueueSounds = queue; }
	BOOL isQueueSounds() const								{ return mQueueSounds; }

	void setPlayedOnce(const BOOL played_once)				{ mPlayedOnce = played_once; }

	void setType(S32 type)                                  { mType = type; }
	S32 getType()                                           { return mType; }

	void setPositionGlobal(const LLVector3d &position_global)		{ mPositionGlobal = position_global; }
	LLVector3d getPositionGlobal() const							{ return mPositionGlobal; }
	LLVector3 getVelocity()	const									{ return mVelocity; }				
	F32 getPriority() const											{ return mPriority; }

	// Gain should always be clamped between 0 and 1.
	F32 getGain() const												{ return mGain; }
	virtual void setGain(const F32 gain)							{ mGain = llclamp(gain, 0.f, 1.f); }

	const LLUUID &getID() const		{ return mID; }
	BOOL isDone();

	LLAudioData *getCurrentData();
	LLAudioData *getQueuedData();
	LLAudioBuffer *getCurrentBuffer();

	BOOL setupChannel();
	BOOL play(const LLUUID &audio_id);	// Start the audio source playing

	BOOL hasPendingPreloads() const;	// Has preloads that haven't been done yet

	friend class LLAudioEngine;
	friend class LLAudioChannel;
protected:
	void setChannel(LLAudioChannel *channelp);
	LLAudioChannel *getChannel() const						{ return mChannelp; }

protected:
	LLUUID			mID; // The ID of the source is that of the object if it's attached to an object.
	LLUUID			mOwnerID;	// owner of the object playing the sound
	F32				mPriority;
	F32				mGain;
	BOOL			mAmbient;
	BOOL			mLoop;
	BOOL			mSyncMaster;
	BOOL			mSyncSlave;
	BOOL			mQueueSounds;
	BOOL			mPlayedOnce;
	S32             mType;
	LLVector3d		mPositionGlobal;
	LLVector3		mVelocity;

	//LLAudioSource	*mSyncMasterp;	// If we're a slave, the source that we're synced to.
	LLAudioChannel	*mChannelp;		// If we're currently playing back, this is the channel that we're assigned to.
	LLAudioData		*mCurrentDatap;
	LLAudioData		*mQueuedDatap;

	typedef std::map<LLUUID, LLAudioData *> data_map;
	data_map mPreloadMap;

	LLFrameTimer mAgeTimer;
};




//
// Generic metadata about a particular piece of audio data.
// The actual data is handled by the derived LLAudioBuffer classes which are
// derived for each audio engine.
//


class LLAudioData
{
public:
	LLAudioData(const LLUUID &uuid);
	BOOL load();

	LLUUID getID() const				{ return mID; }
	LLAudioBuffer *getBuffer() const	{ return mBufferp; }

	BOOL	hasLocalData() const		{ return mHasLocalData; }
	BOOL	hasDecodedData() const		{ return mHasDecodedData; }
	BOOL	hasValidData() const		{ return mHasValidData; }

	void	setHasLocalData(const BOOL hld)		{ mHasLocalData = hld; }
	void	setHasDecodedData(const BOOL hdd)	{ mHasDecodedData = hdd; }
	void	setHasValidData(const BOOL hvd)		{ mHasValidData = hvd; }

	friend class LLAudioEngine; // Severe laziness, bad.

protected:
	LLUUID mID;
	LLAudioBuffer *mBufferp;	// If this data is being used by the audio system, a pointer to the buffer will be set here.
	BOOL mHasLocalData;
	BOOL mHasDecodedData;
	BOOL mHasValidData;
};


//
// Base class for an audio channel, i.e. a channel which is capable of playing back a sound.
// Management of channels is done generically, methods for actually manipulating the channel
// are derived for each audio engine.
//


class LLAudioChannel
{
public:
	LLAudioChannel();
	virtual ~LLAudioChannel();

	virtual void setSource(LLAudioSource *sourcep);
	LLAudioSource *getSource() const			{ return mCurrentSourcep; }

	void setSecondaryGain(F32 gain)             { mSecondaryGain = gain; }
	F32 getSecondaryGain()                      { return mSecondaryGain; }

	friend class LLAudioEngine;
	friend class LLAudioSource;
protected:
	virtual void play() = 0;
	virtual void playSynced(LLAudioChannel *channelp) = 0;
	virtual void cleanup() = 0;
	virtual BOOL isPlaying() = 0;
	void setWaiting(const BOOL waiting)			{ mWaiting = waiting; }
	BOOL isWaiting() const						{ return mWaiting; }

	virtual BOOL updateBuffer(); // Check to see if the buffer associated with the source changed, and update if necessary.
	virtual void update3DPosition() = 0;
	virtual void updateLoop() = 0; // Update your loop/completion status, for use by queueing/syncing.
protected:
	LLAudioSource	*mCurrentSourcep;
	LLAudioBuffer	*mCurrentBufferp;
	BOOL			mLoopedThisFrame;
	BOOL			mWaiting;	// Waiting for sync.
	F32             mSecondaryGain;
};




// Basically an interface class to the engine-specific implementation
// of audio data that's ready for playback.
// Will likely get more complex as we decide to do stuff like real streaming audio.


class LLAudioBuffer
{
public:
	virtual ~LLAudioBuffer() {};
	virtual BOOL loadWAV(const std::string& filename) = 0;
	virtual U32 getLength() = 0;

	friend class LLAudioEngine;
	friend class LLAudioChannel;
	friend class LLAudioData;
protected:
	BOOL mInUse;
	LLAudioData *mAudioDatap;
	LLFrameTimer mLastUseTimer;
};



extern LLAudioEngine* gAudiop;

#endif
