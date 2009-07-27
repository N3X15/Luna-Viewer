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
#include "llchat.h"
#include "importtracker.h"

ImportTracker gImportTracker;

extern LLAgent gAgent;

void ImportTracker::prepare(LLSD& file_data)
{
	linksetgroups=file_data;
	llinfos << "LOADED LINKSETS, PREPARING.." << llendl;
	groupcounter=0;
	LLSD ls_llsd;
	ls_llsd=linksetgroups[groupcounter]["Object"];
	linksetoffset=linksetgroups[groupcounter]["ObjectPos"];
	initialPos=gAgent.getCameraPositionAgent();
	import(ls_llsd);
}

void ImportTracker::import(LLSD& ls_data)
{
	if(!(linkset.size()))
		if(!(linksetgroups.size()))
			initialPos=gAgent.getCameraPositionAgent();
	linkset = ls_data;
	updated=0;
	LLSD rot = linkset[0]["rotation"];
	rootrot.mQ[VX] = (F32)(rot[0].asReal());
	rootrot.mQ[VY] = (F32)(rot[1].asReal());
	rootrot.mQ[VZ] = (F32)(rot[2].asReal());
	rootrot.mQ[VW] = (F32)(rot[3].asReal());
	state = BUILDING;
	//llinfos << "IMPORTED, BUILDING.." << llendl;
	plywood_above_head();
}

void ImportTracker::expectRez()
{
	numberExpected++;
	state = WAND;
	//llinfos << "EXPECTING CUBE..." << llendl;
}

void ImportTracker::clear()
{
	localids.clear();
	linkset.clear();
	state = IDLE;
}
void ImportTracker::cleargroups()
{
	linksetgroups.clear();
	groupcounter=0;
	linksetoffset=LLVector3(0.0f,0.0f,0.0f);
	initialPos=LLVector3(0.0f,0.0f,0.0f);
	state = IDLE;
}

void ImportTracker::cleanUp()
{
	clear();
	if(linksetgroups.size())
	{
		if(groupcounter < (linksetgroups.size() - 1))
		{
			++groupcounter;
			LLSD ls_llsd;
			ls_llsd=linksetgroups[groupcounter]["Object"];
			linksetoffset=linksetgroups[groupcounter]["ObjectPos"];
			import(ls_llsd);
		}
	}
	else cleargroups();
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

				//llinfos << "LGG SENDING CUBE TEXTURE.." << llendl;
			}
		break;
		case BUILDING:
			
			if (justCreated && (int)localids.size() < linkset.size())
			{
				localids.push_back(newid);
				localids.sort();
				localids.unique();

				linkset[localids.size() -1]["LocalID"] = newid;
				LLSD prim = linkset[localids.size() -1];

				//MAKERIGHT
				if (!(prim).has("Updated"))
				{
					++updated;
					send_shape(prim);
					send_image(prim);
					send_extras(prim);
					send_namedesc(prim);
					send_vectors(prim,updated);
					(prim)["Updated"] = true;
				}
				if ((int)localids.size() < linkset.size())
				{
					plywood_above_head();
					return;
				}
				else
				{
					if (updated >= linkset.size())
					{
						updated=0;
						llinfos << "FINISHED BUILDING, LINKING.." << llendl;
						state = LINKING;
						link();
					}
				}
			}
		break;
		case LINKING:
			link();
		break;
	}
}

