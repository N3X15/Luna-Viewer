/**
 * @file LuaBase.swig
 * @brief FlexLife Lua Integration Framework
 * @author N3X15
 *
 *  Copyright (C) 2008-2009 N3X15
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

%module SL
%{
#include "llviewerprecompiledheaders.h"
#include "stdtypes.h"
#include "SLua.h"
#include "LuaBase_f.h"
#include "LuaAvatar_f.h"
#include "LuaAgent_f.h"
#include "llprimitive.h"
#include "llviewerobject.h"
#include "llvovolume.h"
#include "llagent.h"
#include "llselectmgr.h"
%}

%import "../llcommon/stdtypes.h"
%import "../llprimitive/llprimitive.h"
%import "../newview/llagent.h"
%import "../newview/llviewerregion.h"
%import "../newview/llviewerobject.h"
%import "../newview/llvovolume.h"
%import "../newview/llselectmgr.h"

%rename(Floater) SLuaFloater;
%rename(Button) SLuaButton;
%include "SLua.h"

%rename(SendChat)		LuaSendChat(const char *msg, bool IsDebug=false);
%rename(print)			LuaPrint(const char *msg);
%rename(RegionFromName)		LuaRegionFromName(const char *msg);
%rename(GetCurrentRegion)	LuaGetCurrentRegion();
%rename(GetCWD)			LuaGetCWD();
%rename(error)			LuaError(const char* msg);

%rename(say)			LuaSay(const char* msg, const char* channel="0");
%rename(shout)			LuaShout(const char* msg, const char* channel="0");
%rename(whisper)		LuaWhisper(const char* msg, const char* channel="0");

%rename(tp)			Lua_tp(const char* SimName, int x=128, int y=128, int z=128);
%rename(exists)			Lua_exists(const char* Filename);
%rename(touch)			Lua_touch(const char* uuid);

%rename(wear)			LuaWear(const char* uuid);
%rename(RemoveAllWearables)	LuaRemoveAllWearables();
%rename(setTEImage)		LuaSetTEImage(int index,const char *UUID);

%include "LuaBase_f.h"

%rename(key2name)		LuaFindAvatarName(const char* uuid);
%rename(name2key)		LuaFindAvatarKey(const char* fullname);

%include "LuaAvatar_f.h"

extern bool gAllowWorldMap;
extern LLAgent *gAgent;

