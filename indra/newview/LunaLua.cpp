/**
 * @file LunaLua.cpp
 * @brief Luna Viewer Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008-2009 Luna Contributors
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
 *
 * $Id$
 */

/**
* Structure:
*
* STATIC: FLLua
*  - sInstance=FLLua() (THREAD,INTERPRETER)
*/
#include "llviewerprecompiledheaders.h"

#include <vector>
#include <queue>
#include <boost/tokenizer.hpp>
#include "LunaLua.h"

/* Lua libraries */
extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

/* LL Objs */
#include "llhttpclient.h"
#include "llnotify.h"
#include "lluuid.h"
#include "llagent.h"
#include "llconsole.h"
#include "llviewerobject.h"
#include "llchat.h"
#include "llfloaterchat.h"
#include "llstring.h"
#include "message.h"
#include "llviewercamera.h"
#include "llgl.h"
#include "llrender.h"
#include "llglheaders.h"
#include "llversionviewer.h"
#include "llpartdata.h"
#include <sstream>

/* Lua classes */
#include "_LuaSL.h"	// SL module
#include "_LuaGL.h"	// GL module

#include "LuaBase_f.h"

#ifdef _WITH_CEGUI
	#include "FlexUI.h"
#endif

//#define LUA_HOOK_SPAM 

extern LLAgent gAgent;

extern "C" {
extern int luaopen_SL(lua_State* L); // declare the wrapped module
}

///////////////////////////////////////////////
// Hook Request 
///////////////////////////////////////////////

void HookRequest::Send()
{
	FLLua::callLuaHook(this);
}
HookRequest& HookRequest::operator<<(const int &in)
{
	std::ostringstream out;
	out << in;
	mArgs.push_back(out.str());
	return *this;
}
HookRequest& HookRequest::operator<<(const float &in)
{
	std::ostringstream out;
	out << in;
	mArgs.push_back(out.str());
	return *this;
}
HookRequest& HookRequest::operator<<(const std::string &in)
{
	mArgs.push_back(in);
	return *this;
}
HookRequest& HookRequest::operator<<(const char *in)
{
	mArgs.push_back(in);
	return *this;
}
HookRequest& HookRequest::operator<<(const LLUUID &fullid)
{
	mArgs.push_back(fullid.asString());
	return *this;
}

CB_Base *CB_Base::sent=NULL;//prevent perpetual loop

///////////////////////////////////////////////
// Lua Interpreter
///////////////////////////////////////////////
FLLua* FLLua::sInstance = NULL;

//Constructor
FLLua::FLLua() : 
	LLThread("Lua"), 
	pLuaStack(NULL),
	listening(false),
	//mError(false),
	mPendingHooks(false),
	mPendingCommands(false),
	mAllowPause(false),
	mPendingEvents(false),
	mCriticalSections(0)
{
	// Do nothing.
}
//Destructor
FLLua::~FLLua()
{
	while(!mQueuedHooks.empty()) //these hooks are dynamically allocated. We must delete.
	{
		delete mQueuedHooks.front();
		mQueuedHooks.pop();
	}
	while(!mQueuedEvents.empty()) 
	{
#ifdef FL_PRI_EVENTS //Not using yet
		delete mQueuedEvents.top();
#else
		delete mQueuedEvents.front();
#endif
		mQueuedEvents.pop();
	}
	if(pLuaStack)
		lua_close(pLuaStack);
	pLuaStack=NULL;
	if(sInstance==this)//Just incase
		sInstance=NULL;
}

// Static
bool FLLua::init()
{
	LL_INFOS("Lua") << "Starting Lua..." << llendl;
	cleanupClass();
	sInstance=new FLLua();
	if(!sInstance->load())
	{
		LL_INFOS("Lua") << "Failed to load Lua." << llendl;
		LuaError("Likely a script error. Please manually restart Lua when resolved");
		cleanupClass();
		return false;
	}
	sInstance->start();
	LL_INFOS("Lua") << "Lua started." << llendl;
	LUA_CALL0("OnLuaInit");
	return true;
}

