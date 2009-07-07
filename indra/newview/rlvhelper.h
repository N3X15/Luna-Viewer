#ifndef RLV_HELPER_H
#define RLV_HELPER_H

#include "llinventorymodel.h"
#include "llselectmgr.h"
#include "llviewercontrol.h"
#include "llwlparamset.h"

#include <boost/tokenizer.hpp>

#include "rlvmultistringsearch.h"

// ============================================================================
// Defines
//

#define RLV_ROOT_FOLDER				"#RLV"
#define RLV_CMD_PREFIX				'@'
#define RLV_PUTINV_PREFIX			"#RLV/~"

#define RLV_FOLDER_FLAG_NOSTRIP		"nostrip"
#define RLV_FOLDER_PREFIX_HIDDEN	'.'
#define RLV_FOLDER_PREFIX_PUTINV    '~'

#define RLV_SETTING_MAIN			"RestrainedLife"
#define RLV_SETTING_DEBUG			"RestrainedLifeDebug"
#define RLV_SETTING_FORBIDGIVETORLV "RestrainedLifeForbidGiveToRLV"
#define RLV_SETTING_NOSETENV		"RestrainedLifeNoSetEnv"
#define RLV_SETTING_LOGINLOCATION	"RestrainedLifeLoginStartLocation"		// NOTE: per-account setting!

// Version of the specifcation we support
const S32 RLV_VERSION_MAJOR = 1;
const S32 RLV_VERSION_MINOR = 18;
const S32 RLV_VERSION_PATCH = 0;

// Implementation version
const S32 RLVa_VERSION_MAJOR = 0;
const S32 RLVa_VERSION_MINOR = 2;
const S32 RLVa_VERSION_PATCH = 2;
const S32 RLVa_VERSION_BUILD = 0;

// The official viewer version we're patching against
#define RLV_MAKE_TARGET(x, y, z)	((x << 16) | (y << 8) | z)
#define RLV_TARGET					RLV_MAKE_TARGET(1, 23, 4)

// Defining these makes it easier if we ever need to change our tag
#define RLV_WARNS	LL_WARNS("RLV")
#define RLV_INFOS	LL_INFOS("RLV")
#define RLV_DEBUGS	LL_DEBUGS("RLV")

#if LL_RELEASE_WITH_DEBUG_INFO || LL_DEBUG
	// Turn on extended debugging information
	#define RLV_DEBUG
	// Make sure we halt execution on errors
	#define RLV_ERRS  LL_ERRS("RLV")
#else
	// Uncomment if you want extended debugging information on release builds
	//#define RLV_DEBUG
	// Don't halt execution on errors in release
	#define RLV_ERRS  LL_WARNS("RLV")
#endif // LL_RELEASE_WITH_DEBUG_INFO || LL_DEBUG

// Uncomment to use the pimpl (non-public)
//#define RLV_USE_PIMPL

// Comment out if you don't want the Advanced / RLVa menu (may prevent enabling some extensions or experimental features - see below)
#define RLV_ADVANCED_MENU

// Uncomment to enable the Advanced / RLVa / Unit Tests menu (non-public)
//#define RLV_DEBUG_TESTS

// Provides access to "advanced" feature through the RLVa debug menu
#define RLV_EXTENSION_ENABLE_WEAR			// "Enable Wear"
#define RLV_EXTENSION_HIDELOCKED			// "Hide locked layers", "Hide locked attachments" and "Hide locked inventory"

// Extensions
#define RLV_EXTENSION_CMD_GETSETDEBUG_EX	// Extends the debug variables accessible through @getdebug_xxx/@setdebug_xxx
#define RLV_EXTENSION_CMD_FINDFOLDERS		// @findfolders:<option>=<channel> - @findfolder with multiple results
#define RLV_EXTENSION_FLAG_NOSTRIP			// Layers and attachments marked as "nostrip" are exempt from @detach/@remoutfit
#define RLV_EXTENSION_STARTLOCATION			// Reenables "Start Location" at login if not @tploc=n or @unsit=n restricted at last logoff
//#define RLV_EXPERIMENTAL					// Enables/disables experimental features en masse
#define RLV_EXPERIMENTAL_119				// Enables the 1.19 RLV specification commands (experimental pending an actual 1.19 release)

