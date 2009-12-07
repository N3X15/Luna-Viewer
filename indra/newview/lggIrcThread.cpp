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
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS "AS IS"
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

/*
Ok, here is how this is suposed to work.
-----lggIrcData is suposed to just be a data structure object
		--contains all the information needed to connect to irc, as well as a uuid identifier
		--also contains functions to convert to a from llsd
-----lggIrcGroupHanlder is a single object made to just help in handling irc stuff
		--has functions to scan file system for llsd files and return a list of thier lggIrcData
		--has functions to start up a new lggIrcThread and pass it a coresponding lggIrcData to work with
		--keeps a list of all the lggIrcThreads so it can delete them later
-----lggFloaterIrc is the panel in the "irc" tab next to the "groups" tab
		--reads a list of names from the files system using a global lggIrcGroupHanlder object
		--has buttons to edit and create new lggIrcDatas, which get saved to a file to be read and added at reset
		--start IM sends a uid to the lggIrcGroupHandler for it to start up and hanlde a new lggIrcThread
-----lggIrcThread
		--initialized with a lggIrcData so it knows where to connect, etc
		--starts up a NEW irc object, and adds to it handlers
		--included its own methods used to handle the irc objects triggers from when irc info is sent
		--start up a new THREAD called the message listener, to try to connect to a channel, while looping infinite listening
*/

#include "llviewerprecompiledheaders.h"
#include "lggIrcThread.h"
#include "llimpanel.h"
#include "llviewercontrol.h"
//#include "IRC.h"

//#include "llthread.h"
#include "linden_common.h"
#include "llapp.h"

#include "llimview.h"
#if LL_WINDOWS
#include <crtdbg.h>
#endif
#include "llnotifications.h"
#include "lggIrcProfileFloater.h"
#include "llfloaterchat.h"
#include "llfloaterchatterbox.h"
#include "llfloater.h"
#include "llversionviewer.h"
#include "lltimer.h"


//static
std::map<IRC*,lggIrcThread*> lggIrcThread::sInstances;
std::map<MsgListener*,lggIrcThread*> lggIrcThread::msInstances;

void whoisresponce::done()
{
	LLSD args;
	LLSD payload;
	
// IRC Profile -- [NICK]
// Nick:       [NICK]
// User:       [USER]
// Host:       [HOST]
// Real Name:  [REALNAME]
// Channels:   [CHANNELS]
// Servers:    [SERVERS]
// Idle Time:  [IDLE]
	args["NICK"] = nick;
	args["USER"] = user;
	args["HOST"] = host;
	args["REALNAME"] = realName;
	args["CHANNELS"] = channels;
	args["SERVERS"] = servers;
	args["IDLE"] = idle;
	args["RCHANNEL"]= REALChannel;
	LggIrcProfile::show(args);
	//LLNotifications::instance().add("EmeraldIRCInfo", args, payload,callbackProfile);
	//pop up
	newOne();
	
	
}
// void whoisresponce::callbackProfile(const LLSD& notification, const LLSD& response)
// {
// 	S32 option = LLNotification::getSelectedOption(notification, response);
// 
// 	if ( option == 0 )
// 	{
// 
// 	}
// 	//newOne();
// }

lggIrcThread::lggIrcThread(lggIrcData data):
	mData(data),
	dataGotten(FALSE)
{
}

MsgListener::MsgListener(IRC * conn2, std::string chan2, LLUUID mid2): LLThread("IRC LISTENER"),conn(conn2),chan(chan2),mid(mid2)
{

}

void MsgListener::run()
{
	
	//conn->join((char*)chan.c_str());
	//gIMMgr->addMessage(mid,mid,"GreenLife",llformat("Atempting to join %s",chan.c_str()));
	/*ms_sleep(5000);
	conn->join((char*)chan.c_str());	
	gIMMgr->addMessage(mid,mid,"GreenLife",llformat("Atempting to join %s", chan.c_str()));
	ms_sleep(5000);	
	conn->join((char*)chan.c_str());
	gIMMgr->addMessage(mid,mid,"GreenLife",llformat("Atempting to join %s", chan.c_str()));
	*/

	
	conn->message_loop();
}
void MsgListener::shutdown()
{


}

MsgListener::~MsgListener()
{
	
}


lggIrcThread::~lggIrcThread()
{	
	sInstances.erase(conn);
	msInstances.erase(listener);
	delete listener;
	delete conn;
}
//static
lggIrcThread* lggIrcThread::findInstance(IRC* conn)
{
	std::map<IRC*,lggIrcThread*>::iterator itr = sInstances.find(conn);
	if(itr != sInstances.end())
	{
		return (*itr).second;
	}
	return NULL;
}

