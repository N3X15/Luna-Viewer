%include "../LuaAgent_f.h"

// Don't bind internal events to Lua.
%ignore setParams_Event(LLUUID,std::string,double);
%ignore LuaWear_Event(LLUUID);
%ignore LuaRemoveAllWearables_Event();
%ignore LuaUpdateAppearance_Event();

%rename (StartAnimation) 		LuaStartAnimation;
%rename (StopAnimation) 		LuaStopAnimation;

%native(getPlayingAnimations) int getPlayingAnimations(lua_State*L);
%{
// Returns a table of LLUUIDs.
/*
local anims = getPlayingAnimations(getMyID())
...
*/
int getPlayingAnimations(lua_State*L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if(n!=1)
	{
		return luaL_error(L,"getPlayAnimation(target_id): Parameter 1 is missing.");
	}
	
	const char *uuid_s;
	lua_getfield(L, LUA_ENVIRONINDEX, "tostring"); /* get tostring() */
	lua_pushvalue(L, -1);  /* function to be called */
	lua_pushvalue(L, 1);   /* value to print */
	lua_call(L, 1, 1);
	size_t l;
	uuid_s = lua_tolstring(L, -1, &l);  /* get result */

	if(!LLUUID::validate(std::string(uuid_s)))
		return luaL_error(L,"getPlayAnimation(target_id): Parameter 1 is not a UUID.");

	LLViewerObject *o=gObjectList.findObject(LLUUID(uuid_s));

	if(!o || !o->isAvatar())
		return luaL_error(L,"getPlayAnimation(target_id): Parameter 1 does not target an avatar.");

	LLVOAvatar *av=(LLVOAvatar *)o;
	LLVOAvatar::AnimIterator it = av->mPlayingAnimations.begin();
	int i=0;
	
	lua_newtable(L);
    int top = lua_gettop(L);
    int index = 1;
	for(it;it!=av->mPlayingAnimations.end();it++)
	{
		//key
		lua_pushinteger(L,index);

		//value
		lua_pushlstring(L,(&it->first)->asString().data(),(&it->first)->asString().size());

		// set the table entry
		lua_settable(L, -3);

		++index;
	}
    // push the new table
    lua_pushvalue(L,-1);
    return 1;
}
%}