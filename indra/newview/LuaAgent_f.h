#pragma once

#include "llworldmap.h"
#include "llvoavatar.h"
#include "llparcel.h"

std::string LuaFindAvatarName(const LLUUID& id);
std::string LuaFindAvatarKey(const char* fullname);

#ifdef SWIG
%rename (getCurrentParcel) Lua_getCurrentParcel();
#endif

LLParcel* Lua_getCurrentParcel();

LLUUID getParcelOwner(LLParcel* parcel);

// Internal
LLVOAvatar* LuaGetAvatar(const LLUUID& id);
