/** 
 * @file llfloateravatarpicker.cpp
 *
 * $LicenseInfo:firstyear=2003&license=viewergpl$
 * 
 * Copyright (c) 2003-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llfloateravatarpicker.h"

#include "message.h"

#include "llagent.h"
#include "llbutton.h"
#include "llfocusmgr.h"
#include "llinventoryview.h"
#include "llinventorymodel.h"
#include "lllineeditor.h"
#include "llscrolllistctrl.h"
#include "lltextbox.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llworld.h"

const S32 MIN_WIDTH = 200;
const S32 MIN_HEIGHT = 340;
const LLRect FLOATER_RECT(0, 380, 240, 0);
const std::string FLOATER_TITLE = "Choose Resident";

// static
LLFloaterAvatarPicker* LLFloaterAvatarPicker::sInstance = NULL;


LLFloaterAvatarPicker* LLFloaterAvatarPicker::show(callback_t callback, 
												   void* userdata,
												   BOOL allow_multiple,
												   BOOL closeOnSelect)
{
	// TODO: This class should not be a singleton as it's used in multiple places
	// and therefore can't be used simultaneously. -MG
	if (!sInstance)
	{
		sInstance = new LLFloaterAvatarPicker();
		sInstance->mCallback = callback;
		sInstance->mCallbackUserdata = userdata;
		sInstance->mCloseOnSelect = FALSE;

		sInstance->open();	/* Flawfinder: ignore */
		sInstance->center();
		sInstance->setAllowMultiple(allow_multiple);
	}
	else
	{
		sInstance->open();	/*Flawfinder: ignore*/
		sInstance->mCallback = callback;
		sInstance->mCallbackUserdata = userdata;
		sInstance->setAllowMultiple(allow_multiple);
	}
	
	sInstance->mNearMeListComplete = FALSE;
	sInstance->mCloseOnSelect = closeOnSelect;
	return sInstance;
}

// Default constructor
LLFloaterAvatarPicker::LLFloaterAvatarPicker() :
	LLFloater(std::string("avatarpicker"), FLOATER_RECT, FLOATER_TITLE, TRUE, MIN_WIDTH, MIN_HEIGHT),
	mResultsReturned(FALSE),
	mCallback(NULL),
	mCallbackUserdata(NULL)
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_avatar_picker.xml", NULL);
}

BOOL LLFloaterAvatarPicker::postBuild()
{
	childSetKeystrokeCallback("Edit", editKeystroke, this);
	childSetKeystrokeCallback("EditUUID", editKeystroke, this);

	childSetAction("Find", onBtnFind, this);
	childDisable("Find");
	childSetAction("Refresh", onBtnRefresh, this);
	childSetCommitCallback("near_me_range", onRangeAdjust, this);

	childSetDoubleClickCallback("SearchResults", onBtnSelect);
	childSetDoubleClickCallback("NearMe", onBtnSelect);
	childSetCommitCallback("SearchResults", onList, this);
	childSetCommitCallback("NearMe", onList, this);
	childDisable("SearchResults");

	childSetAction("Select", onBtnSelect, this);
	childDisable("Select");

	childSetAction("Cancel", onBtnClose, this);

	childSetFocus("Edit");

	LLPanel* search_panel = getChild<LLPanel>("SearchPanel");
	if (search_panel)
	{
		// Start searching when Return is pressed in the line editor.
		search_panel->setDefaultBtn("Find");
	}

	getChild<LLScrollListCtrl>("SearchResults")->addCommentText(getString("no_results"));

	LLInventoryPanel* inventory_panel = getChild<LLInventoryPanel>("InventoryPanel");
	inventory_panel->setFilterTypes(0x1 << LLInventoryType::IT_CALLINGCARD);
	inventory_panel->setFollowsAll();
	inventory_panel->setShowFolderState(LLInventoryFilter::SHOW_NON_EMPTY_FOLDERS);
	inventory_panel->openDefaultFolderForType(LLAssetType::AT_CALLINGCARD);
	inventory_panel->setSelectCallback(LLFloaterAvatarPicker::onCallingCardSelectionChange, this);

	childSetTabChangeCallback("ResidentChooserTabs", "SearchPanel",			onTabChanged, this);
	childSetTabChangeCallback("ResidentChooserTabs", "CallingCardsPanel",	onTabChanged, this);
	childSetTabChangeCallback("ResidentChooserTabs", "NearMePanel",			onTabChanged, this);
	childSetTabChangeCallback("ResidentChooserTabs", "KeyPanel",			onTabChanged, this);
	setAllowMultiple(FALSE);

	return TRUE;
}

