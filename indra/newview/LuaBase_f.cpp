#include "llviewerprecompiledheaders.h"

#include "LuaBase_f.h"

#include <sstream>

#include "llchat.h"
#include "llappviewer.h"
#include "llfloaterchat.h"
#include "llnotify.h"
#include "llworldmap.h"
#include "llagent.h"
#include "llworld.h"
#include "message.h"
#include "lldir.h"
#include "llviewerstats.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llurldispatcher.h"
#include "lunaconsole.h"
#include "llmutelist.h"
#include "llchatbar.h"

//#include <direct.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

void LuaSendRawChat_Event(std::string &rawmsg, S32 &rawtype, BOOL &animate, S32 &channel);

bool gAllowWorldMap = true;

bool isUUID(const std::string &str)
{
	return LLUUID::validate(str);
}

// Send something to the chat window.
void LuaSendChat(const char *msg,bool IsDebug)
{
	std::string mesg(msg);
	LLChat dongs(mesg);
	dongs.mSourceType=CHAT_SOURCE_LUA;
	if(IsDebug) dongs.mChatType=CHAT_TYPE_DEBUG_MSG;
	LLFloaterChat::addChat(dongs);
	LLFloaterLuaConsole::addOutput(mesg,false); //no debug exception yet.
}

void LuaPrint(const char *msg, const char *arg1,const char *arg2,const char *arg3,const char *arg4,const char *arg5)
{
	std::string out(msg);
	if(arg1)
		(out+="    ")+=arg1;
	if(arg2)
		(out+="    ")+=arg2;
	if(arg3)
		(out+="    ")+=arg3;
	if(arg4)
		(out+="    ")+=arg4;
	if(arg5)
		(out+="    ")+=arg5;

	LuaSendChat(out.c_str(),false);
}

void LuaShout(const char* msg, int channel)
{
	LuaSendRawChat(msg,2,false,channel);
}

void LuaSay(const char* msg, int channel)
{
	LuaSendRawChat(msg,1,false,channel);
}

void LuaWhisper(const char* msg, int channel)
{
	LuaSendRawChat(msg,0,false,channel);
}

void LuaTouch(const LLUUID& id)
{	
	LLViewerObject* object = gObjectList.findObject(id);
	if(!object)
	{
		LuaError("No Object Found");
		return;
	}
	LLMessageSystem *msg = gMessageSystem;

	msg->newMessageFast(_PREHASH_ObjectGrab);
	msg->nextBlockFast( _PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast( _PREHASH_ObjectData);
	msg->addU32Fast(    _PREHASH_LocalID, object->mLocalID);
	msg->addVector3Fast(_PREHASH_GrabOffset, LLVector3::zero );
	msg->nextBlock("SurfaceInfo");
	msg->addVector3("UVCoord", LLVector3::zero);
	msg->addVector3("STCoord", LLVector3::zero);
	msg->addS32Fast(_PREHASH_FaceIndex, 0);
	msg->addVector3("Position", LLVector3::zero);
	msg->addVector3("Normal", LLVector3::zero);
	msg->addVector3("Binormal", LLVector3::zero);
	msg->sendMessage( object->getRegion()->getHost());

	// *NOTE: Hope the packets arrive safely and in order or else
	// there will be some problems.
	// *TODO: Just fix this bad assumption.
	msg->newMessageFast(_PREHASH_ObjectDeGrab);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_LocalID, object->mLocalID);
	msg->nextBlock("SurfaceInfo");
	msg->addVector3("UVCoord", LLVector3::zero);
	msg->addVector3("STCoord", LLVector3::zero);
	msg->addS32Fast(_PREHASH_FaceIndex, 0);
	msg->addVector3("Position", LLVector3::zero);
	msg->addVector3("Normal", LLVector3::zero);
	msg->addVector3("Binormal", LLVector3::zero);
	msg->sendMessage(object->getRegion()->getHost());
}
void LuaSendRawChat(std::string rawmsg, S32 rawtype, bool animate, S32 channel)
{
	CB_Args4<std::string,S32,BOOL,S32>(&LuaSendRawChat_Event,rawmsg,rawtype,(BOOL)animate,channel);
}