// Experimental features
#ifdef RLV_EXPERIMENTAL
	// Stable (will mature to RLV_EXTENSION_XXX in next release if no bugs are found)
	#define RLV_EXPERIMENTAL_DOUBLECLICK_SITTP		// Ties "double click" teleporting to @sittp=n rather than @tploc=n
	#define RLV_EXPERIMENTAL_FLOATER_RESTRICTIONS	// Enables the Advanced / RLVa / Restrictions... floater

	// Under testing (stable, but requires further testing - safe for public release but may be quirky)
	#define RLV_EXPERIMENTAL_FARTOUCH_FEEDBACK		// Enables "cleaner" UI responses when fartouch blocks something

	// Under development (don't include in public release)
	#if LL_RELEASE_WITH_DEBUG_INFO || LL_DEBUG
		#define RLV_EXPERIMENTAL_CMD_TAKESNAPSHOT	// @takesnapshot[:<option>]=force - intended primarily for "vanilla" use
	#endif // LL_RELEASE_WITH_DEBUG_INFO || LL_DEBUG
#endif // RLV_EXPERIMENTAL

// ============================================================================
// Enumeration declarations
//

// NOTE: * any changes to this enumeration should be reflected in initLookupTable()
//       * only uncomment the ones we actually use in a switch() to keep the size of the lookup table down
enum ERlvBehaviour {
	RLV_BHVR_VERSION = 0,		// "version"
	RLV_BHVR_DETACH,			// "detach"
//	RLV_BHVR_SENDCHAT,			// "sendchat"
//	RLV_BHVR_EMOTE,				// "emote"
//	RLV_BHVR_CHATSHOUT,			// "chatshout"
//	RLV_BHVR_CHATNORMAL,		// "chatnormal"
//	RLV_BHVR_CHATWHISPER,		// "chatwhisper"
	RLV_BHVR_REDIRCHAT,			// "redirchat"
	RLV_BHVR_REDIREMOTE,		// "rediremote"
	RLV_BHVR_SENDIM,			// "sendim"
	RLV_BHVR_RECVCHAT,			// "recvchat"
	RLV_BHVR_RECVEMOTE,			// "recvemote"
	RLV_BHVR_RECVIM,			// "recvim"
//	RLV_BHVR_TPLM,				// "tplm"
	RLV_BHVR_TPLOC,				// "tploc"
	RLV_BHVR_TPLURE,			// "tplure"
	RLV_BHVR_SITTP,				// "sittp"
//	RLV_BHVR_CLEAR,				// "clear"
	RLV_BHVR_EDIT,				// "edit"
	RLV_BHVR_REZ,				// "rez"
	RLV_BHVR_ADDOUTFIT,			// "addoutfit"
	RLV_BHVR_REMOUTFIT,			// "remoutfit"
	RLV_BHVR_GETOUTFIT,			// "getoutfit"
	RLV_BHVR_GETATTACH,			// "getattach"
	RLV_BHVR_SHOWINV,			// "showinv"
//	RLV_BHVR_VIEWNOTE,			// "viewnote"
	RLV_BHVR_UNSIT,				// "unsit"
	RLV_BHVR_SIT,				// "sit"
//	RLV_BHVR_SENDCHANNEL,		// "sendchannel"
	RLV_BHVR_GETSTATUS,			// "getstatus"
	RLV_BHVR_GETSTATUSALL,		// "getstatusall"
	RLV_BHVR_GETINV,			// "getinv"
	RLV_BHVR_GETINVWORN,		// "getinvworn"
	RLV_BHVR_FINDFOLDER,		// "findfolder"
	RLV_BHVR_FINDFOLDERS,		// "findfolders"
	RLV_BHVR_ATTACH,			// "attach"
	RLV_BHVR_ATTACHALL,			// "attachall"
	RLV_BHVR_DETACHALL,			// "detachall"
	RLV_BHVR_GETPATH,			// "getpath"
	RLV_BHVR_ATTACHTHIS,		// "attachthis"
	RLV_BHVR_ATTACHALLTHIS,		// "attachallthis"
	RLV_BHVR_DETACHTHIS,		// "detachthis"
	RLV_BHVR_DETACHALLTHIS,		// "detachallthis"
	RLV_BHVR_FARTOUCH,			// "fartouch"
	RLV_BHVR_SHOWWORLDMAP,		// "showworldmap"
	RLV_BHVR_SHOWMINIMAP,		// "showminimap"
	RLV_BHVR_SHOWLOC,			// "showloc"
	RLV_BHVR_TPTO,				// "tpto"
	RLV_BHVR_ACCEPTTP,			// "accepttp"
	RLV_BHVR_SHOWNAMES,			// "shownames"
	RLV_BHVR_FLY,				// "fly"
	RLV_BHVR_GETSITID,			// "getsitid"
	RLV_BHVR_SETDEBUG,			// "setdebug"
	RLV_BHVR_SETENV,			// "setenv"
	RLV_BHVR_DETACHME,			// "detachme"
	RLV_BHVR_SHOWHOVERTEXTALL,	// "showhovertextall"
	RLV_BHVR_SHOWHOVERTEXT,		// "showhovertext"
	RLV_BHVR_SHOWHOVERTEXTHUD,	// "showhovertexthud"

