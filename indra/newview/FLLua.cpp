/**
 * @file FLLua.cpp
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
#include "FLLua.h"

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
#include <sstream>

/* Lua classes */
#include "LuaBase.h"
#include "LuaBase_f.h"

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


///////////////////////////////////////////////
// Lua Interpreter
///////////////////////////////////////////////
FLLua* FLLua::sInstance = NULL;

FLLua::FLLua() : 
	LLThread("Lua"), 
	mError(false),
	pLuaStack(NULL),
	listening(false)
{
	// Do nothing.
}
FLLua::~FLLua()
{
	while(!mQueuedHooks.empty()) //these hooks are dynamically allocated. We must delete.
	{
		delete mQueuedHooks.front();
		mQueuedHooks.pop();
	}
	if(pLuaStack)
		lua_close(pLuaStack);
	pLuaStack=NULL;
	if(sInstance==this)//Just incase
		sInstance=NULL;
}

// Static
FLLua* FLLua::getInstance()
{
	LL_WARNS("Lua") << "Lua interpreter should not be directly accessed!" << llendl;
	return sInstance;
}
	//Called from lua thread
// Static
void FLLua::regClientEvent(CB_Base *entry)
{
	if(sInstance && entry)
		sInstance->mQueuedEvents.push(entry);
}
	//Called from MAIN thread
// Static
void FLLua::execClientEvents()
{
	if(!sInstance)
		return;
	
	sInstance->lockData();
	while(!sInstance->mQueuedEvents.empty())
	{
		CB_Base *cb=sInstance->mQueuedEvents.front();
		cb->OnCall();
		delete cb;
		sInstance->mQueuedEvents.pop();
	}
	sInstance->unlockData();
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
void FLLua::callCommand(const std::string &command)
{
	//sInstance being NULL indicates immediate error in lua loading. User intervention needed (broken script 99% time). 
	//Not convinced an auto-restart would be appropriate. Do manual restart with Lua Console.
	if(!sInstance)
	{
		LuaError("Lua awaiting manual restart.");
		return;
	}
	else if(sInstance->mError && !FLLua::init()) //init is verbose.
		return;
  	
	sInstance->lockData();
	sInstance->mQueuedCommands.push(command);
	sInstance->unlockData();
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
	// Assign _SLUA_CHANNEpLuaStack, which contains the channel name of the host client.
	version = llformat("_SLUA_CHANNEL=\"%s\"",LL_CHANNEL);
	if(luaL_dostring(pLuaStack, version.c_str()))
	{
		LuaError(Lua_getErrorMessage(pLuaStack).c_str());
		return false;
	}

	LL_INFOS("Lua") << __LINE__ << ": Runfile (_init_.lua)" << llendl;
	RunFile(gDirUtilp->getExpandedFilename(FL_PATH_LUA,"_init_.lua"));
	if(mError)
		return false;
#if 0
	RunFile(gDirUtilp->getExpandedFilename(FL_PATH_MACROS,"unit_tests.lua"));
	if(mError)
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
		if (!pLuaStack || LLApp::isError() || LLApp::isStopped())
			break;	//Broked.
			

		// Process Hooks
		lockData();

		//LL_INFOS("Lua") << __LINE__ << ": Checking if hooks are empty" << llendl;
		while(!mQueuedHooks.empty()) //Allow multiple hooks per loop.
		{
			//LL_INFOS("Lua") << __LINE__ << ": Hooks not empty" << llendl;

			// Peek at the top of the stack
			HookRequest *hr = mQueuedHooks.front();
			ExecuteHook(hr); //Currently events are lost if CallHook wasn't found (not listening)
			mQueuedHooks.pop();
			delete hr; //Say no to memory leaks.
		}
		//else 
		//{
		//	LL_INFOS("Lua") << __LINE__ << ": Hooks empty" << llendl;
		//}
		//LL_INFOS("Lua") << __LINE__ << ": Unlocking..." << llendl;
		unlockData();

		if(mError)
			break;

		// Process Macros/Raw Commands
		//LL_INFOS("Lua") << __LINE__ << ": Locking again..." << llendl;
		lockData();

		//LL_INFOS("Lua") << __LINE__ << ": Checking if macro queue is empty..." << llendl;
		while(!mQueuedCommands.empty() && !mError)
		{
		
			//LL_INFOS("Lua") << __LINE__ << ": Macro queued, executing it..." << llendl;

			// Top of stack
			std::string &hr = mQueuedCommands.front(); //byref. faster.

			//LL_INFOS("Lua") << __LINE__ << ": Processing a macro or command." << llendl;
			//LL_INFOS("Lua") << __LINE__ << hr << llendl;

			// Is this shit a macro?
			if(FLLua::isMacro(hr))
				RunMacro(hr); // Run macro.
			else
				RunString(hr); // Run command.

			mQueuedCommands.pop(); //safe to pop now.
		} 
		//else 
		//{			
		//	LL_INFOS("Lua") << __LINE__ << ": Macro vector empty" << llendl;
		//}
		unlockData();
		if(mError)
			break;
		ms_sleep(10);
	}
	LL_INFOS("Lua") << __LINE__ << ": *** THREAD EXITING ***" << llendl;
}

