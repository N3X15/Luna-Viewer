#pragma once
#include "llwearable.h"

void LuaUpdateAppearance();
void LuaDumpVisualParams();
std::string LuaDumpVisualParamsToLuaCode();
//std::string LuaDumpTargetVisParamsToLuaCode();
double getParamDefaultWeight(const char* avid,const char* paramname);
double getParamCurrentWeight(const char* avid,const char* paramname);
double getParamMax(const char* avid,const char* paramname);
double getParamMin(const char* avid,const char* paramname);

void setParamOnSelf(const char* paramname,double weight);
void LuaWear(const LLUUID& assetid);
void LuaRemoveAllWearables();

// Utility functions
bool LuaSaveWearable(LLWearable *w);
LLWearable * LuaLoadWearable(const LLUUID& uuid);
void LuaSetTEImage(int index,const LLUUID& id);
