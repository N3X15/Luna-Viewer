#ifndef RLV_HANDLER_H
#define RLV_HANDLER_H

#include "llagentconstants.h"
#include "llappviewer.h"
#include "llformat.h"
#include "llversionviewer.h"
#include "llviewerjointattachment.h"
#include "llviewerobject.h"
#include "llwearable.h"

#include "rlvhelper.h"
#include "rlvevent.h"
#include "rlvmultistringsearch.h"

// ============================================================================
/*
 * RlvHandler
 * ==========
 *
 */

typedef std::map<LLUUID, RlvObject> rlv_object_map_t;
typedef std::multimap<S32, LLUUID> rlv_detach_map_t;
typedef std::map<S32, LLUUID> rlv_reattach_map_t;
typedef std::multimap<LLUUID, ERlvBehaviour> rlv_exception_map_t;

class RlvHandler
{
public:
	RlvHandler();
	~RlvHandler();

	// --------------------------------

	/*
	 * Rule checking functions
	 */
public:
	// Returns a pointer to the attachment point for a supplied parameter
	LLViewerJointAttachment* getAttachPoint(const std::string& strText, bool fExact) const;
	LLViewerJointAttachment* getAttachPoint(const LLInventoryCategory* pFolder, bool fStrict) const;
	LLViewerJointAttachment* getAttachPoint(const LLInventoryItem* pItem, bool fStrict) const;
	S32                      getAttachPointIndex(std::string strText, bool fExact) const;
	S32                      getAttachPointIndex(LLViewerObject* pObj) const;
	S32                      getAttachPointIndex(const LLViewerJointAttachment* pObj) const;
	bool                     hasAttachPointName(const LLInventoryItem* pItem, bool fStrict) const;

	// Returns a pointer to the currently executing command (do *not* save this pointer)
	const RlvCommand* getCurrentCommand() const { return m_pCurCommand; }
	// Returns the UUID of the object we're currently executing a command for
	const LLUUID& getCurrentObject() const		{ return m_idCurObject; }

	// Returns TRUE is at least one object contains the specified behaviour (and optional parameter)
	// NOTE: - to check @detach=n    -> hasLockedAttachment() / hasLockedHUD() / isDetachable()
	//       - to check @addoutfit=n -> isWearable()
	//       - to check @remoutfit=n -> isRemovable()
	//       - to check exceptions   -> isException()
	//  (You *can* use hasBehaviour(); the specialized ones just don't have to iterate over all the objects)
	bool hasBehaviour(ERlvBehaviour eBehaviour) const { return (eBehaviour < RLV_BHVR_COUNT) ? (0 != m_Behaviours[eBehaviour]) : false; }
	bool hasBehaviour(const std::string& strBehaviour) const;
	bool hasBehaviour(ERlvBehaviour eBehaviour, const std::string& strOption) const;
	bool hasBehaviour(const std::string& strBehaviour, const std::string& strOption) const;

	// Returns TRUE if at least one object (except the specified one) contains the specified behaviour
	bool hasBehaviourExcept(ERlvBehaviour eBehaviour, const LLUUID& idObj) const;
	bool hasBehaviourExcept(const std::string& strBehaviour, const LLUUID& uuid) const;
	bool hasBehaviourExcept(ERlvBehaviour eBehaviour, const std::string& strOption, const LLUUID& idObj) const;
	bool hasBehaviourExcept(const std::string& strBehaviour, const std::string& strOption, const LLUUID& idObj) const;

	// Returns TRUE if there is at least 1 undetachable attachment
	bool hasLockedAttachment() const { return (0 != m_Attachments.size()); }
	// Returns TRUE if there is at least 1 undetachable HUD attachment
	bool hasLockedHUD() const;

	// Returns TRUE if the attachment on the specified attachment point is detachable
	bool isDetachable(S32 idxAttachPt) const { return (idxAttachPt) && (m_Attachments.find(idxAttachPt) == m_Attachments.end()); }
	bool isDetachable(const LLInventoryItem* pItem) const;
	bool isDetachable(LLViewerObject* pObj) const;
	// Marks the specified attachment point as (un)detachable (return value indicates success ; used by unit tests)
	bool setDetachable(S32 idxAttachPt, const LLUUID& idRlvObj, bool fDetachable);
	bool setDetachable(LLViewerObject* pObj, const LLUUID& idRlvObj, bool fDetachable);