// Static
void FLLua::cleanupClass()
{
	if(sInstance)
	{
		delete sInstance;
		sInstance=NULL;
	}
}
// Static
FLLua* FLLua::getInstance()
{
	LL_WARNS("Lua") << "Lua interpreter should not be directly accessed!" << llendl;
	return sInstance;
}
// Static
//	Called from MAIN thread
void FLLua::callCommand(const std::string &command)
{
	//sInstance being NULL indicates immediate error in lua loading. User intervention needed (broken script 99% time). 
	//Not convinced an auto-restart would be appropriate. Do manual restart with Lua Console.
	if(!sInstance)
	{
		LuaError("Lua awaiting manual restart.");
		return;
	}
	//else if(sInstance->mError && !FLLua::init()) //init is verbose.
	//	return;

	sInstance->lockData();
	sInstance->mQueuedCommands.push(command);
	sInstance->mPendingCommands=true;
	sInstance->unlockData();
}

// Static
//	Called from lua thread. Already mutex locked.
void FLLua::regClientEvent(CB_Base *entry)
{
	if(sInstance && entry)
	{
		sInstance->mQueuedEvents.push(entry);
		sInstance->mPendingEvents=true;
	}
}
#ifdef FL_PRI_EVENTS
bool operator< (const CB_Base& entry1, const CB_Base& entry2)
{
	return entry1.pri > entry2.pri;	
}
bool operator> (const CB_Base& entry1, const CB_Base& entry2)
{
	return entry1.pri < entry2.pri;	
}
#endif
// Static
//	Called from MAIN thread
void FLLua::execClientEvents()
{
	if(!sInstance)
		return;
	if(sInstance->mPendingEvents)
	{
		lldebugs << __LINE__ << ": Events pending.  Iterating through events." << llendl;
		sInstance->mPendingEvents=false;
		sInstance->lockData();
		while(!sInstance->mQueuedEvents.empty())
		{
#ifdef FL_PRI_EVENTS
			lldebugs << __LINE__ << ": Acquiring highest-priority event." << llendl;
			CB_Base *cb=sInstance->mQueuedEvents.top();
#else
			lldebugs << __LINE__ << ": Acquiring first event in queue." << llendl;
			CB_Base *cb=sInstance->mQueuedEvents.front();
#endif
			if(!cb)
			{
				llwarns << "Invalid pointer to event!" << llendl;
			} else {
				lldebugs << __LINE__ << ": Calling event." << llendl;
				cb->OnCall();
				delete cb;
			}
			sInstance->mQueuedEvents.pop();
		}
		sInstance->unlockData();
	}
	if(sInstance->isPaused())
	{
		if(sInstance->mAllowPause) //shouldn't ever happen.
		{
			sInstance->unpause();
			return;
		}
		int yields=0;
		LLTimer timer;
		timer.setTimerExpirySec(.25);
		while(!sInstance->mAllowPause)//mAllowPause == true when Lua thread finishes loop.
		{
			sInstance->unpause();
			yield(); //Hopefully let the Lua thread resume
			yields++;
			if(timer.hasExpired())
			{
				LL_WARNS("Lua") << "Aborting critical section after " 
					<< timer.getElapsedTimeF64()*(F64)1000.f << "ms " << llendl;
				sInstance->mAllowPause=true; // NOTE: Lua taking too much time. Force new critical requests
											 // to next frame.
				break;
			}
		}
		int sec=sInstance->mCriticalSections;
		if(sec)	//Main has resumed with Lua in a critical section. Not thread safe.
			LL_WARNS("Lua") << sec << " critical sections active. Unsafe resume!" << llendl;
		LL_INFOS("Lua") << "Finished critical section after " 
			<< timer.getElapsedTimeF64()*(F64)1000.f << "ms. Yields=" << yields << llendl;
	}	
}
// Static
//	Called from lua thread
bool FLLua::setCriticalSection(bool enter)
{
	if(!sInstance || sInstance->isStopped())
		return false;
	if(enter)
	{
		if(!sInstance->mAllowPause) //nested critical section
		{
			sInstance->mCriticalSections++;
			return true;
		}
		LL_INFOS("Lua") << "Lua thread requesting critical section."  << llendl;
		sInstance->mAllowPause=false;
		sInstance->pause();
		
		while(sInstance->shouldSleep())
			sInstance->mRunCondition->wait();
		sInstance->mCriticalSections++;
		LL_INFOS("Lua") << "Entering critical section."  << llendl;
	}
	else
		sInstance->mCriticalSections--;
	return true;
}

