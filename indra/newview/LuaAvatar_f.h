/**
 * @file LuaAvatar_f.h
 * @brief Luna Viewer Lua Framework:  Appearance bindings
 * @author N3X15
 *
 *  Copyright (C) 2008-2009 Luna Contributors
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
#pragma once
#include "llwearable.h"

void LuaUpdateAppearance();
std::map<std::string,double> LuaDumpVisualParams();
double getParamDefaultWeight(const LLUUID &avid,const char*paramname);
double getParamCurrentWeight(const LLUUID &avid,const char*paramname);
double getParamMax(const LLUUID &avid,const char*paramname);
double getParamMin(const LLUUID &avid,const char*paramname);

void setParamOnSelf(std::string paramname,double weight);
void setParamOnTarget(LLUUID target,std::string paramname,double weight);

void LuaWear(const LLUUID& assetid);
void LuaRemoveAllWearables();

void startAnimation(LLUUID avid, LLUUID movement);
void stopAnimation(LLUUID avid, LLUUID movement);

// Utility functions
bool LuaSaveWearable(LLWearable *w);
LLWearable * LuaLoadWearable(const LLUUID& uuid);
void LuaSetTEImage(int index,const LLUUID& id);
