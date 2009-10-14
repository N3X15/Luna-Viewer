// Copyright (c)2009 Thomas Shikami
// 
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "llviewerprecompiledheaders.h"

#include "tslua.h"

// Lia says: I don't know if this is the 'correct' way to do this, so please
//  yell at me if it is wrong. >_>  Fix for standalone builds.
#ifdef LL_STANDALONE
 #include <lua5.1/lua.hpp>
#else
#include "lua.hpp"
#endif

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
