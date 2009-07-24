#include "llviewerprecompiledheaders.h"
#include "llagent.h"
#include "llchatbar.h"
#include "lldrawpoolalpha.h"
#include "llfloateravatarlist.h"
#include "llfloaterbeacons.h"
#include "llfloaterchat.h"
#include "llfloaterdaycycle.h"
#include "llfloaterenvsettings.h"
#include "llfloatergodtools.h"
#include "llfloaterland.h"
#include "llfloatermap.h"
#include "llfloaterregioninfo.h"
#include "llfloatertools.h"
#include "llfloaterwater.h"
#include "llfloaterwindlight.h"
#include "llfloaterworldmap.h"
#include "llgesturemgr.h"
#include "llinventorymodel.h"
#include "llinventoryview.h"
#include "llinventorybridge.h"
#include "llinstantmessage.h"
#include "lloverlaybar.h"
#include "llstartup.h"
#include "llviewermenu.h"
#include "llviewermessage.h"
#include "llviewerobjectlist.h"
#include "llviewerparcelmgr.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llvoavatar.h"
#include "llwearable.h"
#include "llworld.h"
#include "pipeline.h"

#include "rlvhelper.h"
#include "rlvevent.h"
#include "rlvextensions.h"
#include "rlvhandler.h"

// Only defined in llinventorybridge.cpp
#if RLV_TARGET < RLV_MAKE_TARGET(1, 23, 0)			// Version: 1.22.11
	void confirm_replace_attachment_rez(S32 option, void* user_data);
#endif
// Only defined in llinventorymodel.cpp
extern const char* NEW_CATEGORY_NAME;

// ============================================================================
// Static variable initialization
//

BOOL RlvHandler::m_fEnabled = FALSE;
BOOL RlvHandler::fNoSetEnv = FALSE;
BOOL RlvHandler::m_fFetchStarted = FALSE;
BOOL RlvHandler::m_fFetchComplete = FALSE;
RlvMultiStringSearch RlvHandler::m_AttachLookup;

const std::string RlvHandler::cstrSharedRoot = RLV_ROOT_FOLDER;

// Keep these consistent with regular RLV
const std::string RlvHandler::cstrBlockedRecvIM = "*** IM blocked by your viewer";
const std::string RlvHandler::cstrBlockedSendIM = "*** IM blocked by sender's viewer";
const std::string RlvHandler::cstrHidden = "(Hidden)";
const std::string RlvHandler::cstrHiddenParcel = "(Hidden parcel)";
const std::string RlvHandler::cstrHiddenRegion = "(Hidden region)";
const std::string RlvHandler::cstrMsgRecvIM = 
	"The Resident you messaged is prevented from reading your instant messages at the moment, please try again later.";
const std::string RlvHandler::cstrMsgTpLure = 
	"The Resident you invited is prevented from accepting teleport offers. Please try again later.";

const std::string RlvHandler::cstrAnonyms[] =
{
	"A resident", "This resident", "That resident", "An individual", "This individual", "That individual", "A person",
	"This person", "That person", "A stranger", "This stranger", "That stranger", "A human being", "This human being", 
	"That human being", "An agent", "This agent", "That agent", "A soul", "This soul", "That soul", "Somebody", 
	"Some people", "Someone", "Mysterious one", "An unknown being", "Unidentified one", "An unknown person"
};

rlv_handler_t gRlvHandler;

// ============================================================================
// Helper functions

// Checked: 2009-07-12 (RLVa-1.0.0h) | Added: RLVa-0.2.0e
inline bool rlvIsWearingItem(const LLInventoryItem* pItem)
{
	return 
		((LLAssetType::AT_OBJECT == pItem->getType()) && (gAgent.getAvatarObject()->isWearingAttachment(pItem->getUUID()))) ||
		((LLAssetType::AT_GESTURE == pItem->getType()) && (gGestureManager.isGestureActive(pItem->getUUID()))) ||
		(gAgent.isWearingItem(pItem->getUUID()));
}