	// Returns TRUE is the specified UUID is exempt from a restriction (tplure/sendim/recvim/etc)
	bool isException(ERlvBehaviour eBehaviour, const LLUUID& uuid) const;
	bool isException(const std::string& strBehaviour, const LLUUID& uuid) const;

	// Returns TRUE if the specified layer is removable (use hasBehaviour(RLV_BHVR_REMOUTFIT) for the general case)
	bool isRemovable(EWearableType type) const	{ return (type < WT_COUNT) ? (0 == m_LayersRem[type]) : true; }
	// Returns TRUE if the specified layer is not remoutfit blocked by any object (except the one specified by UUID)
	bool isRemovableExcept(EWearableType type, const LLUUID& idObj) const;
	// Returns TRUE if the specified layer is wearable (use hasBehaviour(RLV_BHVR_ADDOUTFIT) for the general case)
	bool isWearable(EWearableType type) const	{ return (type < WT_COUNT) ? (0 == m_LayersAdd[type]) : true; }

	// Returns TRUE if the composite folder doesn't contain any "locked" items
	bool canTakeOffComposite(const LLInventoryCategory* pFolder) const;
	// Returns TRUE if the folder is a composite folder and optionally returns the name
	bool getCompositeInfo(const LLInventoryCategory* pFolder, std::string* pstrName) const;
	// Returns TRUE if the inventory item belongs to a composite folder and optionally returns the name and composite folder
	bool getCompositeInfo(const LLUUID& idItem, std::string* pstrName, LLViewerInventoryCategory** ppFolder) const;
	// Returns TRUE if the folder is a composite folder
	bool isCompositeFolder(const LLInventoryCategory* pFolder) const;
	// Returns TRUE if the inventory item belongs to a composite folder
	bool isCompositeDescendent(const LLUUID& idItem) const;
	// Returns TRUE if the inventory item is part of a folded composite folder and should be hidden from @getoufit or @getattach
	bool isHiddenCompositeItem(const LLUUID& idItem, const std::string& strItemType) const;

	// Returns TRUE if the inventory item is strippable by @detach or @remoutfit
	bool isStrippable(const LLUUID& idItem) const;

	// Returns TRUE if the specified object can 
	bool canShowHoverText(LLViewerObject* pObj) const;

	// --------------------------------

	/*
	 * Helper functions 
	 */
public:
	// Accessors
	bool isReplyInProgress() const		{ return m_fReplyInProgress; }		// send_chat_from_viewer()
	bool getCanCancelTp() const			{ return m_fCanCancelTp; }			// @accepttp and @tpto
	void setCanCancelTp(bool fAllow)	{ m_fCanCancelTp = fAllow; }		// @accepttp and @tpto

	// Command specific helper functions
	void               filterChat(std::string& strUTF8Text) const;					// @sendchat, @recvchat and @redirchat
	void               filterLocation(std::string& strUTF8Text) const;				// @showloc
	void               filterNames(std::string& strUTF8Text) const;					// @shownames
	const std::string& getAnonym(const std::string& strName) const;					// @shownames
	std::string        getVersionString() const;									// @version
	BOOL               isAgentNearby(const LLUUID& uuid) const;						// @shownames
	bool               redirectChatOrEmote(const std::string& strUTF8Test) const;	// @redirchat and @rediremote

	// Externally invoked event handlers
	void onAttach(LLViewerJointAttachment* pAttachPt);						// LLVOAvatar::attachObject()
	void onDetach(LLViewerJointAttachment* pAttachPt);						// LLVOAvatar::detachObject()
	bool onGC();															// RlvGCTimer::tick()
	void onSavedAssetIntoInventory(const LLViewerInventoryItem* pItem);		// LLInventoryModel::processSaveAssetIntoInventory()

	// Command processing helper functions
	BOOL processCommand(const LLUUID& uuid, const std::string& strCommand);
	void retainCommand(const LLUUID& uuid, const std::string& strCmd);
	static void sendBusyMessage(const LLUUID& idTo, const std::string& strMsg, const LLUUID& idSession = LLUUID::null);
	BOOL sendCommandReply(const std::string& strChannel, const std::string& strReply) const;

	// Initialization
	static BOOL canDisable();
	static BOOL isEnabled()	{ return m_fEnabled; }
	static void initLookupTables();
	static BOOL setEnabled(BOOL fEnable);
protected:
	void clearState();

	// --------------------------------