void ImportTracker::send_vectors(LLSD& prim,int counter)
{
	LLVector3 position = ((LLVector3)prim["position"] * rootrot) + root;
	LLSD rot = prim["rotation"];
	LLQuaternion rotq;
	rotq.mQ[VX] = (F32)(rot[0].asReal());
	rotq.mQ[VY] = (F32)(rot[1].asReal());
	rotq.mQ[VZ] = (F32)(rot[2].asReal());
	rotq.mQ[VW] = (F32)(rot[3].asReal());
	LLVector3 rotation;
	if(counter == 1)
		rotation = rotq.packToVector3();
	else
		rotation = (rotq * rootrot).packToVector3();
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
void send_chat_from_viewer(std::string utf8_out_text, EChatType type, S32 channel);
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

	if (prim.has("chat"))
	{
		send_chat_from_viewer(prim["chat"].asString(), CHAT_TYPE_SHOUT, 0);
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

void ImportTracker::send_namedesc(LLSD& prim)
{
	if(prim.has("name"))
	{
		LLMessageSystem* msg = gMessageSystem;
		msg->newMessageFast(_PREHASH_ObjectName);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU32Fast(_PREHASH_LocalID, prim["LocalID"].asInteger());
		msg->addStringFast(_PREHASH_Name, prim["name"]);
		
		msg->sendReliable(gAgent.getRegion()->getHost());
	}
	
	if(prim.has("description"))
	{
		LLMessageSystem* msg = gMessageSystem;
		msg->newMessageFast(_PREHASH_ObjectDescription);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU32Fast(_PREHASH_LocalID, prim["LocalID"].asInteger());
		msg->addStringFast(_PREHASH_Description, prim["description"]);
		
		msg->sendReliable(gAgent.getRegion()->getHost());
	}
}

void ImportTracker::link()
{	
	if(linkset.size() == 256)
	{
		LLMessageSystem* msg = gMessageSystem;
		msg->newMessageFast(_PREHASH_ObjectLink);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		
		LLSD::array_iterator prim = linkset.beginArray();
		++prim;
		for (; prim != linkset.endArray(); ++prim)
		{
			msg->nextBlockFast(_PREHASH_ObjectData);
			msg->addU32Fast(_PREHASH_ObjectLocalID, (*prim)["LocalID"].asInteger());		
		}
		
		msg->sendReliable(gAgent.getRegion()->getHost());

		LLMessageSystem* msg2 = gMessageSystem;
		msg2->newMessageFast(_PREHASH_ObjectLink);
		msg2->nextBlockFast(_PREHASH_AgentData);
		msg2->addUUIDFast(_PREHASH_AgentID, gAgent.getID() );
		msg2->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		
		LLSD prim2 = linkset[0];
		msg2->nextBlockFast(_PREHASH_ObjectData);
		msg2->addU32Fast(_PREHASH_ObjectLocalID, (prim2)["LocalID"].asInteger());		
		prim2 = linkset[1];
		msg2->nextBlockFast(_PREHASH_ObjectData);
		msg2->addU32Fast(_PREHASH_ObjectLocalID, (prim2)["LocalID"].asInteger());		

		msg2->sendReliable(gAgent.getRegion()->getHost());
	}
	else
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
	}

	llinfos << "FINISHED IMPORT" << llendl;
	
	if (linkset[0].has("Attachment"))
	{
		llinfos << "OBJECT IS ATTACHMENT, WAITING FOR POSITION PACKETS.." << llendl;
		state = POSITIONING;
		wear(linkset[0]);
	}
	else
		cleanUp();
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

	LLVector3 position = prim["attachpos"];
	
	LLSD rot = prim["attachrot"];
	LLQuaternion rotq;
	rotq.mQ[VX] = (F32)(rot[0].asReal());
	rotq.mQ[VY] = (F32)(rot[1].asReal());
	rotq.mQ[VZ] = (F32)(rot[2].asReal());
	rotq.mQ[VW] = (F32)(rot[3].asReal());
	LLVector3 rotation = rotq.packToVector3();
	U8 data[256];
	
	LLMessageSystem* msg2 = gMessageSystem;
	msg2->newMessageFast(_PREHASH_MultipleObjectUpdate);
	msg2->nextBlockFast(_PREHASH_AgentData);
	msg2->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg2->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	
	msg2->nextBlockFast(_PREHASH_ObjectData);
	msg2->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg2->addU8Fast(_PREHASH_Type, U8(0x01 | 0x08));
	htonmemcpy(&data[0], &(position.mV), MVT_LLVector3, 12);
	msg2->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg2->nextBlockFast(_PREHASH_ObjectData);
	msg2->addU32Fast(_PREHASH_ObjectLocalID, prim["LocalID"].asInteger());
	msg2->addU8Fast(_PREHASH_Type, U8(0x02 | 0x08));
	htonmemcpy(&data[0], &(rotation.mV), MVT_LLQuaternion, 12); 
	msg2->addBinaryDataFast(_PREHASH_Data, data, 12);
	
	msg2->sendReliable(gAgent.getRegion()->getHost());
	llinfos << "POSITIONED, IMPORT COMPLETED" << llendl;
	cleanUp();
}

void ImportTracker::plywood_above_head()
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
		msg->addVector3Fast(_PREHASH_Scale, LLVector3(0.52345f, 0.52346f, 0.52347f));
		LLQuaternion rot;
		msg->addQuatFast(_PREHASH_Rotation, rot);
		LLViewerRegion *region = gAgent.getRegion();
		
		if (!localids.size())
			root = (initialPos + linksetoffset);
		
		msg->addVector3Fast(_PREHASH_RayStart, root);
		msg->addVector3Fast(_PREHASH_RayEnd, root);
		msg->addU8Fast(_PREHASH_BypassRaycast, (U8)TRUE );
		msg->addU8Fast(_PREHASH_RayEndIsIntersection, (U8)FALSE );
		msg->addU8Fast(_PREHASH_State, (U8)0);
		msg->addUUIDFast(_PREHASH_RayTargetID, LLUUID::null);
		msg->sendReliable(region->getHost());
}

