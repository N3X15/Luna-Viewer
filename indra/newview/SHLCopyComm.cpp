#include "llviewerprecompiledheaders.h"
#include "SHLCopyComm.h"
#include "llhttpclient.h"
#include "llnotify.h"
#include "lluuid.h"
#include "llagent.h"
#include "llviewerobject.h"
#include "llchat.h"
#include "llfloaterchat.h"

extern LLAgent gAgent;

bool gCopybotExists=false;

SHLCopyComm* gCopybot=NULL;

SHLCopyComm::SHLCopyComm(std::string  Port, std::string  Pass) :
	mPort(Port),
	mPass(Pass)
{
	gCopybotExists=this->CheckPresence();
	if(gCopybotExists)
		this->RegisterWithBots();
}

SHLCopyComm::~SHLCopyComm(void)
{

}

bool SHLCopyComm::CheckPresence(void)
{
	std::string  URL = "http://localhost:";
	URL.append(this->mPort);
	URL.append("/");
	URL.append(this->mPass);
	URL.append("/chk/");

	LLSD lolwat = LLHTTPClient::blockingGet(URL); // Because I'm a fucking dick, that's why

	LLChat dump(lolwat.asString());
	LLFloaterChat::addChat(dump);

	switch(lolwat["code"].asInteger())
	{
		// Command not found
		case 404:
			return false;
		// OK
		case 200:
			return true;
		// Command fuckup
		case 500:
			return false;
	}
	return false;
}

/**
* SendCommand("register",new std::string []{"all","Phillip Linden"});
* Will return:
* http://localhost:port/password/register/all/Phillip%20Linden/
*/
LLSD& SHLCopyComm::SendCommand(std::string  Command,std::string  Params[])
{
	std::string  URL = "localhost:";
	URL.append(this->mPort);
	URL.append("/");
	URL.append(this->mPass);
	URL.append("/");
	URL.append(Command);
	URL.append("/");
	int i;
	int sz = sizeof(Params)/sizeof(Params[0]);
	for(i=0;i<sz;i++)
	{
		URL.append(Params[0]);
		URL.append("/");
	}
	//SHLCheckResponder cr = new SHLCheckResponder();
	LLSD& r = LLHTTPClient::blockingGet(URL);
	
	return r;
}

void SHLCopyComm::DuplicateObject(LLUUID object_id)
{
	if(object_id.isNull()) return;

	std::string  o_uuid[] = {"all",object_id.getString()};
	
	LLSD r = SHLCopyComm::SendCommand("duplicate",o_uuid);
	if(r["code"].asInteger()==200)
	{
		this->Add2Log("Duplication in progress.");
	} else {
		this->Add2Log("Duplication failed:");
		this->Add2Log(r["message"].asString());
	}
}


void SHLCopyComm::CloneAvatar(std::string  FName, std::string  LName)
{
	std::string  o_uuid[] = {"all",llformat("%s%%20%s",FName,LName)};
	LLSD r = SendCommand("clone",o_uuid);
	if(r["code"].asInteger()!=200)
	{
		Add2Log("Cloning failed.");
	}
}

void SHLCopyComm::RegisterWithBots(void)
{
	
	std::string  fucking_name;
	gAgent.getName(fucking_name);

	std::string  shitpiss[] = {"all",fucking_name};
	
	Add2Log("Sending registration to XMLRPC host... (Your name: "+fucking_name+")");
	LLSD r = this->SendCommand("register",shitpiss);
	if(r["code"].asInteger()==200)
	{
		Add2Log("Finished registering.");
	}
	else
	{
		Add2Log("Registration failed, disabling Copybot capabilities.");
		gCopybotExists=false;
	}
	
}

void SHLCopyComm::Add2Log(std::string  msg)
{
	LLChat f("[COPYBOT] "+msg);
	LLFloaterChat::addChat(f);
}


#ifndef USE_SHITTY_GET
SHLCheckResponder::SHLCheckResponder(int &response_code, std::string  &response_message):
	LLHTTPClient::Responder(),
	mResponseCode(response_code),
	mResponseMessage(response_message)
{

}

SHLCheckResponder::~SHLCheckResponder()
{
}

SHLCheckResponder::result(const LLSD &content)
{
	mResponseCode = content["code"].asInteger();
	mResponseMessage = content["message"].asInteger();
}
#endif