	/*
	 * Inventory related functions
	 */
public:
	// Starts a fetch of everything under the shared root (if there is one)
	static void fetchSharedInventory();
	// Returns the path of the supplied folder (relative to the shared root)
	std::string getSharedPath(const LLViewerInventoryCategory* pFolder) const;
	std::string getSharedPath(const LLUUID& idFolder) const;
	// Returns a pointer to the shared root folder (if there is one)
	static LLViewerInventoryCategory* getSharedRoot();
protected:
	// Find all folders that match a supplied criteria (clears the supplied array)
	bool findSharedFolders(const std::string& strCriteria, LLInventoryModel::cat_array_t& folders) const;

	// Returns a subfolder of idParent that starts with name (exact match > partial match)
	LLViewerInventoryCategory* getSharedFolder(const LLUUID& idParent, const std::string& strName) const;
	// Looks up a folder from a path (relative to the shared root)
	LLViewerInventoryCategory* getSharedFolder(const std::string& strPath) const;

	bool getWornInfo(const LLInventoryCategory* pFolder, U8& wiFolder, U8& wiChildren) const;

	// --------------------------------

	/*
	 * Event handling (forwards to registered observers if we don't handle the command)
	 */
public:
	BOOL addObserver(RlvObserver* pObserver)	{ return m_Emitter.addObserver(pObserver); }
	BOOL removeObserver(RlvObserver* pObserver)	{ return m_Emitter.remObserver(pObserver); }
	void addBehaviourObserver(RlvBehaviourObserver* pBhvrObserver);
	void removeBehaviourObserver(RlvBehaviourObserver* pBhvrObserver);
	void notifyBehaviourObservers();
protected:
	BOOL processAddCommand(const LLUUID& uuid, const RlvCommand& rlvCmd);
	BOOL processRemoveCommand(const LLUUID& uuid, const RlvCommand& rlvCmd);
	BOOL processReplyCommand(const LLUUID& uuid, const RlvCommand& rlvCmd) const;
	BOOL processForceCommand(const LLUUID& uuid, const RlvCommand& rlvCmd) const;

	/*
	 * Event handlers (exist for no other reason than to keep the length of the processXXX functions down)
	 */
	void onForceDetach(const LLUUID& idObj, const std::string& strOption) const;
	void onForceRemOutfit(const LLUUID& idObj, const std::string& strOption) const;
	bool onForceSit(const LLUUID& uuid, const std::string& strOption) const;
	void onForceWear(const std::string& strPath, bool fAttach, bool fMatchAll) const;
	bool onGetPath(const LLUUID& uuid, const std::string& strOption, std::string& strReply) const;
	void onGetInvWorn(const std::string& strPath, std::string &strReply) const;

	// --------------------------------

	/*
	 * Member variables
	 */
public:
	static BOOL fNoSetEnv;
	static BOOL fDbgEnableWear;
	static BOOL fHideLockedLayers;
	static BOOL fHideLockedAttach;
	static BOOL fHideLockedInventory;

	static const std::string cstrSharedRoot;		// Name of the shared root folder
	static const std::string cstrBlockedRecvIM;		// Stand-in text for incoming IM when recvim restricted
	static const std::string cstrBlockedSendIM;		// Stand-in text for outgoing IM when sendim restricted
	static const std::string cstrHidden;			// General purpose "this was censored" text
	static const std::string cstrHiddenParcel;
	static const std::string cstrHiddenRegion;
	static const std::string cstrMsgRecvIM;			// Message sent to IM sender when sendim restricted
	static const std::string cstrMsgTpLure;			// Message sent to tplure sender when tplure restricted
	static const std::string cstrAnonyms[28];
protected:
	rlv_object_map_t     m_Objects;					// Map of objects that have active restrictions (by UUID)
	rlv_exception_map_t  m_Exceptions;              // Map of UUIDs that are exempt from the associated ERlvBehaviour
	rlv_detach_map_t     m_Attachments;				// Map of locked attachments (attachment point index -> object that issued @detach=n)
	S16                  m_LayersAdd[WT_COUNT];		// Array of locked layers (reference counted)
	S16                  m_LayersRem[WT_COUNT];		// Array of locked layers (reference counted)
	S16                  m_Behaviours[RLV_BHVR_COUNT];
	std::map<S32, S16>   m_AttachShowHoverText;

	rlv_retained_list_t  m_Retained;
	rlv_reattach_map_t   m_AttachPending;
	RlvGCTimer*          m_pGCTimer;
	RlvWLSnapshot*       m_pWLSnapshot;

	RlvCommand*          m_pCurCommand;				// Convenience (see @tpto)
	LLUUID               m_idCurObject;				// Convenience (see @tpto)

