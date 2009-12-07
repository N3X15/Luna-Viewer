/* Copyright (c) 2009
 *
 * Modular Systems All rights reserved.
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

#include "jc_asset_comparer.h"

#include "lltoolobjpicker.h"
#include "lltoolmgr.h"
#include "llviewerobjectlist.h"

JCAssetComparer* JCAssetComparer::sInstance;
AssetSubjectDetails JCAssetComparer::subjectA;
AssetSubjectDetails JCAssetComparer::subjectB;

JCAssetComparer::JCAssetComparer() : LLFloater()
{
	llassert_always(sInstance == NULL);
	sInstance = this;

	subjectA.owner_id.setNull();
	subjectA.desc = "";
	subjectA.id.setNull();
	subjectA.group_id.setNull();
	subjectA.name = "";

	subjectB.owner_id.setNull();
	subjectB.desc = "";
	subjectB.id.setNull();
	subjectB.group_id.setNull();
	subjectB.name = "";
}

JCAssetComparer::~JCAssetComparer()
{
	sInstance = NULL;
}

void JCAssetComparer::toggle()
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
		sInstance = new JCAssetComparer();
		LLUICtrlFactory::getInstance()->buildFloater(sInstance, "floater_assetcompare.xml");
	}
}

BOOL JCAssetComparer::postBuild()
{
	childSetAction("Reload",reload,this);
	return 1;
}

void JCAssetComparer::reload(void* data)
{
	if(sInstance)
	{
		delete sInstance;
		sInstance = NULL;
	}
	JCAssetComparer::toggle();
}

// static 
void JCAssetComparer::onClickObjPickerA(void *userdata)
{
	JCAssetComparer *self = (JCAssetComparer*)userdata;
	LLToolObjPicker::getInstance()->setExitCallback(JCAssetComparer::closePickToolA, self);
	LLToolMgr::getInstance()->setTransientTool(LLToolObjPicker::getInstance());
	self->childSetText("object_a_name", LLStringUtil::null);
	self->childSetText("owner_a_name", LLStringUtil::null);
	LLButton* pick_btn = self->getChild<LLButton>("pick_a_btn");
	if (pick_btn)pick_btn->setToggleState(TRUE);
}

// static 
void JCAssetComparer::onClickObjPickerB(void *userdata)
{
	JCAssetComparer *self = (JCAssetComparer*)userdata;
	LLToolObjPicker::getInstance()->setExitCallback(JCAssetComparer::closePickToolB, self);
	LLToolMgr::getInstance()->setTransientTool(LLToolObjPicker::getInstance());
	self->childSetText("object_b_name", LLStringUtil::null);
	self->childSetText("owner_b_name", LLStringUtil::null);
	LLButton* pick_btn = self->getChild<LLButton>("pick_b_btn");
	if (pick_btn)pick_btn->setToggleState(TRUE);
}

// static
void JCAssetComparer::closePickToolA(void *userdata)
{
	JCAssetComparer *self = (JCAssetComparer *)userdata;

	LLUUID object_id = LLToolObjPicker::getInstance()->getObjectID();
	subjectA.id = object_id;
	getData(object_id);
	//self->getObjectInfo(object_id);
	LLToolMgr::getInstance()->clearTransientTool();
	LLButton* pick_btn = self->getChild<LLButton>("pick_a_btn");
	if (pick_btn)pick_btn->setToggleState(FALSE);
}

// static
void JCAssetComparer::closePickToolB(void *userdata)
{
	JCAssetComparer *self = (JCAssetComparer *)userdata;

	LLUUID object_id = LLToolObjPicker::getInstance()->getObjectID();
	subjectB.id = object_id;
	getData(object_id);
	//self->getObjectInfo(object_id);
	LLToolMgr::getInstance()->clearTransientTool();
	LLButton* pick_btn = self->getChild<LLButton>("pick_b_btn");
	if(pick_btn)pick_btn->setToggleState(FALSE);

}

void JCAssetComparer::getData(LLUUID object)
{
	LLViewerObject* objectp = gObjectList.findObject(object);
	if(objectp)
	{
		gMessageSystem->newMessageFast(_PREHASH_ObjectSelect);
		gMessageSystem->nextBlockFast(_PREHASH_AgentData);
		gMessageSystem->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
		gMessageSystem->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		gMessageSystem->nextBlockFast(_PREHASH_ObjectData);
		gMessageSystem->addU32Fast(_PREHASH_ObjectLocalID, objectp->getLocalID());
		gAgent.sendReliableMessage();
	}
}

void JCAssetComparer::close(bool app)
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

void JCAssetComparer::processObjectProperties(LLMessageSystem* msg, void** user_data)
{
	if(!sInstance || !sInstance->getVisible())return;

	S32 i;
	S32 count = msg->getNumberOfBlocksFast(_PREHASH_ObjectData);
	for (i = 0; i < count; i++)
	{
		LLUUID id;
		msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_ObjectID, id, i);
		if(id == subjectA.id || id == subjectB.id)
		{
			AssetSubjectDetails* lolsubj = id == subjectA.id ? &subjectA : &subjectB;

			LLUUID creator_id;
			LLUUID owner_id;
			LLUUID group_id;
			LLUUID last_owner_id;
			U64 creation_date;
			LLUUID extra_id;
			U32 base_mask, owner_mask, group_mask, everyone_mask, next_owner_mask;
			LLSaleInfo sale_info;
			LLCategory category;
			LLAggregatePermissions ag_perms;
			LLAggregatePermissions ag_texture_perms;
			LLAggregatePermissions ag_texture_perms_owner;
			
			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_CreatorID, creator_id, i);
			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_OwnerID, owner_id, i);
			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_GroupID, group_id, i);
			msg->getU64Fast(_PREHASH_ObjectData, _PREHASH_CreationDate, creation_date, i);
			msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_BaseMask, base_mask, i);
			msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_OwnerMask, owner_mask, i);
			msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_GroupMask, group_mask, i);
			msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_EveryoneMask, everyone_mask, i);
			msg->getU32Fast(_PREHASH_ObjectData, _PREHASH_NextOwnerMask, next_owner_mask, i);
			sale_info.unpackMultiMessage(msg, _PREHASH_ObjectData, i);

			ag_perms.unpackMessage(msg, _PREHASH_ObjectData, _PREHASH_AggregatePerms, i);
			ag_texture_perms.unpackMessage(msg, _PREHASH_ObjectData, _PREHASH_AggregatePermTextures, i);
			ag_texture_perms_owner.unpackMessage(msg, _PREHASH_ObjectData, _PREHASH_AggregatePermTexturesOwner, i);
			category.unpackMultiMessage(msg, _PREHASH_ObjectData, i);

			S16 inv_serial = 0;
			msg->getS16Fast(_PREHASH_ObjectData, _PREHASH_InventorySerial, inv_serial, i);

			LLUUID item_id;
			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_ItemID, item_id, i);
			LLUUID folder_id;
			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_FolderID, folder_id, i);
			LLUUID from_task_id;
			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_FromTaskID, from_task_id, i);

			msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_LastOwnerID, last_owner_id, i);

			std::string name;
			msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Name, name, i);
			std::string desc;
			msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Description, desc, i);

			std::string touch_name;
			msg->getStringFast(_PREHASH_ObjectData, _PREHASH_TouchName, touch_name, i);
			std::string sit_name;
			msg->getStringFast(_PREHASH_ObjectData, _PREHASH_SitName, sit_name, i);

			//unpack TE IDs
			std::vector<LLUUID> texture_ids;
			S32 size = msg->getSizeFast(_PREHASH_ObjectData, i, _PREHASH_TextureID);
			if (size > 0)
			{
				S8 packed_buffer[SELECT_MAX_TES * UUID_BYTES];
				msg->getBinaryDataFast(_PREHASH_ObjectData, _PREHASH_TextureID, packed_buffer, 0, i, SELECT_MAX_TES * UUID_BYTES);

				for (S32 buf_offset = 0; buf_offset < size; buf_offset += UUID_BYTES)
				{
					LLUUID tid;
					memcpy(tid.mData, packed_buffer + buf_offset, UUID_BYTES);		/* Flawfinder: ignore */
					texture_ids.push_back(tid);
				}
			}

			lolsubj->group_id = group_id;
			lolsubj->desc = desc;
			lolsubj->name = name;
			lolsubj->owner_id = owner_id;

			update();
		}
	}
}


void JCAssetComparer::update()
{
	sInstance->childSetValue("object_a_name", subjectA.name);
	sInstance->childSetValue("owner_a_name", "(Loading)");
	gCacheName->get(subjectA.owner_id, FALSE, callbackLoadOwnerName, 0);
	sInstance->childSetValue("object_b_name", subjectB.name);
	sInstance->childSetValue("owner_b_name", "(Loading)");
	gCacheName->get(subjectB.owner_id, FALSE, callbackLoadOwnerName, 0);
}

void JCAssetComparer::callbackLoadOwnerName(const LLUUID& id, const std::string& first, const std::string& last, BOOL is_group, void* data)
{
	if(id == subjectA.owner_id)sInstance->childSetValue("owner_a_name", first + " " + last);
	else if(id == subjectB.owner_id)sInstance->childSetValue("owner_b_name", first + " " + last);
}