// ============================================================================
// Constructor/destructor
//

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0h
RlvHandler::RlvHandler()
	: m_fReplyInProgress(FALSE), m_idCurObject(LLUUID::null), m_pCurCommand(NULL), m_pGCTimer(NULL), m_pWLSnapshot(NULL)
{
	// Array auto-initialization to 0 is non-standard? (Compiler warning in VC-8.0)
	memset(m_LayersAdd, 0, sizeof(S16) * WT_COUNT);
	memset(m_LayersRem, 0, sizeof(S16) * WT_COUNT);
	memset(m_Behaviours, 0, sizeof(S16) * RLV_BHVR_COUNT);
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0h
RlvHandler::~RlvHandler()
{
	//delete m_pGCTimer;	// <- deletes itself
	delete m_pWLSnapshot;	// <- delete on NULL is harmless
}

// ============================================================================
// Attachment related functions 
//

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
inline LLViewerJointAttachment* RlvHandler::getAttachPoint(const std::string& strText, bool fExact) const
{
	LLVOAvatar* pAvatar = gAgent.getAvatarObject();
	return (pAvatar) ? get_if_there(pAvatar->mAttachmentPoints, getAttachPointIndex(strText, fExact), (LLViewerJointAttachment*)NULL)
	                 : NULL;
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
LLViewerJointAttachment* RlvHandler::getAttachPoint(const LLInventoryCategory* pFolder, bool fStrict) const
{
	if (!pFolder)
		return NULL;

	std::string strFolder = pFolder->getName();
	if ( (fStrict) && (!strFolder.empty()) )
	{
		// A strict match for a folder means we look for: ^\.\(\s+attachpt\s+\)
		int idxStart = strFolder.find('('), idxEnd = strFolder.find(')', idxStart);
		if ( (RLV_FOLDER_PREFIX_HIDDEN != strFolder[0]) || (1 != idxStart) || (idxEnd < idxStart + 3) )
			return NULL;

		strFolder = strFolder.substr(idxStart + 1, idxEnd - idxStart - 1);
		LLStringUtil::trim(strFolder);
	}
	return getAttachPoint(strFolder, fStrict);
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
LLViewerJointAttachment* RlvHandler::getAttachPoint(const LLInventoryItem* pItem, bool fStrict) const
{
	// Sanity check - if it's not an object then it can't have an attachment point
	if ( (!pItem) || (LLAssetType::AT_OBJECT != pItem->getType()) )
		return NULL;

	std::string strItem = pItem->getName();
	if (fStrict)
	{
		// A strict match for an item means we look for the last () occurance
		int idxEnd = strItem.rfind(')'), idxStart = strItem.rfind('(', idxEnd);
		if ( (-1 == idxStart) || (idxEnd < idxStart + 3) )
			strItem.clear();
		else
		{
			strItem = strItem.substr(idxStart + 1, idxEnd - idxStart - 1);
			LLStringUtil::trim(strItem);
		}
	}

	// If the item is modify   : we look at the item's name first and only then at the containing folder
	// If the item is no modify: we look at the containing folder's name first and only then at the item itself
	LLViewerJointAttachment* pAttachPt;
	if (pItem->getPermissions().allowModifyBy(gAgent.getID()))
	{
		pAttachPt = (!strItem.empty()) ? getAttachPoint(strItem, fStrict) : NULL;
		if (!pAttachPt)
			pAttachPt = getAttachPoint(gInventory.getCategory(pItem->getParentUUID()), fStrict);
	}
	else
	{
		pAttachPt = getAttachPoint(gInventory.getCategory(pItem->getParentUUID()), fStrict);
		if ( (!pAttachPt) && (!strItem.empty()) )
			pAttachPt = getAttachPoint(strItem, fStrict);
	}
	return pAttachPt;
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Added: RLVa-0.2.2a
S32 RlvHandler::getAttachPointIndex(const LLViewerJointAttachment* pAttachPt) const
{
	LLVOAvatar* pAvatar = gAgent.getAvatarObject();
	if (pAvatar)
	{
		for (LLVOAvatar::attachment_map_t::const_iterator itAttach = pAvatar->mAttachmentPoints.begin(); 
				itAttach != pAvatar->mAttachmentPoints.end(); ++itAttach)
		{
			if (itAttach->second == pAttachPt)
				return itAttach->first;
		}
	}
	return 0;
}

bool RlvHandler::hasLockedHUD() const
{
	LLVOAvatar* pAvatar = gAgent.getAvatarObject();
	if (!pAvatar)
		return false;
	
	LLViewerJointAttachment* pAttachPt;
	for (rlv_detach_map_t::const_iterator itAttachPt = m_Attachments.begin(); itAttachPt != m_Attachments.end(); ++itAttachPt)
	{
		pAttachPt = get_if_there(pAvatar->mAttachmentPoints, (S32)itAttachPt->first, (LLViewerJointAttachment*)NULL);
		if ( (pAttachPt) && (pAttachPt->getIsHUDAttachment()) )
			return true;	// At least one of our locked attachments is a HUD
	}
	return false;			// None of our locked attachments is a HUD
}

bool RlvHandler::isDetachable(const LLInventoryItem* pItem) const
{
	LLVOAvatar* pAvatar = gAgent.getAvatarObject();
	return ( (pItem) && (pAvatar) ) ? isDetachable(pAvatar->getWornAttachment(pItem->getUUID())) : true;
}

// Checked: 2009-05-31 (RLVa-0.2.0e) | Modified: RLVa-0.2.0e
bool RlvHandler::setDetachable(S32 idxAttachPt, const LLUUID& idRlvObj, bool fDetachable)
{
	// Sanity check - make sure it's an object we know about
	rlv_object_map_t::const_iterator itObj = m_Objects.find(idRlvObj);
	if ( (itObj == m_Objects.end()) || (!idxAttachPt) )
		return false;	// If (idxAttachPt) == 0 then: (pObj == NULL) || (pObj->isAttachment() == FALSE)

	// URGENT-RLVa: there is a slight problem if @detach=n is issued by a prim that's rezzed and later attached
	//              The solution would be to add it with 'idxAttachPt == 0' (isDetachable was already changed to account for that)
	//              and then try and resolve it from onAttach()
	if (!fDetachable)
	{
		// Sanity check - make sure it's not already marked undetachable by this object (NOTE: m_Attachments is a *multimap*, not a map)
		for (rlv_detach_map_t::const_iterator itAttach = m_Attachments.lower_bound(idxAttachPt), 
				endAttach = m_Attachments.upper_bound(idxAttachPt); itAttach != endAttach; ++itAttach)
		{
			if (itObj->second.m_UUID == itAttach->second)
				return false;
		}

		m_Attachments.insert(std::pair<S32, LLUUID>(idxAttachPt, itObj->second.m_UUID));
		return true;
	}
	else
	{
		// NOTE: m_Attachments is a *multimap*, not a map
		for (rlv_detach_map_t::iterator itAttach = m_Attachments.lower_bound(idxAttachPt), 
				endAttach = m_Attachments.upper_bound(idxAttachPt); itAttach != endAttach; ++itAttach)
		{
			if (itObj->second.m_UUID == itAttach->second)
			{
				m_Attachments.erase(itAttach);
				return true;
			}
		}
	}
	return false;	// Fall-through for (fDetachable == TRUE) - if the object wasn't undetachable then we consider it a failure
}

#ifdef RLV_EXTENSION_FLAG_NOSTRIP
	// Checked: 2009-05-26 (RLVa-0.2.0d) | Modified: RLVa-0.2.0d
	bool RlvHandler::isStrippable(const LLUUID& idItem) const
	{
		// An item is exempt from @detach or @remoutfit if:
		//   - its name contains "nostrip" (anywhere in the name)
		//   - its parent folder contains "nostrip" (anywhere in the name)
		if (idItem.notNull())
		{
			LLViewerInventoryItem* pItem = gInventory.getItem(idItem);
			if (pItem)
			{
				if (-1 != pItem->getName().find(RLV_FOLDER_FLAG_NOSTRIP))
					return false;

				LLViewerInventoryCategory* pFolder = gInventory.getCategory(pItem->getParentUUID());
				if ( (pFolder) && (-1 != pFolder->getName().find(RLV_FOLDER_FLAG_NOSTRIP)) )
					return false;
			}
		}
		return true;
	}
#endif // RLV_EXTENSION_FLAG_NOSTRIP

// ============================================================================
// Behaviour related functions
//

bool RlvHandler::hasBehaviourExcept(const std::string& strBehaviour, const LLUUID& idObj) const
{
	for (rlv_object_map_t::const_iterator itObj = m_Objects.begin(); itObj != m_Objects.end(); ++itObj)
		if ( (idObj != itObj->second.m_UUID) && (itObj->second.hasBehaviour(strBehaviour)) )
			return true;
	return false;
}

bool RlvHandler::hasBehaviourExcept(ERlvBehaviour eBehaviour, const std::string& strOption, const LLUUID& idObj) const
{
	for (rlv_object_map_t::const_iterator itObj = m_Objects.begin(); itObj != m_Objects.end(); ++itObj)
		if ( (idObj != itObj->second.m_UUID) && (itObj->second.hasBehaviour(eBehaviour, strOption)) )
			return true;
	return false;
}

bool RlvHandler::hasBehaviourExcept(const std::string& strBehaviour, const std::string& strOption, const LLUUID& idObj) const
{
	for (rlv_object_map_t::const_iterator itObj = m_Objects.begin(); itObj != m_Objects.end(); ++itObj)
		if ( (idObj != itObj->second.m_UUID) && (itObj->second.hasBehaviour(strBehaviour, strOption)) )
			return true;
	return false;
}

// ============================================================================
// Command processing functions
//

// Checked: 2009-06-03 (RLVa-0.2.0h)
void RlvHandler::addBehaviourObserver(RlvBehaviourObserver* pBhvrObserver)
{
	std::list<RlvBehaviourObserver*>::iterator itBhvrObserver = std::find(m_BhvrObservers.begin(), m_BhvrObservers.end(), pBhvrObserver);
	if (itBhvrObserver == m_BhvrObservers.end())
		m_BhvrObservers.push_back(pBhvrObserver);
}

// Checked: 2009-06-03 (RLVa-0.2.0h)
void RlvHandler::removeBehaviourObserver(RlvBehaviourObserver* pBhvrObserver)
{
	std::list<RlvBehaviourObserver*>::iterator itBhvrObserver = std::find(m_BhvrObservers.begin(), m_BhvrObservers.end(), pBhvrObserver);
	if (itBhvrObserver != m_BhvrObservers.end())
		m_BhvrObservers.erase(itBhvrObserver);
}

// Checked: 2009-06-03 (RLVa-0.2.0h)
void RlvHandler::notifyBehaviourObservers()
{
	for (std::list<RlvBehaviourObserver*>::const_iterator itBhvrObserver = m_BhvrObservers.begin();
		itBhvrObserver != m_BhvrObservers.end(); ++itBhvrObserver)
	{
		(*itBhvrObserver)->changed();
	}
}

// Checked:
BOOL RlvHandler::processCommand(const LLUUID& uuid, const std::string &strCmd)
{
	#ifdef RLV_DEBUG
		RLV_INFOS << "[" << uuid << "]: " << strCmd << LL_ENDL;
	#endif // RLV_DEBUG

	RlvCommand rlvCmd(strCmd);
	if (!rlvCmd.isValid())
	{
		#ifdef RLV_DEBUG
			RLV_INFOS << "\t-> invalid command: " << strCmd << LL_ENDL;
		#endif // RLV_DEBUG
		return FALSE;
	}
	m_pCurCommand = &rlvCmd; m_idCurObject = uuid;

	BOOL fRet = FALSE;
	switch (rlvCmd.getParamType())
	{
		case RLV_TYPE_ADD:		// Checked: 2009-06-03 (RLVa-0.2.0h) | Modified: RLVa-0.2.0h
			{
				if ( (m_Behaviours[rlvCmd.getBehaviourType()]) && 
					 ( (RLV_BHVR_SETDEBUG == rlvCmd.getBehaviourType()) || (RLV_BHVR_SETENV == rlvCmd.getBehaviourType()) ) )
				{
					// Some restrictions can only be held by one single object to avoid deadlocks
					#ifdef RLV_DEBUG
						RLV_INFOS << "\t- " << rlvCmd.getBehaviour() << " is already set by another object => discarding" << LL_ENDL;
					#endif // RLV_DEBUG
					break;
				}

				rlv_object_map_t::iterator itObj = m_Objects.find(uuid);
				if (itObj != m_Objects.end())
				{
					RlvObject& rlvObj = itObj->second;
					fRet = rlvObj.addCommand(rlvCmd);
				}
				else
				{
					RlvObject rlvObj(uuid);
					fRet = rlvObj.addCommand(rlvCmd);
					m_Objects.insert(std::pair<LLUUID, RlvObject>(uuid, rlvObj));
				}

				#ifdef RLV_DEBUG
					RLV_INFOS << "\t- " << ( (fRet) ? "adding behaviour" : "skipping duplicate") << LL_ENDL;
				#endif // RLV_DEBUG

				if (fRet) {	// If FALSE then this was a duplicate, there's no need to handle those
					if (!m_pGCTimer)
						m_pGCTimer = new RlvGCTimer();
					processAddCommand(uuid, rlvCmd);
					notifyBehaviourObservers();
				}
			}
			break;
		case RLV_TYPE_REMOVE:		// Checked:
			{
				rlv_object_map_t::iterator itObj = m_Objects.find(uuid);
				if (itObj != m_Objects.end())
					fRet = itObj->second.removeCommand(rlvCmd);

				#ifdef RLV_DEBUG
					RLV_INFOS << "\t- " << ( (fRet) ? "removing behaviour"
						                            : "skipping remove (unset behaviour or unknown object)") << LL_ENDL;
				#endif // RLV_DEBUG

				if (fRet) {	// Don't handle non-sensical removes
					processRemoveCommand(uuid, rlvCmd);
					notifyBehaviourObservers();

					if (0 == itObj->second.m_Commands.size())
					{
						#ifdef RLV_DEBUG
							RLV_INFOS << "\t- command list empty => removing " << uuid << LL_ENDL;
						#endif // RLV_DEBUG
						m_Objects.erase(itObj);
					}
				}
			}
			break;
		case RLV_TYPE_FORCE:		// Checked:
			fRet = processForceCommand(uuid, rlvCmd);
			break;
		case RLV_TYPE_REPLY:		// Checked:
			fRet = processReplyCommand(uuid, rlvCmd);
			break;
		case RLV_TYPE_UNKNOWN:		// Checked:
			{
				if ("clear" == rlvCmd.getBehaviour())
				{
					const std::string& strFilter = rlvCmd.getParam(); std::string strCmdRem;

					rlv_object_map_t::const_iterator itObj = m_Objects.find(uuid);
					if (itObj != m_Objects.end())	// No sense in @clear'ing if we don't have any commands for this object
					{
						const RlvObject& rlvObj = itObj->second; bool fContinue = true;
						for (rlv_command_list_t::const_iterator itCmd = rlvObj.m_Commands.begin(), itCurCmd; 
								((fContinue) && (itCmd != rlvObj.m_Commands.end())); )
						{
							itCurCmd = itCmd++;		// Point itCmd ahead so it won't get invalidated if/when we erase a command

							const RlvCommand& rlvCmdRem = *itCurCmd;
							if ( (strFilter.empty()) || (-1 != rlvCmdRem.asString().find(strFilter)) )
							{
								fContinue = (rlvObj.m_Commands.size() > 1); // rlvObj will become invalid once we remove the last command
								strCmdRem = rlvCmdRem.getBehaviour() + ":" + rlvCmdRem.getOption() + "=y";
								processCommand(uuid, strCmdRem);
							}
						}
						fRet = TRUE;
					}
				}
			}
			break;
		#ifdef LL_GNUC
		default:
			break;
		#endif // LL_GNUC
	}

	#ifdef RLV_DEBUG
		RLV_INFOS << "\t--> command " << ((fRet) ? "succeeded" : "failed") << LL_ENDL;
	#endif // RLV_DEBUG

	m_pCurCommand = NULL; m_idCurObject.setNull();
	return fRet;
}

BOOL RlvHandler::processAddCommand(const LLUUID& uuid, const RlvCommand& rlvCmd)
{
	// NOTE: at this point the command has already been added to the corresponding object!
	ERlvBehaviour eBehaviour = rlvCmd.getBehaviourType();
	const std::string& strOption = rlvCmd.getOption();

	if ( (RLV_BHVR_UNKNOWN != eBehaviour) && (strOption.empty()) )
		m_Behaviours[eBehaviour]++;

	switch (eBehaviour)
	{
		case RLV_BHVR_DETACH:				// @detach=n				- Checked:
			{
				LLViewerObject* pObj = gObjectList.findObject(uuid);
				if ( (pObj) && (strOption.empty()) )
				{
					setDetachable(pObj, uuid, false);

					if (pObj->isHUDAttachment())
						LLPipeline::sShowHUDAttachments = TRUE;	// Prevents hiding of locked HUD attachments
				}
			}
			break;
		case RLV_BHVR_REDIRCHAT:			// @redirchat:<option>=n	- Checked: 2009-07-07 (RLVa-1.0.0d)
		case RLV_BHVR_REDIREMOTE:			// @rediremote:<option>=n	- Checked: 2009-07-07 (RLVa-1.0.0d) | Added: RLVa-0.2.2a
			{
				if (!strOption.empty())
					m_Behaviours[eBehaviour]++;	// @redirchat and @rediremote don't have an optionless version so keep track of it here
				else
					m_Behaviours[eBehaviour]--;	// @redirchat=n and @rediremote=n are undefined, don't keep track of them
			}
			break;
		case RLV_BHVR_SHOWWORLDMAP:			// @showworldmap=n			- Checked: 2009-07-05 (RLVa-1.0.0c)
			{
				// Simulate clicking the Map button [see LLToolBar::onClickMap()]
				if (gFloaterWorldMap->getVisible())
					LLFloaterWorldMap::toggle(NULL);
			}
			break;
		case RLV_BHVR_SHOWMINIMAP:			// @showminimap=n			- Checked: 2009-07-05 (RLVa-1.0.0c)
			{
				// Simulate clicking the Minimap button [see LLToolBar::onClickRadar()]
				#if RLV_TARGET < RLV_MAKE_TARGET(1, 23, 0)			// Version: 1.22.11
					if (gFloaterMap->getVisible())
						LLFloaterMap::toggle(NULL);
				#else												// Version: 1.23.4
					if (LLFloaterMap::instanceVisible())
						LLFloaterMap::hideInstance();
				#endif
			}
			break;
		#ifdef RLV_EXTENSION_STARTLOCATION
		case RLV_BHVR_TPLOC:				// @tploc=n					- Checked: 2009-07-08 (RLVa-1.0.0e) | Added: RLVa-0.2.1d
		case RLV_BHVR_UNSIT:				// @unsit=n					- Checked: 2009-07-08 (RLVa-1.0.0e) | Added: RLVa-0.2.1d
			{
				if (strOption.empty())
					RlvSettings::updateLoginLastLocation();
			}
			break;
		#endif // RLV_EXTENSION_STARTLOCATION
		case RLV_BHVR_EDIT:					// @edit=n					- Checked: 2009-07-04 (RLVa-1.0.0b)
			{
				// Turn off "View / Highlight Transparent"
				LLDrawPoolAlpha::sShowDebugAlpha = FALSE;

				// Close the Beacons floater if it's open
				if (LLFloaterBeacons::instanceVisible())
					LLFloaterBeacons::toggleInstance();

				// Get rid of the build floater if it's open [copy/paste from toggle_build_mode()]
				if (gFloaterTools->getVisible())
				{
					gAgent.resetView(FALSE);
					gFloaterTools->close();
					gViewerWindow->showCursor();
				}
			}
			break;
		case RLV_BHVR_ADDOUTFIT:			// @addoutfit[:<layer>]=n	- Checked: 2009-07-07 (RLVa-1.0.0d)
		case RLV_BHVR_REMOUTFIT:			// @remoutfit[:<layer>]=n	- Checked: 2009-07-07 (RLVa-1.0.0d)
			{
				S16* pLayers = (eBehaviour == RLV_BHVR_ADDOUTFIT) ? m_LayersAdd : m_LayersRem;

				if (strOption.empty())
				{
					for (int idx = 0; idx < WT_COUNT; idx++)
						pLayers[idx]++;
				}
				else
				{
					EWearableType type = LLWearable::typeNameToType(strOption);
					if (WT_INVALID != type)
					{
						pLayers[type]++;
						m_Behaviours[eBehaviour]++;
					}
				}
			}
			break;
		case RLV_BHVR_SHOWINV:				// @showinv=n				- Checked: 2009-07-10 (RLVa-1.0.0g) | Modified: RLVa-1.0.0g
			{
				// Close all open inventory windows
				LLInventoryView::closeAll();
			}
			break;
		case RLV_BHVR_SHOWLOC:				// @showloc=n				- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
			{
				// If we're the first @showloc=n restriction refresh all object text so we can filter it if necessary
				if (1 == m_Behaviours[RLV_BHVR_SHOWLOC])
					LLHUDText::refreshAllObjectText();

				// Close the "About Land" floater if it's currently visible
				if (LLFloaterLand::instanceVisible())
					LLFloaterLand::hideInstance();

				// Close the "Estate Tools" floater is it's currently visible
				if (LLFloaterRegionInfo::instanceVisible())
					LLFloaterRegionInfo::hideInstance();

				// NOTE: we should close the "God Tools" floater as well, but since calling LLFloaterGodTools::instance() always
				//       creates a new instance of the floater and since it's very unlikely to be open it's just better not to
			}
			break;
		case RLV_BHVR_SHOWNAMES:			// @shownames=n				- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
			{
				// If we're the first @shownames=n restriction refresh all object text so we can filter it if necessary
				if (1 == m_Behaviours[RLV_BHVR_SHOWNAMES])
					LLHUDText::refreshAllObjectText();

				// Close the "Active Speakers" panel if it's currently visible
				LLFloaterChat::getInstance()->childSetVisible("active_speakers_panel", false);

				// Emerald specific code
				LLFloaterAvatarList* pAvList = LLFloaterAvatarList::getInstance();
				if ( (pAvList) && (pAvList->getVisible()) )
					pAvList->close();
			}
			break;
		case RLV_BHVR_FLY:					// @fly=n					- Checked: 2009-07-05 (RLVa-1.0.0c)
			{
				// If currently flying, simulate clicking the Fly button [see LLToolBar::onClickFly()]
				if (gAgent.getFlying())
					gAgent.toggleFlying();
			}
			break;
		case RLV_BHVR_SETENV:				// @setenv=n				- Checked: 2009-07-10 (RLVa-1.0.0g) | Modified: RLVa-0.2.0a
			{
				if (!fNoSetEnv)
				{
					// Only close the floaters if their instance exists and they're actually visible
					if ( (LLFloaterEnvSettings::isOpen()) && (LLFloaterEnvSettings::instance()->getVisible()) )
						LLFloaterEnvSettings::instance()->close();
					if ( (LLFloaterWindLight::isOpen()) && (LLFloaterWindLight::instance()->getVisible()) )
						LLFloaterWindLight::instance()->close();
					if ( (LLFloaterWater::isOpen()) && (LLFloaterWater::instance()->getVisible()) )
						LLFloaterWater::instance()->close();
					if ( (LLFloaterDayCycle::isOpen()) && (LLFloaterDayCycle::instance()->getVisible()) )
						LLFloaterDayCycle::instance()->close();

					// Save the current WindLight params so we can restore them on @setenv=y
					if (m_pWLSnapshot)
					{
						RLV_ERRS << "m_pWLSnapshot != NULL" << LL_ENDL; // Safety net in case we set @setenv=n for more than 1 object
						delete m_pWLSnapshot;
					}
					m_pWLSnapshot = RlvWLSnapshot::takeSnapshot();
				}
			}
			break;
		case RLV_BHVR_SHOWHOVERTEXTALL:		// @showhovertextal=n		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_SHOWHOVERTEXTWORLD:	// @showhovertextworld=n	- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_SHOWHOVERTEXTHUD:		// @showhovertexthud=n		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
			{
				// Refresh all hover text (LLHUDText::setStringUTF8() will decide what needs clearing and what doesn't)
				LLHUDText::refreshAllObjectText();
			}
			break;
		case RLV_BHVR_SHOWHOVERTEXT:		// @showhovertext:<uuid>=n	- Checked: 2009-07-09 (RLVa-0.2.2a) | Modified: RLVa-1.0.0f
			{
				LLUUID idException(strOption);
				if (!idException.isNull())	// If there's an option it should be a valid UUID
				{
					addException(eBehaviour, LLUUID(strOption));

					// Clear the object's hover text
					LLViewerObject* pObj = gObjectList.findObject(idException);
					if ( (pObj) && (pObj->mText.notNull()) && (!pObj->mText->getObjectText().empty()) )
						pObj->mText->setStringUTF8("");
				}
			}
			break;
		case RLV_BHVR_RECVCHAT:				// @recvchat:<uuid>=add		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_RECVEMOTE:			// @recvemote:<uuid>=add	- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_RECVIM:				// @recvim:<uuid>=add		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_SENDIM:				// @sendim:<uuid>=add		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_TPLURE:				// @tplure:<uuid>=add		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_ACCEPTTP:				// @accepttp:<uuid>=add		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
			{
				addException(eBehaviour, LLUUID(strOption));
			}
			break;
		default:
			{
				// Give our observers a chance to handle any command we don't
				RlvEvent rlvEvent(uuid, rlvCmd);
				m_Emitter.update(&RlvObserver::onAddCommand, rlvEvent);
			}
			break;
	}
	return TRUE; // Add command success/failure is decided by RlvObject::addCommand()
}

BOOL RlvHandler::processRemoveCommand(const LLUUID& uuid, const RlvCommand& rlvCmd)
{
	// NOTE: the RlvObject still exists at this point, but the viewer might already have removed it from its object list
	ERlvBehaviour eBehaviour = rlvCmd.getBehaviourType();
	const std::string& strOption = rlvCmd.getOption();

	if ( (RLV_BHVR_UNKNOWN != eBehaviour) && (strOption.empty()) )
		m_Behaviours[eBehaviour]--;

	switch (eBehaviour)
	{
		case RLV_BHVR_DETACH:				// @detach=y				- Checked: 2009-06-03 (RLVa-0.2.0h) | Modified: RLVa-0.2.0h
			{
				if (strOption.empty())
				{
					// The object may or may not (if it got detached) still exist so clean up the hard way
					if (m_Objects.find(uuid) != m_Objects.end())
					{
						for (rlv_detach_map_t::const_iterator itAttach = m_Attachments.begin(), endAttach = m_Attachments.end();
							itAttach != endAttach; ++itAttach)
						{
							if (itAttach->second == uuid)
							{
								setDetachable(itAttach->first, uuid, true); // <- invalidates our iterators on return
								break;
							}
						}
					}
				}
			}
			break;
		case RLV_BHVR_REDIRCHAT:			// @redirchat:<option>=y	- Checked: 2009-07-07 (RLVa-1.0.0d)
		case RLV_BHVR_REDIREMOTE:			// @rediremote:<option>=y	- Checked: 2009-07-07 (RLVa-1.0.0d) | Added: RLVa-0.2.2a
			{
				if (!strOption.empty())
					m_Behaviours[eBehaviour]--;	// @redirchat and @rediremote don't have an optionless version so keep track of it here
				else
					m_Behaviours[eBehaviour]++;	// @redirchat=n and @rediremote=n are undefined, don't keep track of them
			}
			break;
		#ifdef RLV_EXTENSION_STARTLOCATION
		case RLV_BHVR_TPLOC:				// @tploc=y					- Checked: 2009-07-08 (RLVa-1.0.0e) | Added: RLVa-0.2.1d
		case RLV_BHVR_UNSIT:				// @unsit=y					- Checked: 2009-07-08 (RLVa-1.0.0e) | Added: RLVa-0.2.1d
			{
				if (strOption.empty())
					RlvSettings::updateLoginLastLocation();
			}
			break;
		#endif // RLV_EXTENSION_STARTLOCATION
		case RLV_BHVR_ADDOUTFIT:			// @addoutfit[:<layer>]=y	- Checked: 2009-07-07 (RLVa-1.0.0d)
		case RLV_BHVR_REMOUTFIT:			// @remoutfit[:<layer>]=y	- Checked: 2009-07-07 (RLVa-1.0.0d)
			{
				S16* pLayers = (eBehaviour == RLV_BHVR_ADDOUTFIT) ? m_LayersAdd : m_LayersRem;

				if (strOption.empty())
				{
					for (int idx = 0; idx < WT_COUNT; idx++)
						pLayers[idx]--;
				}
				else
				{
					EWearableType type = LLWearable::typeNameToType(strOption);
					if (WT_INVALID != type)
					{
						pLayers[type]--;
						m_Behaviours[eBehaviour]--;
					}
				}
			}
			break;
		case RLV_BHVR_SETENV:				// @setenv=y				- Checked: 2009-07-10 (RLVa-1.0.0g) | Added: RLVa-0.2.0h
			{
				if (!fNoSetEnv)
				{
					// Restore WindLight parameters to what they were before @setenv=n was issued
					RlvWLSnapshot::restoreSnapshot(m_pWLSnapshot);
					delete m_pWLSnapshot;
					m_pWLSnapshot = NULL;
				}
			}
			break;
		case RLV_BHVR_SHOWLOC:				// @showloc=y				- Checked: 2009-07-09 (RLVa-1.0.0f) | Added: RLVa-1.0.0f
		case RLV_BHVR_SHOWNAMES:			// @shownames=y				- Checked: 2009-07-09 (RLVa-1.0.0f) | Added: RLVa-1.0.0f
		case RLV_BHVR_SHOWHOVERTEXTALL:		// @showhovertextal=y		- Checked: 2009-07-09 (RLVa-1.0.0f) | Added: RLVa-1.0.0f
		case RLV_BHVR_SHOWHOVERTEXTWORLD:	// @showhovertextworld=y	- Checked: 2009-07-09 (RLVa-1.0.0f) | Added: RLVa-1.0.0f
		case RLV_BHVR_SHOWHOVERTEXTHUD:		// @showhovertexthud=y		- Checked: 2009-07-09 (RLVa-1.0.0f) | Added: RLVa-1.0.0f
			{
				// If this was the last of any of the five restrictions we should refresh all hover text in case anything needs restoring
				if (!m_Behaviours[eBehaviour])
					LLHUDText::refreshAllObjectText();
			}
			break;
		case RLV_BHVR_SHOWHOVERTEXT:		// @showhovertext:<uuid>=y	- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
			{
				LLUUID idException(strOption);
				if (!idException.isNull())	// If there's an option it should be a valid UUID
				{
					removeException(eBehaviour, LLUUID(strOption));

					// Restore the object's hover text
					LLViewerObject* pObj = gObjectList.findObject(idException);
					if ( (pObj) && (pObj->mText.notNull()) && (!pObj->mText->getObjectText().empty()) )
						pObj->mText->setStringUTF8(pObj->mText->getObjectText());
				}
			}
			break;
		case RLV_BHVR_RECVCHAT:				// @recvchat:<uuid>=rem		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_RECVEMOTE:			// @recvemote:<uui>=red		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_RECVIM:				// @recvim:<uuid>=rem		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_SENDIM:				// @sendim:<uuid>=rem		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_TPLURE:				// @recvim:<uuid>=rem		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
		case RLV_BHVR_ACCEPTTP:				// @accepttp:<uuid>=rem		- Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
			{
				removeException(eBehaviour, LLUUID(strOption));
			}
			break;
		default:
			{
				// Give our observers a chance to handle any command we don't
				RlvEvent rlvEvent(uuid, rlvCmd);
				m_Emitter.update(&RlvObserver::onRemoveCommand, rlvEvent);
			}
			break;
	}
	return TRUE; // Remove commands don't fail, doesn't matter what we return here
}

BOOL RlvHandler::processForceCommand(const LLUUID& idObj, const RlvCommand& rlvCmd) const
{
	const std::string& strOption = rlvCmd.getOption();
	BOOL fHandled = TRUE;

	switch (rlvCmd.getBehaviourType())
	{
		case RLV_BHVR_DETACH:		// @detach[:<option>]=force		- Checked:
			onForceDetach(idObj, strOption);
			break;
		case RLV_BHVR_REMOUTFIT:	// @remoutfit:<option>=force	- Checked:
			onForceRemOutfit(idObj, strOption);
			break;
		case RLV_BHVR_UNSIT:		// @unsit=force					- Checked: 2009-06-02 (RLVa-0.2.0g)
			{
				LLVOAvatar* pAvatar = gAgent.getAvatarObject();
				if ( (pAvatar) && (pAvatar->mIsSitting) && (!hasBehaviourExcept(RLV_BHVR_UNSIT, idObj)) )
				{
					// See behaviour notes on why we have to force an agent update here
					gAgent.setControlFlags(AGENT_CONTROL_STAND_UP);
					send_agent_update(TRUE, TRUE);
				}
			}
			break;
		case RLV_BHVR_TPTO:			// @tpto:<option>=force			- Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-1.0.0h
			{
				fHandled = FALSE;
				if ( (!strOption.empty()) && (-1 == strOption.find_first_not_of("0123456789/.")) )
				{
					LLVector3d posGlobal;

					typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
					boost::char_separator<char> sep("/", "", boost::keep_empty_tokens);
					tokenizer tokens(strOption, sep); int idx = 0;
					for (tokenizer::iterator itToken = tokens.begin(); itToken != tokens.end(); ++itToken)
					{
						if (idx < 3)
							LLStringUtil::convertToF64(*itToken, posGlobal[idx++]);
					}

					if (idx == 3)
					{
						gAgent.teleportViaLocation(posGlobal);
						fHandled = TRUE;
					}
				} 
			}
			break;
		case RLV_BHVR_SIT:			// @sit:<option>=force			- Checked: 2009-06-02 (RLVa-0.2.0g)
			fHandled = onForceSit(idObj, rlvCmd.getOption());
			break;
		case RLV_BHVR_ADDOUTFIT:	// @addoutfit:<option>=force <- synonym of @attach:<option>=force
		case RLV_BHVR_ATTACH:		// @attach:<option>=force		- Checked:
			onForceWear(rlvCmd.getOption(), true, false);	// Force attach single folder
			break;
		case RLV_BHVR_ATTACHALL:	// @attachall:<option>=force	- Checked:
			onForceWear(rlvCmd.getOption(), true, true);	// Force attach nested folders
			break;
		case RLV_BHVR_DETACHALL:	// @detachall:<option>=force	- Checked:
			onForceWear(rlvCmd.getOption(), false, true);	// Force detach nested folders
			break;
		case RLV_BHVR_ATTACHTHIS:
		case RLV_BHVR_ATTACHALLTHIS:
		case RLV_BHVR_DETACHTHIS:
		case RLV_BHVR_DETACHALLTHIS:
			{
				ERlvBehaviour eBehaviour = rlvCmd.getBehaviourType();
				std::string strReply;
				if (onGetPath(idObj, strOption, strReply))
				{
					LLStringUtil::toLower(strReply);
					onForceWear(strReply, 
						(RLV_BHVR_ATTACHTHIS == eBehaviour) || (RLV_BHVR_ATTACHALLTHIS == eBehaviour), 
						(RLV_BHVR_ATTACHALLTHIS == eBehaviour) || (RLV_BHVR_DETACHALLTHIS == eBehaviour));
				}
			}
			break;
		case RLV_BHVR_DETACHME:		// @detachme=force				- Checked: 2009-06-07 (RLVa-0.2.1c)
			{
				// NOTE: @detachme=force could be seen as a @detach:<attachpt>=force but RLV implements it as a "detach by UUID"
				LLViewerObject* pObj; LLVOAvatar* pAvatar; LLViewerJointAttachment* pAttachPt;
				if ( ((pObj = gObjectList.findObject(idObj)) != NULL) && (pObj->isAttachment()) && 
					 ((pAvatar = gAgent.getAvatarObject()) != NULL) && 
					 ((pAttachPt = pAvatar->getTargetAttachmentPoint(pObj->getRootEdit())) != NULL) )
				{
					handle_detach_from_avatar(pAttachPt);
				}
			}
			break;
		default:
			{
				// Give our observers a chance to handle any command we don't
				RlvEvent rlvEvent(idObj, rlvCmd);
				fHandled = m_Emitter.update(&RlvObserver::onForceCommand, rlvEvent);
			}
			break;
	}
	return fHandled; // If we handled it then it'll still be TRUE; if an observer doesn't handle it'll be FALSE
}

// Checked: 2009-07-12 (RLVa-1.0.0h)
BOOL RlvHandler::processReplyCommand(const LLUUID& uuid, const RlvCommand& rlvCmd) const
{
	const std::string& strOption = rlvCmd.getOption();
	const std::string& strChannel = rlvCmd.getParam();
	std::string strReply;

	BOOL fHandled = TRUE;
	switch (rlvCmd.getBehaviourType())
	{
		case RLV_BHVR_VERSION:			// @version=<channel>				  - Checked: 2009-07-12 (RLVa-1.0.0h)
			strReply = getVersionString();
			break;
		case RLV_BHVR_GETOUTFIT:		// @getoufit[:<layer>]=<channel>	  - Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
			{
				// (Quirk: RLV 1.16.1 will execute @getoutfit=<channel> if <layer> is invalid, so we need to as well)
				EWearableType layerType = LLWearable::typeNameToType(strOption);

				const EWearableType layerTypes[] =
					{ 
						WT_GLOVES, WT_JACKET, WT_PANTS, WT_SHIRT, WT_SHOES, WT_SKIRT, WT_SOCKS, 
						WT_UNDERPANTS, WT_UNDERSHIRT, WT_SKIN, WT_EYES, WT_HAIR, WT_SHAPE
					};

				#ifdef RLV_EXPERIMENTAL_COMPOSITE_FOLDING
					for (int idx = 0, cnt = sizeof(layerTypes) / sizeof(EWearableType); idx < cnt; idx++)
					{
						if ( (WT_INVALID == layerType) || (layerTypes[idx] == layerType) )
						{
							// TODO-RLVa: add support for 'fHideLockedLayers'
							bool fWorn = (gAgent.getWearable(layerTypes[idx])) && 
										 (!isHiddenCompositeItem(gAgent.getWearableItem(layerTypes[idx]), 
																 LLWearable::typeToTypeName(layerTypes[idx])));
							strReply.push_back( (fWorn) ? '1' : '0' );
						}
					}
				#else
					for (int idx = 0, cnt = sizeof(layerTypes) / sizeof(EWearableType); idx < cnt; idx++)
						if ( (WT_INVALID == layerType) || (layerTypes[idx] == layerType) )
						{
							// We never hide body parts, even if they're "locked" and we're hiding locked layers
							// (nor do we hide a layer if the issuing object is the only one that has this layer locked)
							bool fWorn = (gAgent.getWearable(layerTypes[idx])) && 
								( (!RlvSettings::getHideLockedLayers()) || 
								  (LLAssetType::AT_BODYPART == LLWearable::typeToAssetType(layerTypes[idx])) ||
								  ( (isRemovableExcept(layerTypes[idx], uuid)) && 
								    (isStrippable(gAgent.getWearableItem(layerTypes[idx]))) ) );
							strReply.push_back( (fWorn) ? '1' : '0' );
							//strReply.push_back( (gAgent.getWearable(layerTypes[idx])) ? '1' : '0' );
						}
				#endif // RLV_EXPERIMENTAL_COMPOSITE_FOLDING
			}
			break;
		case RLV_BHVR_GETATTACH:		// @getattach[:<layer>]=<channel>	  - Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
			{
				// If we're fetching all worn attachments then the reply should start with 0
				if (strOption.empty())
					strReply.push_back('0');

				LLVOAvatar* pAvatar = gAgent.getAvatarObject(); std::string strAttachName;
				for (LLVOAvatar::attachment_map_t::const_iterator itAttach = pAvatar->mAttachmentPoints.begin(); 
					 itAttach != pAvatar->mAttachmentPoints.end(); ++itAttach)
				{
					LLViewerJointAttachment* pAttachment = itAttach->second;
					if (!pAttachment)
						continue;

					strAttachName = pAttachment->getName(); // Capitalized (see avatar_lad.xml)
					LLStringUtil::toLower(strAttachName);

					#ifdef RLV_EXPERIMENTAL_COMPOSITE_FOLDING
						if ( (strOption.empty()) || (strOption == strAttachName) )
						{
							// TODO-RLVa: add support for 'fHideLockedAttach'
							bool fWorn = (pAttachment->getItemID().notNull()) && 
										 (!isHiddenCompositeItem(pAttachment->getItemID(), strAttachName));
							strReply.push_back( (fWorn) ? '1' : '0' );
						}
					#else
						if ( (strOption.empty()) || (strOption == strAttachName) )
						{
							bool fWorn = (pAttachment->getItemID().notNull()) && 
								( (!RlvSettings::getHideLockedAttach()) || 
								  ( (isDetachable(itAttach->first)) && (isStrippable(pAttachment->getItemID())) ) );
							strReply.push_back( (fWorn) ? '1' : '0' );
							//strReply.push_back( (pAttachment->getItemID().notNull()) ? '1' : '0' );
						}
					#endif // RLV_EXPERIMENTAL_COMPOSITE_FOLDING
				}
			}
			break;
		case RLV_BHVR_GETSTATUS:		// @getstatus[:<option>]=<channel>	  - Checked: 2009-07-12 (RLVa-1.0.0h)
			{
				// NOTE: specification says response should start with '/' but RLV-1.16.1 returns an empty string when no rules are set
				rlv_object_map_t::const_iterator itObj = m_Objects.find(uuid);
				if (itObj != m_Objects.end())
				{
					std::string strObjStatus = itObj->second.getStatusString(strOption);
					if (!strObjStatus.empty())
					{
						strReply.push_back('/');
						strReply += strObjStatus;
					}
				}
			}
			break;
		case RLV_BHVR_GETSTATUSALL:		// @getstatusall[:<option>]=<channel> - Checked: 2009-07-12 (RLVa-1.0.0h)
			{
				// NOTE: specification says response should start with '/' but RLV-1.16.1 returns an empty string when no rules are set
				std::string strObjStatus;
				for (rlv_object_map_t::const_iterator itObj = m_Objects.begin(); itObj != m_Objects.end(); ++itObj)
				{
					strObjStatus = itObj->second.getStatusString(strOption);
					if (!strObjStatus.empty())
					{
						strReply.push_back('/');
						strReply += strObjStatus;
					}
				}
			}
			break;
		case RLV_BHVR_GETINV:			// @getinv[:<path>]=<channel>		  - Checked: 2009-07-12 (RLVa-1.0.0h)
			{
				LLViewerInventoryCategory* pFolder = getSharedFolder(strOption);
				if (pFolder)
				{
					LLInventoryModel::cat_array_t*  pFolders;
					LLInventoryModel::item_array_t* pItems;
					gInventory.getDirectDescendentsOf(pFolder->getUUID(), pFolders, pItems);

					if (pFolders)
					{
						for (S32 idxFolder = 0, cntFolder = pFolders->count(); idxFolder < cntFolder; idxFolder++)
						{
							const std::string& strFolder = pFolders->get(idxFolder)->getName();
							if ( (!strFolder.empty()) && (RLV_FOLDER_PREFIX_HIDDEN != strFolder[0]) )
							{
								if (!strReply.empty())
									strReply.push_back(',');
								strReply += strFolder;
							}
						}
					}
				}
			}
			break;
		case RLV_BHVR_GETINVWORN:		// @getinvworn[:path]=<channel>		  - Checked:
			onGetInvWorn(rlvCmd.getOption(), strReply);
			break;
		case RLV_BHVR_FINDFOLDER:		// @findfolder:<criteria>=<channel>   - Checked: 2009-07-12 (RLVa-1.0.0h)
			{
				// COMPAT-RLV: RLV 1.16.1 returns the first random folder it finds (probably tries to match "" to a folder name?)
				//             (just going to stick with what's there for now... no option => no folder)
				LLInventoryModel::cat_array_t folders;
				if ( (!strOption.empty()) && (findSharedFolders(strOption, folders)) )
				{
					// We need to return an "in depth" result so whoever has the most '/' is our lucky winner
					int maxSlashes = 0, curSlashes; std::string strFolderName;
					for (S32 idxFolder = 0, cntFolder = folders.count(); idxFolder < cntFolder; idxFolder++)
					{
						strFolderName = getSharedPath(folders.get(idxFolder));

						curSlashes = std::count(strFolderName.begin(), strFolderName.end(), '/');
						if (curSlashes > maxSlashes)
						{
							maxSlashes = curSlashes;
							strReply = strFolderName;
						}
					}
				}
			}
			break;
		#ifdef RLV_EXTENSION_CMD_FINDFOLDERS
			case RLV_BHVR_FINDFOLDERS:	// @findfolders:<criteria>=<channel>  - Checked: 2009-07-12 (RLVa-1.0.0h) | Added: RLVa-0.2.0b
				{
					LLInventoryModel::cat_array_t folders;
					if ( (!strOption.empty()) && (findSharedFolders(strOption, folders)) )
					{
						for (S32 idxFolder = 0, cntFolder = folders.count(); idxFolder < cntFolder; idxFolder++)
						{
							if (!strReply.empty())
								strReply.push_back(',');
							strReply += getSharedPath(folders.get(idxFolder));
						}
					}
				}
				break;
		#endif // RLV_EXTENSION_CMD_FINDFOLDERS
		case RLV_BHVR_GETPATH:			// @getpath[:<option>]=<channel>	  - Checked: 2009-07-12 (RLVa-1.0.0h)
			onGetPath(uuid, rlvCmd.getOption(), strReply);
			break;
		case RLV_BHVR_GETSITID:			// @getsitid=<channel>				  - Checked: 2009-07-12 (RLVa-1.0.0h)
			{
				// (Quirk: RLV 1.16.1 returns a NULL uuid if we're not sitting)
				LLVOAvatar* pAvatarObj = gAgent.getAvatarObject(); LLUUID uuid;
				if ( (pAvatarObj) && (pAvatarObj->mIsSitting) )
				{
					// LLVOAvatar inherits from 2 classes so make sure we get the right vfptr
					LLViewerObject* pAvatar = dynamic_cast<LLViewerObject*>(pAvatarObj), *pParent;
					// (If there is a parent, we need to upcast it from LLXform to LLViewerObject to get its UUID)
					if ( (pAvatar) && ((pParent = static_cast<LLViewerObject*>(pAvatar->getRoot())) != pAvatar) )
						uuid = pParent->getID();
				}
				strReply = uuid.asString();
			}
			break;
		default:
			{
				// Give our observers a chance to handle any command we don't
				RlvEvent rlvEvent(uuid, rlvCmd);
				return m_Emitter.update(&RlvObserver::onReplyCommand, rlvEvent);
			}
			break;
	}

	if (fHandled)
		sendCommandReply(strChannel, strReply);
	return fHandled;
}

// ============================================================================
// House keeping (see debug notes for test methodology, test script and last run)
//

void RlvHandler::initLookupTables()
{
	static bool fInitialized = false;
	if (!fInitialized)
	{
		// Initialize the attachment name lookup table
		LLVOAvatar* pAvatar = gAgent.getAvatarObject();
		if (pAvatar)
		{
			std::string strAttachPtName;
			for (LLVOAvatar::attachment_map_t::const_iterator itAttach = pAvatar->mAttachmentPoints.begin(); 
					 itAttach != pAvatar->mAttachmentPoints.end(); ++itAttach)
			{
				LLViewerJointAttachment* pAttachPt = itAttach->second;
				if (pAttachPt)
				{
					strAttachPtName = pAttachPt->getName();
					LLStringUtil::toLower(strAttachPtName);
					m_AttachLookup.addKeyword(strAttachPtName, itAttach->first);
				}
			}
			fInitialized = true;
		}
	}
}

// Checked: 2009-05-31 (RLVa-0.2.0e) | Modified: RLVa-0.2.0e
void RlvHandler::onAttach(LLViewerJointAttachment* pAttachPt)
{
	// Sanity check - LLVOAvatar::attachObject() should call us *after* calling LLViewerJointAttachment::addObject()
	LLViewerObject* pObj = pAttachPt->getObject();
	S32 idxAttachPt = getAttachPointIndex(pObj);	 // getAttachPointIndex() has a NULL pointer check so this is safe
	if ( (!pObj) || (!idxAttachPt) )
	{
		RLV_ERRS << "pAttachPt->getObject() == NULL" << LL_ENDL;
		return;
	}

	// If this is a locked object that got detached and that we're reattaching then remove it from the pending attach list
	rlv_reattach_map_t::iterator itReattach = m_AttachPending.find(idxAttachPt);
	if ( (itReattach != m_AttachPending.end()) && (itReattach->second == pAttachPt->getItemID()) )
	{
		m_AttachPending.erase(itReattach);
	}

	// Execute any pending commands for this attachment (actually just the root prim; the GC will handle child prims)
	if (LLStartUp::getStartupState() >= STATE_MISC)
	{
		rlv_retained_list_t::iterator itCmd = m_Retained.begin(), curCmd;
		while ( (itCmd = std::find(itCmd, m_Retained.end(), pObj->getID())) != m_Retained.end() )
		{
			curCmd = itCmd++;
			#ifdef RLV_DEBUG
				RLV_INFOS << "Executing retained command for " << curCmd->idObject << LL_ENDL;
			#endif // RLV_DEBUG

			processCommand(curCmd->idObject, curCmd->strCmd);
			m_Retained.erase(curCmd); // Invalidates curCmd but not itCmd since it already points ahead
		}
	}

	// If what we're wearing is located under the shared root then append the attachment point name (if needed)
	const LLUUID& idItem = pAttachPt->getItemID();
	LLViewerInventoryCategory* pRlvRoot = getSharedRoot();
	LLViewerInventoryItem* pItem;
	
	if ( (STATE_STARTED == LLStartUp::getStartupState()) && (pRlvRoot) && (idItem.notNull()) && 
		 (gInventory.isInventoryUsable()) && (gInventory.isObjectDescendentOf(idItem, pRlvRoot->getUUID())) &&
		 ((pItem = gInventory.getItem(idItem)) != NULL) && (pItem->isComplete()) )
	{
		std::string strAttachPt = pAttachPt->getName();
		LLStringUtil::toLower(strAttachPt);

		// If we can modify the item then it should contain the attach point name itself, otherwise its parent should
		if (pItem->getPermissions().allowModifyBy(gAgent.getID()))
		{
			if (!getAttachPoint(pItem, true))
			{
				// It doesn't specify an attach point and we can rename it [see LLItemBridge::renameItem()]
				std::string strName = pItem->getName();
				LLStringUtil::truncate(strName, DB_INV_ITEM_NAME_STR_LEN - strAttachPt.length() - 3);

				strName += " (" + strAttachPt + ")";

				pItem->rename(strName);
				pItem->updateServer(FALSE);
				gInventory.updateItem(pItem);
				//gInventory.notifyObservers(); <- done further down in LLVOAvatar::attachObject()
			}
		}
		else
		{
			// Folder can't be the shared root, or be its direct descendant (= nested at least 2 levels deep)
			LLViewerInventoryCategory* pFolder = gInventory.getCategory(pItem->getParentUUID());
			if ( (pFolder) && 
				 (pFolder->getUUID() != pRlvRoot->getUUID()) && (pFolder->getParentUUID() != pRlvRoot->getUUID()) && 
				 (!getAttachPoint(pFolder, true)) )
			{
				// It's no mod and its parent folder doesn't contain an attach point
				LLInventoryModel::cat_array_t*  pFolders;
				LLInventoryModel::item_array_t* pItems;
				gInventory.getDirectDescendentsOf(pFolder->getUUID(), pFolders, pItems);

				if (pItems)
				{
					int cntObjects = 0;
					for (S32 idxItem = 0, cntItem = pItems->size(); idxItem < cntItem; idxItem++)
					{
						if (LLAssetType::AT_OBJECT == pItems->get(idxItem)->getType())
							cntObjects++;
					}

					// Only rename if there's exactly 1 object/attachment inside of it [see LLFolderBridge::renameItem()]
					if ( (1 == cntObjects) && (NEW_CATEGORY_NAME == pFolder->getName()) )
					{
						std::string strName = ".(" + strAttachPt + ")";

						pFolder->rename(strName);
						pFolder->updateServer(FALSE);
						gInventory.updateCategory(pFolder);
						//gInventory.notifyObservers(); <- done further down in LLVOAvatar::attachObject()
					}
				}
			}
		}
	}
}

// Checked: 2009-05-31 (RLVa-0.2.0e) | Modified: RLVa-0.2.0e
void RlvHandler::onDetach(LLViewerJointAttachment* pAttachPt)
{
	LLViewerObject* pObj = pAttachPt->getObject();
	if (!pObj)
	{
		// LLVOAvatar::detachObject() should call us *before* calling LLViewerJointAttachment::removeObject()
		RLV_ERRS << "pAttachPt->getObject() == NULL" << LL_ENDL;
		return;
	}
	S32 idxAttachPt = getAttachPointIndex(pObj);
	if (0 == idxAttachPt)
	{
		// If we ended up here then the user "Drop"'ed this attachment (which we can't recover from)
		return;
	}

	#ifdef RLV_DEBUG
		// TODO-RLV: when we're exiting (for whatever reason) app state won't always reflect it but 
		//           gAgent.getAvatarObject()->mAttachmentPoints will be NULL so anywhere we use
		//			 "get_if_there" will call through a NULL pointer. One case is "idling out" -> test the rest
		//LLViewerJointAttachment* pDbgAttachmentPt = 
		//	get_if_there(gAgent.getAvatarObject()->mAttachmentPoints, (S32)idxAttachPt, (LLViewerJointAttachment*)NULL);
		//RLV_INFOS << "Clean up for '" << pDbgAttachmentPt->getName() << "'" << LL_ENDL;
	#endif // RLV_DEBUG

	// If the attachment was locked then we should reattach it (unless we're already trying to reattach to this attachment point)
	if ( (!isDetachable(idxAttachPt)) && (m_AttachPending.find(idxAttachPt) == m_AttachPending.end()) )
	{
		// In an ideal world we would simply set up an LLInventoryObserver but there's no specific "asset updated" changed flag *sighs*
		// NOTE: attachments *always* know their "inventory item UUID" so we don't have to worry about fetched vs unfetched inventory
		m_AttachPending.insert(std::pair<S32, LLUUID>(idxAttachPt, pAttachPt->getItemID()));
	}

	// We can't - easily - clean up child prims that never issued @detach=n but the GC will get those eventually
	rlv_detach_map_t::iterator itAttach = m_Attachments.find(idxAttachPt);
	while ( (itAttach != m_Attachments.upper_bound(idxAttachPt)) && (itAttach != m_Attachments.end()) )
	{
		LLViewerObject* pTempObj = gObjectList.findObject(itAttach->second);
		if ( (pTempObj) && (pTempObj->getRootEdit()->getID() == pObj->getID()) )
		{
			// Iterator points to the object (or to a child prim) so issue a clear on behalf of the object (there's the 
			// possibility of going into an eternal loop, but that's ok since it indicates a bug in @clear that needs fixing)
			processCommand(itAttach->second, "clear");

			itAttach = m_Attachments.find(idxAttachPt); // @clear will invalidate all iterators so we have to start anew
		}
		else
		{
			itAttach++;
		}
	}

	// Clean up in case there was never a @detach=n (only works for the root prim - see above)
	rlv_object_map_t::iterator itObj = m_Objects.find(pObj->getID());
	if (itObj != m_Objects.end())
		processCommand(itObj->second.m_UUID, "clear");
}

bool RlvHandler::onGC()
{
	// Check for any expired objects
	rlv_object_map_t::iterator itObj = m_Objects.begin();
	while (itObj != m_Objects.end())
	{
		if (NULL == gObjectList.findObject(itObj->second.m_UUID))
		{
			#ifdef RLV_DEBUG
				RLV_INFOS << "Garbage collecting " << itObj->second.m_UUID << LL_ENDL;
			#endif // RLV_DEBUG

			processCommand(itObj->second.m_UUID, "clear");
			itObj = m_Objects.begin();
		}
		else
			++itObj;
	}

	// Check the retained command list
	rlv_retained_list_t::iterator itCmd = m_Retained.begin(), curCmd;
	while (itCmd != m_Retained.end())
	{
		curCmd = itCmd++; // curCmd iterator becomes invalid on deletion; itCmd points ahead so we can keep using that one

		RlvRetainedCommand& rlvCmd = *curCmd; bool fExecute = false;
		if (rlvCmd.fImmediate)
			fExecute = true; // We can't retain reply commands indefinitely because we'd time out impatient scripts
		else if (NULL != gObjectList.findObject(rlvCmd.idObject))
			fExecute = true; // We know about the object, so we're no longer in danger of GC'ing it prematurely
		else
		{
			if (rlvCmd.nLookupMisses < 20)	// We run every 30 seconds so this is about 10 minutes
			{
				rlvCmd.nLookupMisses++;
				#ifdef RLV_DEBUG
					RLV_INFOS << "Holding retained command for " << rlvCmd.idObject 
					          << " (# misses: " << rlvCmd.nLookupMisses << ")" << LL_ENDL;
				#endif // RLV_DEBUG
			}
			else							// While not terribly expensive, we don't want to keep checking indefinitely
			{
				#ifdef RLV_DEBUG
					RLV_WARNS << "Expiring retained command for " << rlvCmd.idObject << LL_ENDL;
				#endif // RLV_DEBUG
				m_Retained.erase(curCmd);
			}
		}

		if (fExecute)
		{
			#ifdef RLV_DEBUG
				RLV_INFOS << "Executing retained command for " << rlvCmd.idObject 
				          << " (fImmediate: " << ( (rlvCmd.fImmediate) ? "true)" : "false)" ) << LL_ENDL;
			#endif // RLV_DEBUG
			processCommand(rlvCmd.idObject, rlvCmd.strCmd);
			m_Retained.erase(curCmd);
		}
	}

	return ( (0 != m_Objects.size()) || (0 != m_Retained.size()) );	// GC will kill itself if it has nothing to do
}

// Checked: 2009-05-31 (RLVa-0.2.0e) | Added: RLVa-0.2.0e
void RlvHandler::onSavedAssetIntoInventory(const LLViewerInventoryItem* pItem)
{
	const LLUUID& idItem = pItem->getUUID();
	for (rlv_reattach_map_t::iterator itAttach = m_AttachPending.begin(); itAttach != m_AttachPending.end(); ++itAttach)
	{
		if (idItem == itAttach->second)
		{
			#if RLV_TARGET < RLV_MAKE_TARGET(1, 23, 0)			// Version: 1.22.11
				LLAttachmentRezAction* rez_action = new LLAttachmentRezAction;
				rez_action->mItemID = itAttach->second;
				rez_action->mAttachPt = itAttach->first;

				confirm_replace_attachment_rez(0/*YES*/, (void*)rez_action); // (Will call delete on rez_action)
			#else												// Version: 1.23.4
				LLSD payload;
				payload["item_id"] = itAttach->second;
				payload["attachment_point"] = itAttach->first;

				LLNotifications::instance().forceResponse(LLNotification::Params("ReplaceAttachment").payload(payload), 0/*YES*/);
			#endif
		}
	}
}

// ============================================================================
// String/chat censoring functions
//

// LL must have included an strlen for UTF8 *somewhere* but I can't seem to find it so this one is home grown
size_t utf8str_strlen(const std::string& utf8)
{
	const char* pUTF8 = utf8.c_str(); size_t length = 0;
	for (int idx = 0, cnt = utf8.length(); idx < cnt ;idx++)
	{
		// We're looking for characters that don't start with 10 as their high bits
		if ((pUTF8[idx] & 0xC0) != 0x80)
			length++;
	}
	return length;
}

// TODO-RLV: works, but more testing won't hurt
std::string utf8str_chtruncate(const std::string& utf8, size_t length)
{
	if (0 == length)
		return std::string();
	if (utf8.length() <= length)
		return utf8;

	const char* pUTF8 = utf8.c_str(); int idx = 0;
	while ( (pUTF8[idx]) && (length > 0) )
	{
		// We're looking for characters that don't start with 10 as their high bits
		if ((pUTF8[idx] & 0xC0) != 0x80)
			length--;
		idx++;
	}

	return utf8.substr(0, idx);
}

// Checked: 2009-07-09 (RLVa-1.0.0f) | Modified: RLVa-1.0.0f
void RlvHandler::filterChat(std::string& strUTF8Text, bool fFilterEmote) const
{
	if (strUTF8Text.empty())
		return;

	if (rlvIsEmote(strUTF8Text))					// Check if it's an emote
	{
		if (fFilterEmote)							// Emote filtering depends on fFilterEmote
		{
			if ( (strUTF8Text.find_first_of("\"()*=^_?~") != -1) || 
				 (strUTF8Text.find(" -") != -1) || (strUTF8Text.find("- ") != -1) || (strUTF8Text.find("''") != -1) )
			{
				strUTF8Text = "...";				// Emote contains illegal character (or character sequence)
			}
			else if (!hasBehaviour("emote"))
			{
				int idx = strUTF8Text.find('.');	// Truncate at 20 characters or at the dot (whichever is shorter)
				strUTF8Text = utf8str_chtruncate(strUTF8Text, ( (idx > 0) && (idx < 20) ) ? idx + 1 : 20);
			}
		}
	} 
	else if (strUTF8Text[0] == '/')					// Not an emote, but starts with a '/'
	{
		if (utf8str_strlen(strUTF8Text) > 7)		// Allow as long if it's 6 characters or less
			strUTF8Text = "...";
	}
	else if ((strUTF8Text.length() < 4) || (strUTF8Text.compare(0, 2, "((")) || (strUTF8Text.compare(strUTF8Text.length() - 2, 2, "))")))
	{
		strUTF8Text = "...";						// Regular chat (not OOC)
	}
}

// Checked: 2009-07-04 (RLVa-1.0.0a) | Modified: RLVa-1.0.0a
void RlvHandler::filterLocation(std::string& strUTF8Text) const
{
	// TODO-RLVa: if either the region or parcel name is a simple word such as "a" or "the" then confusion will ensue?
	//            -> not sure how you would go about preventing this though :|...

	// Filter any mention of the surrounding region names
	LLWorld::region_list_t regions = LLWorld::getInstance()->getRegionList();
	for (LLWorld::region_list_t::const_iterator itRegion = regions.begin(); itRegion != regions.end(); ++itRegion)
		rlvStringReplace(strUTF8Text, (*itRegion)->getName(), rlv_handler_t::cstrHiddenRegion);

	// Filter any mention of the parcel name
	LLViewerParcelMgr* pParcelMgr = LLViewerParcelMgr::getInstance();
	if (pParcelMgr)
		rlvStringReplace(strUTF8Text, pParcelMgr->getAgentParcelName(), rlv_handler_t::cstrHiddenParcel);
}

void RlvHandler::filterNames(std::string& strUTF8Text) const
{
	std::string strFirstName, strLastName, strName;

	// TODO-RLV: make this a bit more efficient (ie people with a large draw distance will have a load of active regions)
	//   -> the cost of multi string matching them all at once seems to be about the same as calling rlvStringReplace 
	//      twice so that would be a tremendous gain (and we'd get first name and word matching for free)
	#if RLV_TARGET < RLV_MAKE_TARGET(1, 23, 0)			// Version: 1.22.11
		for (LLWorld::region_list_t::const_iterator itRegion = LLWorld::getInstance()->mActiveRegionList.begin();
			 itRegion != LLWorld::getInstance()->mActiveRegionList.end(); ++itRegion)
		{
			LLViewerRegion* pRegion = *itRegion;
			
			for (S32 idxAgent = 0, cntAgent = pRegion->mMapAvatars.count(); idxAgent < cntAgent; idxAgent++)
			{
				// LLCacheName::getName() will add the UUID to the lookup queue if we don't know it yet
				if (gCacheName->getName(pRegion->mMapAvatarIDs.get(idxAgent), strFirstName, strLastName))
				{
					strName = strFirstName + " " + strLastName;

					rlvStringReplace(strUTF8Text, strName, getAnonym(strName));
				}
			}
		}
	#else												// Version: trunk
		// TODO-RLV: should restrict this to a certain radius (probably 1-2 sim range?)
		std::vector<LLUUID> idAgents;
		LLWorld::getInstance()->getAvatars(&idAgents, NULL);

		for (int idxAgent = 0, cntAgent = idAgents.size(); idxAgent < cntAgent; idxAgent++)
		{
			// LLCacheName::getName() will add the UUID to the lookup queue if we don't know it yet
			if (gCacheName->getName(idAgents[idxAgent], strFirstName, strLastName))
			{
				strName = strFirstName + " " + strLastName;

				rlvStringReplace(strUTF8Text, strName, getAnonym(strName));
			}
		}
	#endif
}

const std::string& RlvHandler::getAnonym(const std::string& strName) const
{
	const char* pszName = strName.c_str();
	U32 nHash = 0;
	
	// Test with 11,264 SL names showed a 3.33% - 3.82% occurance for each so we *should* get a very even spread
	for (int idx = 0, cnt = strName.length(); idx < cnt; idx++)
		nHash += pszName[idx];

	return cstrAnonyms[nHash % 28];
}

// Checked: 2009-07-07 (RLVa-1.0.0d) | Modified: RLVa-0.2.2a
bool RlvHandler::redirectChatOrEmote(const std::string& strUTF8Text) const
{
	// Sanity check - @redirchat only for chat and @rediremote only for emotes
	bool fIsEmote = rlvIsEmote(strUTF8Text);
	if ( ((!fIsEmote) && (!hasBehaviour(RLV_BHVR_REDIRCHAT))) || ((fIsEmote) && (!hasBehaviour(RLV_BHVR_REDIREMOTE))) )
		return false;

	if (!fIsEmote)
	{
		std::string strText = strUTF8Text;
		filterChat(strText, true);
		if (strText != "...")
			return false;	// @sendchat wouldn't filter it so @redirchat won't redirect it either
	}

	bool fSendChannel = hasBehaviour("sendchannel");
	for (rlv_object_map_t::const_iterator itObj = m_Objects.begin(); itObj != m_Objects.end(); ++itObj)
	{
		for (rlv_command_list_t::const_iterator itCmd = itObj->second.m_Commands.begin(), 
				endCmd = itObj->second.m_Commands.end(); itCmd != endCmd; ++itCmd)
		{
			if ( ( ((!fIsEmote) && (RLV_BHVR_REDIRCHAT == itCmd->getBehaviourType())) ||
				   ((fIsEmote) && (RLV_BHVR_REDIREMOTE == itCmd->getBehaviourType())) ) && 
				 ( (!fSendChannel) || (hasBehaviour("sendchannel", itCmd->getOption())) ) )
			{
				sendCommandReply(itCmd->getOption(), strUTF8Text);
			}
		}
	}
	return true;
}

// ============================================================================
// Public service functions (called by the outside world or by extension handlers)
//

BOOL RlvHandler::isAgentNearby(const LLUUID& uuid) const
{
	#if RLV_TARGET < RLV_MAKE_TARGET(1, 23, 0)			// Version: 1.22.11
		for (LLWorld::region_list_t::const_iterator itRegion = LLWorld::getInstance()->mActiveRegionList.begin();
			 itRegion != LLWorld::getInstance()->mActiveRegionList.end(); ++itRegion)
		{
			LLViewerRegion* pRegion = *itRegion;
			
			for (S32 idxAgent = 0, cntAgent = pRegion->mMapAvatars.count(); idxAgent < cntAgent; idxAgent++)
				if (pRegion->mMapAvatarIDs.get(idxAgent) == uuid)
					return TRUE;
		}
	#else												// Version: trunk
		// TODO-RLV: rewrite this to fit trunk, but still need the radius limited to a sane range
		std::vector<LLUUID> idAgents;
		LLWorld::getInstance()->getAvatars(&idAgents, NULL);

		for (int idxAgent = 0, cntAgent = idAgents.size(); idxAgent < cntAgent; idxAgent++)
		{
			if (idAgents[idxAgent] == uuid)
				return TRUE;
		}
	#endif
	return FALSE;
}

void RlvHandler::sendBusyMessage(const LLUUID& idTo, const std::string& strMsg, const LLUUID& idSession)
{
	// (See process_improved_im)
	std::string strFullName;
	gAgent.buildFullname(strFullName);

	pack_instant_message(gMessageSystem, gAgent.getID(), FALSE, gAgent.getSessionID(), idTo, strFullName,
		strMsg, IM_ONLINE, IM_BUSY_AUTO_RESPONSE, idSession);
	gAgent.sendReliableMessage();
}

// Checked:
BOOL RlvHandler::sendCommandReply(const std::string& strChannel, const std::string& strReply) const
{
	// Sanity checking - channel should be positive and non-zero (the debug channel will show as public chat so block that as well)
	S32 nChannel;
	if ( (!LLStringUtil::convertToS32(strChannel, nChannel)) || (nChannel <= 0) || (CHAT_CHANNEL_DEBUG == nChannel) )
		return FALSE;

	m_fReplyInProgress = TRUE;
	gChatBar->sendChatFromViewer("/" + strChannel + " " + strReply, CHAT_TYPE_SHOUT, FALSE);
	m_fReplyInProgress = FALSE;
	return TRUE;
}

// ============================================================================
// General purpose inventory functions
//

// Checked: 2009-07-12 (RLVa-1.0.0h)
class RlvSharedRootFetcher : public LLInventoryFetchDescendentsObserver
{
public:
	RlvSharedRootFetcher() {}

	virtual void done()
	{
		RLV_INFOS << "Shared folders fetch completed" << LL_ENDL;
		RlvHandler::m_fFetchComplete = TRUE;

		gInventory.removeObserver(this);
		delete this;
	}
};

// Checked: 2009-07-12 (RLVa-1.0.0h)
void RlvHandler::fetchSharedInventory()
{
	// Sanity check - don't fetch if we're already fetching, or if we don't have a shared root
	LLViewerInventoryCategory* pRlvRoot = getSharedRoot();
	if ( (m_fFetchStarted) || (!pRlvRoot) )
		return;

	// Grab all the folders under the shared root
	LLInventoryModel::cat_array_t  folders;
	LLInventoryModel::item_array_t items;
	gInventory.collectDescendents(pRlvRoot->getUUID(), folders, items, FALSE);

	/*
	 * Add them to the "to fetch" list
	 */ 
	LLInventoryFetchDescendentsObserver::folder_ref_t fetchFolders;

	fetchFolders.push_back(pRlvRoot->getUUID());
	for (S32 idxFolder = 0, cntFolder = folders.count(); idxFolder < cntFolder; idxFolder++)
		fetchFolders.push_back(folders.get(idxFolder)->getUUID());

	/*
	 * Now fetch them all in one go
	 */
	RlvSharedRootFetcher* fetcher = new RlvSharedRootFetcher;

	RLV_INFOS << "Starting fetch of " << fetchFolders.size() << " shared folders" << LL_ENDL;
	fetcher->fetchDescendents(fetchFolders);

	if (fetcher->isEverythingComplete())
		fetcher->done();
	else
		gInventory.addObserver(fetcher);
}

bool RlvHandler::findSharedFolders(const std::string& strCriteria, LLInventoryModel::cat_array_t& folders) const
{
	// Sanity check - can't do anything without a shared root
	LLViewerInventoryCategory* pRlvRoot = getSharedRoot();
	if (!pRlvRoot)
		return false;

	folders.clear();
	LLInventoryModel::item_array_t items;
	RlvCriteriaCategoryCollector functor(strCriteria);
	gInventory.collectDescendentsIf(pRlvRoot->getUUID(), folders, items, FALSE, functor);

	return (folders.count() != 0);
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0e
LLViewerInventoryCategory* RlvHandler::getSharedRoot()
{
	LLInventoryModel::cat_array_t*  pFolders;
	LLInventoryModel::item_array_t* pItems;
	gInventory.getDirectDescendentsOf(gAgent.getInventoryRootID(), pFolders, pItems);
	if (pFolders)
	{
		// NOTE: we might have multiple #RLV folders so we'll just go with the first one we come across
		LLViewerInventoryCategory* pFolder;
		for (S32 idxFolder = 0, cntFolder = pFolders->count(); idxFolder < cntFolder; idxFolder++)
		{
			if ( ((pFolder = pFolders->get(idxFolder)) != NULL) && (RlvHandler::cstrSharedRoot == pFolder->getName()) )
				return pFolder;
		}
	}
	return NULL;
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0e
LLViewerInventoryCategory* RlvHandler::getSharedFolder(const LLUUID& idParent, const std::string& strFolderName) const
{
	LLInventoryModel::cat_array_t*  pFolders;
	LLInventoryModel::item_array_t* pItems;
	gInventory.getDirectDescendentsOf(idParent, pFolders, pItems);
	if (!pFolders)
		return NULL;

	// If we can't find an exact match then we'll settle for a "starts with" match
	LLViewerInventoryCategory* pPartial = NULL;

	//LLStringUtil::toLower(strFolderName); <- everything was already converted to lower case before

	std::string strName;
	for (S32 idxFolder = 0, cntFolder = pFolders->count(); idxFolder < cntFolder; idxFolder++)
	{
		LLViewerInventoryCategory* pFolder = pFolders->get(idxFolder);

		strName = pFolder->getName();
		LLStringUtil::toLower(strName);

		if (strFolderName == strName)
			return pFolder;		// Found an exact match, no need to keep on going
		else if ( (!pPartial) && (strName.find(strFolderName) == 0) )	// (First partial match wins)
			pPartial = pFolder;	// Found a partial match, but we might still find an exact one
	}

	return pPartial;
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0e
LLViewerInventoryCategory* RlvHandler::getSharedFolder(const std::string& strPath) const
{
	// Sanity check - no shared root => no shared folder
	LLViewerInventoryCategory* pRlvRoot = getSharedRoot(), *pFolder = pRlvRoot;
	if (!pRlvRoot)
		return NULL;

	// Walk the path (starting at the root)
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("/", "", boost::drop_empty_tokens);
	tokenizer tokens(strPath, sep);
	for (tokenizer::iterator itToken = tokens.begin(); itToken != tokens.end(); ++itToken)
	{
		pFolder = getSharedFolder(pFolder->getUUID(), *itToken);
		if (!pFolder)
			return NULL;	// No such folder
	}

	return pFolder;			// If strPath was empty or just a bunch of //// then: pFolder == pRlvRoot
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0g
std::string RlvHandler::getSharedPath(const LLViewerInventoryCategory* pFolder) const
{
	LLViewerInventoryCategory* pRlvRoot = getSharedRoot();
	// Sanity check - no shared root or no folder => no path
	if ( (!pRlvRoot) || (!pFolder) || (pRlvRoot->getUUID() == pFolder->getUUID()) )
		return std::string();

	const LLUUID& idRLV  = pRlvRoot->getUUID();
	const LLUUID& idRoot = gAgent.getInventoryRootID();
	std::string strPath;

	// Walk up the tree until we reach the top
	while (pFolder)
	{
		strPath = "/" + pFolder->getName() + strPath;

		const LLUUID& idParent = pFolder->getParentUUID();
		if (idRLV == idParent)			// Reached the shared root, we're done
			break;
		else if (idRoot == idParent)	// We reached the agent's inventory root (indicative of a logic error elsewhere)
		{
			RLV_ERRS << "Reached agent's inventory root while building path for shared folder" << LL_ENDL;
			return std::string();
		}
		else
			pFolder = gInventory.getCategory(idParent);
	}

	return strPath.erase(0, 1);
}

// ============================================================================
// Composite folders
//

#ifdef RLV_EXPERIMENTAL_COMPOSITES
	// Checked:
	bool RlvHandler::getCompositeInfo(const LLInventoryCategory* pFolder, std::string* pstrName) const
	{
		if (pFolder)
		{
			// Composite folder naming: ^\.?[Folder]
			const std::string& cstrFolder = pFolder->getName();
			int idxStart = cstrFolder.find('['), idxEnd = cstrFolder.find(']', idxStart);
			if ( ((0 == idxStart) || (1 == idxStart)) && (idxEnd - idxStart > 1) )
			{
				if (pstrName)
					pstrName->assign(cstrFolder.substr(idxStart + 1, idxEnd - idxStart - 1));
				return true;
			}
		}
		return false;
	}

	// Checked:
	bool RlvHandler::getCompositeInfo(const LLUUID& idItem, std::string* pstrName, LLViewerInventoryCategory** ppFolder) const
	{
		LLViewerInventoryCategory* pRlvRoot; LLViewerInventoryItem* pItem;

		if ( (idItem.notNull()) && ((pRlvRoot = getSharedRoot()) != NULL) && 
			 (gInventory.isObjectDescendentOf(idItem, pRlvRoot->getUUID())) && ((pItem = gInventory.getItem(idItem)) != NULL) )
		{
			// We know it's an item in a folder under the shared root...
			LLViewerInventoryCategory* pFolder = gInventory.getCategory(pItem->getParentUUID());
			if (getAttachPoint(pFolder, true))
			{
				// ... but it could be named ".(attachpt)" in which case we need its parent
				pFolder = gInventory.getCategory(pFolder->getParentUUID());
			}

			if ( (pFolder) && (getCompositeInfo(pFolder, pstrName)) )
			{
				if (ppFolder)
					*ppFolder = pFolder;
				return true;
			}
		}
		return false;
	}
#endif // RLV_EXPERIMENTAL_COMPOSITES

#ifdef RLV_EXPERIMENTAL_COMPOSITE_FOLDING
	// Checked:
	inline bool RlvHandler::isHiddenCompositeItem(const LLUUID& idItem, const std::string& cstrItemType) const
	{
		// An item that's part of a composite folder will be hidden from @getoutfit and @getattach if:
		//   (1) the composite name specifies either a wearable layer or an attachment point
		//   (2) the specified wearable layer or attachment point is worn and resides in the folder
		//   (3) cstrItemType isn't the specified wearable layer or attach point
		//
		// Example: #RLV/Separates/Shoes/ChiChi Pumps/.[shoes] with items: "Shoe Base", "Shoe (left foot)" and "Shoe (right foot)"
		//   -> as long as "Shoe Base" is worn, @getattach should not reflect "left foot", nor "right foot"
		std::string strComposite; LLViewerInventoryCategory* pFolder;
		EWearableType type; S32 idxAttachPt;
		if ( (getCompositeInfo(idItem, &strComposite, &pFolder)) && (cstrItemType != strComposite) )
		{
			LLUUID idCompositeItem;
			if ((type = LLWearable::typeNameToType(strComposite)) != WT_INVALID)
			{
				idCompositeItem = gAgent.getWearableItem(type);
			}
			else if ((idxAttachPt = getAttachPointIndex(strComposite, true)) != 0)
			{
				LLVOAvatar* pAvatar; LLViewerJointAttachment* pAttachmentPt;
				if ( ((pAvatar = gAgent.getAvatarObject()) != NULL) && 
					 ((pAttachmentPt = get_if_there(pAvatar->mAttachmentPoints, idxAttachPt, (LLViewerJointAttachment*)NULL)) != NULL) )
				{
					idCompositeItem = pAttachmentPt->getItemID();
				}
			}

			if ( (idCompositeItem.notNull()) && (gInventory.isObjectDescendentOf(idCompositeItem, pFolder->getUUID())) )
				return true;
		}
		return false;
	}
#endif // RLV_EXPERIMENTAL_COMPOSITEFOLDING

#ifdef RLV_EXPERIMENTAL_COMPOSITE_LOCKING
	// Checked:
	bool RlvHandler::canTakeOffComposite(const LLInventoryCategory* pFolder) const
	{
		if (!pFolder)		// If there's no folder then there is nothing to take off
			return false;

		LLInventoryModel::cat_array_t  folders;
		LLInventoryModel::item_array_t items;
		RlvWearableItemCollector functor(pFolder->getUUID(), true, false);

		// Grab a list of all the items @detachthis would be detaching/unwearing
		gInventory.collectDescendentsIf(pFolder->getUUID(), folders, items, FALSE, functor);
		if (!items.count())
			return false;	// There are no wearable items in the folder so there is nothing to take off

		LLViewerInventoryItem* pItem;
		for (S32 idxItem = 0, cntItem = items.count(); idxItem < cntItem; idxItem++)
		{
			pItem = items.get(idxItem);

			switch (pItem->getType())
			{
				case LLAssetType::AT_CLOTHING:
					{
						LLWearable* pWearable = gAgent.getWearableFromWearableItem(pItem->getUUID());
						if ( (pWearable) && (!isRemovable(pWearable->getType())) )
							return false;	// If one clothing layer in the composite folder is unremoveable then the entire folder is
					}
					break;
				case LLAssetType::AT_OBJECT:
					{
						LLVOAvatar* pAvatar; LLViewerObject* pObj;
						if ( ((pAvatar = gAgent.getAvatarObject()) != NULL) && 
							 ((pObj = pAvatar->getWornAttachment(pItem->getUUID())) != NULL) && (!isDetachable(pObj)) )
						{
							return false;	// If one attachment in the composite folder is undetachable then the entire folder is
						}
					}
					break;
				#ifdef LL_GNUC
				default:
					break;
				#endif // LL_GNUC
			}
		}
		return true;
	}
#endif // RLV_EXPERIMENTAL_COMPOSITE_LOCKING

// ============================================================================
// Event handlers
//

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
void RlvHandler::onForceDetach(const LLUUID& idObj, const std::string& strOption) const
{
	U16 nParam;
	if (strOption.empty())
	{
		// Simulate right-click / Take Off > Detach All
		LLAgent::userRemoveAllAttachments(NULL);
	}
	else if (m_AttachLookup.getExactMatchParam(strOption, nParam))
	{
		// Simulate right-click / Take Off > Detach > ...
		LLVOAvatar* pAvatar; LLViewerJointAttachment* pAttachmentPt;
		if ( ((pAvatar = gAgent.getAvatarObject()) != NULL) &&	// Make sure we're actually wearing something on the attachment point
			 ((pAttachmentPt = get_if_there(pAvatar->mAttachmentPoints, (S32)nParam, (LLViewerJointAttachment*)NULL)) != NULL) &&
			 (isStrippable(pAttachmentPt->getItemID())) )		// ... and that it's not marked as "nostrip"
		{
			#ifdef RLV_EXPERIMENTAL_COMPOSITES
				// If we're stripping something that's part of a composite folder then we should @detachthis instead
				if (isCompositeDescendent(pAttachmentPt->getItemID()))
				{
					std::string strCmd = "detachthis:" + strOption + "=force";
					#ifdef RLV_DEBUG
						RLV_INFOS << "\t- '" << strOption << "' belongs to composite folder: @" << strCmd << LL_ENDL;
					#endif // RLV_DEBUG
					processForceCommand(idObj, RlvCommand(strCmd));
				}
				else
			#endif // RLV_EXPERIMENTAL_COMPOSITES
				{
					handle_detach_from_avatar(pAttachmentPt);
				}
		}
	}
	else
	{
		// Force detach single folder
		onForceWear(strOption, false, false);
	}
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0d
void RlvHandler::onForceRemOutfit(const LLUUID& idObj, const std::string& strOption) const
{
	EWearableType typeOption = LLWearable::typeNameToType(strOption), type;
	if ( (WT_INVALID == typeOption) && (!strOption.empty()) )
		return;

	// Before we had an option and optionless branch, but with the addition of composites and nostrip there's less duplication this way
	for (int idxType = 0; idxType < WT_COUNT; idxType++)
	{
		type = (EWearableType)idxType;

		// Only strip clothing (that's currently worn and not marked "nostrip")
		if ( (LLAssetType::AT_CLOTHING != LLWearable::typeToAssetType(type)) ||	
			 (!gAgent.getWearable(type)) || (!isStrippable(gAgent.getWearableItem(type))) )
		{
			continue;
		}

		if ( (typeOption == type) || (strOption.empty()) )
		{
			#ifdef RLV_EXPERIMENTAL_COMPOSITES
				// If we're stripping something that's part of a composite folder then we should @detachthis instead
				if (isCompositeDescendent(gAgent.getWearableItem(type)))
				{
					std::string strCmd = "detachthis:" + LLWearable::typeToTypeName(type) + "=force";
					#ifdef RLV_DEBUG
						RLV_INFOS << "\t- '" << LLWearable::typeToTypeName(type) << "' is composite descendent: @" << strCmd << LL_ENDL;
					#endif // RLV_DEBUG
					processForceCommand(idObj, RlvCommand(strCmd));
				}
				else
			#endif // RLV_EXPERIMENTAL_COMPOSITES
				{
					gAgent.removeWearable(type);
				}
		}
	}
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0g
bool RlvHandler::onForceSit(const LLUUID& idObj, const std::string& strOption) const
{
	LLViewerObject* pObject = NULL; LLUUID idTarget(strOption);
	// Sanity checking - we need to know about the object and it should identify a prim/linkset
	if ( (idTarget.isNull()) || ((pObject = gObjectList.findObject(idTarget)) == NULL) || (LL_PCODE_VOLUME != pObject->getPCode()) )
		return false;

	// Don't force sit if:
	//   1) currently sitting and prevented from standing up
	//   2) prevented from sitting
	//   3) @sittp=n restricted (except if @sittp=n was issued by the same prim that's currently force sitting the avie)
	if ( ( (hasBehaviour(RLV_BHVR_UNSIT)) && (gAgent.getAvatarObject()) && (gAgent.getAvatarObject()->mIsSitting) ) || 
		 ( (hasBehaviour(RLV_BHVR_SIT)) ) ||
		 ( (hasBehaviourExcept(RLV_BHVR_SITTP, idObj)) && 
		   (dist_vec_squared(gAgent.getPositionGlobal(), pObject->getPositionGlobal()) > 1.5f * 1.5f) ))
	{
		return false;
	}

	// Copy/paste from handle_sit_or_stand() [see http://wiki.secondlife.com/wiki/AgentRequestSit]
	gMessageSystem->newMessageFast(_PREHASH_AgentRequestSit);
	gMessageSystem->nextBlockFast(_PREHASH_AgentData);
	gMessageSystem->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	gMessageSystem->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	gMessageSystem->nextBlockFast(_PREHASH_TargetObject);
	gMessageSystem->addUUIDFast(_PREHASH_TargetID, pObject->mID);
	// Offset: "a rough position in local coordinates for the edge to sit on"
	// (we might not even be looking at the object so I don't think we can supply the offset to an edge)
	gMessageSystem->addVector3Fast(_PREHASH_Offset, LLVector3::zero);
	pObject->getRegion()->sendReliableMessage();

	return true;
}

void RlvHandler::onForceWear(const std::string& strPath, bool fAttach, bool fMatchAll) const
{
	// See LLWearableBridge::wearOnAvatar(): don't wear anything until initial wearables are loaded, can destroy clothing items
	if (!gAgent.areWearablesLoaded()) 
		return;

	LLViewerInventoryCategory* pFolder = getSharedFolder(strPath);
	if (!pFolder)	// Folder not found = nothing to attach
		return;

	LLInventoryModel::cat_array_t  folders;
	LLInventoryModel::item_array_t items;
	RlvWearableItemCollector functor(pFolder->getUUID(), fAttach, fMatchAll);

	// Grab a list of all the items we'll be wearing/attaching
	gInventory.collectDescendentsIf(pFolder->getUUID(), folders, items, FALSE, functor);

	LLViewerInventoryItem* pItem;
	for (S32 idxItem = 0, cntItem = items.count(); idxItem < cntItem; idxItem++)
	{
		pItem = items.get(idxItem);

		switch (pItem->getType())
		{
			case LLAssetType::AT_CLOTHING:
			case LLAssetType::AT_BODYPART:
				{
					LLWearable* pWearable = gAgent.getWearableFromWearableItem(pItem->getUUID());

					#ifdef RLV_EXPERIMENTAL_COMPOSITE_LOCKING
						// If we're already wearing something on this layer then we have to check if it isn't part of a composite
						// folder that has at least one unremovable item (in which case we can't wear or remove this item)
						LLViewerInventoryCategory* pCompositeFolder;
						if ( (!pWearable) || (!getCompositeInfo(pItem->getUUID(), NULL, &pCompositeFolder)) || 
							 (canTakeOffComposite(pFolder)))
						{
					#endif // RLV_EXPERIMENTAL_COMPOSITE_LOCKING
							if (fAttach)
							{
								// Simulate wearing a clothing item from inventory (right click / "Wear")
								// LLWearableBridge::performAction() => LLWearableBridge::wearOnAvatar() => wear_inventory_item_on_avatar()
								wear_inventory_item_on_avatar(pItem);
							}
							else
							{
								if ( (pWearable) && (LLAssetType::AT_CLOTHING == pItem->getType()) )
									gAgent.removeWearable(pWearable->getType());
							}
					#ifdef RLV_EXPERIMENTAL_COMPOSITE_LOCKING
						}
					#endif // RLV_EXPERIMENTAL_COMPOSITE_LOCKING
				}
				break;
			case LLAssetType::AT_OBJECT:
				{
					LLVOAvatar* pAvatar = gAgent.getAvatarObject();
					LLViewerObject* pObj;

					#ifdef RLV_EXPERIMENTAL_COMPOSITE_LOCKING
						// If we're already wearing something on this attach point then we have to check if it isn't part of a composite
						// folder that has at least one unremovable item (in which case we can't attach or detach this item)
						LLViewerInventoryCategory* pCompositeFolder;
						if ( (pAvatar) && 
							 ( ((pObj = pAvatar->getWornAttachment(pItem->getUUID())) == NULL) || 
							   (!getCompositeInfo(pItem->getUUID(), NULL, &pCompositeFolder)) || (canTakeOffComposite(pFolder)) ) )
						{
					#endif // RLV_EXPERIMENTAL_COMPOSITE_LOCKING
							if (fAttach)
							{
								// Simulate wearing an object to a specific attachment point (copy/paste to suppress replacement dialog)
								// LLAttachObject::handleEvent() => rez_attachment()
								LLViewerJointAttachment* pAttachPt = getAttachPoint(pItem, true);
								if ( (pAttachPt) && (isDetachable(pAttachPt->getObject())) )
								{
									#if RLV_TARGET < RLV_MAKE_TARGET(1, 23, 0)			// Version: 1.22.11
										LLAttachmentRezAction* rez_action = new LLAttachmentRezAction;
										rez_action->mItemID = pItem->getUUID();
										rez_action->mAttachPt = getAttachPointIndex(pAttachPt->getName(), true);

										confirm_replace_attachment_rez(0/*YES*/, (void*)rez_action); // (Will call delete on rez_action)
									#else												// Version: 1.23.4
										LLSD payload;
										payload["item_id"] = pItem->getUUID();
										payload["attachment_point"] = getAttachPointIndex(pAttachPt->getName(), true);

										LLNotifications::instance().forceResponse(
											LLNotification::Params("ReplaceAttachment").payload(payload), 0/*YES*/);
									#endif
								}
							}
							else
							{
								if ( (pAvatar) && ((pObj = pAvatar->getWornAttachment(pItem->getUUID())) != NULL) )
								{
									LLViewerJointAttachment* pAttachment = pAvatar->getTargetAttachmentPoint(pObj);
									if (pAttachment)
										handle_detach_from_avatar(pAttachment);
								}
							}
					#ifdef RLV_EXPERIMENTAL_COMPOSITE_LOCKING
						}
					#endif // RLV_EXPERIMENTAL_COMPOSITE_LOCKING
				}
				break;
			#ifdef LL_GNUC
			default:
				break;
			#endif // LL_GNUC
		}
	}
}

// Checked: 2009-07-12 (RLVa-1.0.0h) | Modified: RLVa-0.2.0g
bool RlvHandler::onGetPath(const LLUUID &uuid, const std::string& strOption, std::string& strReply) const
{
	// Sanity check - no need to go through all this trouble if we don't have a shared root
	LLViewerInventoryCategory* pRlvRoot = getSharedRoot();
	if (!pRlvRoot)
		return false;

	LLUUID idItem;

	// <option> can be a clothing layer
	EWearableType layerType = LLWearable::typeNameToType(strOption);
	if (WT_INVALID != layerType)
	{
		idItem = gAgent.getWearableItem(layerType);
	}
	else
	{
		LLViewerJointAttachment* pAttachPt = NULL;

		// ... or it can be empty
		if (strOption.empty())
		{
			// (in which case we act on the object that issued the command)
			LLViewerObject* pObj = gObjectList.findObject(uuid);
			if ( (pObj) && (pObj->isAttachment()) && (gAgent.getAvatarObject()) )
				pAttachPt = gAgent.getAvatarObject()->getTargetAttachmentPoint(pObj);
		}
		else
		{
			// ... or it can specify an attach point
			pAttachPt = getAttachPoint(strOption, true);
		}

		// If we found something, get its inventory item UUID
		if (pAttachPt)
			idItem = pAttachPt->getItemID();
	}

	// If we found something and it's under the shared root, then get its path
	if ( (!idItem.isNull()) && (gInventory.isObjectDescendentOf(idItem, pRlvRoot->getUUID())) )
	{
		LLInventoryItem* pItem = gInventory.getItem(idItem);
		if (pItem)
		{
			// ... unless the containing folder's name specifies an attach point (or nostrip) in which case we need its parent
			LLViewerInventoryCategory* pFolder = gInventory.getCategory(pItem->getParentUUID());
			#ifdef RLV_EXTENSION_FLAG_NOSTRIP
				if ( (getAttachPoint(pFolder, true)) || (pFolder->getName() == ".("RLV_FOLDER_FLAG_NOSTRIP")") )
			#else
				if (getAttachPoint(pFolder, true))
			#endif // RLV_EXTENSION_FLAG_NOSTRIP
					strReply = getSharedPath(pFolder->getParentUUID());
				else
					strReply = getSharedPath(pFolder);
		}
	}
	return !strReply.empty();
}

struct rlv_wear_info { U32 cntWorn, cntTotal, cntChildWorn, cntChildTotal; };

// Checked: 2009-05-30 (RLVa-0.2.0e) | Modified: RLVa-0.2.0e
void RlvHandler::onGetInvWorn(const std::string& strPath, std::string& strReply) const
{
	// Sanity check - getAvatarObject() can't be NULL [see rlvIsWearingItem()] and the folder should exist and not be hidden
	LLViewerInventoryCategory* pFolder = getSharedFolder(strPath);
	if ((!gAgent.getAvatarObject()) || (!pFolder) || (pFolder->getName().empty()) || (RLV_FOLDER_PREFIX_HIDDEN == pFolder->getName()[0]))
		return;

	// Collect everything @attachall would be attaching
	LLInventoryModel::cat_array_t  folders;
	LLInventoryModel::item_array_t items;
	RlvWearableItemCollector functor(pFolder->getUUID(), true, true);
	gInventory.collectDescendentsIf(pFolder->getUUID(), folders, items, FALSE, functor);

	rlv_wear_info wi = {0};

	// Add all the folders to a lookup map 
	std::map<LLUUID, rlv_wear_info> mapFolders;
	mapFolders.insert(std::pair<LLUUID, rlv_wear_info>(pFolder->getUUID(), wi));
	for (S32 idxFolder = 0, cntFolder = folders.count(); idxFolder < cntFolder; idxFolder++)
		mapFolders.insert(std::pair<LLUUID, rlv_wear_info>(folders.get(idxFolder)->getUUID(), wi));

	// Iterate over all the found items
	LLViewerInventoryItem* pItem; std::map<LLUUID, rlv_wear_info>::iterator itFolder;
	for (S32 idxItem = 0, cntItem = items.count(); idxItem < cntItem; idxItem++)
	{
		pItem = items.get(idxItem);

		// The "folded parent" is the folder this item should be considered a direct descendent of (may or may not match actual parent)
		const LLUUID& idParent = functor.getFoldedParent(pItem->getParentUUID());

		// Walk up the tree: sooner or later one of the parents will be a folder in the map
		LLViewerInventoryCategory* pParent = gInventory.getCategory(idParent);
		while ( (itFolder = mapFolders.find(pParent->getUUID())) == mapFolders.end() )
			pParent = gInventory.getCategory(pParent->getParentUUID());

		U32 &cntWorn  = (idParent == pParent->getUUID()) ? itFolder->second.cntWorn : itFolder->second.cntChildWorn, 
			&cntTotal = (idParent == pParent->getUUID()) ? itFolder->second.cntTotal : itFolder->second.cntChildTotal;

		if (rlvIsWearingItem(pItem))
			cntWorn++;
		cntTotal++;
	}

	// Extract the result for the main folder
	itFolder = mapFolders.find(pFolder->getUUID());
	wi.cntWorn = itFolder->second.cntWorn;
	wi.cntTotal = itFolder->second.cntTotal;
	mapFolders.erase(itFolder);

	// Build the result for each child folder
	for (itFolder = mapFolders.begin(); itFolder != mapFolders.end(); ++itFolder)
	{
		rlv_wear_info& wiFolder = itFolder->second;

		wi.cntChildWorn += wiFolder.cntWorn + wiFolder.cntChildWorn;
		wi.cntChildTotal += wiFolder.cntTotal + wiFolder.cntChildTotal;

		strReply += llformat(",%s|%d%d", gInventory.getCategory(itFolder->first)->getName().c_str(),
		 (0 == wiFolder.cntTotal) ? 0 : (0 == wiFolder.cntWorn) ? 1 : (wiFolder.cntWorn != wiFolder.cntTotal) ? 2 : 3,
		 (0 == wiFolder.cntChildTotal) ? 0 : (0 == wiFolder.cntChildWorn) ? 1 : (wiFolder.cntChildWorn != wiFolder.cntChildTotal) ? 2 : 3
		);
	}

	// Now just prepend the root and done
	strReply = llformat("|%d%d", (0 == wi.cntTotal) ? 0 : (0 == wi.cntWorn) ? 1 : (wi.cntWorn != wi.cntTotal) ? 2 : 3,
		(0 == wi.cntChildTotal) ? 0 : (0 == wi.cntChildWorn) ? 1 : (wi.cntChildWorn != wi.cntChildTotal) ? 2: 3) + strReply;
}

// (In case anyone cares: this isn't used in public builds)
bool RlvHandler::getWornInfo(const LLInventoryCategory* pFolder, U8& wiFolder, U8& wiChildren) const
{
	// Sanity check - getAvatarObject() can't be NULL [see rlvIsWearingItem()] and the folder should exist and not be hidden
	if ((!gAgent.getAvatarObject()) || (!pFolder) || (pFolder->getName().empty()) || (RLV_FOLDER_PREFIX_HIDDEN == pFolder->getName()[0]))
		return false;

	LLInventoryModel::cat_array_t  folders;
	LLInventoryModel::item_array_t items;
	RlvWearableItemCollector functor(pFolder->getUUID(), true, true);
	gInventory.collectDescendentsIf(pFolder->getUUID(), folders, items, FALSE, functor);

	LLViewerInventoryItem* pItem;
	U32 cntWorn = 0, cntTotal = 0, cntChildWorn = 0, cntChildTotal = 0;
	for (S32 idxItem = 0, cntItem = items.count(); idxItem < cntItem; idxItem++)
	{
		pItem = items.get(idxItem);

		bool fDirectDescendent = (pFolder->getUUID() == functor.getFoldedParent(pItem->getParentUUID()));
		U32 &refWorn = (fDirectDescendent) ? cntWorn : cntChildWorn, &refTotal = (fDirectDescendent) ? cntTotal : cntChildTotal;

		if (rlvIsWearingItem(pItem))
			refWorn++;
		refTotal++;
	}

	wiFolder = (0 == cntTotal + cntChildTotal) ? 0 : (0 == cntWorn + cntChildWorn) ? 1 : 
		(cntWorn + cntChildWorn != cntTotal + cntChildTotal) ? 2 : 3;
	wiChildren = (0 == cntChildTotal) ? 0 : (0 == cntChildWorn) ? 1 : (cntChildWorn != cntChildTotal) ? 2 : 3;

	return true;
}

// ============================================================================
// Initialization helper functions
//

BOOL RlvHandler::setEnabled(BOOL fEnable)
{
	if (m_fEnabled == fEnable)
		return fEnable;

	if (fEnable)
	{
		fNoSetEnv = gSavedSettings.getBOOL(RLV_SETTING_NOSETENV);

		RlvCommand::initLookupTable();
		gRlvHandler.addObserver(new RlvExtGetSet());

		if (LLStartUp::getStartupState() >= STATE_CLEANUP)
			fetchSharedInventory();

		m_fEnabled = TRUE;
	}
	else if (canDisable())
	{
		#ifdef RLV_DEBUG
			RLV_INFOS << "Disabling RLV:" << LL_ENDL;
		#endif // RLV_DEBUG

		gRlvHandler.clearState();

		#ifdef RLV_DEBUG
			RLV_INFOS << "\t--> RLV disabled" << LL_ENDL;
		#endif // RLV_DEBUG

		m_fEnabled = FALSE;
	}

	#ifdef RLV_ADVANCED_MENU
		// RELEASE-RLVa: LL defines CLIENT_MENU_NAME but we can't get to it from here so we need to keep those two in sync manually
		LLMenuGL* pClientMenu = NULL;
		if ( (gMenuBarView) && ((pClientMenu = gMenuBarView->getChildMenuByName("Advanced", FALSE)) != NULL) )
		{
			pClientMenu->setItemVisible("RLVa", m_fEnabled);
			pClientMenu->setItemEnabled("RLVa", m_fEnabled);
		}
	#endif // RLV_ADVANCED_MENU

	return m_fEnabled;		// Return enabled/disabled state
}

BOOL RlvHandler::canDisable()
{
	return TRUE;
}

void RlvHandler::clearState()
{
	// TODO-RLVa: should restore all RLV controlled debug variables to their defaults

	// Issue @clear on behalf of every object that has a currently active RLV restriction (even if it's just an exception)
	LLUUID idObj; LLViewerObject* pObj; bool fDetachable;
	while (m_Objects.size())
	{
		idObj = m_Objects.begin()->first; // Need a copy since after @clear the data it points to will no longer exist
		fDetachable = ((pObj = gObjectList.findObject(idObj)) != NULL) ? isDetachable(pObj) : true;

		processCommand(idObj, "clear");
		if (!fDetachable)
			processCommand(idObj, "detachme=force");
	}

	// Sanity check - these should all be empty after we issue @clear on the last object
	if ( (!m_Objects.empty()) || !(m_Exceptions.empty()) || (!m_Attachments.empty()) )
	{
		RLV_ERRS << "Object, exception or attachment map not empty after clearing state!" << LL_ENDL;
		m_Objects.clear();
		m_Exceptions.clear();
		m_Attachments.clear();
	}

	// These all need manual clearing
	memset(m_LayersAdd, 0, sizeof(S16) * WT_COUNT);
	memset(m_LayersRem, 0, sizeof(S16) * WT_COUNT);
	memset(m_Behaviours, 0, sizeof(S16) * RLV_BHVR_COUNT);
	m_Retained.clear();
	m_AttachPending.clear();
	m_Emitter.clearObservers(); // <- calls delete on all active observers

	// Clear dynamically allocated memory
	if (m_pGCTimer)
	{
		delete m_pGCTimer;
		m_pGCTimer = NULL;
	}
	if (m_pWLSnapshot)
	{
		delete m_pWLSnapshot;
		m_pWLSnapshot = NULL;
	}
}

// ============================================================================
