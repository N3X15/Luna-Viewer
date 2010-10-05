#ifndef LUA_INVENTORY_H
#define LUA_INVENTORY_H
#include "llinventorymodel.h"
#include "llviewerinventory.h"
#include <vector>

void findInventoryInFolder(const std::string& ifolder,std::vector<std::string>& rcats,std::vector<LLUUID>& ritems);
void giveInventoryItem(LLUUID to_agent, LLUUID item_id, LLUUID im_session_id);

LLUUID getCategoryUUID(const std::string& path);

LLUUID requestInventoryAsset(LLUUID item_id,LLUUID task_id=LLUUID::null);
#endif