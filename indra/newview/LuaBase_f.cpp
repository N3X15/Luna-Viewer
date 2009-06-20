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

//#include <direct.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

bool gAllowWorldMap = true;

// Send something to the chat window.
void LuaSendChat(const char *msg,bool IsDebug)
{
	std::string mesg(msg);
	LLChat dongs(mesg);
	dongs.mSourceType=CHAT_SOURCE_LUA;
	if(IsDebug) dongs.mChatType=CHAT_TYPE_DEBUG_MSG;
	LLFloaterChat::addChat(dongs);
}

void LuaPrint(const char *msg)
{
	LuaSendChat(msg,false);
}

void LuaShout(const char* msg, const char* channel)
{
	S32 chan = strtol(channel,NULL,10);
	LuaSendRawChat(msg,2,false,chan);
}

void LuaSay(const char* msg, const char* channel)
{
	S32 chan = strtol(channel,NULL,10);
	LuaSendRawChat(msg,1,false,chan);
}

void LuaWhisper(const char* msg, const char* channel)
{
	S32 chan = strtol(channel,NULL,10);
	LuaSendRawChat(msg,0,false,chan);
}

void LuaSendRawChat(const char* rawmsg, int rawtype, bool animate, S32 channel)
{
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
	std::string mesg(rawmsg);
	LLWString wtext = utf8str_to_wstring(mesg);
	LLMessageSystem* msg = gMessageSystem;

	std::string utf8_out_text = mesg;
	std::string utf8_text = mesg;

	utf8_text = utf8str_trim(utf8_text);
	if (!utf8_text.empty())
	{
		utf8_text = utf8str_truncate(utf8_text, MAX_STRING - 1);
	}

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

	msg->newMessageFast(_PREHASH_ChatFromViewer);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ChatData);
	msg->addStringFast(_PREHASH_Message, utf8_out_text);
	msg->addU8Fast(_PREHASH_Type, type);
	msg->addS32("Channel", channel);

	gAgent.sendReliableMessage();

	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CHAT_COUNT);
}

void LuaError(const char* rawmsg)
{
	std::string msg(rawmsg);
	LLChat err(msg);
	err.mSourceType=CHAT_SOURCE_LUA;
	err.mChatType=CHAT_TYPE_ERROR;
	LLFloaterChat::addChat(err);
}

const char* LuaGetCWD()
{
	return gDirUtilp->getExecutableDir().c_str();
}

// Pop up an alert window.
void LuaAlert(const char* msg)
{
	LLStringUtil::format_map_t args;
	args["[MESSAGE]"]=llformat("%s",msg);
	LLNotifyBox::showXml("LuaAlert",args);
}
 
// Retrieve the region handle of this sim
LLViewerRegion* LuaRegionFromName(const char *msg)
{
	std::string simname(msg);
	LLSimInfo *sim=LLWorldMap::getInstance()->simInfoFromName(simname);
	return LLWorld::getInstance()->getRegionFromHandle(sim->mHandle);
}

LLViewerRegion* LuaGetCurrentRegion()
{
	return gAgent.getRegion();
}

void Lua_tp(const char* SimName, int x, int y, int z)
{
	std::string simname(SimName);
	LLVector3 pl(x,y,z);
	LLSimInfo *si=LLWorldMap::getInstance()->simInfoFromName(simname);
	gAgent.teleportRequest(si->mHandle,pl);
}

bool Lua_exists(const char* Filename)
{
	std::string file(Filename);
	return gDirUtilp->fileExists(file) ? true : false;
}

std::string getLuaFolder()
{
	return gDirUtilp->getExpandedFilename(SHL_PATH_LUA,"");
}


std::string toLuaString(F32 t)
{
	std::stringstream out;
	out << t;
	std::string o=out.str();
	return o;
}