void LLFloaterAvatarPicker::onTabChanged(void* userdata, bool from_click)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)userdata;
	if (!self)
	{
		return;
	}
	
	self->childSetEnabled("Select", self->visibleItemsSelected());
}

// Destroys the object
LLFloaterAvatarPicker::~LLFloaterAvatarPicker()
{
	gFocusMgr.releaseFocusIfNeeded( this );

	sInstance = NULL;
}

void LLFloaterAvatarPicker::onBtnFind(void* userdata)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)userdata;
	if(self) self->find();
}

static void getSelectedAvatarData(const LLScrollListCtrl* from, std::vector<std::string>& avatar_names, std::vector<LLUUID>& avatar_ids)
{
	std::vector<LLScrollListItem*> items = from->getAllSelected();
	for (std::vector<LLScrollListItem*>::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		LLScrollListItem* item = *iter;
		if (item->getUUID().notNull())
		{
			avatar_names.push_back(item->getColumn(0)->getValue().asString());
			avatar_ids.push_back(item->getUUID());
		}
	}
}

void LLFloaterAvatarPicker::onBtnSelect(void* userdata)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)userdata;

	if(self->mCallback)
	{
		LLPanel* active_panel = self->childGetVisibleTab("ResidentChooserTabs");

		if(active_panel == self->getChild<LLPanel>("CallingCardsPanel"))
		{
			self->mCallback(self->mSelectedInventoryAvatarNames, self->mSelectedInventoryAvatarIDs, self->mCallbackUserdata);
		}
		else if(active_panel == self->getChild<LLPanel>("SearchPanel"))
		{
			std::vector<std::string>	avatar_names;
			std::vector<LLUUID>			avatar_ids;
			getSelectedAvatarData(self->getChild<LLScrollListCtrl>("SearchResults"), avatar_names, avatar_ids);
			self->mCallback(avatar_names, avatar_ids, self->mCallbackUserdata);
		}
		else if(active_panel == self->getChild<LLPanel>("NearMePanel"))
		{
			std::vector<std::string>	avatar_names;
			std::vector<LLUUID>			avatar_ids;
			getSelectedAvatarData(self->getChild<LLScrollListCtrl>("NearMe"), avatar_names, avatar_ids);
			self->mCallback(avatar_names, avatar_ids, self->mCallbackUserdata);
		}
		else if(active_panel == self->getChild<LLPanel>("KeyPanel"))
		{
			LLUUID specified = self->getChild<LLLineEditor>("EditUUID")->getValue().asUUID();
			if(specified.isNull())
				return;
			std::vector<std::string>	avatar_names;
			std::vector<LLUUID>			avatar_ids;
			avatar_ids.push_back(specified);
			avatar_names.push_back(specified.asString());
			self->mCallback(avatar_names, avatar_ids, self->mCallbackUserdata);
		}
	}
	self->getChild<LLInventoryPanel>("InventoryPanel")->setSelection(LLUUID::null, FALSE);
	self->getChild<LLScrollListCtrl>("SearchResults")->deselectAllItems(TRUE);
	self->getChild<LLScrollListCtrl>("NearMe")->deselectAllItems(TRUE);
	if(self->mCloseOnSelect)
	{
		self->mCloseOnSelect = FALSE;
		self->close();		
	}
}

void LLFloaterAvatarPicker::onBtnRefresh(void* userdata)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)userdata;
	if (!self)
	{
		return;
	}
	
	self->getChild<LLScrollListCtrl>("NearMe")->deleteAllItems();
	self->getChild<LLScrollListCtrl>("NearMe")->addCommentText(self->getString("searching"));
	self->mNearMeListComplete = FALSE;
}

void LLFloaterAvatarPicker::onBtnClose(void* userdata)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)userdata;
	if(self) self->close();
}

void LLFloaterAvatarPicker::onRangeAdjust(LLUICtrl* source, void* data)
{
	LLFloaterAvatarPicker::onBtnRefresh(data);
}

void LLFloaterAvatarPicker::onList(LLUICtrl* ctrl, void* userdata)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)userdata;
	if (self)
	{
		self->childSetEnabled("Select", self->visibleItemsSelected());
	}
}

