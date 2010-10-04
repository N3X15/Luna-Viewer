
#ifndef LL_LLFLOATERAO_H
#define LL_LLFLOATERAO_H

#include "llfloater.h"
#include "llviewercontrol.h"
#include "llagent.h"


class AONoteCardDropTarget;

const int STATE_AGENT_IDLE = 0;
const int STATE_AGENT_WALK = 1;
const int STATE_AGENT_RUN = 2;
const int STATE_AGENT_STAND = 3;

const int STATE_AGENT_PRE_JUMP = 4;
const int STATE_AGENT_JUMP = 5;
const int STATE_AGENT_TURNLEFT = 6;
const int STATE_AGENT_TURNRIGHT = 7;

const int STATE_AGENT_SIT = 8;
const int STATE_AGENT_GROUNDSIT = 9;

const int STATE_AGENT_HOVER = 10;
const int STATE_AGENT_HOVER_DOWN = 11;
const int STATE_AGENT_HOVER_UP = 12;

const int STATE_AGENT_CROUCH = 13;
const int STATE_AGENT_CROUCHWALK = 14;
const int STATE_AGENT_FALLDOWN = 15;
const int STATE_AGENT_STANDUP = 16;
const int STATE_AGENT_LAND = 17;

const int STATE_AGENT_FLY = 18;
const int STATE_AGENT_FLYSLOW = 19;


const int STATE_AGENT_TYPING = 25;

const int STATE_AGENT_FLOATING = 26;
const int STATE_AGENT_SWIMMINGFORWARD = 27;
const int STATE_AGENT_SWIMMINGUP = 28;
const int STATE_AGENT_SWIMMINGDOWN = 29;

static LLFrameTimer mAODelayTimer;

class AOStandTimer : public LLEventTimer
{
public:
    AOStandTimer();
    ~AOStandTimer();
    virtual BOOL tick();
	virtual void reset();
};

class AOInvTimer : public LLEventTimer
{
public:
	AOInvTimer();
	~AOInvTimer();
	BOOL tick();
};

class LLFloaterAO : public LLFloater
{
public:

    LLFloaterAO();
	virtual BOOL postBuild();
	virtual void onClose(bool app_quitting);
	virtual void onOpen();
	
    virtual ~LLFloaterAO();

	static BOOL init();
	static BOOL fullfetch;

	static void toggle(void*); //Toggles interface visibility
	static void updateLayout(LLFloaterAO* floater);
	static void updateLayout2(LLFloaterAO* floater);

	static LLUICtrl* getComboBox(std::string name);

	static void onClickToggleAO(LLUICtrl *, void*);
	static void onClickToggleSits(LLUICtrl *, void*);
	static void onClickNoMouselookStands(LLUICtrl *, void*);
	
	static BOOL loadAnims();

	static int getAnimationState();
	static void setAnimationState(int state);
	static void setStates(const LLUUID& id, BOOL start);

	static LLUUID getCurrentStandId();
	static void setCurrentStandId(const LLUUID& id);
	static int stand_iterator;

	static void run();
	static void startAOMotion(const LLUUID& id, const BOOL stand, const BOOL announce);
	static void stopAOMotion(const LLUUID& id, const BOOL stand);
	static void ChangeStand(const BOOL announce);

	static LLUUID GetAnimID(const LLUUID& id);
	static LLUUID GetAnimIDFromState(const int state);
	static LLUUID GetAnimIDByName(const std::string name);
	static LLUUID GetOrigAnimIDFromState(const int state);
	static LLUUID GetRandomAnimID(const LLUUID& id);
	static LLUUID GetRandomAnimIDFromState(const int state);
	static int modifyUnderwaterState(const int state);
	static int GetStateFromOrigAnimID(const LLUUID& id);
	static int GetStateFromAOAnimID(const LLUUID& id);
	static std::string GetAnimTypeFromState(const int state);
	static BOOL isRandom(const int state);
	static int GetStateFromToken(std::string strtoken);

	static void onClickLess(void* data) ;
	static void onClickMore(void* data) ;

	static void onClickPrevStand(void* userdata);
	static void onClickNextStand(void* userdata);
	
	static void onClickReloadCard(void* userdata);
	static void onClickOpenCard(void* userdata);


	static LLUUID invfolderid;
	static const LLUUID& getAssetIDByName(const std::string& name);
	static LLFloaterAO* getInstance() { return sInstance;}
	
protected:

	static LLFloaterAO* sInstance;
	static int mAnimationState;
	static LLUUID mCurrentStandId;
	static AONoteCardDropTarget* mAOItemDropTarget;
	static void AOItemDrop(LLViewerInventoryItem* item);
	static void onSpinnerCommit(LLUICtrl* ctrl, void* userdata);
	static void onCheckBoxCommit(LLUICtrl* ctrl, void* userdata);
	static void onComboBoxCommit(LLUICtrl* ctrl, void* userdata);
	static BOOL SetDefault(void *userdata, LLUUID ao_id, std::string defaultanim);
	static void loadComboBoxes();


protected:
	static void onNotecardLoadComplete(LLVFS *vfs,const LLUUID& asset_uuid,LLAssetType::EType type,void* user_data, S32 status, LLExtStat ext_status);
};

#endif
