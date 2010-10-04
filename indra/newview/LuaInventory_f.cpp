#include "llviewerprecompiledheaders.h"

#include "LuaInventory_f.h"

#include "llinventorymodel.h"
#include "llviewerinventory.h"
#include "llmutelist.h"
#include "llagent.h"
#include "llfloatertools.h"
#include "llimview.h"
#include "LuaBase_f.h"
#include <algorithm>    // copy
#include <iterator>     // ostream_operator

#include <boost/tokenizer.hpp>


void giveInventoryItem_Event(LLUUID to_agent, LLUUID item_id, LLUUID im_session_id);
LLUUID TraverseCategories(const std::string& target_cat, LLViewerInventoryCategory *ccat=NULL, int i=0);

// LLUUID catID=TraverseCategories("/My Inventory/#Luna/Durp")
LLUUID TraverseCategories(const std::string& target_cat, LLViewerInventoryCategory *ccat, int i)
{
	// Split the path into an array.
	boost::char_separator<char> sep("/");
	boost::tokenizer<boost::char_separator<char> > tok(target_cat,sep);
	std::vector<std::string> vec;
	vec.assign(tok.begin(),tok.end());

	std::string cat = vec[i];
	LLUUID id;
	if(i==0)
	{
		id=gAgent.getInventoryRootID();
	} else {
		id = ccat->getUUID();
	}
	LLInventoryModel::cat_array_t *cats;
	LLInventoryModel::item_array_t *items;

	gInventory.getDirectDescendentsOf(id,cats,items);

	LLInventoryModel::cat_array_t::iterator cat_iter = cats->begin();
	LLInventoryModel::cat_array_t::iterator cat_end = cats->end();
	for( ; cat_iter != cat_end; ++cat_iter)
	{
		LLViewerInventoryCategory *c_cat=(*cat_iter);
		if(c_cat->getName() == cat)
		{
			if(vec.size() == i+1)
			{
				return c_cat->getUUID();
			} else {
				++i;
				return TraverseCategories(target_cat,*cat_iter,i);
			}
		}
	}
	return LLUUID::null;
}
LLUUID getCategoryUUID(const std::string& path)
{
	return TraverseCategories(path);
}
void findInventoryInFolder(const std::string& ifolder,std::vector<std::string>& rcats,std::vector<LLUUID>& ritems)
{
	LLUUID folder = gInventory.findCategoryByName(ifolder);
	LLViewerInventoryCategory::cat_array_t cats;
	LLViewerInventoryItem::item_array_t items;
	//ObjectContentNameMatches objectnamematches(ifolder);
	gInventory.collectDescendents(folder,cats,items,FALSE);//,objectnamematches);
	
	for(int i = 0;i<cats.count();++i)
	{
		rcats.push_back(((LLViewerInventoryCategory *)cats.get(i))->getName());
	}
	for(int i = 0;i<items.count();++i)
	{
		ritems.push_back(((LLViewerInventoryItem *)items.get(i))->getUUID());
	}
}
void giveInventoryItem_Event(LLUUID &to_agent, LLUUID &item_id, LLUUID &im_session_id)
{
	LLViewerInventoryItem *item = gInventory.getItem(item_id);
	if(!item) 
	{
		std::string dongs = llformat("Could not find item %s.",item_id.asString());
		LuaError(dongs.c_str());
		return;
	}
	std::string name;
	gAgent.buildFullname(name);
	LLUUID transaction_id;
	transaction_id.generate();
	const S32 BUCKET_SIZE = sizeof(U8) + UUID_BYTES;
	U8 bucket[BUCKET_SIZE];
	bucket[0] = (U8)item->getType();
	memcpy(&bucket[1], &(item->getUUID().mData), UUID_BYTES);		/* Flawfinder: ignore */
	pack_instant_message(
		gMessageSystem,
		gAgent.getID(),
		FALSE,
		gAgent.getSessionID(),
		to_agent,
		name,
		item->getName(),
		IM_ONLINE,
		IM_INVENTORY_OFFERED,
		transaction_id,
		0,
		LLUUID::null,
		gAgent.getPositionAgent(),
		NO_TIMESTAMP,
		bucket,
		BUCKET_SIZE);
	gAgent.sendReliableMessage(); 
	// No VEffects.
	gFloaterTools->dirty();

	LLMuteList::getInstance()->autoRemove(to_agent, LLMuteList::AR_INVENTORY);

	// If this item was given by drag-and-drop into an IM panel, log this action in the IM panel chat.
	if (im_session_id != LLUUID::null)
	{
		LLSD args;
		gIMMgr->addSystemMessage(im_session_id, "inventory_item_offered", args);
	}
}

void giveInventoryItem(LLUUID to_agent, LLUUID item_id, LLUUID im_session_id)
{
	CB_Args3<LLUUID,LLUUID,LLUUID>(giveInventoryItem_Event,to_agent,item_id,im_session_id);
}