#ifndef LUA_INVENTORY_H
#define LUA_INVENTORY_H
#include "llinventorymodel.h"
#include "llviewerinventory.h"
#include <vector>

int findInventoryInFolder(lua_State*L);
void giveInventoryItem(LLUUID to_agent, LLUUID item_id, LLUUID im_session_id);

LLUUID getCategoryUUID(const std::string& path);

LLUUID requestInventoryAsset(LLUUID item_id,LLUUID task_id=LLUUID::null);

LLUUID		getInventoryItemUUID(const std::string& name,	int type);
std::string	getInventoryItemName(LLUUID name,				int type);

#endif