void FLLua::RunFile(std::string  file)
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
		mError=true; //Probably shouldn't do this for macros.. In fact, mError could probably be done away with.
	}
	else if(!listening)
	{
		lua_getglobal(pLuaStack,"CallHook"); //Would rather not issue lua functions every 10ms when we don't need to.
		listening = lua_isfunction(pLuaStack,-1);
		lua_pop(pLuaStack,1);  
	}
}
void FLLua::RunMacro(const std::string what)
{
	std::string  tokenized = std::string (what.c_str());

	BOOL found_macro = FALSE;
	//BOOL first_token = TRUE;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(" ");
	tokenizer tokens(tokenized, sep);
	tokenizer::iterator token_iter;

	std::string  macroname="";
	std::stringstream args;
	for( token_iter = tokens.begin(); token_iter != tokens.end(); ++token_iter)
	{
		std::string  cur_token = token_iter->c_str();
		//llinfos << cur_token << llendl;
		
		if(cur_token=="/m" || cur_token=="/macro")
		{
			continue;
		}

		if(!found_macro)
		{
			macroname=std::string (cur_token);
			found_macro=true;
		}
		else
		{
			args << "\"" << cur_token << "\",";
		}
	}
	
	if(macroname=="")
	{
		LuaError("Macro Syntax: /m[acro] <MacroName>"
			//" [args ...]"
			);
		return;
	}
	std::string  macrofile="";
	
	macrofile =  gDirUtilp->getExpandedFilename(FL_PATH_MACROS,macroname+".lua");

	if(!gDirUtilp->fileExists(macrofile))
	{
		std::string  err("Unable to find " + macrofile + ". Aborting.");
		LuaError(err.c_str());
		return;
	}

	std::string  message("Executing Lua Macro "+macrofile);
	std::string  arglist("MACRO_ARGS={");
	// HACK.  Set global via luaL_dostring
	arglist.append(args.str()).append("};");
	luaL_dostring(pLuaStack,arglist.c_str());
	if(luaL_dofile(pLuaStack,macrofile.c_str()))
		LuaError(Lua_getErrorMessage(pLuaStack).c_str());
}

//static
void FLLua::RunString(std::string s)
{
	if(luaL_dostring(sInstance->pLuaStack,s.c_str()))
	{
		LL_INFOS("Lua") << "Run string(" << s << ") failed with" << Lua_getErrorMessage(sInstance->pLuaStack) << llendl;
		LuaError(Lua_getErrorMessage(sInstance->pLuaStack).c_str());
	}
}