// static callback for inventory picker (select from calling cards)
void LLFloaterAvatarPicker::onCallingCardSelectionChange(const std::deque<LLFolderViewItem*> &items, BOOL user_action, void* data)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)data;
	if (self)
	{
		self->doCallingCardSelectionChange( items, user_action, data );
	}
}

// Callback for inventory picker (select from calling cards)
void LLFloaterAvatarPicker::doCallingCardSelectionChange(const std::deque<LLFolderViewItem*> &items, BOOL user_action, void* data)
{
	bool panel_active = (childGetVisibleTab("ResidentChooserTabs") == getChild<LLPanel>("CallingCardsPanel"));
	
	mSelectedInventoryAvatarIDs.clear();
	mSelectedInventoryAvatarNames.clear();
	
	if (panel_active)
	{
		childSetEnabled("Select", FALSE);
	}

	std::deque<LLFolderViewItem*>::const_iterator item_it;
	for (item_it = items.begin(); item_it != items.end(); ++item_it)
	{
		LLFolderViewEventListener* listenerp = (*item_it)->getListener();
		if (listenerp->getInventoryType() == LLInventoryType::IT_CALLINGCARD)
		{
			LLInventoryItem* item = gInventory.getItem(listenerp->getUUID());
			if (item)
			{
				mSelectedInventoryAvatarIDs.push_back(item->getCreatorUUID());
				mSelectedInventoryAvatarNames.push_back(listenerp->getName());
			}
		}
	}

	if (panel_active)
	{
		childSetEnabled("Select", visibleItemsSelected());
	}
}

void LLFloaterAvatarPicker::populateNearMe()
{
	BOOL all_loaded = TRUE;
	BOOL empty = TRUE;
	LLScrollListCtrl* near_me_scroller = getChild<LLScrollListCtrl>("NearMe");
	near_me_scroller->deleteAllItems();

	std::vector<LLUUID> avatar_ids;
	LLWorld::getInstance()->getAvatars(&avatar_ids, NULL, gAgent.getPositionGlobal(), gSavedSettings.getF32("NearMeRange"));
	for(U32 i=0; i<avatar_ids.size(); i++)
	{
		LLUUID& av = avatar_ids[i];
		if(av == gAgent.getID()) continue;
		LLSD element;
		element["id"] = av; // value
		std::string fullname;
		if(!gCacheName->getFullName(av, fullname))
		{
			element["columns"][0]["value"] = LLCacheName::getDefaultName();
			all_loaded = FALSE;
		}			
		else
		{
			element["columns"][0]["value"] = fullname;
		}
		near_me_scroller->addElement(element);
		empty = FALSE;
	}

	if (empty)
	{
		childDisable("NearMe");
		childDisable("Select");
		near_me_scroller->addCommentText(getString("no_one_near"));
	}
	else 
	{
		childEnable("NearMe");
		childEnable("Select");
		near_me_scroller->selectFirstItem();
		onList(near_me_scroller, this);
		near_me_scroller->setFocus(TRUE);
	}

	if (all_loaded)
	{
		mNearMeListComplete = TRUE;
	}
}

void LLFloaterAvatarPicker::draw()
{
	LLFloater::draw();
	if (!mNearMeListComplete && childGetVisibleTab("ResidentChooserTabs") == getChild<LLPanel>("NearMePanel"))
	{
		populateNearMe();
	}
}

BOOL LLFloaterAvatarPicker::visibleItemsSelected() const
{
	LLPanel* active_panel = childGetVisibleTab("ResidentChooserTabs");

	if(active_panel == getChild<LLPanel>("SearchPanel"))
	{
		return getChild<LLScrollListCtrl>("SearchResults")->getFirstSelectedIndex() >= 0;
	}
	else if(active_panel == getChild<LLPanel>("CallingCardsPanel"))
	{
		return mSelectedInventoryAvatarIDs.size() > 0;
	}
	else if(active_panel == getChild<LLPanel>("NearMePanel"))
	{
		return getChild<LLScrollListCtrl>("NearMe")->getFirstSelectedIndex() >= 0;
	}
	return FALSE;
}

