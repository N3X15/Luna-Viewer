
#include "llviewerprecompiledheaders.h"

#include "jc_lslviewerbridge.h"
#include "llchatbar.h"
#include "llagent.h"
#include "stdtypes.h"
#include "llviewerregion.h"
#include "llworld.h"
#include "lluuid.h"
#include "llfilepicker.h"
#include "llassetstorage.h"
#include "llviewerobjectlist.h"

#include "importtracker.h"

#include "llviewerobject.h"

#include "llvoavatar.h"

#include "llinventorymodel.h"

#include "lltooldraganddrop.h"

#include "llmd5.h"

#include "llstartup.h"

#include "llcontrol.h"

#include "llviewercontrol.h"



#define vCatType (LLAssetType::EType)128
#define vBridgeName "#LSL<->Client Bridge v0.01"
#define vBridgeOpCat "#Emerald"

//void cmdline_printchat(std::string message);

//static
U8 JCLSLBridge::sBridgeStatus;
//static
JCLSLBridge* JCLSLBridge::sInstance;

LLViewerObject* JCLSLBridge::sBridgeObject;

U32 JCLSLBridge::lastcall;

std::map<U32,JCBridgeCallback*> JCLSLBridge::callback_map;

JCLSLBridge::JCLSLBridge() : LLEventTimer( (F32)1.0 )
{
	if(sInstance)
	{
		////cmdline_printchat("duplicate bridge?");
		delete this;
	}else
	{
		////cmdline_printchat("instanciated bridge");
		sInstance = this;
		lastcall = 0;
	}
}
JCLSLBridge::~JCLSLBridge()
{
}

bool JCLSLBridge::lsltobridge(std::string message, std::string from_name, LLUUID source_id, LLUUID owner_id)
{
	if(message == "someshit")
	{
		////cmdline_printchat("got someshit from "+source_id.asString());
		
		return true;
	}else
	{
		std::string clip = message.substr(0,5);
		if(clip == "#@#@#")
		{
			std::string rest = message.substr(5);
			LLSD arguments = JCLSLBridge::parse_string_to_list(rest, '|');
			////cmdline_printchat(std::string(LLSD::dumpXML(arguments)));
			U32 call = atoi(arguments[0].asString().c_str());
			if(call)
			{
				arguments.erase(0);
				////cmdline_printchat(std::string(LLSD::dumpXML(arguments)));
				callback_fire(call, arguments);
				return true;
			}
		}
	}
	return false;
}
void send_chat_from_viewer(std::string utf8_out_text, EChatType type, S32 channel);
void JCLSLBridge::bridgetolsl(std::string cmd, JCBridgeCallback* cb)
{
	if(sBridgeStatus == ACTIVE)
	{
		std::string chat = llformat("%d",registerCB(cb)) + "|"+cmd;
		send_chat_from_viewer(chat, CHAT_TYPE_SHOUT, JCLSLBridge::bridge_channel(gAgent.getID()));
	}else
	{
		////cmdline_printchat("bridge not active");
		delete cb;
	}
}

std::string md5hash(const std::string &text, U32 thing)
{
	char temp[MD5HEX_STR_SIZE];
	LLMD5 toast((const U8*)text.c_str(), thing);
	toast.hex_digest(temp);
	return std::string(temp);
}

/*LLUUID md5hash(const std::string &text, U32 thing)
{
 LLUUID temp;
 LLMD5 toast((const U8*)text.c_str(), thing);
 toast.raw_digest(temp.mData);
 return temp;
}*/

S32 JCLSLBridge::bridge_channel(LLUUID user)
{
	std::string tmps = md5hash(user.asString(),1);
	int i = (int)strtol((tmps.substr(0, 7) + "\n").c_str(),(char **)NULL,16);
	return (S32)i;
}

class ObjectBNameMatches : public LLInventoryCollectFunctor
{
public:
	ObjectBNameMatches(std::string name)
	{
		sName = name;
	}
	virtual ~ObjectBNameMatches() {}
	virtual bool operator()(LLInventoryCategory* cat,
							LLInventoryItem* item)
	{
		if(item)
		{
			//LLViewerInventoryCategory* folderp = gInventory.getCategory((item->getParentUUID());
			return (item->getName() == sName);// && cat->getName() == "#v");
		}
		return false;
	}
private:
	std::string sName;
};



