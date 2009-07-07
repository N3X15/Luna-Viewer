/** 
 * @file importtracker.cpp
 * @brief A utility for importing linksets from XML.
 * Discrete wuz here
 */

#include "llviewerprecompiledheaders.h"

#include "llframetimer.h"
#include "llprimitive.h"
#include "llviewerregion.h"
#include "llvolumemessage.h"

#include "importtracker.h"

ImportTracker gImportTracker;

extern LLAgent gAgent;

void ImportTracker::import(LLSD& file_data)
{
	linkset = file_data;
	state = REZZING;
	llinfos << "IMPORTED, REZZING.." << llendl;
	plywood_above_head();
}

void ImportTracker::expectRez()
{
	numberExpected++;
	state = WAND;
	llinfos << "EXPECTING CUBE..." << llendl;
}

void ImportTracker::clear()
{
	localids.clear();
	linkset.clear();
	state = IDLE;
}

void ImportTracker::get_update(S32 newid, BOOL justCreated, BOOL createSelected)
{
	switch (state)
	{
		//lgg crap
		case WAND:
			if(justCreated && createSelected)
			{
				numberExpected--;
				if(numberExpected<=0)
					state=IDLE;
				LLMessageSystem* msg = gMessageSystem;
				msg->newMessageFast(_PREHASH_ObjectImage);
				msg->nextBlockFast(_PREHASH_AgentData);
				msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
				msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());	
				msg->nextBlockFast(_PREHASH_ObjectData);				
				msg->addU32Fast(_PREHASH_ObjectLocalID,  (U32)newid);
				msg->addStringFast(_PREHASH_MediaURL, NULL);
	
				LLPrimitive obj;
				obj.setNumTEs(U8(10));	
				S32 shinnyLevel = 0;
				if(gSavedSettings.getString("EmeraldBuildPrefs_Shiny")== "None") shinnyLevel = 0;
				if(gSavedSettings.getString("EmeraldBuildPrefs_Shiny")== "Low") shinnyLevel = 1;
				if(gSavedSettings.getString("EmeraldBuildPrefs_Shiny")== "Medium") shinnyLevel = 2;
				if(gSavedSettings.getString("EmeraldBuildPrefs_Shiny")== "High") shinnyLevel = 3;
				
				for (int i = 0; i < 10; i++)
				{
					LLTextureEntry tex =  LLTextureEntry(LLUUID(gSavedSettings.getString("EmeraldBuildPrefs_Texture")));
					tex.setColor(gSavedSettings.getColor4("EmeraldBuildPrefs_Color"));
					tex.setAlpha(1.0 - ((gSavedSettings.getF32("EmeraldBuildPrefs_Alpha")) / 100.0));
					tex.setGlow(gSavedSettings.getF32("EmeraldBuildPrefs_Glow"));
					if(gSavedSettings.getBOOL("EmeraldBuildPrefs_FullBright"))
					{
						tex.setFullbright(TEM_FULLBRIGHT_MASK);
					}
									
					tex.setShiny((U8) shinnyLevel & TEM_SHINY_MASK);
					
					obj.setTE(U8(i), tex);
				}
	
				obj.packTEMessage(gMessageSystem);
	
				msg->sendReliable(gAgent.getRegion()->getHost());
				
				msg->newMessage("ObjectFlagUpdate");
				msg->nextBlockFast(_PREHASH_AgentData);
				msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
				msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
				msg->addU32Fast(_PREHASH_ObjectLocalID, (U32)newid );
				msg->addBOOLFast(_PREHASH_UsePhysics, gSavedSettings.getBOOL("EmeraldBuildPrefs_Physical"));
				msg->addBOOL("IsTemporary", gSavedSettings.getBOOL("EmeraldBuildPrefs_Temporary"));
				msg->addBOOL("IsPhantom", gSavedSettings.getBOOL("EmeraldBuildPrefs_Phantom") );
				msg->addBOOL("CastsShadows", true );
				msg->sendReliable(gAgent.getRegion()->getHost());				

				llinfos << "LGG SENDING CUBE TEXTURE.." << llendl;
			}
		break;
		case REZZING:
			if (justCreated && (int)localids.size() < linkset.size())
			{
				localids.push_back(newid);
				localids.sort();
				localids.unique();
				
				if ((int)localids.size() < linkset.size())
					//plywood_above_head();
					return;
				else
				{
					for (int i = 0; i < linkset.size(); i++)
					{
						linkset[i]["LocalID"] = localids.front();
						localids.pop_front();
					}
					
					llinfos << "FINISHED REZZING, UPDATING.." << llendl;
					state = COPYING;
					update_next();
				}
			}
		break;
		case COPYING:
			for (LLSD::array_iterator prim = linkset.beginArray(); prim != linkset.endArray(); ++prim)
			{
				if ((*prim)["LocalID"].asInteger() == newid)
					(*prim)["Received"] = true;
			}
			update_next();
		break;
		case LINKING:
			link();
		break;
		case POSITIONING:
			if (newid == linkset[0]["LocalID"].asInteger())
			{
				position(linkset[0]);
				llinfos << "POSITIONED, IMPORT COMPLETED" << llendl;
				clear();
			}
		break;
	}
}

