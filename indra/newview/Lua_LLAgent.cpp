/**
 * @file Lua_LLAgent.cpp
 * @brief FlexLife Viewer Lua Integration Framework
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

extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

#include "llagent.h"
#include "lluuid.h"
#include "llworldmap.h" 

#include "Lua_LLAgent.h"

#include "SHLLua.h"

extern LLAgent gAgent;
extern LLWorldMap *gWorldMap;

static int luaAgentName(lua_State* L)
{

		LLString aname;
		gAgent.getName(aname);
		lua_pushstring(L,aname.c_str());
		return 1;
}

static int luaAgentID(lua_State* L)
{
	
		LLUUID uuid(gAgent.getID());
		lua_pushstring(L, uuid.asString().c_str());
		return 1;
}

static int luaAgentIsFlying(lua_State* L)
{

		lua_pushboolean(L, gAgent.getFlying());
		return 1;

}

// To set shit
static int luaAgentSetTyping(lua_State* L)
{
	Lua_CheckArgs(L, 2, 2, "One argument required for Agent:SetTyping");
       
    if (!lua_isnumber(L,2) && !lua_isnil(L,2))
    {
		llerrs << "Agent:SetTyping needs a integer argument." << llendl;
    }


		double ret= lua_tonumber(L,2);
		if(ret==1)
			gAgent.startTyping();
		else
			gAgent.stopTyping();
		return 1;

}



// Follow/Autopilot
//	Agent:StartFollow "UUID"
static int luaAgentStartFollow(lua_State* L)
{
	Lua_CheckArgs(L, 2, 2, "One argument required for Agent:SetTyping");
       
    if (!lua_isstring(L,2) && !lua_isnil(L,2))
    {
		llerrs << "Agent:StartFollow needs a string argument and it better fucking be a UUID." << llendl;
    }


		const char *ret= lua_tostring(L,2);
		LLUUID targetuuid(LLUUID::null);
		if(!targetuuid.set(ret))
			gAgent.startFollowPilot(targetuuid);
		else
			llerrs << "Agent:StartFollow needs a valid UUID." << llendl;
		return 1;

}

// Autopilot, baby. 
//	Agent:StartAutoPilot global_x global_y global_z
static int luaAgentStartAutoPilot(lua_State* L)
{
	Lua_CheckArgs(L, 4, 4, "3 *global* location coordinates required for Agent:StartAutoPilot");

    if (!lua_isnumber(L,2) && !lua_isnil(L,2))
    {
		llerrs << "Agent:StartAutopilot needs 3 global location coords." << llendl;
    }
	if (!lua_isnumber(L,3) && !lua_isnil(L,3))
    {
		llerrs << "Agent:StartAutopilot needs 3 global location coords." << llendl;
    }
	if (!lua_isnumber(L,4) && !lua_isnil(L,4))
    {
		llerrs << "Agent:StartAutopilot needs 3 global location coords." << llendl;
    }

		LLVector3d vec(lua_tonumber(L,2),lua_tonumber(L,3),lua_tonumber(L,4));
		if(vec.isFinite())
		{
			gAgent.startAutoPilotGlobal(vec);
			return 1;
		}
return 0;
}

// Teleport
// Local (Agent:Teleport "Shitfuck Sim" 10 0 1)
static int luaAgentTeleport(lua_State* L)
{
	Lua_CheckArgs(L, 5, 5, "Sim name and local coords required.");

    if (!lua_isstring(L,2) && !lua_isnil(L,2))
    {
		llerrs << "Agent:Teleport needs sim name and 3 local location coords." << llendl;
    }
	if (!lua_isnumber(L,3) && !lua_isnil(L,3))
    {
		llerrs << "Agent:Teleport needs sim name and 3 local location coords." << llendl;
    }
	if (!lua_isnumber(L,4) && !lua_isnil(L,4))
    {
		llerrs << "Agent:Teleport needs sim name and 3 local location coords." << llendl;
	}
	if (!lua_isnumber(L,5) && !lua_isnil(L,5))
    {
		llerrs << "Agent:Teleport needs sim name and 3 local location coords." << llendl;
	}
	

		LLString sim_name(lua_tostring(L,2));
		LLVector3 vec(lua_tonumber(L,3),lua_tonumber(L,4),lua_tonumber(L,5));

		LLSimInfo *sim = gWorldMap->simInfoFromName(sim_name);
		
		//*TODO:  Add in access checking prior to teleport.
		gAgent.teleportRequest((const U64)sim->mHandle,(const LLVector3)vec);
	
	return 1;
}

static int luaAgentTeleportGlobal(lua_State* L)
{
	Lua_CheckArgs(L,4,4,"To teleport globally, Agent:TeleportGlobal needs the x, y and z of the position in global coords.");
	
	if (!lua_isnumber(L,2) && !lua_isnil(L,2))
    {
		llerrs << "Agent:StartAutopilot needs 3 global location coords." << llendl;
    }
	if (!lua_isnumber(L,3) && !lua_isnil(L,3))
    {
		llerrs << "Agent:StartAutopilot needs 3 global location coords." << llendl;
    }
	if (!lua_isnumber(L,4) && !lua_isnil(L,4))
    {
		llerrs << "Agent:StartAutopilot needs 3 global location coords." << llendl;
    }
	

		LLVector3d vec(lua_tonumber(L,2),lua_tonumber(L,3),lua_tonumber(L,4));

		gAgent.teleportViaLocation(vec);

	return 1;
}

void LuaAgent_CreateMetatable(lua_State* l)
{
    Lua_CreateClassMetatable(l, "Agent");
    lua_rawget(l, LUA_REGISTRYINDEX);
    
	Lua_RegisterMethod(l, "getFlying",	luaAgentIsFlying );
	//Lua_RegisterMethod(l, "getName",	luaAgentName );
	Lua_RegisterMethod(l, "getID",		luaAgentID );

	Lua_RegisterMethod(l, "StartAutoPilot",	luaAgentStartAutoPilot );
	Lua_RegisterMethod(l, "StartFollow",	luaAgentStartFollow );
	Lua_RegisterMethod(l, "Teleport",		luaAgentTeleport );
	Lua_RegisterMethod(l, "TeleportGlobal",	luaAgentTeleportGlobal );

    lua_pop(l, 1);
}