//Attempt to bind with Lua
bool FLLua::load()
{
	LL_INFOS("Lua") << "Thread initializing." << llendl;
	pLuaStack = lua_open();
	if(!pLuaStack)
	{
		LL_INFOS("Lua") << __LINE__ << ": Failed lua_open" << llendl;
		LuaError("lua_open() Failed");
		return false;
	}

	LL_INFOS("Lua") << __LINE__ << ": Skipping lua_atpanic" << llendl;
	lua_atpanic(pLuaStack, luaOnPanic);

	LL_INFOS("Lua") << __LINE__ << ": Loading standard Lua libs" << llendl;
	luaL_openlibs(pLuaStack);

	LL_INFOS("Lua") << __LINE__ << ": *** LOADING SWIG BINDINGS ***" << llendl;
	luaopen_SL(pLuaStack);

#ifdef _WITH_CEGUI
	LL_INFOS("Lua") << __LINE__ << ": *** LOADING Crazy Eddie's GUI BINDINGS ***" << llendl;
	initUI(pLuaStack);
#endif

	std::string  version; 

	LL_INFOS("Lua") << __LINE__ << ": Assigning _SLUA_VERSION" << llendl;
	// Assign _SLUA_VERSION, which contains the version number of the host viewer.
	version = llformat("_SLUA_VERSION=\"%d.%d.%d.%d\"",LL_VERSION_MAJOR,LL_VERSION_MINOR,LL_VERSION_PATCH,LL_VERSION_BUILD);
	if(luaL_dostring(pLuaStack, version.c_str()))
	{
		LuaError(Lua_getErrorMessage(pLuaStack).c_str());
		return false;
	}

	LL_INFOS("Lua") << __LINE__ << ": Assigning _SLUA_CHANNEL" << llendl;	
	// Assign _SLUA_CHANNEL, which contains the channel name of the host client.
	version = llformat("_SLUA_CHANNEL=\"%s\"",LL_DEFAULT_VIEWER_CHANNEL);
	if(luaL_dostring(pLuaStack, version.c_str()))
	{
		LuaError(Lua_getErrorMessage(pLuaStack).c_str());
		return false;
	}


	LL_INFOS("Lua") << __LINE__ << ": Assigning _SLUA_REVISION" << llendl;	
	// Assign _SLUA_CHANNEL, which contains the channel name of the host client.
	version = "_SLUA_REVISION=\"$Rev$\"";
	if(luaL_dostring(pLuaStack, version.c_str()))
	{
		LuaError(Lua_getErrorMessage(pLuaStack).c_str());
		return false;
	}

	LL_INFOS("Lua") << __LINE__ << ": LoadFile (_init_.lua)" << llendl;

	if(!LoadFile(gDirUtilp->getExpandedFilename(FL_PATH_LUA,"_init_.lua")))
		return false;
#if 0
	if(!LoadFile(gDirUtilp->getExpandedFilename(FL_PATH_MACROS,"unit_tests.lua")))
		return false;
#endif
	return true;
}
/// Run the interpreter.
void FLLua::run()
{
	LL_INFOS("Lua") << __LINE__ << ": *** THREAD LOOP STARTS HERE ***" << llendl;
	while(1)
	{
		if (!pLuaStack /*|| mError*/ || LLApp::isError() || LLApp::isStopped())
			break;	//Broked.

		bool locked=false;
		mAllowPause=true; //Let FLLUa::CriticalSection() sync with MAIN on first call.
		// Process Hooks
		if(mPendingHooks)
		{
			lockData();
			locked=true;
			mPendingHooks=false;
			while(!mQueuedHooks.empty()) //Allow multiple hooks per loop.
			{
				// Peek at the top of the stack
				HookRequest *hr = mQueuedHooks.front();	
				ExecuteHook(hr);
				mQueuedHooks.pop();
				delete hr; //Say no to memory leaks.
			}
			//if(mError)goto endloop;
		}
		// Process Macros/Raw Commands
		if(mPendingCommands)
		{
			if(!locked)
				lockData();
			locked=true;
			mPendingCommands=false;
			while(!mQueuedCommands.empty())
			{
				// Peek at the top of the stack
				std::string &hr = mQueuedCommands.front(); //byref. faster.
				if(FLLua::isMacro(hr))
					RunMacro(hr); // Run macro.
				else
					RunString(hr); // Run command.
				mQueuedCommands.pop(); //safe to pop now.
				//if(mError)goto endloop;
			} 
		}
		mAllowPause=true;
//endloop:
		if(locked) 
			unlockData(); //Always.
		//if(mError)break;
		yield();
		ms_sleep(10);
	}
	LL_INFOS("Lua") << __LINE__ << ": *** THREAD EXITING ***" << llendl;
}
//	Called from MAIN thread.
bool FLLua::LoadFile(std::string  file)
{
	if(luaL_dofile(pLuaStack,file.c_str()))
	{
		std::string  errmsg(Lua_getErrorMessage(pLuaStack));

		llwarns << "Unable to load " << file << ": " << errmsg << llendl;
			
		LuaError("Failed to load:");
		LuaError(file.c_str());
		LuaError(errmsg.c_str());
		LuaError("Aborting.");
		
		//Not locking thread here. RunFile is only ran from main loop now.
		// Locking while already in the (already locked) threaded loop would hardlock.
		//mError=true; //Probably shouldn't do this for macros.. In fact, mError could probably be done away with.

		return false;
	}
	else if(!listening)
	{
		lua_getglobal(pLuaStack,"CallHook"); //Would rather not issue lua functions every 10ms when we don't need to.
		listening = lua_isfunction(pLuaStack,-1);
		lua_pop(pLuaStack,1);  
	}
	return true;
}
//	Called from lua thread.
void FLLua::RunMacro(const std::string &what)
{
	std::string  macroname;
	std::stringstream args;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(" ");
	tokenizer tokens(what, sep);
	tokenizer::iterator token_iter;
	for(tokenizer::iterator token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter)
	{
		const std::string  &cur_token = *token_iter;
		if(cur_token=="/m" || cur_token=="/macro")
			continue;
		else if(macroname.empty())
			macroname=cur_token;
		else
			args << "\"" << cur_token << "\",";
	}
	
	if(macroname.empty())
	{
		LuaError("Macro Syntax: /m[acro] <MacroName> [args ...]");
		return;
	}

	std::string  macrofile(gDirUtilp->getExpandedFilename(FL_PATH_MACROS,macroname+".lua"));

	if(!gDirUtilp->fileExists(macrofile))
	{
		std::string  err("Unable to find " + macrofile + ". Aborting.");
		LuaError(err.c_str());
		return;
	}
	//std::string  message("Executing Lua Macro "+macrofile);
	// HACK.  Set global via luaL_dostring
	std::string  arglist("MACRO_ARGS={" + args.str() + "};");
	luaL_dostring(pLuaStack,arglist.c_str());
	if(luaL_dofile(pLuaStack,macrofile.c_str()))
		LuaError(Lua_getErrorMessage(pLuaStack).c_str());
}

