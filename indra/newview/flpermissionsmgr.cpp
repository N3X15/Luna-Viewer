// From LLSelectMgr, refactored and ungayed.
// static
void LLSelectMgr::processObjectProperties(LLMessageSystem* msg, void** user_data)
{
	S32 i;
	S32 count = msg->getNumberOfBlocksFast(_PREHASH_ObjectData);
	for (i = 0; i < count; i++)
	{
		LLUUID id;
		msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_ObjectID, id, i);

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


		// Iterate through nodes at end, since it can be on both the regular AND hover list
		struct f : public LLSelectedNodeFunctor
		{
			LLUUID mID;
			f(const LLUUID& id) : mID(id) {}
			virtual bool apply(LLSelectNode* node)
			{
				return (node->getObject() && node->getObject()->mID == mID);
			}
		} func(id);
		LLSelectNode* node = LLSelectMgr::getInstance()->getSelection()->getFirstNode(&func);

		if (node)
		{
			if (node->mInventorySerial != inv_serial)
			{
				node->getObject()->dirtyInventory();
			}

			// save texture data as soon as we get texture perms first time
			if (!node->mValid)
			{
				BOOL can_copy = FALSE;
				BOOL can_transfer = FALSE;

				LLAggregatePermissions::EValue value = LLAggregatePermissions::AP_NONE;
				if(node->getObject()->permYouOwner())
				{
					value = ag_texture_perms_owner.getValue(PERM_COPY);
					if (value == LLAggregatePermissions::AP_EMPTY || value == LLAggregatePermissions::AP_ALL)
					{
						can_copy = TRUE;
					}
					value = ag_texture_perms_owner.getValue(PERM_TRANSFER);
					if (value == LLAggregatePermissions::AP_EMPTY || value == LLAggregatePermissions::AP_ALL)
					{
						can_transfer = TRUE;
					}
				}
				else
				{
					value = ag_texture_perms.getValue(PERM_COPY);
					if (value == LLAggregatePermissions::AP_EMPTY || value == LLAggregatePermissions::AP_ALL)
					{
						can_copy = TRUE;
					}
					value = ag_texture_perms.getValue(PERM_TRANSFER);
					if (value == LLAggregatePermissions::AP_EMPTY || value == LLAggregatePermissions::AP_ALL)
					{
						can_transfer = TRUE;
					}
				}

				if (can_copy && can_transfer)
				{
					// this should be the only place that saved textures is called
					node->saveTextures(texture_ids);
				}
			}

			node->mValid = TRUE;
			node->mPermissions->init(creator_id, owner_id,
									 last_owner_id, group_id);
			node->mPermissions->initMasks(base_mask, owner_mask, everyone_mask, group_mask, next_owner_mask);
			node->mCreationDate = creation_date;
			node->mItemID = item_id;
			node->mFolderID = folder_id;
			node->mFromTaskID = from_task_id;
			node->mName.assign(name);
			node->mDescription.assign(desc);
			node->mSaleInfo = sale_info;
			node->mAggregatePerm = ag_perms;
			node->mAggregateTexturePerm = ag_texture_perms;
			node->mAggregateTexturePermOwner = ag_texture_perms_owner;
			node->mCategory = category;
			node->mInventorySerial = inv_serial;
			node->mSitName.assign(sit_name);
			node->mTouchName.assign(touch_name);
		}
	}

	dialog_refresh_all();

	// silly hack to allow 'save into inventory' 
	if(gPopupMenuView->getVisible())
	{
		gPopupMenuView->setItemEnabled(SAVE_INTO_INVENTORY,
									   enable_save_into_inventory(NULL));
	}

	// hack for left-click buy object
	LLToolPie::selectionPropertiesReceived();
}

// static
void LLSelectMgr::processObjectPropertiesFamily(LLMessageSystem* msg, void** user_data)
{
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

	// the reporter widget askes the server for info about picked objects
	if (request_flags & (COMPLAINT_REPORT_REQUEST | BUG_REPORT_REQUEST))
	{
		EReportType report_type = (COMPLAINT_REPORT_REQUEST & request_flags) ? COMPLAINT_REPORT : BUG_REPORT;
		LLFloaterReporter *reporterp = LLFloaterReporter::getReporter(report_type);
		if (reporterp)
		{
			std::string fullname;
			gCacheName->getFullName(owner_id, fullname);
			reporterp->setPickedObjectProperties(name, fullname, owner_id);
		}
	}
	else if (request_flags & OBJECT_PAY_REQUEST)
	{
		// check if the owner of the paid object is muted
		LLMuteList::getInstance()->autoRemove(owner_id, LLMuteList::AR_MONEY);
	}

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

