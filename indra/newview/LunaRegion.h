/**
The idea is this

local region = Region.FindByName("Boom")
local region = Region.FindByID(UUID)
local region = Region.FindByIP(IPAddress)

region:setWaterHeight(100)
*/

#include "llviewerregion.h"
//#include "llprimitive.h"
//#include "../../llviewerobject.h"
//#include "../../llvovolume.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"
#include "m4math.h"

LLViewerRegion* FindByName(std::string name);
LLViewerRegion* FindByUUID(     LLUUID id);
//LLViewerRegion* FindByIP(  std::string ip);
