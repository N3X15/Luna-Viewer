/** 
 * @file llfloaterinspect.cpp
 * @brief Floater for object inspection tool
 *
 * $LicenseInfo:firstyear=2006&license=viewergpl$
 * 
 * Copyright (c) 2006-2009, Linden Research, Inc.
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
#include "llfloateravatarinfo.h"
#include "llfloaterinspect.h"
#include "llfloatertools.h"
#include "llcachename.h"
#include "llscrolllistctrl.h"
#include "llselectmgr.h"
#include "lltoolcomp.h"
#include "lltoolmgr.h"
#include "llviewercontrol.h"
#include "llviewerobject.h"
#include "llviewerimagelist.h"
#include "lluictrlfactory.h"
#include "lltexturectrl.h"
#include "llviewerpartsource.h"
#include "floaterblacklist.h"
#include "llagent.h"


struct struct_tex
{
	LLTextureCtrl* ctrl;
	LLRect texpanel_rect;
	LLPanel* texpanel;
	LLTextBox* line1;
	LLTextBox* line2;
	LLTextBox* line3;
	LLButton* blacklistbtn;
	LLButton* profilebtn;
	LLUUID uploaderkey;
	LLUUID assetid;
	std::string color;
	std::string time;
};
static std::vector<struct_tex> mTex;

LLFloaterInspect* LLFloaterInspect::sInstance = NULL;

LLFloaterInspect::LLFloaterInspect(void) :
	LLFloater(std::string("Inspect Object")),
	mDirty(FALSE)
{
	sInstance = this;
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_inspect.xml");
}

LLFloaterInspect::~LLFloaterInspect(void)
{
	if(!gFloaterTools->getVisible())
	{
		if(LLToolMgr::getInstance()->getBaseTool() == LLToolCompInspect::getInstance())
		{
			LLToolMgr::getInstance()->clearTransientTool();
		}
		// Switch back to basic toolset
		LLToolMgr::getInstance()->setCurrentToolset(gBasicToolset);	
	}
	else
	{
		gFloaterTools->setFocus(TRUE);
	}
	sInstance = NULL;
}

BOOL LLFloaterInspect::isVisible()
{
	return (!!sInstance);
}

void LLFloaterInspect::show(void* ignored)
{
	// setForceSelection ensures that the pie menu does not deselect things when it 
	// looses the focus (this can happen with "select own objects only" enabled
	// VWR-1471
	BOOL forcesel = LLSelectMgr::getInstance()->setForceSelection(TRUE);

	if (!sInstance)	// first use
	{
		sInstance = new LLFloaterInspect;
	}

	sInstance->open();
	LLToolMgr::getInstance()->setTransientTool(LLToolCompInspect::getInstance());
	LLSelectMgr::getInstance()->setForceSelection(forcesel);	// restore previouis value

	sInstance->mObjectSelection = LLSelectMgr::getInstance()->getSelection();
	sInstance->refresh();
}

void LLFloaterInspect::onClickCreatorProfile(void* ctrl)
{
	if(sInstance->mObjectList->getAllSelected().size() == 0)
	{
		return;
	}
	LLScrollListItem* first_selected =
		sInstance->mObjectList->getFirstSelected();

	if (first_selected)
	{
		struct f : public LLSelectedNodeFunctor
		{
			LLUUID obj_id;
			f(const LLUUID& id) : obj_id(id) {}
			virtual bool apply(LLSelectNode* node)
			{
				return (obj_id == node->getObject()->getID());
			}
		} func(first_selected->getUUID());
		LLSelectNode* node = sInstance->mObjectSelection->getFirstNode(&func);
		if(node)
		{
			LLFloaterAvatarInfo::showFromDirectory(node->mPermissions->getCreator());
		}
	}
}

void LLFloaterInspect::onClickOwnerProfile(void* ctrl)
{
	if(sInstance->mObjectList->getAllSelected().size() == 0) return;
	LLScrollListItem* first_selected =
		sInstance->mObjectList->getFirstSelected();

	if (first_selected)
	{
		LLUUID selected_id = first_selected->getUUID();
		struct f : public LLSelectedNodeFunctor
		{
			LLUUID obj_id;
			f(const LLUUID& id) : obj_id(id) {}
			virtual bool apply(LLSelectNode* node)
			{
				return (obj_id == node->getObject()->getID());
			}
		} func(selected_id);
		LLSelectNode* node = sInstance->mObjectSelection->getFirstNode(&func);
		if(node)
		{
			const LLUUID& owner_id = node->mPermissions->getOwner();
			LLFloaterAvatarInfo::showFromDirectory(owner_id);
		}
	}
}

BOOL LLFloaterInspect::postBuild()
{
	mObjectList = getChild<LLScrollListCtrl>("object_list");
	childSetAction("button owner",onClickOwnerProfile, this);
	childSetAction("button creator",onClickCreatorProfile, this);
	childSetCommitCallback("object_list", onSelectObject);
	return TRUE;
}

void LLFloaterInspect::onSelectObject(LLUICtrl* ctrl, void* user_data)
{
	if(LLFloaterInspect::getSelectedUUID() != LLUUID::null)
	{
		sInstance->childSetEnabled("button owner", true);
		sInstance->childSetEnabled("button creator", true);
	}

	mTex.clear();
	mTex.empty();

	for (U32 i=0; i < 10; i++)
	{
		sInstance->getChild<LLTextureCtrl>(llformat("tex%d",i))->setImageAssetID(LLUUID::null);
		sInstance->getChild<LLTextureCtrl>(llformat("tex%d",i))->setEnabled(FALSE);
//		sInstance->getChild<LLTextureCtrl>(llformat("tex%d",i))->setIsMasked(TRUE);
		sInstance->childSetText(llformat("line1_%d",i),std::string(""));
		sInstance->childSetText(llformat("line2_%d",i),std::string(""));
		sInstance->childSetText(llformat("line3_%d",i),std::string(""));
		sInstance->childSetVisible(llformat("texpanel%d",i), FALSE);
	}
	sInstance->childSetText("line1_s",std::string(""));
	sInstance->childSetText("line2_s",std::string(""));
	sInstance->childSetText("line3_s",std::string(""));

	sInstance->childSetText("line1_p",std::string(""));
	sInstance->childSetText("line2_p",std::string(""));
	sInstance->childSetText("line3_p",std::string(""));

	sInstance->getChild<LLButton>("blacklist_btn_s")->setVisible(FALSE);
	sInstance->getChild<LLButton>("blacklist_btn_p")->setVisible(FALSE);
	sInstance->getChild<LLButton>("profile_btn_s")->setVisible(FALSE);
	sInstance->getChild<LLButton>("profile_btn_p")->setVisible(FALSE);

	////////////////////
	if(sInstance->mObjectList->getAllSelected().size() == 0) return;

	LLScrollListItem* first_selected = sInstance->mObjectList->getFirstSelected();

	if (first_selected)
	{
		struct f : public LLSelectedNodeFunctor
		{
			LLUUID obj_id;
			f(const LLUUID& id) : obj_id(id) {}
			virtual bool apply(LLSelectNode* node)
			{
				return (obj_id == node->getObject()->getID());
			}
		} func(first_selected->getUUID());
		LLSelectNode* node = sInstance->mObjectSelection->getFirstNode(&func);
		if(node)
		{
			U8 te_count = node->getObject()->getNumTEs();
			// map from texture ID to list of faces using it
			typedef std::map< LLUUID, std::vector<U8> > map_t;
			map_t faces_per_texture;

			//std::map<LLUUID, bool> unique_textures;

			int j = 0;
			
			for (U8 i = 0; i < te_count; i++)
			{
				struct_tex loader_tex;
				loader_tex.texpanel = sInstance->getChild<LLPanel>(llformat("texpanel%d",j));
				loader_tex.texpanel->setVisible(TRUE);
				loader_tex.ctrl = sInstance->getChild<LLTextureCtrl>(llformat("tex%d",j));
				loader_tex.ctrl->setEnabled(FALSE);
//				loader_tex.ctrl->setIsMasked(TRUE);

//				sInstance->childGetRect(llformat("texpanel%d",j), loader_tex.texpanel_rect);
				loader_tex.line1 = sInstance->getChild<LLTextBox>(llformat("line1_%d",j));
				loader_tex.line2 = sInstance->getChild<LLTextBox>(llformat("line2_%d",j));
				loader_tex.line3 = sInstance->getChild<LLTextBox>(llformat("line3_%d",j));
				loader_tex.uploaderkey = LLUUID::null;
//				loader_tex.time == "";

				loader_tex.blacklistbtn = sInstance->getChild<LLButton>(llformat("blacklist_btn_%d",j));
				sInstance->childSetAction(llformat("blacklist_btn_%d",j), onClickBlacklist,(void*)j);
				loader_tex.profilebtn = sInstance->getChild<LLButton>(llformat("profile_btn_%d",j));
				sInstance->childSetAction(llformat("profile_btn_%d",j), onClickProfile,(void*)j);

				LLViewerImage* img = node->getObject()->getTEImage(i);
				loader_tex.assetid = img->getID();


				mTex.push_back(loader_tex);
				drawTextureEntry(img,i);

				j++;
			}
			int i=te_count;
			if(node->getObject()->isSculpted())
			{
				struct_tex loader_tex;
				LLSculptParams *sculpt_params = (LLSculptParams *)(node->getObject()->getParameterEntry(LLNetworkData::PARAMS_SCULPT));
				LLUUID sculpt_id = sculpt_params->getSculptTexture();

				loader_tex.texpanel = sInstance->getChild<LLPanel>("texpanels");
				//loader_tex.texpanel->setVisible(TRUE);
				loader_tex.ctrl = sInstance->getChild<LLTextureCtrl>("texs");
				loader_tex.ctrl->setEnabled(FALSE);
//				loader_tex.ctrl->setIsMasked(TRUE);

//				sInstance->childGetRect("texpanels", loader_tex.texpanel_rect);
				loader_tex.line1 = sInstance->getChild<LLTextBox>("line1_s");
				loader_tex.line2 = sInstance->getChild<LLTextBox>("line2_s");
				loader_tex.line3 = sInstance->getChild<LLTextBox>("line3_s");

				loader_tex.blacklistbtn = sInstance->getChild<LLButton>("blacklist_btn_s");
				sInstance->childSetAction("blacklist_btn_s", onClickBlacklist,(void*)i);

				loader_tex.profilebtn = sInstance->getChild<LLButton>("profile_btn_s");
				sInstance->childSetAction("profile_btn_s", onClickProfile,(void*)i);

				LLViewerImage* img = gImageList.getImage(sculpt_id);
				loader_tex.assetid = sculpt_id;

				mTex.push_back(loader_tex);
				if (img) drawTextureEntry(img,i);
				i++;
			}
			else
			{
				sInstance->getChild<LLTextureCtrl>("texs")->setImageAssetID(LLUUID::null);
				sInstance->getChild<LLTextureCtrl>("texs")->setEnabled(FALSE);
				sInstance->childSetText("line1_s",std::string(""));
				sInstance->childSetText("line2_s",std::string(""));
				sInstance->childSetText("line3_s",std::string(""));
			}

			if(node->getObject()->isParticleSource())
			{
				struct_tex loader_tex;

				loader_tex.texpanel = sInstance->getChild<LLPanel>("texpanels");
				//loader_tex.texpanel->setVisible(TRUE);
				loader_tex.ctrl = sInstance->getChild<LLTextureCtrl>("texp");
				loader_tex.ctrl->setEnabled(FALSE);
//				loader_tex.ctrl->setIsMasked(TRUE);

//				sInstance->childGetRect("texpanelp", loader_tex.texpanel_rect);
				loader_tex.line1 = sInstance->getChild<LLTextBox>("line1_p");
				loader_tex.line2 = sInstance->getChild<LLTextBox>("line2_p");
				loader_tex.line3 = sInstance->getChild<LLTextBox>("line3_p");
				//mTex.push_back(loader_tex);

				loader_tex.blacklistbtn = sInstance->getChild<LLButton>("blacklist_btn_p");
				sInstance->childSetAction("blacklist_btn_p", onClickBlacklist,(void*)i);

				loader_tex.profilebtn = sInstance->getChild<LLButton>("profile_btn_p");
				sInstance->childSetAction("profile_btn_p", onClickProfile,(void*)i);

				LLViewerImage* img = node->getObject()->mPartSourcep->getImage();
				loader_tex.assetid = img->getID();

				mTex.push_back(loader_tex);
				if (img) drawTextureEntry(img,i);
			}
			else
			{
				sInstance->getChild<LLTextureCtrl>("texp")->setImageAssetID(LLUUID::null);
				sInstance->getChild<LLTextureCtrl>("texp")->setEnabled(FALSE);
				sInstance->childSetText("line1_p",std::string(""));
				sInstance->childSetText("line2_p",std::string(""));
				sInstance->childSetText("line3_p",std::string(""));
			}

		}
	}
}

void LLFloaterInspect::drawTextureEntry(const LLViewerImage* img, const U8 i)
{
//	if (i > (U8)6) return; //fffffffffffffff
	LLUUID image_id = img->getID();
	if (image_id.notNull())
	{
		mTex[i].blacklistbtn->setVisible(TRUE);
		if (mTex[i].ctrl)
		{
			std::map<std::string,std::string> decodedComment = img->decodedComment;
			if (decodedComment.find("a")!=decodedComment.end())
			{
				mTex[i].uploaderkey = LLUUID(decodedComment["a"]);
				gCacheName->get(mTex[i].uploaderkey, FALSE, callbackLoadAvatarName, mTex[i].line2);
				if (decodedComment.find("z")!=decodedComment.end()) 
				{
					std::string strtime= decodedComment["z"];
					std::string year = strtime.substr(0,4);
					std::string month = strtime.substr(4,2);
					std::string day = strtime.substr(6,2);
					std::string hour = strtime.substr(8,2);
					std::string minute = strtime.substr(10,2);
					std::string second = strtime.substr(12,2);
					mTex[i].time = llformat("%s/%s/%s - %s:%s:%s",year.c_str(),month.c_str(),day.c_str(),hour.c_str(),minute.c_str(),second.c_str());
//					mTex[i].blacklistbtn->setVisible(TRUE);
					mTex[i].profilebtn->setVisible(TRUE);
				}
				else
				{
					mTex[i].time = std::string("");
//					mTex[i].blacklistbtn->setVisible(FALSE);
					mTex[i].profilebtn->setVisible(FALSE);
				}
				mTex[i].line3->setText(std::string(mTex[i].time));
			}
			else
			{
//				mTex[i].blacklistbtn->setVisible(FALSE);
				mTex[i].profilebtn->setVisible(FALSE);
			}

			mTex[i].ctrl->setImageAssetID(image_id);

			S32 height = img->getHeight();
			S32 width = img->getWidth();
			S32 components = img->getComponents();
			S32 total_memory = (img->getWidth() * img->getHeight() * img->getComponents());
			F32 mb_memory = (F32)total_memory;
			mb_memory = mb_memory/1000000;

			mTex[i].line1->setText(llformat("%dx%d %s, %3.5f MB",width,height,(components == 4 ? "alpha" : "opaque"),mb_memory));
			//mTex[i].ctrl->setToolTip(msg);
		}
	}
}

// static
void LLFloaterInspect::onClickBlacklist(void* user_data)
{
	llinfos << (intptr_t) user_data << llendl;
	intptr_t j = 0;
	if ((intptr_t) user_data <= 12) j = (intptr_t) user_data;

	LLFloaterBlacklist::show();
	std::string entry_name, owner_name;
	LLSelectNode* sel_node = sInstance->mObjectSelection->getFirstRootNode();
	if(sel_node)
	  entry_name = sel_node->mName.c_str();
	else
	  entry_name = sInstance->mObjectSelection->getFirstNode()->mName.c_str();
	gCacheName->getFullName(sInstance->mObjectSelection->getFirstNode()->mPermissions->getOwner(), owner_name);
	if (entry_name.empty()) entry_name = std::string("(unknown)");
	if (owner_name.empty()) entry_name = std::string("(unknown)");

	entry_name = llformat("Texture from object '%s' owned by %s",entry_name.c_str(),owner_name.c_str());

	if (!mTex[j].assetid.isNull())
	{
		LLSD indata;
		indata["entry_type"] = 0; //AT_TEXTURE
		indata["entry_name"] = entry_name;
		indata["entry_agent"] = gAgent.getID();

		LLFloaterBlacklist::addEntry(mTex[j].assetid,indata);
	}
}

// static
void LLFloaterInspect::onClickProfile(void* user_data)
{
//	llinfos << (int) user_data << llendl;
	intptr_t j = 0;
	if ((intptr_t) user_data <= 12) j = (intptr_t) user_data;
	if (!mTex[j].uploaderkey.isNull())
	{
		LLFloaterAvatarInfo::showFromDirectory(mTex[j].uploaderkey);
	}
	
}

void LLFloaterInspect::callbackLoadAvatarName(const LLUUID& id, const std::string& first, const std::string& last, BOOL is_group, void* data)
{
	if(!sInstance) return;

	std::ostringstream fullname;
	fullname << "Uploaded by " << first << " " << last;
	LLTextBox* line = (LLTextBox*) data;
	line->setText(fullname.str());
}

LLUUID LLFloaterInspect::getSelectedUUID()
{
	if(sInstance)
	{
		if(sInstance->mObjectList->getAllSelected().size() > 0)
		{
			LLScrollListItem* first_selected =
				sInstance->mObjectList->getFirstSelected();
			if (first_selected)
			{
				return first_selected->getUUID();
			}
		}
	}
	return LLUUID::null;
}

void LLFloaterInspect::refresh()
{
	LLUUID creator_id;
	std::string creator_name;
	S32 pos = mObjectList->getScrollPos();
	childSetEnabled("button owner", false);
	childSetEnabled("button creator", false);
	LLUUID selected_uuid;
	S32 selected_index = mObjectList->getFirstSelectedIndex();
	if(selected_index > -1)
	{
		LLScrollListItem* first_selected = mObjectList->getFirstSelected();
		if (first_selected)
		{
			selected_uuid = first_selected->getUUID();
		}
	}
	mObjectList->operateOnAll(LLScrollListCtrl::OP_DELETE);
	//List all transient objects, then all linked objects


	for (LLObjectSelection::valid_iterator iter = mObjectSelection->valid_begin();
		 iter != mObjectSelection->valid_end(); iter++)
	{
		LLSelectNode* obj = *iter;
		LLSD row;
		char time[MAX_STRING];
		std::string owner_name, creator_name, last_owner_name;

		if (obj->mCreationDate == 0)
		{	// Don't have valid information from the server, so skip this one
			continue;
		}

		time_t timestamp = (time_t) (obj->mCreationDate/1000000);
		LLStringUtil::copy(time, ctime(&timestamp), MAX_STRING);
		time[24] = '\0';
		gCacheName->getFullName(obj->mPermissions->getOwner(), owner_name);
		gCacheName->getFullName(obj->mPermissions->getCreator(), creator_name);
		gCacheName->getFullName(obj->mPermissions->getLastOwner(), last_owner_name);

		row["id"] = obj->getObject()->getID();
		row["columns"][0]["column"] = "object_name";
		row["columns"][0]["type"] = "text";
		// make sure we're either at the top of the link chain
		// or top of the editable chain, for attachments
		if(!(obj->getObject()->isRoot() || obj->getObject()->isRootEdit()))
		{
			row["columns"][0]["value"] = std::string("   ") + obj->mName;
		}
		else
		{
			row["columns"][0]["value"] = obj->mName;
		}
		int i = 1;
		row["columns"][i]["column"] = "owner_name";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = owner_name;
		++i;
		row["columns"][i]["column"] = "last_owner_name";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = last_owner_name;
		++i;
		row["columns"][i]["column"] = "creator_name";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = creator_name;
		++i;
		row["columns"][i]["column"] = "face_num";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = llformat("%d",obj->getObject()->getNumFaces());
		++i;
		row["columns"][i]["column"] = "vertex_num";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = llformat("%d",obj->getObject()->getNumVertices());
		++i;
		S32 scripts,total_inv;
		std::map<LLUUID,std::pair<S32,S32> >::iterator itr = mInventoryNums.find(obj->getObject()->getID());
		if(itr != mInventoryNums.end())
		{
			scripts = itr->second.first;
			total_inv = itr->second.second;
		}
		else
		{
			scripts = 0;
			total_inv = 0;
			if(std::find(mQueue.begin(),mQueue.end(),obj->getObject()->getID()) == mQueue.end())
			{
				mQueue.push_back(obj->getObject()->getID());
				registerVOInventoryListener(obj->getObject(),NULL);
				requestVOInventory();
			}
		}
		row["columns"][i]["column"] = "script_num";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = llformat("%d",scripts);
		++i;
		row["columns"][i]["column"] = "inv_num";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = llformat("%d",total_inv);
		++i;
		row["columns"][i]["column"] = "creation_date";
		row["columns"][i]["type"] = "text";
		row["columns"][i]["value"] = time;
		mObjectList->addElement(row, ADD_TOP);
	}
	if(selected_index > -1 && mObjectList->getItemIndex(selected_uuid) == selected_index)
	{
		mObjectList->selectNthItem(selected_index);
	}
	else
	{
		mObjectList->selectNthItem(0);
	}
	onSelectObject(this, NULL);
	mObjectList->setScrollPos(pos);
}
void LLFloaterInspect::inventoryChanged(LLViewerObject* viewer_object,
											 InventoryObjectList* inv,
											 S32,
											 void* q_id)
{
	S32 scripts = 0;
	std::vector<LLUUID>::iterator iter = std::find(mQueue.begin(),mQueue.end(),viewer_object->getID());
	if (viewer_object && inv && iter != mQueue.end() )
	{
		InventoryObjectList::const_iterator it = inv->begin();
		InventoryObjectList::const_iterator end = inv->end();
		for ( ; it != end; ++it)
		{
			if((*it)->getType() == LLAssetType::AT_LSL_TEXT)
			{
				scripts++;
			}
		}
		mInventoryNums[viewer_object->getID()] = std::make_pair(scripts,inv->size());
		mQueue.erase(iter);
		mDirty = TRUE;
	}
}

void LLFloaterInspect::onFocusReceived()
{
	LLToolMgr::getInstance()->setTransientTool(LLToolCompInspect::getInstance());
	LLFloater::onFocusReceived();
}

void LLFloaterInspect::dirty()
{
	if(sInstance)
	{
		sInstance->mInventoryNums.clear();
		sInstance->mQueue.clear();
		sInstance->setDirty();
	}
}

void LLFloaterInspect::updateDimensions()
{
	if(sInstance)
	{
		LLRect floater_rect;
		childGetRect("inspect", floater_rect);

		int i = 0;	
		for (std::vector<struct_tex>::iterator iter = mTex.begin(); iter != mTex.end(); ++iter)
		{
			if ((iter->texpanel->getName() == std::string("texpanels"))||(iter->texpanel->getName() == std::string("texpanelp"))) return;
//			llinfos << "resize " << iter->texpanel_rect.mTop << " < " << floater_rect.mBottom << llendl;
			sInstance->childGetRect(llformat("texpanel%d",i), iter->texpanel_rect);
			iter->texpanel->setVisible(iter->texpanel_rect.mTop > floater_rect.mBottom+100);
			//SetVisible(llformat("texpanel%d",i),iter->texpanel_rect.mTop > floater_rect.mBottom+100);
			i++;
		}
	}
}


void LLFloaterInspect::draw()
{
	if (mDirty)
	{
		refresh();
		mDirty = FALSE;
	}
	updateDimensions();
	LLFloater::draw();
}
