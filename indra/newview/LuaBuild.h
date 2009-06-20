/*
** Lua binding: LuaBuild
** Generated automatically by tolua++-1.0.92 on 06/20/08 18:05:24.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_LuaBuild_open (lua_State* tolua_S);

#include "LuaBuild_f.h"
#include "llvolumeparams.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_LuaPrimBuilder (lua_State* tolua_S)
{
 LuaPrimBuilder* self = (LuaPrimBuilder*) tolua_tousertype(tolua_S,1,0);
	delete self;
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"LLVolumeParams");
 tolua_usertype(tolua_S,"LuaPrimBuilder");
}

/* method: new of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_new00
static int tolua_LuaBuild_LuaPrimBuilder_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   LuaPrimBuilder* tolua_ret = (LuaPrimBuilder*)  new LuaPrimBuilder();
   tolua_pushusertype(tolua_S,(void*)tolua_ret,"LuaPrimBuilder");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_new00_local
static int tolua_LuaBuild_LuaPrimBuilder_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   LuaPrimBuilder* tolua_ret = (LuaPrimBuilder*)  new LuaPrimBuilder();
   tolua_pushusertype_and_takeownership(tolua_S,(void *)tolua_ret,"LuaPrimBuilder");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_delete00
static int tolua_LuaBuild_LuaPrimBuilder_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'",NULL);
#endif
  delete self;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetSize of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_SetSize00
static int tolua_LuaBuild_LuaPrimBuilder_SetSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
  const double x = ((const double)  tolua_tonumber(tolua_S,2,0));
  const double y = ((const double)  tolua_tonumber(tolua_S,3,0));
  const double z = ((const double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetSize'",NULL);
#endif
  {
   self->SetSize(x,y,z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetSize'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetLocation of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_SetLocation00
static int tolua_LuaBuild_LuaPrimBuilder_SetLocation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
  const double x = ((const double)  tolua_tonumber(tolua_S,2,0));
  const double y = ((const double)  tolua_tonumber(tolua_S,3,0));
  const double z = ((const double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetLocation'",NULL);
#endif
  {
   self->SetLocation(x,y,z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetLocation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTexture of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_SetTexture00
static int tolua_LuaBuild_LuaPrimBuilder_SetTexture00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
  const char* uuid = ((const char*)  tolua_tostring(tolua_S,2,0));
  const int FaceID = ((const int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTexture'",NULL);
#endif
  {
   self->SetTexture(uuid,FaceID);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTexture'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetShape of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_SetShape00
static int tolua_LuaBuild_LuaPrimBuilder_SetShape00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
  int pc = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetShape'",NULL);
#endif
  {
   self->SetShape(pc);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetShape'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Commit of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_LuaBuild_LuaPrimBuilder_Commit00
static int tolua_LuaBuild_LuaPrimBuilder_Commit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaPrimBuilder",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Commit'",NULL);
#endif
  {
   bool tolua_ret = (bool)  self->Commit();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Commit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: VolumeParams of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_get_LuaPrimBuilder_VolumeParams
static int tolua_get_LuaPrimBuilder_VolumeParams(lua_State* tolua_S)
{
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'VolumeParams'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->VolumeParams,"LLVolumeParams");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: VolumeParams of class  LuaPrimBuilder */
#ifndef TOLUA_DISABLE_tolua_set_LuaPrimBuilder_VolumeParams
static int tolua_set_LuaPrimBuilder_VolumeParams(lua_State* tolua_S)
{
  LuaPrimBuilder* self = (LuaPrimBuilder*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'VolumeParams'",NULL);
  if (!tolua_isusertype(tolua_S,2,"LLVolumeParams",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->VolumeParams = *((LLVolumeParams*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_LuaBuild_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"PCODE_CUBE",PCODE_CUBE);
  tolua_constant(tolua_S,"PCODE_PRISM",PCODE_PRISM);
  tolua_constant(tolua_S,"PCODE_TETRAHEDRON",PCODE_TETRAHEDRON);
  tolua_constant(tolua_S,"PCODE_PYRAMID",PCODE_PYRAMID);
  tolua_constant(tolua_S,"PCODE_CYLINDER",PCODE_CYLINDER);
  tolua_constant(tolua_S,"PCODE_CONE",PCODE_CONE);
  tolua_constant(tolua_S,"PCODE_SPHERE",PCODE_SPHERE);
  tolua_constant(tolua_S,"PCODE_TORUS",PCODE_TORUS);
  tolua_constant(tolua_S,"PCODE_VOLUME",PCODE_VOLUME);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"LuaPrimBuilder","LuaPrimBuilder","",tolua_collect_LuaPrimBuilder);
  #else
  tolua_cclass(tolua_S,"LuaPrimBuilder","LuaPrimBuilder","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"LuaPrimBuilder");
   tolua_function(tolua_S,"new",tolua_LuaBuild_LuaPrimBuilder_new00);
   tolua_function(tolua_S,"new_local",tolua_LuaBuild_LuaPrimBuilder_new00_local);
   tolua_function(tolua_S,".call",tolua_LuaBuild_LuaPrimBuilder_new00_local);
   tolua_function(tolua_S,"delete",tolua_LuaBuild_LuaPrimBuilder_delete00);
   tolua_function(tolua_S,"SetSize",tolua_LuaBuild_LuaPrimBuilder_SetSize00);
   tolua_function(tolua_S,"SetLocation",tolua_LuaBuild_LuaPrimBuilder_SetLocation00);
   tolua_function(tolua_S,"SetTexture",tolua_LuaBuild_LuaPrimBuilder_SetTexture00);
   tolua_function(tolua_S,"SetShape",tolua_LuaBuild_LuaPrimBuilder_SetShape00);
   tolua_function(tolua_S,"Commit",tolua_LuaBuild_LuaPrimBuilder_Commit00);
   tolua_variable(tolua_S,"VolumeParams",tolua_get_LuaPrimBuilder_VolumeParams,tolua_set_LuaPrimBuilder_VolumeParams);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_LuaBuild (lua_State* tolua_S) {
 return tolua_LuaBuild_open(tolua_S);
};
#endif

