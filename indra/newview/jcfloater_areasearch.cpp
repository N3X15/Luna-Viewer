/* Copyright (c) 2009
 *
 * Modular Systems Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems Ltd nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "llviewerprecompiledheaders.h"

#include "lluuid.h"
#include "lluictrlfactory.h"

#include "llagent.h"

#include "llscrolllistctrl.h"

#include "llviewerobjectlist.h"

#include "jcfloater_areasearch.h"

#include "llviewerobjectlist.h"

#include "llcategory.h"

#include "llfloaterchat.h"

#include "llchat.h"
#include "lltracker.h"

JCFloaterAreaSearch* JCFloaterAreaSearch::sInstance = NULL;
std::map<LLUUID, AObjectDetails> JCFloaterAreaSearch::mObjectDetails;
std::string JCFloaterAreaSearch::searched_name;
std::string JCFloaterAreaSearch::searched_desc;
std::string JCFloaterAreaSearch::searched_owner;
std::string JCFloaterAreaSearch::searched_group;
bool JCFloaterAreaSearch::payable_object;
bool JCFloaterAreaSearch::buyable_object;

JCFloaterAreaSearch::JCFloaterAreaSearch() :  LLFloater()
{
	llassert_always(sInstance == NULL);
	sInstance = this;
}

JCFloaterAreaSearch::~JCFloaterAreaSearch()
{
	sInstance = NULL;
}

void JCFloaterAreaSearch::toggle()
{
	if (sInstance)
	{
		if(sInstance->getVisible())
		{
			sInstance->setVisible(FALSE);
		}
		else
		{
			sInstance->setVisible(TRUE);
		}
	}
	else
	{
		sInstance = new JCFloaterAreaSearch();
		LLUICtrlFactory::getInstance()->buildFloater(sInstance, "floater_searcharea.xml");
	}
}

BOOL JCFloaterAreaSearch::postBuild()
{
	mResultList = getChild<LLScrollListCtrl>("result_list");
	mResultList->setCallbackUserData(this);
	mResultList->setDoubleClickCallback(onDoubleClick);
	mResultList->sortByColumn("Name", TRUE);

	childSetAction("Reload",reload,this);

	childSetAction("Search",search,this);

	///childSetCommitCallback("Name query chunk", onCommitLine);
	//childSetCommitCallback("Description query chunk", onCommitLine);
	//childSetCommitCallback("Owner query chunk", onCommitLine);
	//childSetCommitCallback("group query chunk", onCommitLine);

	childSetKeystrokeCallback("Name query chunk", onCommitLine, 0);
	childSetKeystrokeCallback("Description query chunk", onCommitLine, 0);
	childSetKeystrokeCallback("Owner query chunk", onCommitLine, 0);
	childSetKeystrokeCallback("Group query chunk", onCommitLine, 0);

	return 1;
}

void JCFloaterAreaSearch::onDoubleClick(void *userdata)
{
	JCFloaterAreaSearch *self = (JCFloaterAreaSearch*)userdata;
 	LLScrollListItem *item =   self->mResultList->getFirstSelected();
	if(!item)return;
	LLUUID object_id = item->getUUID();
	LLViewerObject* objectp = gObjectList.findObject(object_id);
	if(objectp)
	{
	//gAgent.setFocusObject(gObjectList.findObject(object_id));
	//gAgent.setFocusOnAvatar(FALSE, TRUE);
		//LLVector3 pos_agent = objectp->getPositionGlobal()
		//LLVector3d pos_global = gAgent.getPosGlobalFromAgent(pos_agent);
		LLTracker::trackLocation(objectp->getPositionGlobal(), mObjectDetails[object_id].name, "", LLTracker::LOCATION_ITEM);
	}
}

void JCFloaterAreaSearch::reload(void* data)
{
	if(sInstance)sInstance->close(TRUE);
	JCFloaterAreaSearch::toggle();
	JCFloaterAreaSearch::toggle();
}

void JCFloaterAreaSearch::search(void* data)
{
	{LLChat chat;
	chat.mText = "clicked search";
	LLFloaterChat::addChat(chat);}
	results();
}

void JCFloaterAreaSearch::draw()
{
	//results();
	LLFloater::draw();
}

void JCFloaterAreaSearch::onCommitLine(LLLineEditor* line, void* user_data)
{
	//LLLineEditor* line = (LLLineEditor*)caller;
	std::string name = line->getName();
	std::string text = line->getText();
	LLStringUtil::toLower(text);
	line->setText(text);
	if(name == "Name query chunk")searched_name = text;
	else if(name == "Description query chunk")searched_desc = text;
	else if(name == "Owner query chunk")searched_owner = text;
	else if(name == "Group query chunk")searched_group = text;
	/*{LLChat chat;
	chat.mText = "loaded "+name+" with "+text;
	LLFloaterChat::addChat(chat);}*/

	if(text.length() > 3)results();
	//return TRUE;
}
void mark(std::string lo)//debug
{
	{LLChat chat;
	chat.mText = lo;
	LLFloaterChat::addChat(chat);}
}
std::string request_string = "Requested_123456";
void JCFloaterAreaSearch::requestifneed(LLViewerObject *objectp)
{
	if(objectp)
	{
		LLUUID object_id = objectp->getID();
		if(mObjectDetails.count( object_id ) == 0)
		{
			//mark("not in list");
			AObjectDetails* details = &mObjectDetails[object_id];
			details->name = request_string;
			details->desc = request_string;
			details->owner_id = LLUUID::null;
			details->group_id = LLUUID::null;

			LLMessageSystem* msg = gMessageSystem;
			msg->newMessageFast(_PREHASH_RequestObjectPropertiesFamily);
			msg->nextBlockFast(_PREHASH_AgentData);
			msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
			msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
			msg->nextBlockFast(_PREHASH_ObjectData);
			msg->addU32Fast(_PREHASH_RequestFlags, 0 );
			msg->addUUIDFast(_PREHASH_ObjectID, object_id);
			gAgent.sendReliableMessage();
			//mark("sent data req");
		}
	}
}
void JCFloaterAreaSearch::results()
{
	if(!sInstance)return;
	if(!(sInstance->getVisible()))return;
	//mark("results()");
	LLDynamicArray<LLUUID> selected = sInstance->mResultList->getSelectedIDs();
	S32 scrollpos = sInstance->mResultList->getScrollPos();
	sInstance->mResultList->deleteAllItems();
	S32 i;
	S32 total = gObjectList.getNumObjects();

	LLViewerRegion* our_region = gAgent.getRegion();
	for (i = 0; i < total; i++)
	{
		LLViewerObject *objectp = gObjectList.getObject(i);
		if (objectp)
		{
			if(objectp->getRegion() == our_region)
			{
				LLUUID object_id = objectp->getID();
				if(objectp->isRoot() && !objectp->flagTemporary() && !objectp->flagTemporaryOnRez())
				{
					//mark("isRoot()");
					bool emptyfields = (searched_name == "" && searched_desc == "" && searched_owner == "" && searched_group == "");
					if(!emptyfields && mObjectDetails.count( object_id ) == 0)
					{
						//mark("not all entries are \"\"");
						requestifneed(objectp);
						//obj_counts[objectp->getPCode()]++;
					}else
					{
						//mark("all entries are \"\" or we have data");
						AObjectDetails* details = &mObjectDetails[object_id];
						std::string object_name = details->name;
						std::string object_desc = details->desc;
						std::string object_owner;
						std::string object_group;
						gCacheName->getFullName(details->owner_id,object_owner);
						gCacheName->getGroupName(details->group_id,object_group);
						if(
							emptyfields || object_name != request_string
							)
						{
							//mark("both names are loaded or aren't needed");
							std::string onU = object_owner;
							std::string cnU = object_group;
							LLStringUtil::toLower(object_name);
							LLStringUtil::toLower(object_desc);
							LLStringUtil::toLower(object_owner);
							LLStringUtil::toLower(object_group);
							if(
								(searched_name == "" || object_name.find(searched_name) != -1) &&
								(searched_desc == "" || object_desc.find(searched_desc) != -1) &&
								(searched_owner == "" || object_owner.find(searched_owner) != -1) &&
								(searched_group == "" || object_group.find(searched_group) != -1) &&
								(!payable_object || (bool)objectp->flagTakesMoney() == payable_object)
								)
							{
								requestifneed(objectp);
								//mark("pass");
								LLSD element;
								element["id"] = object_id;
								element["columns"][LIST_OBJECT_NAME]["column"] = "Name";
								element["columns"][LIST_OBJECT_NAME]["type"] = "text";
								element["columns"][LIST_OBJECT_NAME]["value"] = details->name;//item->getName();//ai->second//"avatar_icon";
								element["columns"][LIST_OBJECT_DESC]["column"] = "Description";
								element["columns"][LIST_OBJECT_DESC]["type"] = "text";
								element["columns"][LIST_OBJECT_DESC]["value"] = details->desc;//ai->second;
								element["columns"][LIST_OBJECT_OWNER]["column"] = "Owner";
								element["columns"][LIST_OBJECT_OWNER]["type"] = "text";
								element["columns"][LIST_OBJECT_OWNER]["value"] = onU;//aifirst;
								element["columns"][LIST_OBJECT_GROUP]["column"] = "Group";
								element["columns"][LIST_OBJECT_GROUP]["type"] = "text";
								element["columns"][LIST_OBJECT_GROUP]["value"] = cnU;//ai->second;
								sInstance->mResultList->addElement(element, ADD_BOTTOM);
							}
						}
					}
				}
			}
		}
	}

	sInstance->mResultList->sortItems();
	sInstance->mResultList->selectMultiple(selected);
	sInstance->mResultList->setScrollPos(scrollpos);
}

