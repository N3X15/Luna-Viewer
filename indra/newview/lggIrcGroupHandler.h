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
 

#ifndef GROUP_HANDLER_IRC22
#define GROUP_HANDLER_IRC22

//using namespace std;

#include "llviewerprecompiledheaders.h"
#include "lggIrcThread.h"
#include "lggIrcData.h"
#include "lggFloaterIrc.h"

class lggIrcGroupHandler
{
	public:

		lggIrcGroupHandler() {}
		~lggIrcGroupHandler() { }
	public:
		lggPanelIRC * listPanel;
		void setListPanel(lggPanelIRC * ilistPanel);
		void fillListPanel();
		std::list<lggIrcThread*> activeThreads;
		std::vector<lggIrcData> getFileNames();
		lggIrcData	getIrcGroupInfo(std::string filename);
		lggIrcData	getIrcGroupInfoByID(LLUUID id); 
		void deleteIrcGroup(std::string filename);		
		void deleteIrcGroupByID(LLUUID id);
		void startUpAutoRunIRC();
		BOOL sendWhoisToAll(LLUUID who);
		void startUpIRCListener(lggIrcData dat);
		void endDownIRCListener(LLUUID id);
		lggIrcThread* getThreadByID(LLUUID id);
		void sendIrcChatByID(LLUUID id, std::string msg);
		bool trySendPrivateImToID(std::string msg, LLUUID id,BOOL testingForExistance);

		
};



extern lggIrcGroupHandler glggIrcGroupHandler;

#endif