void LLFloaterAvatarPicker::find()
{
	const std::string& text = childGetValue("Edit").asString();

	mQueryID.generate();

	LLMessageSystem* msg = gMessageSystem;

	msg->newMessage("AvatarPickerRequest");
	msg->nextBlock("AgentData");
	msg->addUUID("AgentID", gAgent.getID());
	msg->addUUID("SessionID", gAgent.getSessionID());
	msg->addUUID("QueryID", mQueryID);	// not used right now
	msg->nextBlock("Data");
	msg->addString("Name", text);

	gAgent.sendReliableMessage();

	getChild<LLScrollListCtrl>("SearchResults")->deleteAllItems();
	getChild<LLScrollListCtrl>("SearchResults")->addCommentText(getString("searching"));
	
	childSetEnabled("Select", FALSE);
	mResultsReturned = FALSE;
}

void LLFloaterAvatarPicker::setAllowMultiple(BOOL allow_multiple)
{
	getChild<LLScrollListCtrl>("SearchResults")->setAllowMultipleSelection(allow_multiple);
	getChild<LLInventoryPanel>("InventoryPanel")->setAllowMultiSelect(allow_multiple);
	getChild<LLScrollListCtrl>("NearMe")->setAllowMultipleSelection(allow_multiple);
}

// static 
void LLFloaterAvatarPicker::processAvatarPickerReply(LLMessageSystem* msg, void**)
{
	LLUUID	agent_id;
	LLUUID	query_id;
	LLUUID	avatar_id;
	std::string first_name;
	std::string last_name;

	msg->getUUID("AgentData", "AgentID", agent_id);
	msg->getUUID("AgentData", "QueryID", query_id);

	// Not for us
	if (agent_id != gAgent.getID()) return;

	// Dialog already closed
	LLFloaterAvatarPicker *self = sInstance;
	if (!self) return;

	// these are not results from our last request
	if (query_id != self->mQueryID)
	{
		return;
	}

	LLScrollListCtrl* search_results = self->getChild<LLScrollListCtrl>("SearchResults");

	// clear "Searching" label on first results
	search_results->deleteAllItems();

	self->mResultsReturned = TRUE;

	BOOL found_one = FALSE;
	S32 num_new_rows = msg->getNumberOfBlocks("Data");
	for (S32 i = 0; i < num_new_rows; i++)
	{			
		msg->getUUIDFast(  _PREHASH_Data,_PREHASH_AvatarID,	avatar_id, i);
		msg->getStringFast(_PREHASH_Data,_PREHASH_FirstName, first_name, i);
		msg->getStringFast(_PREHASH_Data,_PREHASH_LastName,	last_name, i);
	
		std::string avatar_name;
		if (avatar_id.isNull())
		{
			LLStringUtil::format_map_t map;
			map["[TEXT]"] = self->childGetText("Edit");
			avatar_name = self->getString("not_found", map);
			search_results->setEnabled(FALSE);
			self->childDisable("Select");
		}
		else
		{
			avatar_name = first_name + " " + last_name;
			search_results->setEnabled(TRUE);
			found_one = TRUE;
		}
		LLSD element;
		element["id"] = avatar_id; // value
		element["columns"][0]["value"] = avatar_name;
		search_results->addElement(element);
	}

	if (found_one)
	{
		self->childEnable("Select");
		search_results->selectFirstItem();
		self->onList(search_results, self);
		search_results->setFocus(TRUE);
	}
}

//static
void LLFloaterAvatarPicker::editKeystroke(LLLineEditor* caller, void* user_data)
{
	LLFloaterAvatarPicker* self = (LLFloaterAvatarPicker*)user_data;
	LLPanel* active_panel = self->childGetVisibleTab("ResidentChooserTabs");
	if(active_panel == self->getChild<LLPanel>("SearchPanel"))
		self->childSetEnabled("Find", caller->getText().size() >= 3);
	else if(active_panel == self->getChild<LLPanel>("KeyPanel"))
	{
		LLUUID specified = self->getChild<LLLineEditor>("EditUUID")->getValue().asUUID();
		self->childSetEnabled("Select", specified.notNull());
	}
}

// virtual
BOOL LLFloaterAvatarPicker::handleKeyHere(KEY key, MASK mask)
{
	if (key == KEY_RETURN && mask == MASK_NONE)
	{
		if (childHasFocus("Edit"))
		{
			onBtnFind(this);
		}
		else
		{
			onBtnSelect(this);
		}
		return TRUE;
	}
	else if (key == KEY_ESCAPE && mask == MASK_NONE)
	{
		close();
		return TRUE;
	}

	return LLFloater::handleKeyHere(key, mask);
}