void JCFloaterAreaSearch::callbackLoadOwnerName(const LLUUID& id, const std::string& first, const std::string& last, BOOL is_group, void* data)
{
	results();
}

void JCFloaterAreaSearch::processObjectPropertiesFamily(LLMessageSystem* msg, void** user_data)
{
	//JCFloaterAreaSearch *self = JCFloaterAreaSearch::getInstance();
	LLUUID object_id;
	U32 request_flags;
	//LLUUID group_id;
	LLUUID owner_id;
	LLUUID group_id;
	LLUUID extra_id;
	U32 base_mask, owner_mask, group_mask, everyone_mask, next_owner_mask;
	LLSaleInfo sale_info;
	LLCategory category;
	msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_RequestFlags,	request_flags );
	msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_ObjectID,		object_id );
	msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_OwnerID,		owner_id );
	msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_GroupID,		group_id );
	msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_BaseMask,		base_mask );
	msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_OwnerMask,		owner_mask );
	msg->getU32Fast(_PREHASH_ObjectData,_PREHASH_GroupMask,		group_mask );
	msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_EveryoneMask,	everyone_mask );
	msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_NextOwnerMask, next_owner_mask);
	sale_info.unpackMessage(msg, _PREHASH_ObjectData);
	category.unpackMessage(msg, _PREHASH_ObjectData);
	LLUUID last_owner_id;
	msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_LastOwnerID, last_owner_id );
	std::string name;
	msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Name, name);
	std::string desc;
	msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Description, desc);


	if(mObjectDetails.count( object_id ) != 0)
	{
		AObjectDetails* details = &mObjectDetails[object_id];
		details->name = name;
		details->desc = desc;
		details->owner_id = owner_id;
		details->group_id = group_id;
		//mark(owner_id.asString());
		//mark(group_id.asString());
		gCacheName->get(owner_id, FALSE, callbackLoadOwnerName);
		gCacheName->get(group_id, TRUE, callbackLoadOwnerName);
	}
}

void JCFloaterAreaSearch::close(bool app)
{
	if(app)
	{
		LLFloater::close(app);
	}else
	{
		if(sInstance)
		{
			sInstance->setVisible(FALSE);
		}
	}
}


