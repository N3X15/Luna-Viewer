#ifndef LUA_SETTINGS_H
#define LUA_SETTINGS_H

#include "llcontrol.h"
#include "llviewercontrol.h"

std::string getSavedSetting(std::string name);

void setSavedSetting(std::string name,std::string value);

#endif