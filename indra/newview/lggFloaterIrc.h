/* Copyright (c) 2009
 *
 * Greg Hendrickson (LordGregGreg Back). All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS "AS IS"
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

#ifndef LL_LLFLOATERIRC_H
#define LL_LLFLOATERIRC_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class llfloaterirc
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "lluuid.h"
#include "llfloater.h"
#include <map>

class LLUICtrl;
class LLTextBox;
class LLScrollListCtrl;
class LLButton;

class lggPanelIRC : public LLPanel, public LLSimpleListener
{
public:
	lggPanelIRC();
	virtual ~lggPanelIRC();

	void newList();

protected:
	// initialize based on the type
	BOOL postBuild();
	
	/*virtual*/ bool handleEvent(LLPointer<LLEvent> event, const LLSD& userdata);
	// highlight_id is a group id to highlight
	void enableButtons();

	static void onIrcList(LLUICtrl* ctrl, void* userdata);
	static void onBtnNewIrc(void* userdata);
	static void onBtnEdit(void* userdata);
	static void onBtnRefresh(void* userdata);
	static void onBtnIM(void* userdata);
	static void onBtnRemove(void* userdata);
	static void onDoubleClickGroup(void* userdata);
	static void onClickHelp(void* data);

	void newirc();
	void editirc();
	void startirc();
	void startIM();
	void removeirc();
	void initHelpBtn(const std::string& name, const std::string& xml_alert);

	
};


#endif // LL_LLFLOATERIRCS_H
