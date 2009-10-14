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
 

#include "llviewerprecompiledheaders.h" 
#include "lggIrcGroupHandler.h"
#include "llfile.h"
#include "llagent.h"
#include "llsdserialize.h"
#include "llimview.h"
//#include "lggIrcThread.h"
using namespace std;

lggIrcGroupHandler glggIrcGroupHandler;

////////////////////////////////////////////////////
/////////////////HANDLER////////////////////////////
std::vector<lggIrcData> lggIrcGroupHandler::getFileNames()
{
	
	std::vector<lggIrcData> names;	
	llinfos << "grabbing irc file names.." << llendl;
	
	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "IRCGroups", ""));
	bool found = true;			
	while(found) 
	{
		std::string name;
		found = gDirUtilp->getNextFileInDir(path_name, "*.xml", name, false);
		if(found)
		{

			name=name.erase(name.length()-4);
			names.push_back(getIrcGroupInfo(name));
		}
	}
	std::string path_name2(gDirUtilp->getExpandedFilename(LL_PATH_PER_SL_ACCOUNT, "IRCGroups", ""));
	found = true;			
	while(found) 
	{
		std::string name;
		found = gDirUtilp->getNextFileInDir(path_name2, "*.xml", name, false);
		if(found)
		{

			name=name.erase(name.length()-4);

			names.push_back(getIrcGroupInfo(name));
		}
	}
	return names;

	

}
void lggIrcGroupHandler::setListPanel(lggPanelIRC * ilistPanel)
{
	listPanel = ilistPanel;
}
void lggIrcGroupHandler::fillListPanel()
{
	if(listPanel)
	listPanel->newList();
	else llwarns << "list panel not there" << llendl;
}


