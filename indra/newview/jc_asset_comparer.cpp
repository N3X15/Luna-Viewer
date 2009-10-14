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