	RLV_BHVR_COUNT,
	RLV_BHVR_UNKNOWN
};

enum ERlvParamType {
	RLV_TYPE_UNKNOWN,
	RLV_TYPE_ADD,				// <param> == "n"|"add"
	RLV_TYPE_REMOVE,			// <param> == "y"|"rem"
	RLV_TYPE_FORCE,				// <param> == "force"
	RLV_TYPE_REPLY				// <param> == <number>
};

enum ERlvCmdRet {
	RLV_RET_NOERROR,			// Command executed succesfully
	RLV_RET_RETAINED,			// Command was retained
	RLV_RET_FAILED,				// Command failed (general failure)
	RLV_RET_FAILED_SYNTAX,		// Command failed (syntax error)
	RLV_RET_FAILED_UNSET,		// Command failed (unset restriction)
	RLV_RET_FAILED_DUPLICATE,	// Command failed (duplicate)
	RLV_RET_FAILED_OPTION,		// Command failed (invalid option)
	RLV_RET_UNKNOWN				// Command unkown
};

// ============================================================================
/*
 * RlvCommand
 * ==========
 * Encapsulates an "RLV command" (duh :p)
 *
 */

class RlvCommand
{
public:
	/*
	 * Constructors
	 */
	explicit RlvCommand(const std::string& strCommand);
	RlvCommand(const RlvCommand& rlvCmd);

	/*
	 * Accessors
	 */
	BOOL isValid() const { return m_fValid; }

	const std::string& getBehaviour() const		{ return m_strBehaviour; }
	ERlvBehaviour      getBehaviourType() const	{ return m_eBehaviour; }
	const std::string& getOption() const		{ return m_strOption; }
	const std::string& getParam() const			{ return m_strParam; }
	ERlvParamType      getParamType() const		{ return m_eParamType; }

	std::string        asString() const;

	/*
	 * Operators
	 */
	bool operator ==(const RlvCommand&) const;

	// Parses an RLV command into its "tokens"
	static BOOL parseCommand(/*[in]*/ const std::string& strCommand,  
		/*[out]*/ std::string& strBehaviour, /*[out]*/ std::string& strOption, /*[out]*/ std::string& strParam);
	static void initLookupTable();

	/*
	 * Member variables
	 */
protected:
	BOOL			m_fValid;
	std::string		m_strBehaviour;
	ERlvBehaviour	m_eBehaviour;
	std::string		m_strOption;
	std::string		m_strParam;
	ERlvParamType	m_eParamType;

	static RlvMultiStringSearch m_BhvrLookup;
	friend class RlvHandler;
};