int boot( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->ircresponce(params, hostd, conn);
	else
		return 0;
}
int ijoined( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->channelJoinedResponce(params, hostd, conn);
	else
		return 0;
}
int privmsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->PrivMessageResponce(params, hostd, conn);
	else
		return 0;
}
int noticemsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->NoticeMessageResponce(params, hostd, conn);
	else
		return 0;
}
int joinmsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->JoinMessageResponce(params, hostd, conn);
	else
		return 0;
}

int partmsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->PartMessageResponce(params, hostd, conn);
	else
		return 0;
}
int quitmsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->QuitMessageResponce(params, hostd, conn);
	else
		return 0;
}
int nickmsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->NickMessageResponce(params, hostd, conn);
	else
		return 0;
}
int kickmsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->KickMessageResponce(params, hostd, conn);
	else
		return 0;
}
int modemsg( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->ModeMessageResponce(params, hostd, conn);
	else
		return 0;
}
int bRPL_WHOISUSER( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->RPL_WHOISUSER(params, hostd, conn);
	else
		return 0;
}
int bRPL_WHOISCHANNELS( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->RPL_WHOISCHANNELS(params, hostd, conn);
	else
		return 0;
}int bRPL_AWAY( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->RPL_AWAY(params, hostd, conn);
	else
		return 0;
}int bRPL_WHOISIDLE( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->RPL_WHOISIDLE(params, hostd, conn);
	else
		return 0;
}
int bRPL_WHOISSERVER( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->RPL_WHOISSERVER(params, hostd, conn);
	else
		return 0;
}

int bRPL_ENDOFWHOIS( char * params, irc_reply_data * hostd, void * conn)
{
	lggIrcThread* thread = lggIrcThread::findInstance((IRC*)conn);
	if(thread)
		return thread->RPL_ENDOFWHOIS(params, hostd, conn);
	else
		return 0;
}

void lggIrcThread::setData(lggIrcData dat)
{
	mData =dat;
	dataGotten=TRUE;

}
int lggIrcThread::PrivMessageResponce( char * params, irc_reply_data * hostd, void * connn)
{
	
	
// 	if displaying
// 		chat.mText = std::string("IRC: ") + name + separator_string + message.substr(message_offset);
// 	LLFloaterChat::addChat( chat, TRUE );
	if(hostd && params)
	{

		if(hostd->target && hostd->nick)
		{

		
			if(!strcmp(hostd->target,getChannel().c_str()))
			{
				//chan msg
				//if(strstr(&params[1],"ACTION"))
				//if(std::string(&params[1]).find_first_of("ACTION")!=std::string::npos)
				string::size_type pos = std::string(&params[1]).find_first_of(" \001", 1);
				string command = std::string(&params[1]).substr(1, pos - 1);
				if (command == "ACTION")
				{
					actionDisp(std::string(hostd->nick),(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2)));
					if(gSavedSettings.getBOOL("IMInChatConsole"))
					{				
						chat.mText = std::string("IRC: ") + std::string(hostd->nick) + std::string(" ") + stripColorCodes(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2));
						LLFloaterChat::addChat( chat, TRUE );
					}
					return 0;
				}
				if(gSavedSettings.getBOOL("EmeraldIRC_ShowChannel"))
					msg(
					llformat(": %s",std::string(&params[1]).c_str())
					,llformat("%s",hostd->nick),
					gSavedSettings.getColor("EmeraldIRC_ColorChannel"),true);
			}else
			{
				
				displayPrivateIm(std::string(&params[1]),std::string(hostd->nick));
			}
		}else
		{
			//msg("No target or nick given");
		}
	}else
	{
		//msg("No host given");
	}
	
	return 0;
}
void lggIrcThread::whois(LLUUID who)
{
	
	for(int innerItter = 0; innerItter < (int)conn->allparticipants.size(); innerItter++)
	{
		if(conn->allparticipants[innerItter] == who)
		{
			whois(conn->allcorespondingNick[innerItter]);
		}
	}
}
void lggIrcThread::whois(std::string user)
{
	conn->raw((char *)std::string("WHOIS "+user+"\r\n").c_str());
}
int lggIrcThread::NoticeMessageResponce( char * params, irc_reply_data * hostd, void * conn)
{

	if(hostd && params)
	{

		if(hostd->target && hostd->nick)
		{
			string::size_type pos = std::string(&params[1]).find_first_of(" \001", 1);
			string command = std::string(&params[1]).substr(1, pos - 1);
			if(command == "VERSION")
			{
				//displayPrivateIm(std::string(&params[1]),std::string(hostd->nick));
				msg("CTCP version reply \""+ stripColorCodes(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2)) +"\" from "+std::string(hostd->nick));
			}
			else if(command == "PING")
			{
				//displayPrivateIm(std::string(&params[1]),std::string(hostd->nick));
				F64 temp = atof(stripColorCodes(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2)).c_str());
				temp = LLTimer::getTotalSeconds() - temp;
				msg("Ping reply from "+std::string(hostd->nick)+" in "+llformat("%.2f",temp)+" seconds.");
				//msg("CTCP version reply \""+ stripColorCodes(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2)) +"\" from "+std::string(hostd->nick));
			}
			else if(!strcmp(hostd->target,getChannel().c_str()))
			{
				if (command == "ACTION")
				{
					actionDisp(std::string(hostd->nick),(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2)));
					if(gSavedSettings.getBOOL("IMInChatConsole"))
					{				
						chat.mText = std::string("IRC: ") + std::string(hostd->nick) + std::string(" ") + stripColorCodes(std::string(&params[1]).substr(pos + 1, (std::string(&params[1]).length() - pos) - 2));
						LLFloaterChat::addChat( chat, TRUE );
					}
					return 0;
				}
				//chan msg
				if(gSavedSettings.getBOOL("EmeraldIRC_ShowNotice"))
					msg(llformat(": %s",std::string(&params[1]).c_str()),
					llformat("%s",hostd->nick),
					gSavedSettings.getColor("EmeraldIRC_ColorNotice"),true);
			}else
				displayPrivateIm(std::string(&params[1]),std::string(hostd->nick));
		}
	}
	return 0;
}

