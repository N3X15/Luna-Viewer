#pragma once
#define USE_SHITTY_GET 1
#include <string>
#include "llsd.h"
#include "lluuid.h"

extern bool gCopybotExists;

class SHLCopyComm
{
public:
	SHLCopyComm(std::string Port="9001", std::string Password="DONGS");
	~SHLCopyComm(void);

	bool CheckPresence();
	LLSD& SendCommand(std::string Command, std::string Params[]);
	void DuplicateObject(LLUUID object_id);
	void CloneAvatar(std::string  FName,std::string  LName);
	void RegisterWithBots();
private:
	std::string mPort;
	std::string mPass;

	// Add shit to log
	void Add2Log(std::string message);
};

extern SHLCopyComm *gCopybot;

#ifndef USE_SHITTY_GET
class SHLCheckResponder : public LLHTTPClient::Responder
{
public:
	SHLCheckResponder();
	~SHLCheckResponder();

	void result(const LLSD& content);
	void error(U32 statusNum, const std::string& reason);
};
#endif