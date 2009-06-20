/*
** Lua binding: LuaAgent
** Generated automatically by tolua++-1.0.92 on 05/03/08 09:02:32.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_LuaAgent_open (lua_State* tolua_S);

#include "llagent.h"
#include "llworldmap.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"LLWorldMap");
 tolua_usertype(tolua_S,"LLAgent");
}

/* get function: gAgent */
#ifndef TOLUA_DISABLE_tolua_get_Agent
static int tolua_get_Agent(lua_State* tolua_S)
{
  tolua_pushusertype(tolua_S,(void*)&gAgent,"LLAgent");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: gAgent */
#ifndef TOLUA_DISABLE_tolua_set_Agent
static int tolua_set_Agent(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S,2,"LLAgent",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  gAgent = *((LLAgent*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: gWorldMap */
#ifndef TOLUA_DISABLE_tolua_get_WorldMap
static int tolua_get_WorldMap(lua_State* tolua_S)
{
  tolua_pushusertype(tolua_S,(void*)&gWorldMap,"LLWorldMap");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: gWorldMap */
#ifndef TOLUA_DISABLE_tolua_set_WorldMap
static int tolua_set_WorldMap(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S,2,"LLWorldMap",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  gWorldMap = *((LLWorldMap*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_LuaAgent_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,1);
 tolua_beginmodule(tolua_S,NULL);
  tolua_variable(tolua_S,"Agent",tolua_get_Agent,tolua_set_Agent);
  tolua_variable(tolua_S,"WorldMap",tolua_get_WorldMap,tolua_set_WorldMap);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_LuaAgent (lua_State* tolua_S) {
 return tolua_LuaAgent_open(tolua_S);
};
#endif

