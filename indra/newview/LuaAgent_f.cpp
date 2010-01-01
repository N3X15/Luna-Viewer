#include "llviewerprecompiledheaders.h"

#include "LuaBase_f.h"
#include "LuaAgent_f.h"

#include "llviewerobjectlist.h"

#include "llagent.h"
// linden library includes
#include "message.h"

// viewer project includes
#include "llqueryflags.h"


/* No way to get LuaAgent object in lua. These functions seem pointless until such is added.
std::string LuaAgent::LuaGetFullName()
{
	std::string  name("wot");
	gAgent.getName(name);
	return std::string (name.c_str());
}

std::string  LuaAgent::LuaGetID()
{
	return gAgent.getID().asString();
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
	LLVOAvatar *av=gObjectList.findAvatar(name);
	if(!av)
	{
		LuaError("No Avatar Found");
		return "";
	}
	return av->getID().asString();
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