	mutable RlvEventEmitter<RlvObserver>     m_Emitter;
	mutable std::list<RlvBehaviourObserver*> m_BhvrObservers;

	static  BOOL				 m_fEnabled;		// Use setEnabled() to toggle this
	mutable BOOL				 m_fReplyInProgress;// See sendCommandReply
	static	BOOL				 m_fFetchStarted;	// TRUE if we fired off an inventory fetch
	static  BOOL				 m_fFetchComplete;	// TURE if everything was fetched
	static  RlvMultiStringSearch m_AttachLookup;	// Lookup table for attachment names (lower case)

	bool m_fCanCancelTp;

	friend class RlvSharedRootFetcher;				// Fetcher needs access to m_fFetchComplete
	friend class RlvGCTimer;						// Timer clear its own point at destruction

	// --------------------------------

	/*
	 * Internal access functions used by unit tests
	 */
public:
	const rlv_object_map_t*    getObjectMap() const		{ return &m_Objects; }
	const rlv_exception_map_t* getExceptionMap() const	{ return &m_Exceptions; }
	const rlv_detach_map_t*    getDetachMap() const		{ return &m_Attachments; }
	#ifdef RLV_DEBUG_TESTS
		const S16*                 getAddLayers() const		{ return m_LayersAdd; }
		const S16*                 getRemLayers() const		{ return m_LayersRem; }
		const S16*                 getBehaviours() const	{ return m_Behaviours; }
		const rlv_retained_list_t* getRetainedList() const	{ return &m_Retained; }
	#endif // RLV_DEBUG_TESTS
};

#ifndef RLV_USE_PIMPL
	typedef RlvHandler rlv_handler_t;
	extern rlv_handler_t gRlvHandler;
#endif // RLV_USE_PIMPL

// ============================================================================
// Inlined member functions
//

// Checked: 2009-07-01 (RLVa-0.2.2a) | Added: RLVa-0.2.2a
inline bool RlvHandler::canShowHoverText(LLViewerObject *pObj) const
{
	#ifdef RLV_EXPERIMENTAL_119
		return !( ( (hasBehaviour(RLV_BHVR_SHOWHOVERTEXTALL)) && (LL_PCODE_VOLUME == pObj->getPCode()) ) || 
				  ( (hasBehaviour(RLV_BHVR_SHOWHOVERTEXTHUD)) && (pObj->isHUDAttachment()) ) ||
				  ( (hasBehaviour(RLV_BHVR_SHOWHOVERTEXT)) &&
					( ( (pObj->isAttachment()) && (pObj->permYouOwner()) && 
						(m_AttachShowHoverText.find(ATTACHMENT_ID_FROM_STATE(pObj->getState())) != m_AttachShowHoverText.end()) ) ||
					  (isException(RLV_BHVR_SHOWHOVERTEXT, pObj->getID())) ) ) );
	#else
		return true;
	#endif // RLV_EXPERIMENTAL_119
}

// Checked: 2009-05-23 (RLVa-0.2.0d) | Modified: RLVa-0.2.0d
inline S32 RlvHandler::getAttachPointIndex(std::string strText, bool fExact) const
{
	U16 nParam; RlvMultiStringSearchMatch match;
	LLStringUtil::toLower(strText);
	return (fExact) ? ((m_AttachLookup.getExactMatchParam(strText, nParam)) ? nParam : 0)
	                : ((m_AttachLookup.findLast(strText, match)) ? match.nParam : 0);
}

// Checked: 2009-05-23 (RLVa-0.2.0d) | Modified: RLVa-0.2.0d
inline S32 RlvHandler::getAttachPointIndex(LLViewerObject* pObj) const
{
	return (pObj) ? ATTACHMENT_ID_FROM_STATE(pObj->getState()) : 0;
}

// Checked: 2009-06-02 (RLVa-0.2.0g)
inline std::string RlvHandler::getSharedPath(const LLUUID& idFolder) const
{
	return getSharedPath(gInventory.getCategory(idFolder)); // getSharedPath() has a NULL pointer check so this is safe
}

// Checked: 2009-06-07 (RLVa-0.2.1c)
inline std::string RlvHandler::getVersionString()  const
{
	return llformat("RestrainedLife viewer v%d.%d.%d (%s %d.%d.%d - RLVa %d.%d.%d)",
		RLV_VERSION_MAJOR, RLV_VERSION_MINOR, RLV_VERSION_PATCH,
		LLAppViewer::instance()->getSecondLifeTitle().c_str(), LL_VERSION_MAJOR, LL_VERSION_MINOR, LL_VERSION_PATCH,
		RLVa_VERSION_MAJOR, RLVa_VERSION_MINOR, RLVa_VERSION_PATCH);
}

