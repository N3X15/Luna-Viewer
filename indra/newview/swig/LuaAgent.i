%include "../LuaAgent_f.h"

// Don't bind internal events to Lua.
%ignore setParams_Event(LLUUID,std::string,double);
%ignore LuaWear_Event(LLUUID);
%ignore LuaRemoveAllWearables_Event();
%ignore LuaUpdateAppearance_Event();

%rename (StartAnimation) 		LuaStartAnimation;
%rename (StopAnimation) 		LuaStopAnimation;
%rename (GetPlayingAnimations) 	LuaGetPlayingAnimations;