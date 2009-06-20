#ifndef SHLLUA_H
#define SHLLUA_H
/**
 * @file SHLLua.h
 * @brief ShoopedLife Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008 Patriotic Nigras
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

extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

#include "llstring.h"
typedef std::string LLString; // Fixing broken LL code.
#include "message.h"
#include "llviewerimage.h"

class SHLLua : public LLThread
{
	//LOG_CLASS(SHLLua);
public:
	SHLLua();
	~SHLLua();

	bool callLuaHook(const char *EventName,int numargs,...);
	void run();

	void RunFile(std::string file);
	static bool isMacro(const std::string what);
	void RunMacro(const std::string what);
private:
	lua_State *L; // Lua stack
public:
};

extern SHLLua *gLuaHooks;

void SHLHooks_CreateMetatable(lua_State *L);
void SHLHooks_InitTable(lua_State *L, SHLLua* lol);
int SHLHooks_SetHook(lua_State* l);

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