// Checked: 2009-05-23 (RLVa-0.2.0d) | Modified: RLVa-0.2.0d
inline bool RlvHandler::hasAttachPointName(const LLInventoryItem *pItem, bool fStrict) const
{
	return (getAttachPoint(pItem, fStrict) != NULL); // getAttachPoint() has a NULL pointer check so this is safe
}

// Checked:
inline bool RlvHandler::hasBehaviour(ERlvBehaviour eBehaviour, const std::string& strOption) const
{
	return hasBehaviourExcept(eBehaviour, strOption, LLUUID::null);
}

// Checked:
inline bool RlvHandler::hasBehaviour(const std::string& strBehaviour) const
{
	return hasBehaviourExcept(strBehaviour, LLUUID::null);
}

// Checked:
inline bool RlvHandler::hasBehaviour(const std::string& strBehaviour, const std::string& strOption) const
{
	return hasBehaviourExcept(strBehaviour, strOption, LLUUID::null);
}

// Checked:
inline bool RlvHandler::hasBehaviourExcept(ERlvBehaviour eBehaviour, const LLUUID& idObj) const
{
	return hasBehaviourExcept(eBehaviour, std::string(), idObj);
}

#ifdef RLV_EXPERIMENTAL_COMPOSITES
	// Checked:
	inline bool RlvHandler::isCompositeFolder(const LLInventoryCategory* pFolder) const
	{
		return getCompositeInfo(pFolder, NULL);
	}

	// Checked:
	inline bool RlvHandler::isCompositeDescendent(const LLUUID& idItem) const
	{
		return getCompositeInfo(idItem, NULL, NULL);
	}
#endif // RLV_EXPERIMENTAL_COMPOSITES

// Checked: 2009-05-23 (RLVa-0.2.0d) | Modified: RLVa-0.2.0d
inline bool RlvHandler::isDetachable(LLViewerObject* pObj) const
{
	return (pObj == NULL) || (!pObj->isAttachment()) || (isDetachable(getAttachPointIndex(pObj)));
}

// Checked:
inline bool RlvHandler::isException(ERlvBehaviour eBehaviour, const LLUUID& uuid) const
{
	for (rlv_exception_map_t::const_iterator itException = m_Exceptions.lower_bound(uuid), 
			endException = m_Exceptions.upper_bound(uuid); itException != endException; ++itException)
	{
		if (itException->second == eBehaviour)
			return true;
	}
	return false;
}

// Checked:
inline bool RlvHandler::isException(const std::string& strBehaviour, const LLUUID& uuid) const
{
	return hasBehaviour(strBehaviour, uuid.asString());
}

// Checked: 2009-05-23 (RLVa-0.2.0d) | Added: RLVa-0.2.0d
inline bool RlvHandler::isRemovableExcept(EWearableType type, const LLUUID& idObj) const
{
	// NOTE: mind the bitwise OR rather than the logical OR!!
	return (isRemovable(type)) || !( (hasBehaviourExcept(RLV_BHVR_REMOUTFIT, idObj)) |
		                             (hasBehaviourExcept(RLV_BHVR_REMOUTFIT, LLWearable::typeToTypeName(type), idObj)) );
}

#ifndef RLV_EXTENSION_FLAG_NOSTRIP
	// Checked: 2009-05-23 (RLVa-0.2.0d) | Added: RLVa-0.2.0d
	bool RlvHandler::isStrippable(const LLUUID& idItem) const
	{
		return true;
	}
#endif // RLV_EXTENSION_FLAG_NOSTRIP

// Checked:
inline void RlvHandler::retainCommand(const LLUUID& uuid, const std::string& strCmd)
{
	#ifdef RLV_DEBUG
		RLV_INFOS << "[" << uuid << "]: " << strCmd << " (retaining)" << LL_ENDL;
	#endif // RLV_DEBUG
	m_Retained.push_back(RlvRetainedCommand(uuid, strCmd));
}

// Checked: 2009-05-23 (RLVa-0.2.0d) | Modified: RLVa-0.2.0d
inline bool RlvHandler::setDetachable(LLViewerObject* pObj, const LLUUID& idRlvObj, bool fDetachable)
{
	return setDetachable(getAttachPointIndex(pObj), idRlvObj, fDetachable); // getAttachPointIndex() has a NULL pointer check
}

// ============================================================================

#endif // RLV_HANDLER_H