// ============================================================================
/*
 * RlvObject
 * =========
 * Encapsulates an "RLV Object" (= an object that has issued an RLV command)
 *
 */

typedef std::list<RlvCommand> rlv_command_list_t;

class RlvObject
{
public:
	RlvObject(const LLUUID& uuid) : m_UUID(uuid) {}

	BOOL addCommand(const RlvCommand& rlvCmd);
	BOOL removeCommand(const RlvCommand& rlvCmd);

	BOOL hasBehaviour(ERlvBehaviour eBehaviour) const;
	BOOL hasBehaviour(const std::string& strBehaviour) const;
	BOOL hasBehaviour(ERlvBehaviour eBehaviour, const std::string& strOption) const;
	BOOL hasBehaviour(const std::string& strBehaviour, const std::string& strOption) const;

	std::string getStatusString(const std::string& strMatch) const;

	const rlv_command_list_t* getCommandList() const { return &m_Commands; }
protected:
	LLUUID             m_UUID;
	rlv_command_list_t m_Commands;

	friend class RlvHandler;
};

// ============================================================================
/*
 * RlvCriteriaCategoryCollector
 * ============================
 * Criteria based folder matching filter used by @findfolder and @findfolders
 *
 */

class RlvCriteriaCategoryCollector : public LLInventoryCollectFunctor
{
public:
	RlvCriteriaCategoryCollector(const std::string& strCriteria)
	{
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep("&&", "", boost::drop_empty_tokens);
		tokenizer tokens(strCriteria, sep);
		for (tokenizer::iterator itToken = tokens.begin(); itToken != tokens.end(); ++itToken)
			m_Criteria.push_back(*itToken);
	}
	virtual ~RlvCriteriaCategoryCollector() {}

	virtual bool operator()(LLInventoryCategory* pFolder, LLInventoryItem* pItem)
	{
		if ( (!pFolder) || (m_Criteria.empty()) )	// We're only interested in matching folders, we don't care about items
			return false;							// (if there are no criteria then we don't want to return a match)

		std::string strFolderName = pFolder->getName();
		LLStringUtil::toLower(strFolderName);

		if ( (strFolderName.empty()) ||	(RLV_FOLDER_PREFIX_HIDDEN == strFolderName[0]) )
			return false;

		for (std::list<std::string>::const_iterator itCrit = m_Criteria.begin(); itCrit != m_Criteria.end(); ++itCrit)
			if (-1 == strFolderName.find(*itCrit))	// Return false on the first mismatch
				return false;
		return true;
	}

protected:
	std::list<std::string> m_Criteria;
};

// ============================================================================
/*
 * RlvWearableItemCollector
 * ========================
 * Inventory item filter used by attach/detach/attachall/detachall/getinvworn (also used by "Add/Replace Outfit" and "Take Off Items")
 *
 */

class RlvWearableItemCollector : public LLInventoryCollectFunctor
{
public:
	RlvWearableItemCollector(const LLUUID& idFolder, bool fAttach, bool fMatchAll)
		: m_idFolder(idFolder), m_fAttach(fAttach), m_fMatchAll(fMatchAll)
	{
		m_Wearable.push_back(idFolder);
	}
	virtual ~RlvWearableItemCollector() {}

	virtual bool operator()(LLInventoryCategory* pFolder, LLInventoryItem* pItem);

	S32           getDirectDescendentsCount(const LLInventoryCategory* pFolder, LLAssetType::EType type) const;
	const LLUUID& getFoldedParent(const LLUUID& idFolder) const;
protected:
	bool         m_fAttach;
	bool         m_fMatchAll;
	const LLUUID m_idFolder;

	bool onCollectFolder(const LLInventoryCategory* pFolder);
	bool onCollectItem(const LLInventoryItem* pItem);

	std::list<LLUUID> m_Tentative;
	std::list<LLUUID> m_Wearable;

	std::map<LLUUID, LLUUID> m_Folding;
};

// ============================================================================
/*
 * RlvRetainedCommand
 * ==================
 *
 */