const LLUUID& JCLSLBridge::findInventoryByName(const std::string& object_name)
{
	LLViewerInventoryCategory::cat_array_t cats;
	LLViewerInventoryItem::item_array_t items;
	ObjectBNameMatches objectnamematches(object_name);
	gInventory.collectDescendentsIf(gAgent.getInventoryRootID(),cats,items,FALSE,objectnamematches);

	if (items.count())
	{
		return items[0]->getUUID();
	}
	return LLUUID::null;
}

bool isworn(LLUUID item)
{
	LLVOAvatar* avatar = gAgent.getAvatarObject();
	if(avatar && avatar->isWearingAttachment(item) )
	{
		return true;
	}
	return false;
}

BOOL JCLSLBridge::tick()
{
	if(LLStartUp::getStartupState() >= STATE_INVENTORY_SEND)
	{
		switch(sBridgeStatus)
		{
		case UNINITIALIZED:
			{
				if(!gSavedSettings.getBOOL("EmeraldBuildBridge"))
				{
					sBridgeStatus = FAILED;
					break;
				}
				//cmdline_printchat("initializing");//<< llendl;
				if(gInventory.isEverythingFetched())
				{
					//cmdline_printchat("inv is fetched");//<< llendl;
					LLUUID item_id = findInventoryByName(vBridgeName);
					if(item_id.notNull())
					{
						//cmdline_printchat("id="+item_id.asString());
						LLViewerInventoryItem* bridge = gInventory.getItem(item_id);
						if(bridge)
						{
							//cmdline_printchat("bridge is ready to attach");//<< llendl;
							if(isworn(bridge->getUUID()))
							{
								//cmdline_printchat("bridge is already worn");//<< llendl;
								sBridgeStatus = ACTIVE;
							}else if(bridge->isComplete())
							{
								//cmdline_printchat("bridge is complete, attaching");//<< llendl;
								LLMessageSystem* msg = gMessageSystem;
								msg->newMessageFast(_PREHASH_RezSingleAttachmentFromInv);
								msg->nextBlockFast(_PREHASH_AgentData);
								msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
								msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
								msg->nextBlockFast(_PREHASH_ObjectData);
								msg->addUUIDFast(_PREHASH_ItemID, bridge->getUUID());
								msg->addUUIDFast(_PREHASH_OwnerID, bridge->getPermissions().getOwner());
								msg->addU8Fast(_PREHASH_AttachmentPt, 128);
								pack_permissions_slam(msg, bridge->getFlags(), bridge->getPermissions());
								msg->addStringFast(_PREHASH_Name, bridge->getName());
								msg->addStringFast(_PREHASH_Description, bridge->getDescription());
								msg->sendReliable(gAgent.getRegionHost());
								sBridgeStatus = ACTIVE;
							}
						}
					}else
					{
						//cmdline_printchat("no bridge");//<< llendl;
						//sBridgeStatus = BUILDING;
						std::string directory = gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS,"bridge.xml");
						if(!LLFile::isfile(directory.c_str()))
						{
							//cmdline_printchat("file not there o.o");//<< llendl;
							sBridgeStatus = FAILED;
						}else
						{
							//cmdline_printchat("bridge.xml located. importing..");//<< llendl;
							gImportTracker.importer(directory,&setBridgeObject);
							sBridgeStatus = BUILDING;
						}
					}
				}
			}
			break;
		case RENAMING:
			{
				////cmdline_printchat("renaming");
				LLMessageSystem* msg = gMessageSystem;
				msg->newMessageFast(_PREHASH_ObjectAttach);
				msg->nextBlockFast(_PREHASH_AgentData);
				msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
				msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
				msg->addU8Fast(_PREHASH_AttachmentPoint, vCatType);
				
				msg->nextBlockFast(_PREHASH_ObjectData);
				msg->addU32Fast(_PREHASH_ObjectLocalID, sBridgeObject->getLocalID());
				msg->addQuatFast(_PREHASH_Rotation, LLQuaternion(0.0f, 0.0f, 0.0f, 1.0f));
				
				msg->sendReliable(gAgent.getRegion()->getHost());
				sBridgeStatus = FOLDERING;
			}
			break;
		case FOLDERING:
			{
				////cmdline_printchat("foldering");
				LLUUID vcatid;
				vcatid = gInventory.findCategoryByName(vBridgeOpCat);
				if(vcatid.isNull())
				{
					////cmdline_printchat("creating folder");
					vcatid = gInventory.createNewCategory(gAgent.getInventoryRootID(), LLAssetType::AT_NONE, vBridgeOpCat);
				}
				LLUUID bridge_id = findInventoryByName(vBridgeName);
				//cmdline_printchat("id="+bridge_id.asString());
				LLViewerInventoryItem* bridge = gInventory.getItem(bridge_id);
				if(bridge)
				{
					move_inventory_item(gAgent.getID(),gAgent.getSessionID(),bridge->getUUID(),vcatid,vBridgeName, NULL);
					sBridgeStatus = ACTIVE;
					////cmdline_printchat("moving to folder");
				}
			}
			break;
		case ACTIVE:
			LLUUID bridge = findInventoryByName(vBridgeName);
			//if(bridge)
			{
				//LLVOAvatar* avatar = gAgent.getAvatarObject();
				if(bridge.isNull() || !isworn(bridge))
				{
					////cmdline_printchat("reattaching");
					sBridgeStatus = UNINITIALIZED;
				}
			}
			break;
		}
	}
	return FALSE;
}
void JCLSLBridge::setBridgeObject(LLViewerObject* obj)
{
	sBridgeObject = obj;
	////cmdline_printchat("callback reached");
	sBridgeStatus = RENAMING;
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectName);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID,gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID,gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_LocalID,obj->getLocalID());
	msg->addStringFast(_PREHASH_Name,vBridgeName);
	gAgent.sendReliableMessage();
}

