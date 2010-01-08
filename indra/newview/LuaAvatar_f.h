#pragma once
#include "llwearable.h"

void LuaUpdateAppearance();
void LuaDumpVisualParams();
std::string LuaDumpVisualParamsToLuaCode();
//std::string LuaDumpTargetVisParamsToLuaCode();
double getParamDefaultWeight(const LLUUID &avid,const char*paramname);
double getParamCurrentWeight(const LLUUID &avid,const char*paramname);
double getParamMax(const LLUUID &avid,const char*paramname);
double getParamMin(const LLUUID &avid,const char*paramname);

void setParamOnSelf(const std::string &paramname,double weight);
void setParamOnTarget(const LLUUID &avid,const std::string &paramname,double weight);

void LuaWear(const LLUUID& assetid);
void LuaRemoveAllWearables();

// Utility functions
bool LuaSaveWearable(LLWearable *w);
LLWearable * LuaLoadWearable(const LLUUID& uuid);
void LuaSetTEImage(int index,const LLUUID& id);
bool HasPermissions(LLViewerInventoryItem* item);