//	Called from lua thread.
void FLLua::RunString(std::string &s)
{
	if(luaL_dostring(sInstance->pLuaStack,s.c_str()))
	{
		LL_INFOS("Lua") << "Run string(" << s << ") failed with" << Lua_getErrorMessage(sInstance->pLuaStack) << llendl;
		LuaError(Lua_getErrorMessage(sInstance->pLuaStack).c_str());
	}
}
//	Called from lua thread.
void FLLua::ExecuteHook(HookRequest *hook)
{
#ifdef LUA_HOOK_SPAM
	LL_INFOS("Lua") << "Firing event: " << hook->getName() << llendl;
#endif
		lua_getglobal(pLuaStack,"CallHook");		//+1
		lua_pushstring(pLuaStack,hook->getName());	//+1
		int lim=hook->getNumArgs();
		if(lim)										//+args
		{
#ifdef LUA_HOOK_SPAM
			LL_INFOS("Lua") << " args:";
#endif
			for(int i = 0;i<lim;i++)
			{
#ifdef LUA_HOOK_SPAM
				LL_CONT << " " << hook->getArg(i);
#endif
				lua_pushstring(pLuaStack,hook->getArg(i));
			}
#ifdef LUA_HOOK_SPAM
			LL_ENDL;
#endif
		}
		if(lua_pcall(pLuaStack,hook->getNumArgs()+1,1,0)!=0) //-1 -(args+1) +1   Current stack = +1
		{
			char errbuff[1024];
			sprintf(errbuff,"Error executing the %s hook: %s",hook->getName(),lua_tostring(pLuaStack,-1));
			LuaError(errbuff);
		}
#ifdef LUA_HOOK_SPAM
		else
		{
			const char *str = lua_tostring(pLuaStack,-1);
			if(atoi(str))
				LL_INFOS("Lua") << " called " << str << " callbacks" << llendl;
		}
#endif
		lua_pop(pLuaStack,1);	// -1
								// Current stack = 0
}

