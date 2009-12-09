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
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

#include "llstring.h"
//typedef std::string LLString; // Fixing broken LL code.
#include "message.h"
#include "llviewerimage.h"

class HookRequest
{
public:
	HookRequest(const char *Name);

	void Add(const char *arg);
	
	const char* getName(){ return mName; }
	const char* getArg(unsigned idx){ return mArgs[idx]; }
	int getNumArgs(){ return mArgs.size(); };
private:
	std::vector<const char *> mArgs;
	const char *mName;
};

class FLLua : public LLThread
{
	//LOG_CLASS(FLLua);
public:
	FLLua();
	~FLLua();

	static void init();
	static FLLua* getInstance();

	static void callLuaHook(const char *EventName,int numargs,...);
	void RunString(std::string s);
	void run();

	void RunFile(std::string file);
	static bool isMacro(const std::string what);
	static void callMacro(const std::string cmd);
private:
	void RunMacro(const std::string what);
	
	lua_State *L; // Lua stack
	static FLLua *sInstance;

	// Queued hooks
	std::queue<HookRequest*> mQueuedHooks;
	std::queue<std::string> mQueuedCommands;

	void ExecuteHook(HookRequest *hook);
};

//extern FLLua *gLuaHooks;

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

int luaOnPanic(lua_State *L);
std::string Lua_getErrorMessage(lua_State *L);

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
