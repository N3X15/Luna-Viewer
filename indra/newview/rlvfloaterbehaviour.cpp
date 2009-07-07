#include "llviewerprecompiledheaders.h"

#include "llagent.h"
#include "llscrolllistctrl.h"
#include "lluictrlfactory.h"
#include "llviewerinventory.h"
#include "llviewerobjectlist.h"
#include "llvoavatar.h"

#include "rlvfloaterbehaviour.h"

// ============================================================================

RlvFloaterBehaviour::RlvFloaterBehaviour(const LLSD& key) 
	: LLFloater(std::string("rlvBehaviours"))
{
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_rlv_behaviour.xml");
}


void RlvFloaterBehaviour::show(void*)
{
	RlvFloaterBehaviour::showInstance();
}

void RlvFloaterBehaviour::refreshAll()
{
	LLVOAvatar* pAvatar = gAgent.getAvatarObject();
	LLCtrlListInterface* pList = childGetListInterface("behaviour_list");
	const rlv_object_map_t* pRlvObjects = gRlvHandler.getObjectMap();
	if ( (!pAvatar) || (!pList) || (!pRlvObjects) )
		return;

	pList->operateOnAll(LLCtrlListInterface::OP_DELETE);

	for (rlv_object_map_t::const_iterator itObj = pRlvObjects->begin(), endObj = pRlvObjects->end(); itObj != endObj; ++itObj)
	{
		std::string strName = itObj->first.asString();

		LLViewerInventoryItem* pItem = NULL;
		LLViewerObject* pObj = gObjectList.findObject(itObj->first);
		if (pObj)
		{
			LLViewerJointAttachment* pAttachPt = 
				get_if_there(pAvatar->mAttachmentPoints, gRlvHandler.getAttachPointIndex(pObj), (LLViewerJointAttachment*)NULL);
			if (pAttachPt)
			{
				pItem = gInventory.getItem(pAttachPt->getItemID());
			}
		}

		if (pItem)
			strName = pItem->getName();

		const rlv_command_list_t* pCommands = itObj->second.getCommandList();
		for (rlv_command_list_t::const_iterator itCmd = pCommands->begin(), endCmd = pCommands->end(); itCmd != endCmd; ++itCmd)
		{
			std::string strBhvr = itCmd->asString(); LLUUID uuid(itCmd->getOption());
			if (uuid.notNull())
			{
				std::string strLookup;
				if ( (gCacheName->getFullName(uuid, strLookup)) || (gCacheName->getGroupName(uuid, strLookup)) )
					strBhvr.assign(itCmd->getBehaviour()).append(":").append(strLookup);
			}

			LLSD element;

			// Restriction column
			element["columns"][0]["column"] = "behaviour";
			element["columns"][0]["value"] = strBhvr;
			element["columns"][0]["font"] = "SANSSERIF";
			element["columns"][0]["font-style"] = "NORMAL";

			// Object Name column
			element["columns"][1]["column"] = "name";
			element["columns"][1]["value"] = strName;
			element["columns"][1]["font"] = "SANSSERIF";
			element["columns"][1]["font-style"] = "NORMAL";

			pList->addElement(element, ADD_BOTTOM);
		}
	}
}

// ============================================================================
/*
 * LLFloater overrides
 */

BOOL RlvFloaterBehaviour::canClose()
{
	return !LLApp::isExiting();
}

void RlvFloaterBehaviour::onOpen()
{
	gRlvHandler.addBehaviourObserver(this);

	refreshAll();
}

void RlvFloaterBehaviour::onClose(bool fQuitting)
{
	LLFloater::setVisible(FALSE);

	gRlvHandler.removeBehaviourObserver(this);
}

BOOL RlvFloaterBehaviour::postBuild()
{
	return TRUE;
}

// ============================================================================
/*
 * RlvBehaviourObserver overrides
 */

void RlvFloaterBehaviour::changed()
{
	refreshAll();
}

// ============================================================================