// Because Phoenix is dildos.
void send_chat_from_viewer(std::string utf8_out_text, EChatType type, S32 channel);

void LuaSendRawChat_Event(std::string &rawmsg, S32 &rawtype, BOOL &animate, S32 &channel)
{

	std::string mesg(rawmsg);
	EChatType type;
	switch(rawtype)
	{
	case 0:
		type=CHAT_TYPE_WHISPER;
		break;
	case 1:
	default:
		type=CHAT_TYPE_NORMAL;
		break;
	case 2:
		type=CHAT_TYPE_SHOUT;
		break;
	}

	send_chat_from_viewer(std::string(rawmsg),(EChatType)rawtype,channel);
}

void LuaError(const char* rawmsg)
{
	llwarns << rawmsg << llendl;
	std::string msg(rawmsg);
	LLChat err(msg);
	err.mSourceType=CHAT_SOURCE_LUA;
	err.mChatType=CHAT_TYPE_ERROR;
	LLFloaterChat::addChat(err);
	LLFloaterLuaConsole::addOutput(msg,true);
}

const char* LuaGetCWD()
{
	return gDirUtilp->getExecutableDir().c_str();
}

// Pop up an alert window.
void LuaAlert(const char* msg)
{
	LLSD args;
	args["MESSAGE"]=llformat("%s",msg);
	//LLNotifyBox::showXml("LuaAlert",args);
	LLNotifications::instance().add("LuaAlert",args);
}
 
// Retrieve the region handle of this sim
LLViewerRegion* LuaRegionFromName(const char *msg)
{
	std::string simname(msg);
	LLSimInfo *sim=LLWorldMap::getInstance()->simInfoFromName(simname);
	if(!sim)
	{
		LuaError("Sim Not Found");
		return NULL;
	}
	return LLWorld::getInstance()->getRegionFromHandle(sim->getHandle());
}

LLViewerRegion* LuaGetCurrentRegion()
{
	return gAgent.getRegion();
}
void LuaTp_Event(const std::string &val)
{
	if(val=="home")
		gAgent.teleportViaLandmark(LLUUID::null);
	else 
		LLURLDispatcher::dispatchFromTextEditor(val);
}
void LuaTp(const char* SimName, int x, int y, int z) //dispatchFromTextEditor calls gl destroy
{
	std::string name(SimName);
	std::transform(name.begin(), name.end(), name.begin(), tolower);
	if(name != "home")
		name=llformat("secondlife:///app/teleport/%s/%d/%d/%d",SimName,x,y,z);
	CB_Args1<const std::string>(&LuaTp_Event,name);
}

bool Lua_exists(const char* Filename)
{
	std::string file(Filename);
	return gDirUtilp->fileExists(file) ? true : false;
}

std::string getLuaFolder()
{
	return gDirUtilp->getExpandedFilename(FL_PATH_LUA,"");
}


std::string toLuaString(F32 t)
{
	std::stringstream out;
	out << t;
	std::string o=out.str();
	return o;
}

/*
2010-01-10T01:04:21Z INFO: add: Muting  id d1360af3-98cc-40fd-aa6f-5e4041025115 flags 0

Program received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0x881f3b70 (LWP 10962)]
0x084e28c9 in LLUUID (this=0x881f2920, rhs=...)
    at /root/Luna/trunk/indra/llcommon/lluuid.h:210
*/
bool muteAvatar(const LLUUID& derp)
{
	return LLMuteList::getInstance()->add(LLMute(derp,"",LLMute::AGENT,0))==TRUE;
}

bool isMuted(const LLUUID& avid,std::string name)
{
	return (LLMuteList::getInstance()->isMuted(avid,name)==TRUE);
}
