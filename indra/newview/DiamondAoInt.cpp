#include "llviewerprecompiledheaders.h"

#include "DiamondAoInt.h"
#include "floaterao.h"
#include "jc_lslviewerbridge.h"

void send_chat_to_object(std::string chat, S32 channel, LLUUID target)
{
	if(target.isNull())target = gAgent.getID();
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessage(_PREHASH_ScriptDialogReply);
	msg->nextBlock(_PREHASH_AgentData);
	msg->addUUID(_PREHASH_AgentID, gAgent.getID());
	msg->addUUID(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlock(_PREHASH_Data);
	msg->addUUID(_PREHASH_ObjectID, target);
	msg->addS32(_PREHASH_ChatChannel, channel);
	msg->addS32(_PREHASH_ButtonIndex, 0);
	msg->addString(_PREHASH_ButtonLabel, chat);
	gAgent.sendReliableMessage();
}

S32 DiamondAoInt::regchan;
DiamondAoInt::DiamondAoInt()
{
}

DiamondAoInt::~DiamondAoInt()
{
}

bool DiamondAoInt::AOCommand(std::string message)
{
	std::string clip = message.substr(0,3);
	if(clip == "~`~")
	{
		std::string rest = message.substr(3);
		LLSD args = JCLSLBridge::parse_string_to_list(rest, '|');
		std::string cmd = args[0].asString();
		if(cmd == "dmdAoInt")
		{
			cmd = args[1].asString();
			if(cmd == "on")
			{
				gSavedPerAccountSettings.setBOOL("PhoenixAOEnabled",TRUE);
				LLFloaterAO::run();
			}
			else if(cmd == "off")
			{
				gSavedPerAccountSettings.setBOOL("PhoenixAOEnabled",FALSE);
				LLFloaterAO::run();
			}
			else if(cmd == "status")
			{
				S32 chan = atoi(args[2].asString().c_str());
				std::string tmp="off";
				if(gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled"))tmp="on";
				send_chat_to_object(tmp,chan,gAgent.getID());
			}
			else if(cmd == "regchan")
			{
				regchan = atoi(args[2].asString().c_str());
				send_chat_to_object(std::string("Channel registerd"),regchan,gAgent.getID());
			}
			else if(cmd == "loadcard")
			{
				std::string tmp = (JCLSLBridge::findInventoryByName(args[2].asString(),phoenix_category_name)).asString();
				if(LLUUID(tmp).notNull())
				{
					gSavedPerAccountSettings.setString("PhoenixAOConfigNotecardID",tmp);
					LLFloaterAO::updateLayout2(LLFloaterAO::getInstance());
				}
			}
		}
		return true;
	}
	return false;
}

void DiamondAoInt::AOStatusUpdate(bool status)
{
	if(regchan != 0)
	{
		std::string tmp="off";
		if(gSavedPerAccountSettings.getBOOL("PhoenixAOEnabled"))tmp="on";
		send_chat_to_object(tmp,regchan,gAgent.getID());
	}
}