void ImportTracker::send_vectors(LLSD& prim)
{
	LLVector3 position = prim["position"] + root;
	
	LLSD rot = prim["rotation"];
	LLQuaternion rotq;
	rotq.mQ[VX] = (F32)(rot[0].asReal());
	rotq.mQ[VY] = (F32)(rot[1].asReal());
	rotq.mQ[VZ] = (F32)(rot[2].asReal());
	rotq.mQ[VW] = (F32)(rot[3].asReal());
	LLVector3 rotation = rotq.packToVector3();
	LLVector3 scale = prim["scale"];
	U8 data[256];
	
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_MultipleObjectUpdate);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addU8Fast(_PREHASH_Type, U8(0x01));
	htonmemcpy(&data[0], &(position.mV), MVT_LLVector3, 12);
	msg->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addU8Fast(_PREHASH_Type, U8(0x02));
	htonmemcpy(&data[0], &(rotation.mV), MVT_LLQuaternion, 12); 
	msg->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addU8Fast(_PREHASH_Type, U8(0x04));
	htonmemcpy(&data[0], &(scale.mV), MVT_LLVector3, 12); 
	msg->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}

void ImportTracker::send_shape(LLSD& prim)
{
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectShape);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	
	LLVolumeParams params;
	params.fromLLSD(prim["volume"]);
	LLVolumeMessage::packVolumeParams(&params, msg);
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}

void ImportTracker::send_image(LLSD& prim)
{
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectImage);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addStringFast(_PREHASH_MediaURL, NULL);
	
	LLPrimitive obj;
	LLSD tes = prim["textures"];
	obj.setNumTEs(U8(tes.size()));
	
	for (int i = 0; i < tes.size(); i++)
	{
		LLTextureEntry tex;
		tex.fromLLSD(tes[i]);
		obj.setTE(U8(i), tex);
	}
	
	obj.packTEMessage(gMessageSystem);
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}

void ImportTracker::send_extras(LLSD& prim)
{	
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectExtraParams);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	LLPrimitive obj;
	
	if (prim.has("flexible"))
	{
		LLFlexibleObjectData flexi;
		flexi.fromLLSD(prim["flexible"]);
		U8 tmp[MAX_OBJECT_PARAMS_SIZE];
		LLDataPackerBinaryBuffer dpb(tmp, MAX_OBJECT_PARAMS_SIZE);
		
		if (flexi.pack(dpb))
		{
			U32 datasize = (U32)dpb.getCurrentSize();
			
			msg->nextBlockFast(_PREHASH_ObjectData);
			msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());

			msg->addU16Fast(_PREHASH_ParamType, 0x10);
			msg->addBOOLFast(_PREHASH_ParamInUse, true);

			msg->addU32Fast(_PREHASH_ParamSize, datasize);
			msg->addBinaryDataFast(_PREHASH_ParamData, tmp, datasize);
		}
	}
	
	if (prim.has("light"))
	{
		LLLightParams light;
		light.fromLLSD(prim["light"]);
		
		U8 tmp[MAX_OBJECT_PARAMS_SIZE];
		LLDataPackerBinaryBuffer dpb(tmp, MAX_OBJECT_PARAMS_SIZE);
		
		if (light.pack(dpb))
		{
			U32 datasize = (U32)dpb.getCurrentSize();
			
			msg->nextBlockFast(_PREHASH_ObjectData);
			msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());

			msg->addU16Fast(_PREHASH_ParamType, 0x20);
			msg->addBOOLFast(_PREHASH_ParamInUse, true);

			msg->addU32Fast(_PREHASH_ParamSize, datasize);
			msg->addBinaryDataFast(_PREHASH_ParamData, tmp, datasize);
		}
	}
	
	if (prim.has("sculpt"))
	{
		LLSculptParams sculpt;
		sculpt.fromLLSD(prim["sculpt"]);
		
		U8 tmp[MAX_OBJECT_PARAMS_SIZE];
		LLDataPackerBinaryBuffer dpb(tmp, MAX_OBJECT_PARAMS_SIZE);
		
		if (sculpt.pack(dpb))
		{
			U32 datasize = (U32)dpb.getCurrentSize();
			
			msg->nextBlockFast(_PREHASH_ObjectData);
			msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());

			msg->addU16Fast(_PREHASH_ParamType, 0x30);
			msg->addBOOLFast(_PREHASH_ParamInUse, true);

			msg->addU32Fast(_PREHASH_ParamSize, datasize);
			msg->addBinaryDataFast(_PREHASH_ParamData, tmp, datasize);
		}
	}
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}

