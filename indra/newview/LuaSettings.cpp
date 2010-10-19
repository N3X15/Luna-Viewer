#include "llviewerprecompiledheaders.h"

#include "llviewercontrol.h"
#include "llcontrol.h"

std::string getSavedSettings(std::string name)
{
	return gSavedSettings.getString(name);
}

void setSavedSetting(std::string name,std::string value)
{
	gSavedSettings.setString(name,value);
}