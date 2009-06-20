#pragma once

#include "llworldmap.h"
#include "llvoavatar.h"

class LuaAgent
{	
public:
	std::string LuaGetFullName();
	std::string LuaGetID();

	void LuaSetTyping(bool On);
	bool LuaGetTyping();

	void LuaFollow(const char* name);
	//void LuaFollow(LLUUID faggotid);
};

class LuaWorldMap
{
public:
	LLSimInfo* LuaGetSimFromName(const char* name);
};

std::string LuaFindAvatarName(const char* uuid);
std::string LuaFindAvatarKey(const char* fullname);

// Internal
LLVOAvatar* LuaGetAvatar(const LLUUID& id);