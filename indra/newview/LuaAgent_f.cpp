#include "llviewerprecompiledheaders.h"

#include "LuaBase_f.h"
#include "LuaAgent_f.h"

#include "llviewerobjectlist.h"

#include "llagent.h"
#include "llviewerparcelmgr.h"
// linden library includes
#include "message.h"

// viewer project includes
#include "llqueryflags.h"

#include "chatbar_as_cmdline.h"


/* No way to get LuaAgent object in lua. These functions seem pointless until such is added.
std::string LuaAgent::LuaGetFullName()
{
	std::string  name("wot");
	gAgent.getName(name);
	return std::string (name.c_str());
}

bool LuaAgent::LuaGetTyping()
{
	return (gAgent.getRenderState() & AGENT_STATE_TYPING);
}

void LuaAgent::LuaSetTyping(bool On)
{
	if(On)
		gAgent.startTyping();
	else
		gAgent.stopTyping();
}

void LuaAgent::LuaFollow(const char* name)
{
	
	LLUUID id(LuaFindAvatarKey(name));
	gAgent.startFollowPilot(id);
}

LLSimInfo* LuaWorldMap::LuaGetSimFromName(const char* name)
{
	LLSimInfo* wut = LLWorldMap::getInstance()->simInfoFromName(std::string (name));
	return wut;
}
*/
std::string  LuaFindAvatarName(const LLUUID &id)
{
	std::string  name;
	if(!gCacheName)
	{
		LuaError("gCacheName is NULL");
		return "";
	}
	gCacheName->getFullName(id,name);
	return name;
}

std::string  LuaFindAvatarKey(const char* fullname)
{
	std::string  name(fullname);
	LLUUID key = cmdline_partial_name2key(name);
	return key.asString();
}

LLParcel* Lua_getCurrentParcel()
{
	LLParcel* parcel = LLViewerParcelMgr::getInstance()->getAgentParcel();
	if (!parcel)
	{
		LuaError("Not standing on a parcel...");
		return FALSE;
	}
	return parcel;
}

/**
bool getParcelPermFlight(LLParcel* parcel)
{
	if (!parcel) return FALSE;

	// Allow owners to fly on their own land.
	if (LLViewerParcelMgr::isParcelOwnedByAgent(parcel, GP_LAND_ALLOW_FLY))
	{
		return TRUE;
	}

	return parcel->getAllowFly();
}
**/

LLUUID getParcelOwner(LLParcel* parcel)
{
	return parcel->getOwnerID();
}

LLUUID getMyID()
{
	return gAgent.getID();
}

std::string getMyName()
{
	std::string name;
	gAgent.getName(name);
	return name;
}

// Internal
LLVOAvatar* LuaGetAvatar(const LLUUID& id)
{
	LLViewerObject *obj = gObjectList.findObject(id);
	while (obj && obj->isAttachment())
	{
		obj = (LLViewerObject *)obj->getParent();
	}

	if (obj && obj->isAvatar())
	{
		return (LLVOAvatar*)obj;
	}
	else
	{
		return NULL;
	}
}
