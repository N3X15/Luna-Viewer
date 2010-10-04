#include "llinventorymodel.h"
#include "llviewerinventory.h"

void findInventoryInFolder(const std::string& ifolder,LLViewerInventoryCategory *rcats,LLViewerInventoryItem *ritems)
{
	LLInventoryItem *rItems,LLInventoryCategory
	LLUUID folder = gInventory.findCategoryByName(ifolder);
	LLViewerInventoryCategory::cat_array_t cats;
	LLViewerInventoryItem::item_array_t items;
	//ObjectContentNameMatches objectnamematches(ifolder);
	gInventory.collectDescendents(folder,cats,items,FALSE);//,objectnamematches);
	
	rcats=new LLViewerInventoryCategory[cats.count()];
	titems=new LLViewerInventoryItem[items.count()];
	
	for(int i = 0;i<cats.count();++i)
	{
		rcats[i]=cats[i];
	}
	for(int i = 0;i<items.count();++i)
	{
		ritems[i]=items[i];
	}

	return items;
}