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
	HookRequest& operator<<(const int &in);
	HookRequest& operator<<(const float &in);
	HookRequest& operator<<(const std::string &in);
	HookRequest& operator<<(const char *in);
	HookRequest& operator<<(const LLUUID &in);
	HookRequest& operator<<(const lua_done *in)
		{Send(); return *this;}//Send off.

	HookRequest(const char *Name) { mName=Name; }
	HookRequest(const std::string& Name) { mName=Name; }

	void add(const std::string &in)
		{mArgs.push_back(in);}
	const char* getName() const
		{return mName.c_str(); }
	const char* getArg(unsigned idx) const
		{return (idx >= 0 && idx < mArgs.size()) ? mArgs[idx].c_str() : NULL;}
	int getNumArgs() const
		{return mArgs.size(); };

	void Send();
private:
	std::vector<std::string> mArgs;
	std::string mName;
};

struct CB_Base;
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

	//Injection of events into MAIN thread. Needed for gl calls. Render context does NOT carry over threads.
		//Called from lua thread
	static void regClientEvent(CB_Base *entry);
		//Called from MAIN thread
	static void execClientEvents();
private:
	bool load(); //pulled out of run so we can determine if load failed immediately.

	void run();
	
	bool LoadFile(std::string file);
	void RunMacro(const std::string &what);
	void RunString(std::string &s);
	void ExecuteHook(HookRequest *hook);

	static bool isMacro(const std::string &what);
	static void callLuaHook(HookRequest *hook); //Called via HookRequest::Send()
	
	//Set in init:
	// Object instance
	static FLLua *sInstance;
	// Lua stack
	lua_State *pLuaStack; 
	// Is CallHook present.
	bool listening;

	//Must mutex to access:
	// Outbound queued hooks
	std::queue<HookRequest*> mQueuedHooks;
	// Outbound queued commands
	std::queue<std::string> mQueuedCommands;
	// Inbound queued events
	std::queue<CB_Base*> mQueuedEvents;
	
	//Mutex free:
	// Read in both loops
	//LLAtomic32<bool> mError;	//Not used right now
	// Read in lua loop
	LLAtomic32<bool> mPendingHooks;		//!mQueuedHooks.isEmpty()
	LLAtomic32<bool> mPendingCommands;	//!mQueuedCommands.isEmpty()
	// Read in MAIN loop
	LLAtomic32<bool> mPendingEvents;	//!mQueuedEvents.isEmpty()
	
};

/*Ugly hacky code follows. Kludgy workaround for making SWIG (somewhat) thread-safe.

	Client callback classes. Checked and executed every frame.
	DONT USE POINTERS WITH THESE. These classes rely on static 
	allocation of passed arguments to avoid destruction of them 
	before next frame.
	Make sure passed variables are NON-VOLATILE.

	Usage:
		new CB_Args2<std::string,double>(setQueuedParams,paramname,1);

	Note the 'new'. It's required. These objects have to be dynamically allocated.

	Now, why is this needed at all? Some functions exposed to Lua will not be able to 
	safely be ran from the lua thread. 
	For instance LLCharacter::setVisualParamWeight will crash. It must be called from
	MAIN due to eventually calling glGenTextures. Opengl render context doesn't carry over
	to the lua thread, therefore opengl will fail to assign a texture index.

*/

struct CB_Base
{
	virtual void OnCall(){}; //overridden by derived classes.
	CB_Base(){FLLua::regClientEvent(this);}; //Always called
};
struct CB_Args0 : public CB_Base
{
	typedef void (*CB_FN)();
	virtual void OnCall()	{fn();}
	CB_FN fn;
	CB_Args0(const CB_FN _fn) : CB_Base(), fn(_fn){}
};
template <typename T1>
struct CB_Args1 : public CB_Base
{
	typedef void (*CB_FN)(T1& _t1);
	virtual void OnCall()	{fn(t1);}
	CB_FN fn;
	T1 t1;
	CB_Args1(const CB_FN _fn,const T1& _t1) : CB_Base(), fn(_fn), t1(_t1)
	{}
};
template <typename T1,typename T2>
struct CB_Args2 : public CB_Base
{
	typedef void (*CB_FN)(T1& _t1,T2& _t2);
	virtual void OnCall()	{fn(t1,t2);}
	CB_FN fn;
	T1 t1;	T2 t2;
	CB_Args2(const CB_FN _fn,const T1& _t1,const T2& _t2) : fn(_fn), CB_Base(), t1(_t1), t2(_t2)
	{}
};
template <typename T1,typename T2,typename T3>
struct CB_Args3 : public CB_Base
{
	typedef void (*CB_FN)(T1& _t1,T2& _t2,T3& _t3);
	virtual void OnCall()	{fn(t1,t2,t3);}
	CB_FN fn;
	T1 t1;	T2 t2;	T3 t3;
	CB_Args3(const CB_FN _fn,const T1& _t1,const T2& _t2,const T3& _t3) : fn(_fn), CB_Base(), t1(_t1), t2(_t2), t3(_t3)
	{}
};
//etc...

int luaOnPanic(lua_State *L);
std::string Lua_getErrorMessage(lua_State *L);


#endif