void JCLSLBridge::callback_fire(U32 callback_id, LLSD data)
{
	if (!callback_id)
		return;

	std::map<U32, JCBridgeCallback*>::iterator i;

	i = callback_map.find(callback_id);
	if (i != callback_map.end())
	{
		(*i).second->fire(data);
		callback_map.erase(i);
	}
}

U32 JCLSLBridge::registerCB(JCBridgeCallback* cb)
{
	if (!cb)
		return 0;

	lastcall++;
	if (!lastcall)
		lastcall++;

	callback_map[lastcall] = cb;
	while(callback_map.size() > 100)callback_map.erase(0);
	return lastcall;
}

LLSD JCLSLBridge::parse_string_to_list(std::string list, char sep)
{
	LLSD newlist;
	U32 count = 0;
	std::string token;
	std::istringstream iss(list);
	while ( getline(iss, token, sep) )
	{
		newlist[count] = token;
		count += 1;
	}
	return newlist;
}


void JCLSLBridge::processSoundTrigger(LLMessageSystem* msg,void**)
{
	LLUUID	sound_id,owner_id;
	msg->getUUIDFast(_PREHASH_SoundData, _PREHASH_SoundID, sound_id);
	msg->getUUIDFast(_PREHASH_SoundData, _PREHASH_OwnerID, owner_id);
	if(owner_id == gAgent.getID())
	{
		
		if(sound_id == LLUUID("420d76ad-c82b-349d-7b81-f00d0ca0f38f"))
		{
			if(sBridgeStatus == ACTIVE)
			{
				send_chat_from_viewer("emerald_bridge_rdy", CHAT_TYPE_SHOUT, JCLSLBridge::bridge_channel(gAgent.getID()));
			}else if(sBridgeStatus == FAILED)
			{
				send_chat_from_viewer("emerald_bridge_failed", CHAT_TYPE_SHOUT, JCLSLBridge::bridge_channel(gAgent.getID()));
			}else
			{
				send_chat_from_viewer("emerald_bridge_working", CHAT_TYPE_SHOUT, JCLSLBridge::bridge_channel(gAgent.getID()));
			}
		}
		
	}
}