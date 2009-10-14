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
#ifndef IRC_THREAD_IRC22
#define IRC_THREAD_IRC22


#include "llthread.h"
#include "lggIrcData.h"
#include "llviewerprecompiledheaders.h"
#include "IRC.h"
#include "llimpanel.h"
#include "llchat.h"

class MsgListener : public LLThread
{
public:
	MsgListener(IRC * conn2, std::string chan2, LLUUID mid2);
	~MsgListener();	
	/*virtual*/ void run(void);
	/*virtual*/ void shutdown(void);
	
protected:
	std::string chan;
	IRC * conn;
	LLUUID mid;

};

class whoisresponce
{
public:
	std::string nick;
	std::string user;
	std::string host;
	std::string realName;
	std::string channels;
	std::string servers;
	std::string idle;
	std::string away;
		
	std::string REALChannel;


	whoisresponce(){user=channels=servers=idle=away=nick=host=realName="";}
	void newOne(){user=channels=servers=idle=away=nick=host=realName="";}
	void done();//need to make this one
	static void callbackProfile(const LLSD& notification, const LLSD& response);
	
};

class lggIrcThread
{
public:
	lggIrcThread(lggIrcData data);
	~lggIrcThread();
	
	MsgListener * listener;
	whoisresponce whoR;
	LLChat chat;

	LLFloaterIMPanel* floater;

	std::vector<LLUUID> getParticipants();
	void displayPrivateIm(std::string msg, std::string name);
	void sendPrivateImToID(std::string msg, LLUUID id);
	void run(void);
	void join();
	void notifyStuff();
	void stopRun(void);
	void whois(std::string user);
	void whois(LLUUID who);
	void setData(lggIrcData dat);
	lggIrcData getData() const;
	void actionDisp(std::string name, std::string msg);
	void msg(std::string message); 
	//void msg(std::string message, std::string name, bool notify);
	void msg(std::string message, LLColor4 color, bool notify);
	void msg(std::string message, std::string name, LLColor4 color,bool notify);
	void sendChat(std::string chat);
	void updateNames();
	int ircresponce(char* params, irc_reply_data* hostd, void* conn);
	int PrivMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int NoticeMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int channelJoinedResponce( char * params, irc_reply_data * hostd, void * conn);
	int JoinMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int PartMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int QuitMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int NickMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int ModeMessageResponce( char * params, irc_reply_data * hostd, void * conn);
	int KickMessageResponce( char * params, irc_reply_data * hostd, void * conn);

	int RPL_WHOISUSER( char * params, irc_reply_data * hostd, void * conn);
	int RPL_WHOISCHANNELS( char * params, irc_reply_data * hostd, void * conn);
	int RPL_WHOISSERVER( char * params, irc_reply_data * hostd, void * conn);
	int RPL_AWAY( char * params, irc_reply_data * hostd, void * conn);
	int RPL_WHOISIDLE( char * params, irc_reply_data * hostd, void * conn);
	int RPL_ENDOFWHOIS( char * params, irc_reply_data * hostd, void * conn);
	
	std::string stripColorCodes(std::string input);
	std::string getChannel();
	LLUUID getMID();
	static lggIrcThread* findInstance(IRC* conn);
	friend bool operator==(const lggIrcThread &a, const lggIrcThread &b);		// Return a == b
	friend bool operator!=(const lggIrcThread &a, const lggIrcThread &b);		// Return a == b
	friend bool operator<(const lggIrcThread &a, const lggIrcThread &b);		// Return a < b	
	friend bool operator>(const lggIrcThread &a, const lggIrcThread &b);

protected:
	lggIrcData mData; // User data associated with this thread
	IRC * conn;
	BOOL dataGotten;
private:
	static std::map<IRC*,lggIrcThread*> sInstances;
	static std::map<MsgListener*,lggIrcThread*> msInstances;

};

inline bool operator==(const lggIrcThread &a, const lggIrcThread &b)
{
	return (  a.getData().id == b.getData().id);
}

inline bool operator!=(const lggIrcThread &a, const lggIrcThread &b)
{
	return ( !(a==b) );
}
inline bool operator<(const lggIrcThread &a, const lggIrcThread &b)
{
	return (  a.getData().name < b.getData().name);
}
inline bool operator>(const lggIrcThread &a, const lggIrcThread &b)
{
	return (  a.getData().name > b.getData().name);
}
#endif