// Static
bool FLLua::isMacro(const std::string &str)
{
	return(str.substr(0,6)=="/macro" || str.substr(0,2)=="/m");
}

//static
// called from MAIN thread
void FLLua::callLuaHook(HookRequest *hook)
{
	if(!hook)
		return;
	else if(!sInstance || !sInstance->listening/*|| (sInstance->mError && !FLLua::init())*/)
		delete hook;
	else
	{
#ifdef LUA_HOOK_SPAM
		LL_INFOS("Lua") << "Adding event: " << hook.getName() << llendl;
#endif
		sInstance->lockData();
		sInstance->mQueuedHooks.push(hook);
		sInstance->mPendingHooks=true;
		sInstance->unlockData();
	}
}

int luaOnPanic(lua_State *L)
{	
	LuaError(("Lua Error: "+Lua_getErrorMessage(L)).c_str());
	return 0;
}

std::string  Lua_getErrorMessage(lua_State *L)
{
	if (lua_gettop(L) > 0)
	{
		if (lua_isstring(L, -1))
			return lua_tostring(L, -1);
	}
	return "";
}

#ifdef _WITH_CEGUI
void FLLua::initUI(lua_State *L)
{
	CEGUI::OpenGLRenderer& mRenderer=CEGUI::OpenGLRenderer::create();
	CEGUI::LuaScriptModule& mLuaModule=new CEGUI::LuaScriptModule(L);

	// (Pass 0 for the second argument, which is the xml parser to use. 0 means the default one)
	new CEGUI::System(mRenderer, 0, mLuaModule );
}

// static
void FLLua::render()
{
	LUA_CALL0("PreUIRender");

	// draw GUI (should not be between glBegin/glEnd pair)
	CEGUI::System::getSingleton().renderGUI();

	LUA_CALL0("PostUIRender");
}
#endif