struct RlvRetainedCommand
{
public:
	LLUUID      idObject;
	std::string strObject;
	std::string strCmd;
	bool        fImmediate;
	S16         nLookupMisses;

	RlvRetainedCommand(const LLUUID& uuid, const std::string& cmd) 
		: idObject(uuid), strCmd(cmd), nLookupMisses(0), fImmediate(false)
	{
		fImmediate = isImmediate(cmd);
	}

	static bool isImmediate(const std::string& strCmd)
	{
		int idx = strCmd.rfind('=');
		if ( (-1 != idx) && (-1 == strCmd.find_first_not_of("0123456789", idx + 1)) )
			return true;
		return false;
	}

	bool operator ==(const LLUUID& uuid) const { return idObject == uuid; }
private:
	RlvRetainedCommand();
};
typedef std::list<RlvRetainedCommand> rlv_retained_list_t;

// ============================================================================
/*
 * RlvWLSnapshot
 * =============
 *
 */

struct RlvWLSnapshot
{
public:
	static void           restoreSnapshot(const RlvWLSnapshot* pWLSnapshot);
	static RlvWLSnapshot* takeSnapshot();
private:
	RlvWLSnapshot() {}

	bool		 fIsRunning;
	bool		 fUseLindenTime;
	LLWLParamSet WLParams;
};

// ============================================================================
/*
 * Various helper classes/timers/functors/functions
 *
 */

class RlvGCTimer : public LLEventTimer
{
public:
	RlvGCTimer() : LLEventTimer(30.0) {}
	virtual BOOL tick();
};

struct RlvSelectHasLockedAttach : public LLSelectedNodeFunctor
{
	virtual bool apply(LLSelectNode* pNode);
};

struct RlvSelectIsOwnedByOrGroupOwned : public LLSelectedNodeFunctor
{
	RlvSelectIsOwnedByOrGroupOwned(const LLUUID& uuid) : m_idAgent(uuid) {}
	virtual bool apply(LLSelectNode* pNode);
	LLUUID m_idAgent;
};

struct RlvSelectIsSittingOn : public LLSelectedNodeFunctor
{
	RlvSelectIsSittingOn(LLXform* pObject) : m_pObject(pObject) {}
	virtual bool apply(LLSelectNode* pNode);
	LLXform* m_pObject;
};

void rlvStringReplace(std::string& strText, std::string strFrom, const std::string& strTo);
bool rlvCanDeleteOrReturn();

inline bool rlvIsEmote(const std::string& strUTF8Text)
{
	return (strUTF8Text.length() > 4) && ( (strUTF8Text.compare(0, 4, "/me ") == 0) || (strUTF8Text.compare(0, 4, "/me'") == 0) );
}

#ifdef RLV_EXTENSION_STARTLOCATION
	// TODO-RLVa: Clean this up a bit once we get it all working properly
	inline BOOL rlvGetLoginLocationSetting()
	{
		return (gSavedPerAccountSettings.controlExists(RLV_SETTING_LOGINLOCATION)) 
					? gSavedPerAccountSettings.getBOOL(RLV_SETTING_LOGINLOCATION) : FALSE;
	}
	void rlvUpdateLoginLocationSetting();
#endif // RLV_EXTENSION_STARTLOCATION

#ifdef RLV_DEBUG
	// "Advanced / RLVa / Enable RLV" menu option
	void rlvDbgToggleEnabled(void*);
	BOOL rlvDbgGetEnabled(void*);
#endif // RLV_DEBUG

// ============================================================================
// RlvCommand inlined member functions
//

inline bool RlvCommand::operator ==(const RlvCommand& rhs) const
{
	// The specification notes that "@detach=n" is semantically identical to "@detach=add" (same for "y" and "rem"
	return (m_strBehaviour == rhs.m_strBehaviour) && (m_strOption == rhs.m_strOption) &&
		( (RLV_TYPE_UNKNOWN != m_eParamType) ? (m_eParamType == rhs.m_eParamType) : (m_strParam == rhs.m_strParam) );
}

// ============================================================================

#endif // RLV_HELPER_H