void FLLua::ExecuteHook(HookRequest *hook)
{
#ifdef LUA_HOOK_SPAM
	LL_INFOS("Lua") << "Firing event: " << hook->getName() << llendl;
#endif
	if(listening)
	{
		lua_getglobal(pLuaStack,"CallHook");
		lua_pushstring(pLuaStack,hook->getName());
		int lim=hook->getNumArgs();
		if(lim)
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
		if(lua_pcall(pLuaStack,hook->getNumArgs()+1,1,0)!=0)
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
 }
 
// Static
bool FLLua::isMacro(const std::string &str)
{
	return(str.substr(0,6)=="/macro" || str.substr(0,2)=="/m");
}

//static
void FLLua::callLuaHook(HookRequest *hook)
{
	if(!hook)
		return;
	else if(!sInstance || (sInstance->mError && !FLLua::init()))
		delete hook;
	else
	{
#ifdef LUA_HOOK_SPAM
		LL_INFOS("Lua") << "Adding event: " << hook.getName() << llendl;
#endif
		sInstance->lockData(); //Fixed a very obscure crash.
		sInstance->mQueuedHooks.push(hook);
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

/*void FLHooks_InitTable(lua_State *l, FLLua* lol)
{
	FLLua** ud = reinterpret_cast<FLLua**>(lua_newuserdata(l, sizeof(FLLua*)));
	
	*ud = lol;

	Lua_SetClass(l, "Hooks");
}*/
/*
void Lua_RegisterMethod(lua_State* l, const char* name, lua_CFunction fn)
{

	lua_pushstring(l, name);
	lua_pushvalue(l, -2);
	lua_pushcclosure(l, fn, 1);
	lua_settable(l, -3);

}

void Lua_CreateClassMetatable(lua_State* l, const char* name)
{

	lua_newtable(l);
	Lua_PushClass(l, name);
	lua_pushvalue(l, -2);
	lua_rawset(l, LUA_REGISTRYINDEX); // registry.name = metatable
	lua_pushvalue(l, -1);
	Lua_PushClass(l, name);
	lua_rawset(l, LUA_REGISTRYINDEX); // registry.metatable = name
	lua_pushliteral(l, "__index");
	lua_pushvalue(l, -2);
	lua_rawset(l, -3);
}

void Lua_CheckArgs(lua_State* l,int minArgs, int maxArgs, const char* errorMessage)
{
	int argc = lua_gettop(l);
	if (argc < minArgs || argc > maxArgs)
	{
		llerrs << errorMessage << llendl;
	}
}

 bool Lua_IsType(lua_State* l, int index, int id)
{
	// get registry[metatable]
	if (!lua_getmetatable(l, index))
		return false;
	
	lua_rawget(l, LUA_REGISTRYINDEX);

	if (lua_type(l, -1) != LUA_TSTRING)
	{
		llerrs << "Lua_IsType failed!  Unregistered class." << llendl; 
		lua_pop(l, 1);
		return false;
	}
	
	const char* classname = lua_tostring(l, -1);
	
	if (classname != NULL)
	{
		lua_pop(l, 1);
		return true;
	}
	
	lua_pop(l, 1);
	
	return false;
}

void Lua_PushClass(lua_State* l, const char* classname)
{
	lua_pushlstring(l, classname, strlen(classname));
}

void Lua_SetClass(lua_State *l,const char* classname)
{
	Lua_PushClass(l,classname);
	lua_rawget(l, LUA_REGISTRYINDEX);
	if (lua_type(l, -1) != LUA_TTABLE)
		llwarns << "Metatable for " << classname << " not found!" << llendl;
	if (lua_setmetatable(l, -2) == 0)
		llwarns << "Error setting metatable for " << classname << llendl;
}
*/



/*
void LuaSetAvOverlay(const char *uuid,int type)
{
	/// NOT IMPLEMENTED YET ///
}

LuaAvatarOverlay::LuaAvatarOverlay(LLViewerImage *ViewerImage):
mImage(ViewerImage)
{
}

LuaAvatarOverlay::~LuaAvatarOverlay()
{
}

void LuaAvatarOverlay::render()
{
	if(mImage==NULL)
		return;
	
	LLVector3 WORLD_UPWARD_DIRECTION = LLVector3( 0.0f, 0.0f, 1.0f );

	LLVector3 pos = mParentPos + WORLD_UPWARD_DIRECTION * 0.3f;
	LLGLSPipelineAlpha alpha_blend;
	LLGLDepthTest depth(GL_TRUE, GL_FALSE);
	LLVector3 l	= LLViewerCamera::getInstance()->getLeftAxis() * 0.192f;
	LLVector3 u	= LLViewerCamera::getInstance()->getUpAxis()   * 0.064f;

	LLVector3 bottomLeft	= pos + l - u;
	LLVector3 bottomRight	= pos - l - u;
	LLVector3 topLeft		= pos + l + u;
	LLVector3 topRight		= pos - l + u;

	mImage->bind(); 
		
	gGL.color4fv( LLColor4( 1.0f, 1.0f, 1.0f, 1.0f ).mV );	
	
	gGL.begin( LLVertexBuffer::TRIANGLE_STRIP );
		gGL.texCoord2i( 0,	0	); gGL.vertex3fv( bottomLeft.mV );
		gGL.texCoord2i( 1,	0	); gGL.vertex3fv( bottomRight.mV );
		gGL.texCoord2i( 0,	1	); gGL.vertex3fv( topLeft.mV );
	gGL.end();

	gGL.begin( LLVertexBuffer::TRIANGLE_STRIP );
		gGL.texCoord2i( 1,	0	); gGL.vertex3fv( bottomRight.mV );
		gGL.texCoord2i( 1,	1	); gGL.vertex3fv( topRight.mV );
		gGL.texCoord2i( 0,	1	); gGL.vertex3fv( topLeft.mV );
	gGL.end();
}

void LuaAvatarOverlay::setParentPos( const LLVector3 p )
{
	this->mParentPos=p;
}
*/
