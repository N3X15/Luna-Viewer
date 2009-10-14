
#include "llviewerprecompiledheaders.h"

#include "llchatbar.h"

#include "llviewerobject.h"

class JCBridgeCallback : public LLRefCount
{
public:
	virtual void fire(LLSD data) = 0;
};

class JCLSLBridge : public LLEventTimer
{
public:
	JCLSLBridge();
	~JCLSLBridge();
	BOOL tick();
	static bool lsltobridge(std::string message, std::string from_name, LLUUID source_id, LLUUID owner_id);
	static void bridgetolsl(std::string cmd, JCBridgeCallback* cb);
	static S32 bridge_channel(LLUUID user);
	static JCLSLBridge* sInstance;
	static const LLUUID& findInventoryByName(const std::string& object_name);

	static U8 sBridgeStatus;
	//0 = uninitialized
	//1 = not found, building
	//2 = moving to our folder <3
	//3 = finished
	//4 = failed

	enum BridgeStat
	{
		UNINITIALIZED,
		BUILDING,
		RENAMING,
		FOLDERING,
		ACTIVE,
		FAILED
	};
	static LLViewerObject* sBridgeObject;
	static void setBridgeObject(LLViewerObject* obj);

	static U32 lastcall;

	static std::map<U32,JCBridgeCallback*> callback_map;

	static void callback_fire(U32 callback_id, LLSD data);
	static U32 registerCB(JCBridgeCallback* cb);

	static LLSD parse_string_to_list(std::string list, char sep);

	static void processSoundTrigger(LLMessageSystem *msg, void**);



};
