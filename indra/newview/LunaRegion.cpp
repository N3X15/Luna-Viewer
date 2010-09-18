#include "llviewerprecompiledheaders.h"

#include "LunaRegion.h"

#include "llhttpclient.h"
#include "llviewerregion.h"
#include "llworld.h"

LLViewerRegion* FindByName(std::string name)
{
	return LLWorld::getInstance()->findByName(name);
}

LLViewerRegion* FindByUUID(LLUUID ID)
{
	return LLWorld::getInstance()->findByUUID(ID);
}
