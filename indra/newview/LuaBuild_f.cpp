#include "llviewerprecompiledheaders.h"
#include "LuaBuild_f.h"

#include "llprimitive.h"
#include "llviewerobject.h"
#include "llvolume.h" // PCodes.
#include "llworld.h"
#include "llviewerregion.h"
#include "llworldmap.h"
#include "llagent.h"
#include "llviewerobjectlist.h"
#include "object_flags.h"
#include "llvolumemessage.h"

extern LLAgent gAgent;

Object::Object(int pcode,bool viewerside)
{
	if(viewerside)
		mObject=gObjectList.createObjectViewer((LLPCode)pcode, gAgent.getRegion());
	else
	{
		mObject=NULL;
		mWaiting=true;
		CB_Args0(plyWood);
		while(true)
		{
			// lock to main
			{
				FLLua::CriticalSection cs();
				if(mReady.notNull())
				{
					mObject=gObjectList.findObject(mReady);
					mWaiting=false;
					break;
				}
			}
			FLLua::yield();
		}
	}
}

Object::Object(LLUUID id)
{
	mObject=gObjectList.findObject(mReady);
}

void Object::plyWood()
{
		LLMessageSystem* msg = gMessageSystem;
		msg->newMessageFast(_PREHASH_ObjectAdd);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		msg->addUUIDFast(_PREHASH_GroupID, gAgent.getGroupID());
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU8Fast(_PREHASH_Material, 3);
		//msg->addU32Fast(_PREHASH_AddFlags, ); // CREATE_SELECTED
		LLVolumeParams	volume_params;
		volume_params.setType(0x01, 0x10);
		volume_params.setBeginAndEndS(0.f, 1.f);
		volume_params.setBeginAndEndT(0.f, 1.f);
		volume_params.setRatio(1, 1);
		volume_params.setShear(0, 0);
		LLVolumeMessage::packVolumeParams(&volume_params, msg);
		msg->addU8Fast(_PREHASH_PCode, 9);
		msg->addVector3Fast(_PREHASH_Scale, LLVector3(0.52346f, 0.52347f, 0.52348f));
		LLQuaternion rot;
		msg->addQuatFast(_PREHASH_Rotation, rot);
		LLViewerRegion *region = gAgent.getRegion();
		LLVector3 root(0.f,0.f,0.1f);
		root+=gAgent.getCameraPositionAgent();
		msg->addVector3Fast(_PREHASH_RayStart, root);
		msg->addVector3Fast(_PREHASH_RayEnd, root);
		msg->addU8Fast(_PREHASH_BypassRaycast, (U8)TRUE );
		msg->addU8Fast(_PREHASH_RayEndIsIntersection, (U8)FALSE );
		msg->addU8Fast(_PREHASH_State, (U8)0);
		msg->addUUIDFast(_PREHASH_RayTargetID, LLUUID::null);
		msg->sendReliable(region->getHost());
}


