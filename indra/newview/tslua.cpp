/* Copyright (c) 2009
 *
 * Modular Systems. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "llviewerprecompiledheaders.h"

#include "tslua.h"
#include "lua.hpp"

class TSInternalLua {
public:
	TSInternalLua(TSLua *tslua);
	~TSInternalLua();

public:
	lua_State *mLuaState;
	TSLua *mTSLua;
};

static int tsluaI_print(lua_State *L)
{
	TSLua *self;
	int n = lua_gettop(L);
	int i;
	lua_getfield(L, LUA_REGISTRYINDEX, "tslua");
	self = (TSLua *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_getglobal(L, "tostring");
	for(i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
		if (i > 1) self->output(" ");
		self->output(s);
		lua_pop(L, 1);
	}
	self->output("\n");
	return 0;
}

static int tsluaI_atpanic(lua_State *L)
{
	llerrs << "LUA error: " << lua_tostring(L, -1) << llendl;
	return 0;
}

static void tsluaI_hook(lua_State *L, lua_Debug *ar)
{
	TSLua *self;
	lua_getfield(L, LUA_REGISTRYINDEX, "tslua");
	self = (TSLua *)lua_touserdata(L, -1);
	if(self->abortCondition()) {
		luaL_error(L, "task aborted");
	}
}

TSInternalLua::TSInternalLua(TSLua *tslua)
{
	mTSLua = tslua;
	mLuaState = luaL_newstate();
	lua_atpanic(mLuaState, tsluaI_atpanic);
	lua_pushlightuserdata(mLuaState, tslua);
	lua_setfield(mLuaState, LUA_REGISTRYINDEX, "tslua");
	lua_pushlightuserdata(mLuaState, this);
	lua_setfield(mLuaState, LUA_REGISTRYINDEX, "tsinternallua");
	luaL_openlibs(mLuaState);
	lua_pushcfunction(mLuaState, tsluaI_print);
	lua_setglobal(mLuaState, "print");
	lua_sethook(mLuaState, tsluaI_hook, LUA_MASKLINE | LUA_MASKRET, 0);
}

TSInternalLua::~TSInternalLua()
{
	lua_close(mLuaState);
}

TSLua::TSLua()
{
	mInternalLua = new TSInternalLua(this);
}

TSLua::~TSLua()
{
	delete mInternalLua;
}

void TSLua::output(const char *s)
{
	// do nothing
}

void TSLua::error(const char *s)
{
	output(s);
}

bool TSLua::abortCondition()
{
	return false;
}

int TSLua::execute(const char *s)
{
	int stackpos = lua_gettop(mInternalLua->mLuaState);
	int num;
	int result = luaL_dostring(mInternalLua->mLuaState, s);
	if(result) {
		error(lua_tostring(mInternalLua->mLuaState, -1));
	}
	num = lua_gettop(mInternalLua->mLuaState) - stackpos;
	if(num) {
		lua_pop(mInternalLua->mLuaState, num);
	}
	return result;
}
