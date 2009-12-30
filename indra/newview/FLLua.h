#ifndef SHLLUA_H
#define SHLLUA_H
/**
 * @file FLLua.h
 * @brief FlexLife Viewer Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008-2009 FlexLife Contributors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "llviewerprecompiledheaders.h"

#include <string>
#include <queue>
#include <vector>

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include "llstring.h"
#include "message.h"
#include "llviewerimage.h"

//Macros for Lua event calls
//No arguments. LUA_CALL0("OnSomething");
//Arguments.	LUA_CALL("OnSomething") << arg1 << arg2 << ... << LUA_END;
class lua_done {};
#define LUA_CALL0(name) do{(new HookRequest(name))->Send();}while(0)
#define LUA_CALL(name) do{(*(new HookRequest(name))) 
#define LUA_END (lua_done*)0;}while(0)

class HookRequest
{
public:
	//These overrides create a simple shift method for adding arguments painlessly.
	//If unhandled type use add.
	HookRequest& operator<<(int in);
	HookRequest& operator<<(float in);
	HookRequest& operator<<(std::string &in);
	HookRequest& operator<<(const char *in);
	HookRequest& operator<<(LLUUID &in);
	HookRequest& operator<<(lua_done *in)
		{Send(); return *this;}//Send off.

	HookRequest(const char *Name) { mName=Name; }
	HookRequest(){} //Should do nothing. may resolve compiler bug on MSVC
	const char* getName()
		{return mName.c_str(); }
	void add(std::string &in)
		{mArgs.push_back(in);}
	const char* getArg(unsigned idx)
		{return (idx >= 0 && idx < mArgs.size()) ? mArgs[idx].c_str() : NULL;}
	int getNumArgs()
		{return mArgs.size(); };

	void Send();
private:
	std::vector<std::string> mArgs;
	std::string mName;
};

class FLLua : public LLThread
{
friend class HookRequest;
	//LOG_CLASS(FLLua);
public:
	FLLua();
	~FLLua();

	static bool init();
	static void cleanupClass();

	static FLLua* getInstance();

	static void callCommand(const std::string &cmd);

private:

	bool load(); //pulled out of run so we can determine if load failed immediately.

	void run();
	
	void RunFile(std::string file);
	void RunMacro(const std::string what);
	void RunString(std::string s);
	void ExecuteHook(HookRequest *hook);

	static bool isMacro(const std::string &what);
	static void callLuaHook(HookRequest *hook);
	
	static FLLua *sInstance;
	// Lua stack
	lua_State *pLuaStack; 
	// Queued hooks
	std::queue<HookRequest*> mQueuedHooks;
	std::queue<std::string> mQueuedCommands;
	// Is shit broken.
	bool mError;
	// Is CallHook present.
	bool listening;
};

int luaOnPanic(lua_State *L);
std::string Lua_getErrorMessage(lua_State *L);

//extern FLLua *gLuaHooks;

//Unused
/*
void FLHooks_CreateMetatable(lua_State *L);
void FLHooks_InitTable(lua_State *L, FLLua* lol);
int FLHooks_SetHook(lua_State* l);

int LuaBase_SendChat(lua_State *L);
void LuaBase_CreateMetatable(lua_State *L);

void Lua_RegisterMethod(lua_State* l, const char* name, lua_CFunction fn);
void Lua_SetClass(lua_State *l,const char* classname);
void Lua_CreateClassMetatable(lua_State* l, const char* name);
void Lua_CheckArgs(lua_State* l,int minArgs, int maxArgs, const char* errorMessage);
void Lua_PushClass(lua_State* l, const char* classname);
*/

void LuaSetAvOverlay(const char *uuid,int type);

class LuaAvatarOverlay
{
public:
	LuaAvatarOverlay(LLViewerImage *ViewerImage);
	~LuaAvatarOverlay();
	void render(void);

	void setParentPos( const LLVector3 p );
	LLVector3		mParentPos;
private:
	LLViewerImage	*mImage;
};
#endif
