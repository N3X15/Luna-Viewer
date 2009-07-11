#ifndef LUABASE_F_H
#define LUABASE_F_H

#include "llworld.h"

/**
 * @file LuaBase_f.h
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
 *
 * $Id$
 */

void LuaSendChat(const char *msg,bool IsDebug=false);

void LuaPrint(const char *msg);

// Pop up an alert window.
void LuaAlert(const char* msg);
 
const char* LuaGetCWD();

// Retrieve the region handle of this sim
LLViewerRegion* LuaRegionFromName(const char *msg);

LLViewerRegion* LuaGetCurrentRegion();

void LuaError(const char* rawmsg);

void LuaTouch(const char* uuid);

/*void LuaSleep(int milliseconds);*/

// Chat functions
void LuaSay(const char* rawmsg,const char* channel="0");
void LuaShout(const char* rawmsg,const char* channel="0");
void LuaWhisper(const char* rawmsg,const char* channel="0");

// Not exposed to Lua (yet)
void LuaSendRawChat(const char* rawmsg, int rawtype, bool animate, S32 channel);

void Lua_tp(const char* SimName, int x=128, int y=128, int z=128);

bool Lua_exists(const char* Filename);

std::string  getLuaFolder();

std::string  toLuaString(F32 t);

extern bool gAllowWorldMap;

#endif