int lggIrcThread::JoinMessageResponce( char * params, irc_reply_data * hostd, void * conn)
{
	//msg( llformat("JOIN Params: %s and host: %s and ident: %s and nick %s and target %s ",params,hostd->host,hostd->ident,hostd->nick,hostd->target).c_str());
	if(hostd)
	{
		if(hostd->nick)
		{
			if(gSavedSettings.getBOOL("EmeraldIRC_ShowJoin"))
				msg( 
				llformat("%s has joined this chat.",hostd->nick).c_str(),
				gSavedSettings.getColor("EmeraldIRC_ColorJoin"),false);

		}
	}
	
	updateNames();
	return 0;
}
int lggIrcThread::PartMessageResponce( char * params, irc_reply_data * hostd, void * conn)
{
	//[20:36]  PART Params: #emerald and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick lgg and target (null) 

	//msg( llformat("PART Params: %s and host: %s and ident: %s and nick %s and target %s ",params,hostd->host,hostd->ident,hostd->nick,hostd->target).c_str());
	if(hostd)
	{
		if(hostd->nick)
		{
			if(gSavedSettings.getBOOL("EmeraldIRC_ShowQuit"))
				msg( llformat("%s has left this chat.",hostd->nick).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorQuit"),false);
		}
	}
	
	updateNames();
	return 0;
}
int lggIrcThread::QuitMessageResponce( char * params, irc_reply_data * hostd, void * conn)
{
	//[20:45]  QUIT Params: :Quit:  and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick lgg and target (null) 
	//msg( llformat("QUIT Params: %s and host: %s and ident: %s and nick %s and target %s ",params,hostd->host,hostd->ident,hostd->nick,hostd->target).c_str());
	if(hostd)
	{
		if(hostd->nick)
		{
			if(gSavedSettings.getBOOL("EmeraldIRC_ShowQuit"))
				msg( llformat("%s has logged off.",hostd->nick).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorQuit"),false);
		}
	}
	updateNames();
	return 0;
}
int lggIrcThread::NickMessageResponce( char * params, irc_reply_data * hostd, void * conn)
{
	//[20:46]  NICK Params: :new and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick lgg and target (null) 
	//[20:46]  NICK Params: :lgg and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick new and target (null) 

	//msg( llformat("NICK Params: %s and host: %s and ident: %s and nick %s and target %s ",params,hostd->host,hostd->ident,hostd->nick,hostd->target).c_str());
	if(hostd)
	{
		if(hostd->nick && hostd->ident && params)
		{
			if(gSavedSettings.getBOOL("EmeraldIRC_ShowNick"))
				msg( llformat("%s (%s) is now known as %s.",hostd->nick,hostd->ident,&params[1]).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorNick"),false);
		}
	}
	updateNames();
	return 0;
}
int lggIrcThread::ModeMessageResponce( char * params, irc_reply_data * hostd, void * conn)
{
	//[20:46]  NICK Params: :new and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick lgg and target (null) 
	//[20:46]  NICK Params: :lgg and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick new and target (null) 

	//msg( llformat("NICK Params: %s and host: %s and ident: %s and nick %s and target %s ",params,hostd->host,hostd->ident,hostd->nick,hostd->target).c_str());
	
	if(hostd)
	{
		if(hostd->nick && hostd->ident && params && hostd->target)
		{
			if(gSavedSettings.getBOOL("EmeraldIRC_ShowNick"))
				msg( llformat("%s (%s) sets mode on %s. (%s)",hostd->nick,hostd->ident,&params[0],hostd->target).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorNick"),false);
		}
	}
	
	updateNames();
	return 0;
}

int lggIrcThread::RPL_WHOISUSER( char * params, irc_reply_data * hostd, void * conn)
{
	//2009-09-08T22:13:55Z INFO: lggIrcThread::RPL_WHOISUSER: Params Shadow Liny Liny msl-95D65E08.dsl.irvnca.pacbell.net * :Liny
	if(params)
	{
		llinfos << "Params " << params << llendl;
		//"<nick> <user> <host> * :<real name>"
		std::istringstream i(params);
		std::string us;
		if(i >> us)
		{
			if(i >> whoR.nick)
				if( i >> whoR.user)
					if(i>>whoR.host)
						if(i>>us)
							while(i>>whoR.realName)
							{
								//do nothing.. stupid warnings.. uhm..
								us = "";
							}

		}
	}
	return 0;
}
int lggIrcThread::RPL_WHOISCHANNELS( char * params, irc_reply_data * hostd, void * conn)
{
	//2009-09-08T22:13:55Z INFO: lggIrcThread::RPL_WHOISCHANNELS: Params Shadow Liny :#emerald 
	if(params)
	{
		llinfos << "Params " << params << llendl;
		std::istringstream i(params);
		std::string us;
		if(i >> us)//shadow
			if(i >> us )//liny
				while(i >> us)
				{

					whoR.channels = whoR.channels + us+ " ";
				}
	}
	return 0;
}
int lggIrcThread::RPL_WHOISSERVER( char * params, irc_reply_data * hostd, void * conn)
{
	//2009-09-08T22:13:55Z INFO: lggIrcThread::RPL_WHOISSERVER: Params Shadow Liny irc.modularsystems.sl :Modular Systems IRC
	if(params)
	{
		llinfos << "Params " << params << llendl;
		std::istringstream i(params);
		std::string us;
		if(i >> us)//shadow
			if(i>> us)//liny
				while(i >> us)
				{

					whoR.servers = whoR.servers + us + " ";
				}
	}
	return 0;
}
int lggIrcThread::RPL_AWAY( char * params, irc_reply_data * hostd, void * conn)
{
	if(params)
	{
		llinfos << "Params " << params << llendl;
		std::istringstream i(params);
		std::string us;
		if(i >> us)//shadow
			if(i>> us)//liny
				while(i >> us)
				{

					whoR.away= whoR.away + us + " ";
				}
	}
	return 0;
}
int lggIrcThread::RPL_WHOISIDLE( char * params, irc_reply_data * hostd, void * conn)
{
	//2009-09-08T22:13:55Z INFO: lggIrcThread::RPL_WHOISIDLE: Params Shadow Liny 17 1252440858 :seconds idle, signon time
	if(params)
	{
		llinfos << "Params " << params << llendl;
		std::istringstream i(params);
		std::string us;
		if(i >> us)//shadow
			if(i>> us)//liny
				while(i >> us)
				{

					whoR.away= whoR.away + us + " ";
				}
	}
	return 0;
}
int lggIrcThread::RPL_ENDOFWHOIS( char * params, irc_reply_data * hostd, void * conn)
{
	whoR.done();
	return 0;
}



int lggIrcThread::KickMessageResponce( char * params, irc_reply_data * hostd, void * connn)
{
	//[20:10]  KICK Params: #emerald Emerald-User354541ac :test and host: 507F089C.80FD756D.8FBBEBA0.IP and ident: lgg and nick lgg and target (null) 
	//msg( llformat("KICK Params: %s and host: %s and ident: %s and nick %s and target %s ",params,hostd->host,hostd->ident,hostd->nick,hostd->target).c_str());
	if(hostd)
	{

	
		if(hostd->nick && hostd->ident && params)
		{
			std::string twho;
			std::string twhy;
			std::string paramstring(&params[1]);
			istringstream iss(paramstring);
			iss >> twho;//first part we dont need
			
			if(iss >> twho)
			{
				if(!strcmp(twho.c_str(),conn->current_nick()) && gSavedSettings.getBOOL("EmeraldIRC_AutoReJoin"))
				{
					if(iss >> twhy)
					{
						msg( llformat("%s has been kicked by %s (%s).",twho.c_str(),hostd->nick,twhy.substr(1).c_str()).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorKick"),false);	
					}else
					{
						msg( llformat("%s has been kicked by %s.",twho.c_str(),hostd->nick).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorKick"),false);	
					}
					msg(llformat("Attempting to rejoin %s.",getChannel().c_str()));
					join();
				}
				else if(gSavedSettings.getBOOL("EmeraldIRC_ShowKick"))
				{
					if(iss >> twhy)
					{
						msg( llformat("%s has been kicked by %s (%s).",twho.c_str(),hostd->nick,twhy.substr(1).c_str()).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorKick"),false);	
					}else
					{
						msg( llformat("%s has been kicked by %s.",twho.c_str(),hostd->nick).c_str(),gSavedSettings.getColor("EmeraldIRC_ColorKick"),false);	
					}
				}
			}
		}
	}
	updateNames();
	return 0;
}
int lggIrcThread::channelJoinedResponce(char *params, irc_reply_data *hostd, void *conn)
{
	//353

	//msg(mData.toString());
	llinfos << "Chanel joined" << llendl;
	updateNames();

	msg("Channel Joined.  Please type /help for a list of commands.");
	gIMMgr->findFloaterBySession(getMID())->childSetVisible("active_speakers_panel",true);

	//msg(mData.toString());
	llinfos << "Chan join end" << llendl;
	return 0;
}
int lggIrcThread::ircresponce( char * params, irc_reply_data * hostd, void * conn)
{
	
	llinfos << "irc response " << mData.toString() << llendl;
	//IRC* irc_conn=(IRC*)conn; /* notice that you are passed a pointer to the connection object */
	//gIMMgr->
	msg("Attempting to join channel...");
	join();
	return 0;
}


lggIrcData lggIrcThread::getData() const
{
	return mData;
}


void lggIrcThread::run()
{
	
	
	conn = new IRC();
	sInstances[conn] = this;
	//msg(mData.toString());
	llinfos << mData.toString() << llendl;
	//msg("started, waiting for data");
	//msg("data recieved");	
	conn->hook_irc_command("376",&boot);
	conn->hook_irc_command("422",&boot);
	
	conn->hook_irc_command("PRIVMSG", &privmsg);
	conn->hook_irc_command("NOTICE", &noticemsg);
	conn->hook_irc_command("353",&ijoined);	
	conn->hook_irc_command("JOIN",&joinmsg);	
	conn->hook_irc_command("PART",&partmsg);
	conn->hook_irc_command("QUIT",&quitmsg);
	conn->hook_irc_command("NICK",&nickmsg);
	conn->hook_irc_command("KICK",&kickmsg);
	conn->hook_irc_command("MODE",&modemsg);

	conn->hook_irc_command("311",&bRPL_WHOISUSER);
	conn->hook_irc_command("319",&bRPL_WHOISCHANNELS);
	conn->hook_irc_command("312",&bRPL_WHOISSERVER);
	conn->hook_irc_command("301",&bRPL_AWAY);
	conn->hook_irc_command("317",&bRPL_WHOISIDLE); 
	conn->hook_irc_command("318",&bRPL_ENDOFWHOIS); 
	
	if(
	conn->start((char*) mData.server.c_str(),
		atoi(mData.port.c_str()),
		(char*)mData.nick.c_str(),
		(char*)mData.nick.c_str(),
		(char*)mData.nick.c_str(),
		(char*)mData.serverPassword.c_str()))
	{
		msg("Fawk, couldnt connect, some error bzns");
	}/* connect to a server */
	
	//conn.join(chan.c_str());
	//msg("Starting the thread...");
	listener = new MsgListener(conn,getChannel(),getMID());
	listener->start();

	whoR.REALChannel = getChannel();

	
	msInstances[listener]=this;

	//msg("done setting up dis");

}
std::string lggIrcThread::getChannel()
{
	
	//llinfos << "get chanel" << mData.toString() << llendl;
	std::string chan = mData.channel;
	if(chan.at(0)!='#')
	{
		chan = chan.insert(0,"#");
	}
	return chan;
}
LLUUID lggIrcThread::getMID()
{
	
	//llinfos << " get mid " << mData.toString() << llendl;
	return mData.id;
}
void lggIrcThread::join()
{
	
			if(mData.nickPassword != "")
			{
				/*conn->privmsg((char*)std::string("NickServ").c_str(),
					(char *)
					llformat("REGISTER %s bogus@email.com",mData.password.c_str()).c_str()
					);
				ms_sleep(10);*/

				conn->privmsg((char*)std::string("NickServ").c_str(),
					(char *)
					llformat("IDENTIFY %s",mData.nickPassword.c_str()).c_str()
					);
			}

			//msg(mData.toString());
			llinfos << "joining.." << llendl;
			conn->join((char*)getChannel().c_str(),(char*)mData.channelPassword.c_str());

			//msg(mData.toString());
			llinfos << "join sent" << llendl;
}
void lggIrcThread::sendChat(std::string chat)
{	
	std::istringstream i(chat);
	std::string command;
	i >> command;
	if(command == "/help")
	{
		msg(std::string("\"/join\" will attempt to re-join the current channel.\n\"/msg NICK MSG\" will send a private MSG to NICK\n\"/kick NICK [REASON]\" will kick NICK from that chat (if you have op rights)\n\"/nick NEWNICK\" will change your current nick to NEWNICK"));
	}
	else if(command == "/kick")
	{
		std::string targetNick;
		if(i >> targetNick)
		{
			std::string reason;
			if(i >> reason)
			{
				conn->kick((char *)getChannel().c_str(),(char *)targetNick.c_str(),(char *)reason.c_str());
				//msg(llformat("You have kicked %s from this chat. (%s)",targetNick.c_str(),reason.c_str()));
			}
			else
			{
				conn->kick((char *)getChannel().c_str(),(char *)targetNick.c_str());
				msg(llformat("You have kicked %s from this chat.",targetNick.c_str()));
			}
		}
	}
	else if(command == "/nick")
	{
		std::string targetNick;
		if(i >> targetNick)
		{
			conn->nick((char*)targetNick.c_str());
			//msg(llformat("Changing your nickname to %s",targetNick.c_str()));
		}
		else
		{
			msg("Invalid format for /nick, format is \"/nick NEWNICK\"");
		}
		
	}
	else if(command == "/join")
	{
		msg(llformat("Attempting to rejoin %s, if you wish to join a different channel, please make a new irc group",getChannel().c_str()));
		join();
	}
	else if(command == "/me")
	{
		// WARNING: Liandra made ugly ANSI C hax here. >_>
        std::string toSend = llformat("%cACTION%s%c", (char)1, chat.substr(3).c_str(), (char)1 );
		actionDisp(std::string(conn->current_nick()),chat.substr(4).c_str());
		conn->privmsg((char*)getChannel().c_str(),(char*)toSend.c_str());
	}
	else if(command == "/msg")
	{
		std::string theTarget,theMsg;
		if(i >> theTarget)
		{
			if(i >> theMsg)
			{
				theMsg = chat.substr(5+theTarget.length()+1);
				conn->privmsg((char*)theTarget.c_str(),(char *)theMsg.c_str());
				msg(llformat("Send Private Message to %s : %s",theTarget.c_str(),theMsg.c_str()));
			}
			else
			{
				msg("No Message Specified");
			}
		}
		else
		{
			msg("No target name specified");
		}
	}
	else if(command == "/version")
	{
		std::string theTarget,theMsg;
		if(i >> theTarget)
		{
			conn->privmsg((char*)theTarget.c_str(),(char*)"\001VERSION\001");
		}
		else
		{
			msg("No target name specified");
		}
	}
	else if(command == "/ping")
    {
        std::string theTarget,theMsg;
        if(i >> theTarget)
        {
            conn->privmsg((char*)theTarget.c_str(),(char*)llformat("\001PING %.2f\001", LLTimer::getTotalSeconds()).c_str());
        }
        else
        {
            msg("No target name specified");
        }
        //msg(llformat("%.2f", LLTimer::getTotalSeconds()));
    }
	else if(command == "/slap")
	{
		std::string theTarget;
		if(i >> theTarget)
		{
			std::string toSend = llformat("%cACTION slaps %s with a goldfish%c", (char)1, theTarget.c_str(), (char)1 );
			conn->privmsg((char*)getChannel().c_str(),(char *)toSend.c_str());
			actionDisp(std::string(conn->current_nick()),llformat("slaps %s with a goldfish",theTarget.c_str()));
		}
		else
		{
			msg("No target name specified");
		}
	}
	else if(command.substr(0,1)=="/")
	{
		conn->raw((char *)std::string(chat.substr(1)+"\r\n").c_str());
	}
	else
	{
		msg(llformat("%s: %s",	conn->current_nick(),chat.c_str()),
			gSavedSettings.getColor("EmeraldIRC_ColorChannel"),false);
		conn->privmsg((char*)getChannel().c_str(),(char *)chat.c_str());
	}
}
void lggIrcThread::stopRun()
{
	// close other IRC windows
	for(int i = 0; i < (int)conn->allparticipants.size() ; i++)
	{

		LLUUID computed_session_id=LLIMMgr::computeSessionID(IM_PRIVATE_IRC,conn->allparticipants[i]);
		if(!gIMMgr->hasSession(computed_session_id))
		{
			
			//gIMMgr->findFloaterBySession(computed_session_id);
		}
	}

	conn->disconnect();
	listener->shutdown();


}
void lggIrcThread::actionDisp(std::string name, std::string msg)
{
	if(gSavedSettings.getBOOL("EmeraldIRC_ShowAction"))
	{
		msg = " " + msg;
		floater = gIMMgr->findFloaterBySession(getMID());
		floater->addHistoryLine(stripColorCodes(
		name + msg
		),
		gSavedSettings.getColor("EmeraldIRC_ColorAction"));
		notifyStuff();
	}
}
void lggIrcThread::notifyStuff()
{
	LLFloaterChatterBox* chat_floater = LLFloaterChatterBox::getInstance(LLSD());

	if( !chat_floater->getVisible() && !floater->getVisible())
	{
		//if the IM window is not open and the floater is not visible (i.e. not torn off)
		LLFloater* previouslyActiveFloater = chat_floater->getActiveFloater();
		// select the newly added floater (or the floater with the new line added to it).
		// it should be there.
		chat_floater->selectFloater(floater);

		//there was a previously unseen IM, make that old tab flashing
		//it is assumed that the most recently unseen IM tab is the one current selected/active
		if ( previouslyActiveFloater && gIMMgr->getIMReceived() )
		{
			chat_floater->setFloaterFlashing(previouslyActiveFloater, TRUE);
		}
		if(!gIMMgr->getFloaterOpen())
		{
			gIMMgr->notifyNewIM();
		}
	}
}
void lggIrcThread::displayPrivateIm(std::string msg, std::string name)
{
	LLUUID uid;
	uid.generate(name+"lgg"+getChannel());//dont touch this one
	BOOL found = false;
	for(int i = 0; i < (int) conn->allparticipants.size();i++)
	{
		if(conn->allparticipants[i] == uid)found=true;
	}
	if(found)
	{

		string::size_type pos = std::string(msg.c_str()).find_first_of(" \001", 1);
		string command = std::string(msg.c_str()).substr(1, pos - 1);
		if(command == "VERSION")
		{
			char	version[45];
			sprintf(version, "\001VERSION Emerald Viewer %d.%d.%d (%d)\001", LL_VERSION_MAJOR, LL_VERSION_MINOR, LL_VERSION_PATCH, LL_VERSION_BUILD);
			conn->notice((char *)name.c_str(), (char *)version);
			return;
		}
		else if(command == "PING")
		{
			conn->notice((char *)name.c_str(), (char *)msg.c_str());
		}
		else
		{
			if(!gSavedSettings.getBOOL("EmeraldIRC_ShowPrivate"))return;
			LLUUID computed_session_id=LLIMMgr::computeSessionID(IM_PRIVATE_IRC,uid);
			floater = gIMMgr->findFloaterBySession(computed_session_id);
			if(!gIMMgr->hasSession(computed_session_id))
			{
				make_ui_sound("UISndNewIncomingIMSession");
				gIMMgr->addSession(llformat("%s",name.c_str()),IM_PRIVATE_IRC,uid);
				floater = gIMMgr->findFloaterBySession(computed_session_id);
			}
			if(!floater)return;
			if (command == "ACTION")
			{
				msg  = stripColorCodes(
					name + msg.substr(pos, (msg.length() - pos) - 1)
					);
				if(gSavedSettings.getBOOL("EmeraldIRC_ShowAction"))
				{
					floater->addHistoryLine(msg,
					gSavedSettings.getColor("EmeraldIRC_ColorAction"));
				}
				if(gSavedSettings.getBOOL("IMInChatConsole"))
				{				
					chat.mText = std::string("IRC: ") + msg;
					LLFloaterChat::addChat( chat, TRUE );
				}
				notifyStuff();
				return;
			}
			else
			{
				if(gSavedSettings.getBOOL("IMInChatConsole"))
				{				
					chat.mText = std::string("IRC: ") + llformat("#%s",name.c_str())+
						llformat(": %s",msg.c_str());
					LLFloaterChat::addChat( chat, TRUE );
				}
 				floater->addHistoryLine(
	 				llformat(": %s",msg.c_str()),
 					gSavedSettings.getColor("EmeraldIRC_ColorPrivate"),
 					true,
 					uid,
 					llformat("%s",name.c_str())
 					);
				notifyStuff();
			}
		}
	}else
	{
		this->msg( 
			llformat("%s: %s",name.c_str(),msg.c_str()),
			gSavedSettings.getColor("EmeraldIRC_ColorPrivate"),
			true);
	}
}

void lggIrcThread::msg(std::string message)
{
	//sends a message to the window

	//llinfos << " msg " << mData.toString() << llendl;
	//gIMMgr->addMessage(getMID(),getMID(),mData.name,message);
	if(gSavedSettings.getBOOL("EmeraldIRC_ShowSystem"))
		gIMMgr->findFloaterBySession(getMID())->addHistoryLine(stripColorCodes(message),
		gSavedSettings.getColor("EmeraldIRC_ColorSystem"));

}
void lggIrcThread::msg(std::string message, LLColor4 color, bool notify)
{
	floater = gIMMgr->findFloaterBySession(getMID());
	floater->addHistoryLine(stripColorCodes(message),color);
	if(notify)
	{
		notifyStuff();
		if(gSavedSettings.getBOOL("IMInChatConsole"))
		{				
			chat.mText = std::string("IRC: ") + stripColorCodes(message);
			LLFloaterChat::addChat( chat, TRUE );
		}
	}
}
// void lggIrcThread::msg(std::string message, std::string name, bool notify)
// {
// 	LLUUID uid;
// 	uid.generate(name.substr(1)+"lgg"+getChannel());
// 
// 	floater = gIMMgr->findFloaterBySession(getMID());
// 	floater->addHistoryLine(stripColorCodes(message),
// 		gSavedSettings.getColor("IMChatColor"),
// 		true,
// 		uid,
// 		name);
// 	if(notify)notifyStuff();
// 	if(gSavedSettings.getBOOL("IMInChatConsole"))
// 	{				
// 		chat.mText = std::string("IRC: ") + name + std::string(": ") + stripColorCodes(message);
// 		LLFloaterChat::addChat( chat, TRUE );
// 	}
// }
void lggIrcThread::msg(std::string message, std::string name, LLColor4 color, bool notify)
{
	LLUUID uid;
	uid.generate(name+"lgg"+getChannel().c_str());

	llinfos << "Generating Disp uuid from |" << name << "| and |" << getChannel().c_str() << "| it was " << uid.asString() << llendl;


	floater  = gIMMgr->findFloaterBySession(getMID());
	floater->addHistoryLine(stripColorCodes(message),
		color,
		true,
		uid,
		name);
	if(notify)
	{
		notifyStuff();

		if(gSavedSettings.getBOOL("IMInChatConsole"))
		{				
			chat.mText = std::string("IRC: ") + name + std::string(": ") + stripColorCodes(message);
			LLFloaterChat::addChat( chat, TRUE );
		}
	}
}
std::vector<LLUUID> lggIrcThread::getParticipants()
{
	return conn->allparticipants;
}
void lggIrcThread::sendPrivateImToID(std::string msg, LLUUID id)
{
	
	LLUUID uid;
	
	for(int i = 0; i < (int)conn->allparticipants.size(); i++)
	{
		if(conn->allparticipants[i] == id)
		{
			std::string name = conn->allcorespondingNick[i];
			uid.generate(name+"lgg"+getChannel());
			LLUUID computed_session_id=LLIMMgr::computeSessionID(IM_PRIVATE_IRC,uid);
			std::istringstream i(msg);
			std::string command;
			i >> command;
			
			if(command == "/me")
			{
				std::string toSend = llformat("%cACTION%s%c", (char)1, msg.substr(3).c_str(), (char)1 );
				gIMMgr->findFloaterBySession(computed_session_id)->addHistoryLine(
					llformat("%s %s",conn->current_nick(),msg.substr(4).c_str()),
					gSavedSettings.getColor("EmeraldIRC_ColorPrivate")
					);
				conn->privmsg((char*)name.c_str(),(char*)toSend.c_str());
			}else
			{			
				conn->privmsg((char*)name.c_str(),(char*)msg.c_str());
				
				gIMMgr->findFloaterBySession(computed_session_id)->addHistoryLine(
					llformat("%s: %s",conn->current_nick(),msg.c_str()),
				gSavedSettings.getColor("EmeraldIRC_ColorPrivate")
				);
			}

			return;
		}
	}
	return;

	
}
std::string lggIrcThread::stripColorCodes(std::string input)
{
	const char * source = input.c_str();
	std::string returning;	
	const char *p;

	
		for ( p = source; *p; p++ )
		{
			switch (*p)
			{
			
			case 0x03:	// set color
				if ( isdigit (p[1]) )
				{
					// Parse 
					p++;

					if ( isdigit (p[1]) )
					{
						p++;
					}

					if ( p[1] == ',' && isdigit (p[2]) )
					{
						p += 2;

						if ( isdigit (p[1]) )
						{
							p++;
						}
					}

				}
				break;

			default:
					returning+=p[0];
				
				break;
		
			}
		}

	return returning;

}

void lggIrcThread::updateNames()
{
	LLSD test;
	gIMMgr->findFloaterBySession(getMID())->setIRCSpeakers(test);
	gIMMgr->findFloaterBySession(getMID())->setIRCSpeakers(conn->getSpeakersLLSD());
}