void lggIrcGroupHandler::deleteIrcGroup(std::string filename)
{
	filename = filename+".xml";
	llinfos << "lggIrcGroupHanlder::" << filename << llendl;
	
	std::string path_name1(gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "IRCGroups", filename));
	std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_PER_SL_ACCOUNT , "IRCGroups", filename));
		
	if(gDirUtilp->fileExists(path_name1))
	{
		LLFile::remove(path_name1);
	}
	if(gDirUtilp->fileExists(path_name2))
	{
		LLFile::remove(path_name2);
	}
	
}
BOOL lggIrcGroupHandler::sendWhoisToAll(LLUUID who)
{
	llinfos << "LOOKING FOR " << who.asString() << llendl;
	for(std::list<lggIrcThread*>::iterator it = activeThreads.begin(); it != activeThreads.end(); it++)
	{

		lggIrcThread* ita = *it;		
		std::vector<LLUUID> parts = ita->getParticipants();
		for(int innerItter = 0; innerItter < (int)parts.size(); innerItter++)
		{
			//llinfos << "--" << parts[innerItter].asString() << llendl;

			if(parts[innerItter] == who)
			{
				ita->whois(who);
				return true;

			}
		}
	}
	return false;
}
void lggIrcGroupHandler::startUpAutoRunIRC()
{
	
	std::string name;
	bool found = true;			

	//first move files...
	std::string old_path_name(gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "IRCGroups", ""));
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(old_path_name, "*.xml", name, false);
		if(found)
		{
			//copy files over... to per sl account settings..
		}
	}



	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "IRCGroups", ""));
	lggIrcData toReturn;
	found=true;
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(path_name, "*.xml", name, false);
		if(found)
		{

			toReturn = getIrcGroupInfo(name.erase(name.length()-4));
			if(toReturn.autoLogin)
			{
				startUpIRCListener(toReturn);
			}
		}
	}
	std::string path_name2(gDirUtilp->getExpandedFilename(LL_PATH_PER_SL_ACCOUNT, "IRCGroups", ""));
	found = true;			
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(path_name2, "*.xml", name, false);
		if(found)
		{
			toReturn = getIrcGroupInfo(name.erase(name.length()-4));
			if(toReturn.autoLogin)
			{
				startUpIRCListener(toReturn);
			}
		}
	}
	fillListPanel();

}
void lggIrcGroupHandler::deleteIrcGroupByID(LLUUID id)
{
	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "IRCGroups", ""));
	lggIrcData toReturn;
	std::string name;
	bool found = true;			
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(path_name, "*.xml", name, false);
		if(found)
		{
				
			toReturn = getIrcGroupInfo(name.erase(name.length()-4));
			if(toReturn.id==id)
			{
				deleteIrcGroup(toReturn.name);
			}
		}
	}
	std::string path_name2(gDirUtilp->getExpandedFilename(LL_PATH_PER_SL_ACCOUNT, "IRCGroups", ""));
	found = true;			
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(path_name2, "*.xml", name, false);
		if(found)
		{
			toReturn = getIrcGroupInfo(name.erase(name.length()-4));
			if(toReturn.id==id)
			{
				deleteIrcGroup(toReturn.name);
			}
		}
	}
	//TODO cleanup toreturn
	
}
lggIrcData lggIrcGroupHandler::getIrcGroupInfoByID(LLUUID id)
{
	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "IRCGroups", ""));
	lggIrcData toReturn;// = lggIrcData();
	std::string name;
	bool found = true;			
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(path_name, "*.xml", name, false);
		if(found)
		{
			name=name.erase(name.length()-4);		
			toReturn=getIrcGroupInfo(name);
			if(toReturn.id==id)
			{
				return toReturn;
			}
		}
	}
	std::string path_name2(gDirUtilp->getExpandedFilename(LL_PATH_PER_SL_ACCOUNT, "IRCGroups", ""));
	found = true;			
	while(found) 
	{
		found = gDirUtilp->getNextFileInDir(path_name2, "*.xml", name, false);
		if(found)
		{
			name=name.erase(name.length()-4);		
			toReturn = getIrcGroupInfo(name);
			if(toReturn.id==id)
			{
				return toReturn;
			}
		}
	}
	return lggIrcData();
	
}
lggIrcData lggIrcGroupHandler::getIrcGroupInfo(std::string filename)
{
	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "IRCGroups", ""));
	std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_PER_SL_ACCOUNT , "IRCGroups", ""));
	std::string name =path_name +filename+".xml";
	if(gDirUtilp->fileExists(filename))
	{
	}else
	{
		name =path_name2 +filename+".xml";
	}
	LLSD data;
	llifstream importer(name);
	LLSDSerialize::fromXMLDocument(data, importer);
	llinfos << " loading data from filename " << name << llendl;
		
	//lggIrcData* test = lggIrcData::fromLLSD(data);
	//llinfos << " filename resolved to this.." << llendl;
	//llinfos << test->toString() << llendl;
	
	return lggIrcData::fromLLSD(data);
	
}
void lggIrcGroupHandler::sendIrcChatByID(LLUUID id, std::string msg)
{
	getThreadByID(id)->sendChat(msg);
}
void lggIrcGroupHandler::startUpIRCListener(lggIrcData dat)
{

	for(std::list<lggIrcThread*>::iterator it = activeThreads.begin(); it != activeThreads.end(); it++)
	{
		lggIrcThread* ita = *it;
		
		if(dat==ita->getData())
		{
			
			llwarns << "This session is already started" << llendl;
			return;//duplicates r bad yo
		}
	}
	gIMMgr->setFloaterOpen(TRUE);
	
	gIMMgr->addSession(dat.name,IM_SESSION_IRC_START,dat.id);
	llinfos << " add session..." << llendl;
	make_ui_sound("UISndStartIM");
	LLSD args;
	gIMMgr->addMessage(dat.id,dat.id,std::string("GreenLife"),
		//gIMMgr->addSystemMessage(idat.id,
		llformat("IRC Session Initiated on server: %s:%s \nYour nick is %s and you are on the channel: %s\nWARNING: THIS IS A IRC CHAT WINDOW, Chat can not be verified by Linden Labs or Modular Systems, people may or may not have the same nick name as their second life avatar, or may not even be human at all"
		, dat.server.c_str(), dat.port.c_str(),dat.nick.c_str(),dat.channel.c_str()));
	
	lggIrcThread* mThreadIRC = new lggIrcThread(dat);
	llinfos << "Starting irc thread" << dat.toString() << llendl;
	//mThreadIRC = new lggIrcThread();
	mThreadIRC->run();
	activeThreads.push_back(mThreadIRC);
}
void lggIrcGroupHandler::endDownIRCListener(LLUUID id)
{
	lggIrcThread* remove;
	
	for(std::list<lggIrcThread*>::iterator it = activeThreads.begin(); it != activeThreads.end(); it++)
	{
		lggIrcThread* ita = *it;
		
		if(id==ita->getData().id)
		{
			ita->stopRun();
			remove = ita;
		}
	}
	activeThreads.remove(remove);
	delete remove;
}
lggIrcThread* lggIrcGroupHandler::getThreadByID(LLUUID id)
{
	for(std::list<lggIrcThread*>::iterator it = activeThreads.begin(); it != activeThreads.end(); it++)
	{
		lggIrcThread* ita = *it;		
		if(id==ita->getData().id)
		{
			return ita;
		}
	}
	return NULL;
}
bool lggIrcGroupHandler::trySendPrivateImToID(std::string msg, LLUUID id, BOOL testingForExistance)
{
	for(std::list<lggIrcThread*>::iterator it = activeThreads.begin(); it != activeThreads.end(); it++)
	{
		
		lggIrcThread* ita = *it;		
		std::vector<LLUUID> parts = ita->getParticipants();
		for(int innerItter = 0; innerItter < (int)parts.size(); innerItter++)
		{
			if(parts[innerItter] == id)
			{
				if(!testingForExistance)
					ita->sendPrivateImToID(msg,id);
				return true;

			}
		}
	}
	return false;
}