void ImportTracker::update_next()
{
	int received = 0;
	
	for (LLSD::array_iterator prim = linkset.beginArray(); prim != linkset.endArray(); ++prim)
	{
		if (!(*prim).has("Updated"))
		{
			send_shape(*prim);
			send_image(*prim);
			send_extras(*prim);
			send_vectors(*prim);
			(*prim)["Updated"] = true;
			return;
		}
		else if ((*prim).has("Received"))
			++received;
	}
	
	if (received == linkset.size())
	{
		llinfos << "FINISHED UPDATING, LINKING.." << llendl;
		state = LINKING;
		link();
	}
}

void ImportTracker::link()
{	
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectLink);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	for (LLSD::array_iterator prim = linkset.beginArray(); prim != linkset.endArray(); ++prim)
	{
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU32Fast(_PREHASH_ObjectLocalID, (*prim)["LocalID"].asInteger());		
	}
	
	msg->sendReliable(gAgent.getRegion()->getHost());
	
	llinfos << "FINISHED IMPORT" << llendl;
	
	if (linkset[0].has("Attachment"))
	{
		llinfos << "OBJECT IS ATTACHMENT, WAITING FOR POSITION PACKETS.." << llendl;
		state = POSITIONING;
		wear(linkset[0]);
	}
	else
		clear();
}

void ImportTracker::wear(LLSD &prim)
{
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ObjectAttach);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->addU8Fast(_PREHASH_AttachmentPoint, U8(prim["Attachment"].asInteger()));
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addQuatFast(_PREHASH_Rotation, LLQuaternion(0.0f, 0.0f, 0.0f, 1.0f));
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}

void ImportTracker::position(LLSD &prim)
{
	LLVector3 position = prim["attachpos"];
	
	LLSD rot = prim["attachrot"];
	LLQuaternion rotq;
	rotq.mQ[VX] = (F32)(rot[0].asReal());
	rotq.mQ[VY] = (F32)(rot[1].asReal());
	rotq.mQ[VZ] = (F32)(rot[2].asReal());
	rotq.mQ[VW] = (F32)(rot[3].asReal());
	LLVector3 rotation = rotq.packToVector3();
	U8 data[256];
	
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_MultipleObjectUpdate);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addU8Fast(_PREHASH_Type, U8(0x01 | 0x08));
	htonmemcpy(&data[0], &(position.mV), MVT_LLVector3, 12);
	msg->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg->nextBlockFast(_PREHASH_ObjectData);
	msg->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg->addU8Fast(_PREHASH_Type, U8(0x02 | 0x08));
	htonmemcpy(&data[0], &(rotation.mV), MVT_LLQuaternion, 12); 
	msg->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg->sendReliable(gAgent.getRegion()->getHost());
}

void ImportTracker::plywood_above_head()
{
	for (int i = 0; i < linkset.size(); i++)
	{
		LLMessageSystem* msg = gMessageSystem;
		msg->newMessageFast(_PREHASH_ObjectAdd);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		msg->addUUIDFast(_PREHASH_GroupID, gAgent.getGroupID());
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU8Fast(_PREHASH_Material, 3);
		msg->addU32Fast(_PREHASH_AddFlags, 0);
		LLVolumeParams	volume_params;
		volume_params.setType(0x01, 0x10);
		volume_params.setBeginAndEndS(0.f, 1.f);
		volume_params.setBeginAndEndT(0.f, 1.f);
		volume_params.setRatio(1, 1);
		volume_params.setShear(0, 0);
		LLVolumeMessage::packVolumeParams(&volume_params, msg);
		msg->addU8Fast(_PREHASH_PCode, 9);
		msg->addVector3Fast(_PREHASH_Scale, LLVector3(0.5f, 0.5f, 0.5f));
		LLQuaternion rot;
		msg->addQuatFast(_PREHASH_Rotation, rot);
		LLViewerRegion *region = gAgent.getRegion();
		
		if (!localids.size())
			root = region->getPosRegionFromGlobal(gAgent.getPositionGlobal()) + LLVector3(0.0f, 0.0f, .0f);
		
		msg->addVector3Fast(_PREHASH_RayStart, root);
		msg->addVector3Fast(_PREHASH_RayEnd, root);
		msg->addU8Fast(_PREHASH_BypassRaycast, (U8)TRUE );
		msg->addU8Fast(_PREHASH_RayEndIsIntersection, (U8)FALSE );
		msg->addU8Fast(_PREHASH_State, (U8)0);
		msg->addUUIDFast(_PREHASH_RayTargetID, LLUUID::null);
		msg->sendReliable(region->getHost());
	}
}

