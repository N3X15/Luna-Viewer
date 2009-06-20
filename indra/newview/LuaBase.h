/*
** Lua binding: LuaBase
** Generated automatically by tolua++-1.0.92 on 10/23/08 22:05:11.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_LuaBase_open (lua_State* tolua_S);

#include "LuaBase_f.h"
#include "LuaAvatar_f.h"
#include "llworld.h"
#include "SHLCopyComm.h"
#include "LuaAgent_f.h"
#include "llworldmap.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"LuaAgent");
 tolua_usertype(tolua_S,"LLSimInfo");
 tolua_usertype(tolua_S,"SHLCopyComm");
 tolua_usertype(tolua_S,"LLViewerRegion");
 tolua_usertype(tolua_S,"LuaWorldMap");
}

/* method: LuaGetFullName of class  LuaAgent */
#ifndef TOLUA_DISABLE_tolua_LuaBase_agent_getFullName00
static int tolua_LuaBase_agent_getFullName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaAgent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaAgent* self = (LuaAgent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LuaGetFullName'",NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->LuaGetFullName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getFullName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LuaGetID of class  LuaAgent */
#ifndef TOLUA_DISABLE_tolua_LuaBase_agent_getID00
static int tolua_LuaBase_agent_getID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaAgent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaAgent* self = (LuaAgent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LuaGetID'",NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->LuaGetID();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LuaSetTyping of class  LuaAgent */
#ifndef TOLUA_DISABLE_tolua_LuaBase_agent_setTyping00
static int tolua_LuaBase_agent_setTyping00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaAgent",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaAgent* self = (LuaAgent*)  tolua_tousertype(tolua_S,1,0);
  bool On = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LuaSetTyping'",NULL);
#endif
  {
   self->LuaSetTyping(On);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setTyping'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LuaGetTyping of class  LuaAgent */
#ifndef TOLUA_DISABLE_tolua_LuaBase_agent_isTyping00
static int tolua_LuaBase_agent_isTyping00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaAgent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaAgent* self = (LuaAgent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LuaGetTyping'",NULL);
#endif
  {
   bool tolua_ret = (bool)  self->LuaGetTyping();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isTyping'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LuaFollow of class  LuaAgent */
#ifndef TOLUA_DISABLE_tolua_LuaBase_agent_follow00
static int tolua_LuaBase_agent_follow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaAgent",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaAgent* self = (LuaAgent*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LuaFollow'",NULL);
#endif
  {
   self->LuaFollow(name);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'follow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LuaGetSimFromName of class  LuaWorldMap */
#ifndef TOLUA_DISABLE_tolua_LuaBase_worldmap_getSimFromName00
static int tolua_LuaBase_worldmap_getSimFromName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaWorldMap",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaWorldMap* self = (LuaWorldMap*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LuaGetSimFromName'",NULL);
#endif
  {
    LLSimInfo* tolua_ret = (  LLSimInfo*)  self->LuaGetSimFromName(name);
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"LLSimInfo");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getSimFromName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaFindAvatarName */
#ifndef TOLUA_DISABLE_tolua_LuaBase_key2name00
static int tolua_LuaBase_key2name00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* uuid = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   std::string tolua_ret = (std::string)  LuaFindAvatarName(uuid);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'key2name'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaFindAvatarKey */
#ifndef TOLUA_DISABLE_tolua_LuaBase_name2key00
static int tolua_LuaBase_name2key00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* fullname = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   std::string tolua_ret = (std::string)  LuaFindAvatarKey(fullname);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'name2key'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaSendChat */
#ifndef TOLUA_DISABLE_tolua_LuaBase_SendChat00
static int tolua_LuaBase_SendChat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  bool IsDebug = ((bool)  tolua_toboolean(tolua_S,2,false));
  {
   LuaSendChat(msg,IsDebug);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SendChat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaPrint */
#ifndef TOLUA_DISABLE_tolua_LuaBase_print00
static int tolua_LuaBase_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   LuaPrint(msg);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaAlert */
#ifndef TOLUA_DISABLE_tolua_LuaBase_Alert00
static int tolua_LuaBase_Alert00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   LuaAlert(msg);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Alert'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaRegionFromName */
#ifndef TOLUA_DISABLE_tolua_LuaBase_RegionFromName00
static int tolua_LuaBase_RegionFromName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   LLViewerRegion* tolua_ret = (LLViewerRegion*)  LuaRegionFromName(msg);
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"LLViewerRegion");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RegionFromName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaGetCurrentRegion */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getCurrentRegion00
static int tolua_LuaBase_getCurrentRegion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   LLViewerRegion* tolua_ret = (LLViewerRegion*)  LuaGetCurrentRegion();
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"LLViewerRegion");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getCurrentRegion'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaGetCWD */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getCWD00
static int tolua_LuaBase_getCWD00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   const char* tolua_ret = (const char*)  LuaGetCWD();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getCWD'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaError */
#ifndef TOLUA_DISABLE_tolua_LuaBase_error00
static int tolua_LuaBase_error00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* rawmsg = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   LuaError(rawmsg);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'error'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaSay */
#ifndef TOLUA_DISABLE_tolua_LuaBase_say00
static int tolua_LuaBase_say00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* channel = ((const char*)  tolua_tostring(tolua_S,2,"0"));
  {
   LuaSay(msg,channel);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'say'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaShout */
#ifndef TOLUA_DISABLE_tolua_LuaBase_shout00
static int tolua_LuaBase_shout00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* channel = ((const char*)  tolua_tostring(tolua_S,2,"0"));
  {
   LuaShout(msg,channel);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'shout'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaWhisper */
#ifndef TOLUA_DISABLE_tolua_LuaBase_whisper00
static int tolua_LuaBase_whisper00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* msg = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* channel = ((const char*)  tolua_tostring(tolua_S,2,"0"));
  {
   LuaWhisper(msg,channel);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'whisper'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Lua_tp */
#ifndef TOLUA_DISABLE_tolua_LuaBase_tp00
static int tolua_LuaBase_tp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* SimName = ((const char*)  tolua_tostring(tolua_S,1,0));
  int x = ((int)  tolua_tonumber(tolua_S,2,128));
  int y = ((int)  tolua_tonumber(tolua_S,3,128));
  int z = ((int)  tolua_tonumber(tolua_S,4,128));
  {
   Lua_tp(SimName,x,y,z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'tp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Lua_exists */
#ifndef TOLUA_DISABLE_tolua_LuaBase_exists00
static int tolua_LuaBase_exists00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* Filename = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   bool tolua_ret = (bool)  Lua_exists(Filename);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'exists'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getLuaFolder */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getLuaFolder00
static int tolua_LuaBase_getLuaFolder00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::string tolua_ret = (std::string)  getLuaFolder();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getLuaFolder'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaDumpVisualParams */
#ifndef TOLUA_DISABLE_tolua_LuaBase_LuaDumpVisualParams00
static int tolua_LuaBase_LuaDumpVisualParams00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   LuaDumpVisualParams();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LuaDumpVisualParams'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getParamDefaultWeight */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getParamDefaultWeight00
static int tolua_LuaBase_getParamDefaultWeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* avid = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* paramname = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   double tolua_ret = (double)  getParamDefaultWeight(avid,paramname);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getParamDefaultWeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getParamCurrentWeight */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getParamCurrentWeight00
static int tolua_LuaBase_getParamCurrentWeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* avid = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* paramname = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   double tolua_ret = (double)  getParamCurrentWeight(avid,paramname);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getParamCurrentWeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getParamMax */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getParamMax00
static int tolua_LuaBase_getParamMax00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* avid = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* paramname = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   double tolua_ret = (double)  getParamMax(avid,paramname);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getParamMax'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: getParamMin */
#ifndef TOLUA_DISABLE_tolua_LuaBase_getParamMin00
static int tolua_LuaBase_getParamMin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* avid = ((const char*)  tolua_tostring(tolua_S,1,0));
  const char* paramname = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   double tolua_ret = (double)  getParamMin(avid,paramname);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getParamMin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: setParamOnSelf */
#ifndef TOLUA_DISABLE_tolua_LuaBase_setParamOnSelf00
static int tolua_LuaBase_setParamOnSelf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* paramname = ((const char*)  tolua_tostring(tolua_S,1,0));
  double weight = ((double)  tolua_tonumber(tolua_S,2,0));
  {
   setParamOnSelf(paramname,weight);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setParamOnSelf'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaDumpVisualParamsToLuaCode */
#ifndef TOLUA_DISABLE_tolua_LuaBase_LuaDumpVisualParamsToLuaCode00
static int tolua_LuaBase_LuaDumpVisualParamsToLuaCode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::string tolua_ret = (std::string)  LuaDumpVisualParamsToLuaCode();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LuaDumpVisualParamsToLuaCode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaWear */
#ifndef TOLUA_DISABLE_tolua_LuaBase_wear00
static int tolua_LuaBase_wear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* assetid = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   LuaWear(assetid);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'wear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaRemoveAllWearables */
#ifndef TOLUA_DISABLE_tolua_LuaBase_RemoveAllWearables00
static int tolua_LuaBase_RemoveAllWearables00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   LuaRemoveAllWearables();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemoveAllWearables'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LuaSetTEImage */
#ifndef TOLUA_DISABLE_tolua_LuaBase_setTEImage00
static int tolua_LuaBase_setTEImage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int index = ((int)  tolua_tonumber(tolua_S,1,0));
  const char* UUID = ((const char*)  tolua_tostring(tolua_S,2,0));
  {
   LuaSetTEImage(index,UUID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setTEImage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: gCopybot */
#ifndef TOLUA_DISABLE_tolua_get_gCopybot_ptr
static int tolua_get_gCopybot_ptr(lua_State* tolua_S)
{
  tolua_pushusertype(tolua_S,(void*)gCopybot,"SHLCopyComm");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: gCopybot */
#ifndef TOLUA_DISABLE_tolua_set_gCopybot_ptr
static int tolua_set_gCopybot_ptr(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!tolua_isusertype(tolua_S,2,"SHLCopyComm",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  gCopybot = ((SHLCopyComm*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: gAllowWorldMap */
#ifndef TOLUA_DISABLE_tolua_get_gAllowWorldMap
static int tolua_get_gAllowWorldMap(lua_State* tolua_S)
{
  tolua_pushboolean(tolua_S,(bool)gAllowWorldMap);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: gAllowWorldMap */
#ifndef TOLUA_DISABLE_tolua_set_gAllowWorldMap
static int tolua_set_gAllowWorldMap(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  gAllowWorldMap = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_LuaBase_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,1);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"agent","LuaAgent","",NULL);
  tolua_beginmodule(tolua_S,"agent");
   tolua_function(tolua_S,"getFullName",tolua_LuaBase_agent_getFullName00);
   tolua_function(tolua_S,"getID",tolua_LuaBase_agent_getID00);
   tolua_function(tolua_S,"setTyping",tolua_LuaBase_agent_setTyping00);
   tolua_function(tolua_S,"isTyping",tolua_LuaBase_agent_isTyping00);
   tolua_function(tolua_S,"follow",tolua_LuaBase_agent_follow00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"worldmap","LuaWorldMap","",NULL);
  tolua_beginmodule(tolua_S,"worldmap");
   tolua_function(tolua_S,"getSimFromName",tolua_LuaBase_worldmap_getSimFromName00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"key2name",tolua_LuaBase_key2name00);
  tolua_function(tolua_S,"name2key",tolua_LuaBase_name2key00);
  tolua_function(tolua_S,"SendChat",tolua_LuaBase_SendChat00);
  tolua_function(tolua_S,"print",tolua_LuaBase_print00);
  tolua_function(tolua_S,"Alert",tolua_LuaBase_Alert00);
  tolua_function(tolua_S,"RegionFromName",tolua_LuaBase_RegionFromName00);
  tolua_function(tolua_S,"getCurrentRegion",tolua_LuaBase_getCurrentRegion00);
  tolua_function(tolua_S,"getCWD",tolua_LuaBase_getCWD00);
  tolua_function(tolua_S,"error",tolua_LuaBase_error00);
  tolua_function(tolua_S,"say",tolua_LuaBase_say00);
  tolua_function(tolua_S,"shout",tolua_LuaBase_shout00);
  tolua_function(tolua_S,"whisper",tolua_LuaBase_whisper00);
  tolua_function(tolua_S,"tp",tolua_LuaBase_tp00);
  tolua_function(tolua_S,"exists",tolua_LuaBase_exists00);
  tolua_function(tolua_S,"getLuaFolder",tolua_LuaBase_getLuaFolder00);
  tolua_function(tolua_S,"LuaDumpVisualParams",tolua_LuaBase_LuaDumpVisualParams00);
  tolua_function(tolua_S,"getParamDefaultWeight",tolua_LuaBase_getParamDefaultWeight00);
  tolua_function(tolua_S,"getParamCurrentWeight",tolua_LuaBase_getParamCurrentWeight00);
  tolua_function(tolua_S,"getParamMax",tolua_LuaBase_getParamMax00);
  tolua_function(tolua_S,"getParamMin",tolua_LuaBase_getParamMin00);
  tolua_function(tolua_S,"setParamOnSelf",tolua_LuaBase_setParamOnSelf00);
  tolua_function(tolua_S,"LuaDumpVisualParamsToLuaCode",tolua_LuaBase_LuaDumpVisualParamsToLuaCode00);
  tolua_function(tolua_S,"wear",tolua_LuaBase_wear00);
  tolua_function(tolua_S,"RemoveAllWearables",tolua_LuaBase_RemoveAllWearables00);
  tolua_function(tolua_S,"setTEImage",tolua_LuaBase_setTEImage00);
  tolua_variable(tolua_S,"gCopybot",tolua_get_gCopybot_ptr,tolua_set_gCopybot_ptr);
  tolua_variable(tolua_S,"gAllowWorldMap",tolua_get_gAllowWorldMap,tolua_set_gAllowWorldMap);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_LuaBase (lua_State* tolua_S) {
 return tolua_LuaBase_open(tolua_S);
};
#endif

