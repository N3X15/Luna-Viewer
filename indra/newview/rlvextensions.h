/** 
 *
 * Copyright (c) 2009-2010, Kitty Barnett
 * 
 * The source code in this file is provided to you under the terms of the 
 * GNU General Public License, version 2.0, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. Terms of the GPL can be found in doc/GPL-license.txt 
 * in this distribution, or online at http://www.gnu.org/licenses/gpl-2.0.txt
 * 
 * By copying, modifying or distributing this software, you acknowledge that
 * you have read and understood your obligations described above, and agree to 
 * abide by those obligations.
 * 
 */

#ifndef RLV_EXTENSIONS_H
#define RLV_EXTENSIONS_H

#include "rlvcommon.h"

// ============================================================================
/*
 * RlvExtGetSet
 * ============
 * Implements @get_XXX:<option>=<channel> and @set_XXX:<option>=force
 *
 */

class RlvExtGetSet : public RlvCommandHandler
{
public:
	RlvExtGetSet();
	virtual ~RlvExtGetSet() {}

	virtual bool onForceCommand(const LLUUID& idObj, const RlvCommand& rlvCmd, ERlvCmdRet& cmdRet);
	virtual bool onReplyCommand(const LLUUID& idObj, const RlvCommand& rlvCmd, ERlvCmdRet& cmdRet);
protected:
	std::string onGetDebug(std::string strSetting);
	std::string onGetPseudoDebug(const std::string& strSetting);
	ERlvCmdRet  onSetDebug(std::string strSetting, const std::string& strValue);
	ERlvCmdRet  onSetPseudoDebug(const std::string& strSetting, const std::string& strValue);

	std::string onGetEnv(std::string strSetting);
	ERlvCmdRet  onSetEnv(std::string strSetting, const std::string& strValue);

	bool processCommand(const LLUUID& idObj, const RlvCommand& rlvCmd, ERlvCmdRet& eRet);

public:
	enum { DBG_READ = 0x01, DBG_WRITE = 0x02, DBG_PERSIST = 0x04, DBG_PSEUDO = 0x08 };
	static std::map<std::string, S16> m_DbgAllowed;
	static std::map<std::string, std::string> m_PseudoDebug;

	static bool findDebugSetting(/*[in,out]*/ std::string& strSetting, /*[out]*/ S16& flags);
	static S16  getDebugSettingFlags(const std::string& strSetting);
};

// ============================================================================

#endif // RLV_EXTENSIONS_H
