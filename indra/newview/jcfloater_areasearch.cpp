/* Copyright (c) 2009
 *
 * Modular Systems. All rights reserved.
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
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
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

#include "llcachename.h"

#include "llfloaterchat.h"

#include "llchat.h"
#include "lltracker.h"
#include "llviewerregion.h"

JCFloaterAreaSearch* JCFloaterAreaSearch::sInstance = NULL;
std::map<LLUUID, AObjectDetails> JCFloaterAreaSearch::mObjectDetails;
std::string JCFloaterAreaSearch::searched_name;
std::string JCFloaterAreaSearch::searched_desc;
std::string JCFloaterAreaSearch::searched_owner;
std::string JCFloaterAreaSearch::searched_group;
bool JCFloaterAreaSearch::payable_object;
bool JCFloaterAreaSearch::scripted_object;
bool JCFloaterAreaSearch::touch_object;
bool JCFloaterAreaSearch::temporary_object;
bool JCFloaterAreaSearch::sculpted_object;
bool JCFloaterAreaSearch::flexible_object;
bool JCFloaterAreaSearch::phantom_object;
bool JCFloaterAreaSearch::sound_object;
bool JCFloaterAreaSearch::particle_object;
bool JCFloaterAreaSearch::animation_object;
bool JCFloaterAreaSearch::inventoryadd_object;
bool JCFloaterAreaSearch::attachment_object;
bool JCFloaterAreaSearch::hudattachment_object;

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

	childSetAction("Search",search,this);

	childSetCommitCallback("payable", onCheckPayable,this);
	childSetCommitCallback("touch_only", onCheckTouchOnly,this);
	childSetCommitCallback("scripted", onCheckScripted,this);
	childSetCommitCallback("sounds", onCheckSounds,this);
	childSetCommitCallback("particles", onCheckParticles,this);
	childSetCommitCallback("animations", onCheckAnimations,this);
	childSetCommitCallback("temporary", onCheckTemporary,this);
	childSetCommitCallback("sculpted", onCheckSculpted,this);
	childSetCommitCallback("flexible", onCheckFlexible,this);
	childSetCommitCallback("phantom", onCheckPhantom,this);
	childSetCommitCallback("inventoryadd", onCheckInventoryAdd,this);
	childSetCommitCallback("attachment", onCheckAttachment,this);
	childSetCommitCallback("hudattachment", onCheckHudAttachment,this);



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
		LLTracker::trackLocation(objectp->getPositionGlobal(), mObjectDetails[object_id].name, "", LLTracker::LOCATION_ITEM);
	}
}

void JCFloaterAreaSearch::search(void* data)
{
	results();
}

void JCFloaterAreaSearch::draw()
{
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
}

void JCFloaterAreaSearch::onCheckPayable(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	payable_object = self->childGetValue("payable").asBoolean();
}

void JCFloaterAreaSearch::onCheckTouchOnly(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	touch_object = self->childGetValue("touch_only").asBoolean();
	scripted_object = self->childGetValue("touch_only").asBoolean();
}

void JCFloaterAreaSearch::onCheckScripted(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	scripted_object = self->childGetValue("scripted").asBoolean();
}

void JCFloaterAreaSearch::onCheckSounds(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	sound_object = self->childGetValue("sounds").asBoolean();
}

void JCFloaterAreaSearch::onCheckParticles(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	particle_object = self->childGetValue("particles").asBoolean();
}

void JCFloaterAreaSearch::onCheckAnimations(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	animation_object = self->childGetValue("animations").asBoolean();
}

void JCFloaterAreaSearch::onCheckTemporary(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	temporary_object = self->childGetValue("temporary").asBoolean();
}

void JCFloaterAreaSearch::onCheckSculpted(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	sculpted_object = self->childGetValue("sculpted").asBoolean();
}

void JCFloaterAreaSearch::onCheckFlexible(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	flexible_object = self->childGetValue("flexible").asBoolean();
}

void JCFloaterAreaSearch::onCheckPhantom(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	phantom_object = self->childGetValue("phantom").asBoolean();
}

void JCFloaterAreaSearch::onCheckInventoryAdd(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	inventoryadd_object = self->childGetValue("inventoryadd").asBoolean();
}

void JCFloaterAreaSearch::onCheckAttachment(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	attachment_object = self->childGetValue("attachment").asBoolean();
}

void JCFloaterAreaSearch::onCheckHudAttachment(LLUICtrl* ctrl,void* user_data)
{
	JCFloaterAreaSearch* self = (JCFloaterAreaSearch*)user_data;
	hudattachment_object = self->childGetValue("hudattachment").asBoolean();
}

void JCFloaterAreaSearch::results()
{
	if(!sInstance)return;
	if(!(sInstance->getVisible()))return;
	//mark("results()");
	LLDynamicArray<LLUUID> selected = sInstance->mResultList->getSelectedIDs();
	sInstance->mResultList->deleteAllItems();
	S32 i;
	S32 total = gObjectList.getNumObjects();

	F32 throttle = gSavedSettings.getF32("OutBandwidth");
	if(throttle < 128000.)
	{
		gMessageSystem->mPacketRing.setOutBandwidth(128000.0);
	}
	gMessageSystem->mPacketRing.setUseOutThrottle(TRUE);

	LLViewerRegion* our_region = gAgent.getRegion();
	for (i = 0; i < total; i++)
	{
		LLViewerObject *objectp = gObjectList.getObject(i);
		if (objectp)
		{
			if(objectp->getRegion() == our_region)
			{
				if(objectp->isRoot()){

				if(objectp->isAvatar()){ continue; }
				else if(payable_object || (bool)objectp->flagTakesMoney() != payable_object){ continue; }
				else if(scripted_object || (bool)objectp->flagScripted() != scripted_object){ continue; }
				else if(touch_object || (bool)objectp->flagHandleTouch() != touch_object){ continue; }
				else if(temporary_object || (bool)objectp->flagTemporary() != temporary_object){ continue; }
				else if(sculpted_object || (bool)objectp->isSculpted() != sculpted_object){ continue; }
				else if(flexible_object || (bool)objectp->isFlexible() != flexible_object){ continue; }
				else if(phantom_object || (bool)objectp->flagPhantom() != phantom_object){ continue; }
				else if(sound_object || (bool)objectp->isAudioSource() != sound_object){ continue; }
				else if(particle_object || (bool)objectp->isParticleSource() != particle_object){ continue; }
				else if(animation_object || (bool)objectp->flagAnimSource() != animation_object){ continue; }
				else if(inventoryadd_object || (bool)objectp->flagAllowInventoryAdd() != inventoryadd_object){ continue; }
				else if(attachment_object || (bool)objectp->isAttachment() != attachment_object){ continue; }
				else if(hudattachment_object || (bool)objectp->isHUDAttachment() != hudattachment_object){ continue; } 


					LLUUID object_id = objectp->getID();
					// we have to query the simulator for information 
					// about this object
					LLMessageSystem* msg = gMessageSystem;
					msg->newMessageFast(_PREHASH_RequestObjectPropertiesFamily);
					msg->nextBlockFast(_PREHASH_AgentData);
					msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
					msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
					msg->nextBlockFast(_PREHASH_ObjectData);
					msg->addU32Fast(_PREHASH_RequestFlags, MY_REQUEST );
					msg->addUUIDFast(_PREHASH_ObjectID, object_id);
					gAgent.sendReliableMessage();
				}
			}
		}
	}
}

void JCFloaterAreaSearch::close(bool app)
{
	F32 throttle = gSavedSettings.getF32("OutBandwidth");
	if(throttle != 0.)
	{
		gMessageSystem->mPacketRing.setOutBandwidth(throttle);
		gMessageSystem->mPacketRing.setUseOutThrottle(TRUE);
	}
	else
	{
		gMessageSystem->mPacketRing.setOutBandwidth(0.0);
		gMessageSystem->mPacketRing.setUseOutThrottle(FALSE);
	}

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

// static
void JCFloaterAreaSearch::processObjectPropertiesFamily(LLMessageSystem* msg, void** user_data)
{
	if(!sInstance) return;
	LLUUID id;

	U32 request_flags;
	LLUUID creator_id;
	LLUUID owner_id;
	LLUUID group_id;
	LLUUID extra_id;
	U32 base_mask, owner_mask, group_mask, everyone_mask, next_owner_mask;
	LLSaleInfo sale_info;
	LLCategory category;
	
	msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_RequestFlags,	request_flags );
	msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_ObjectID,		id );
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

	// unpack name & desc
	std::string name;
	msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Name, name);

	std::string desc;
	msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Description, desc);

	std::string fullname;
	gCacheName->getFullName(owner_id, fullname);

	std::string groupname;
	gCacheName->getFullName(group_id, groupname);

	LLStringUtil::toLower(name);
	LLStringUtil::toLower(desc);
	LLStringUtil::toLower(fullname);
	LLStringUtil::toLower(groupname);

	if(searched_name != ""){
		if(name.find(searched_name) == -1){
			return;
		}
	}

	if(searched_desc != ""){
		if(desc.find(searched_desc) == -1){
			return;
		}
	}

	if(searched_owner != ""){
		if(fullname.find(searched_owner) == -1){
			return;
		}
	}

	if(searched_group != ""){
		if(groupname.find(searched_group) == -1){
			return;
		}
	}

	LLSD element;
	element["id"] = id;
	element["columns"][LIST_OBJECT_NAME]["column"] = "Name";
	element["columns"][LIST_OBJECT_NAME]["type"] = "text";
	element["columns"][LIST_OBJECT_NAME]["color"] = gColors.getColor("DefaultListText").getValue();
	element["columns"][LIST_OBJECT_NAME]["value"] = name;//item->getName();//ai->second//"avatar_icon";
	element["columns"][LIST_OBJECT_DESC]["column"] = "Description";
	element["columns"][LIST_OBJECT_DESC]["type"] = "text";
	element["columns"][LIST_OBJECT_DESC]["value"] = desc;//ai->second;
	element["columns"][LIST_OBJECT_DESC]["color"] = gColors.getColor("DefaultListText").getValue();
	element["columns"][LIST_OBJECT_OWNER]["column"] = "Owner";
	element["columns"][LIST_OBJECT_OWNER]["type"] = "text";
	element["columns"][LIST_OBJECT_OWNER]["value"] = fullname;//aifirst;
	element["columns"][LIST_OBJECT_OWNER]["color"] = gColors.getColor("DefaultListText").getValue();
	element["columns"][LIST_OBJECT_GROUP]["column"] = "Group";
	element["columns"][LIST_OBJECT_GROUP]["type"] = "text";
	element["columns"][LIST_OBJECT_GROUP]["value"] = groupname;//ai->second;
	element["columns"][LIST_OBJECT_GROUP]["color"] = gColors.getColor("DefaultListText").getValue();
	
	sInstance->mResultList->addElement(element, ADD_BOTTOM);

	sInstance->mResultList->sortItems();

	// Now look through all of the hovered nodes
	struct f : public LLSelectedNodeFunctor
	{
		LLUUID mID;
		f(const LLUUID& id) : mID(id) {}
		virtual bool apply(LLSelectNode* node)
		{
			return (node->getObject() && node->getObject()->mID == mID);
		}
	} func(id);
	LLSelectNode* node = LLSelectMgr::getInstance()->getHoverObjects()->getFirstNode(&func);

	if (node)
	{
		node->mValid = TRUE;
		node->mPermissions->init(LLUUID::null, owner_id,
								 last_owner_id, group_id);
		node->mPermissions->initMasks(base_mask, owner_mask, everyone_mask, group_mask, next_owner_mask);
		node->mSaleInfo = sale_info;
		node->mCategory = category;
		node->mName.assign(name);
		node->mDescription.assign(desc);
	}

	dialog_refresh_all();
}