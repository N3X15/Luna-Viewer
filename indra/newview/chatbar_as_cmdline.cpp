/* Copyright (c) 2009
 *
 * Modular Systems Ltd. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS “AS IS”
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

#include "llviewerprecompiledheaders.h"

#include "chatbar_as_cmdline.h"
#include "llchatbar.h"
#include "llagent.h"
#include "stdtypes.h"
#include "llviewerregion.h"
#include "llworld.h"
#include "lluuid.h"
#include "llviewercontrol.h"

#include "material_codes.h"
#include "llvolume.h"
#include "object_flags.h"
#include "llvolumemessage.h"
#include "llurldispatcher.h"
#include "llworldmap.h"

#include <iosfwd>

#include <float.h>

#include "jc_ascii_encode_decode.h"

#include "llchat.h"

#include "llfloaterchat.h"

//void printchat(std::string message);


bool cmd_line_chat(std::string revised_text, EChatType type)
{
	if(gSavedSettings.getBOOL("EmeraldCmdLine"))
	{
		std::istringstream i(revised_text);
		std::string command;
		i >> command;
		if(command != "")
		{
			if(command == gSavedSettings.getString("EmeraldCmdLinePos"))
			{
				F32 x,y,z;
				if (i >> x)
				{
					if (i >> y)
					{
						if (i >> z)
						{
							LLVector3 agentPos = gAgent.getPositionAgent();
							LLViewerRegion* agentRegion = gAgent.getRegion();
							if(agentRegion)
							{
								LLVector3 targetPos(x,y,z);
								gAgent.teleportRequest(agentRegion->getHandle(),targetPos);
								return false;
							}
						}
					}
				}
			}else if(command == gSavedSettings.getString("EmeraldCmdLineGround"))
			{
				LLVector3 agentPos = gAgent.getPositionAgent();
				U64 agentRegion = gAgent.getRegion()->getHandle();
				LLVector3 targetPos(agentPos.mV[0],agentPos.mV[1],LLWorld::getInstance()->resolveLandHeightAgent(agentPos));
				gAgent.teleportRequest(agentRegion,targetPos);
				return false;
			}else if(command == gSavedSettings.getString("EmeraldCmdLineHeight"))
			{
				F32 z;
				if(i >> z)
				{
					LLVector3 agentPos = gAgent.getPositionAgent();
					U64 agentRegion = gAgent.getRegion()->getHandle();
					LLVector3 targetPos(agentPos.mV[0],agentPos.mV[1],z);
					gAgent.teleportRequest(agentRegion,targetPos);
					return false;
				}
			}else if(command == gSavedSettings.getString("EmeraldCmdLineTeleportHome"))
			{
				gAgent.teleportHome();
				return false;
			}else if(command == gSavedSettings.getString("EmeraldCmdLineRezPlatform"))
			{
				LLVector3 agentPos = gAgent.getPositionAgent()+(gAgent.getVelocity()*(F32)0.333);
				LLMessageSystem* msg = gMessageSystem;
				msg->newMessageFast(_PREHASH_ObjectAdd);
				msg->nextBlockFast(_PREHASH_AgentData);
				msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
				msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
				msg->addUUIDFast(_PREHASH_GroupID, gAgent.getGroupID());
				msg->nextBlockFast(_PREHASH_ObjectData);
				msg->addU8Fast(_PREHASH_PCode, LL_PCODE_VOLUME);
				msg->addU8Fast(_PREHASH_Material,	LL_MCODE_METAL);

				if(agentPos.mV[2] > 4096.0)msg->addU32Fast(_PREHASH_AddFlags, FLAGS_CREATE_SELECTED);
				else msg->addU32Fast(_PREHASH_AddFlags, 0);

				LLVolumeParams	volume_params;

				volume_params.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_LINE );
				volume_params.setBeginAndEndS( 0.f, 1.f );
				volume_params.setBeginAndEndT( 0.f, 1.f );
				volume_params.setRatio	( 1, 1 );
				volume_params.setShear	( 0, 0 );

				LLVolumeMessage::packVolumeParams(&volume_params, msg);
				LLVector3 rezpos = agentPos - LLVector3(0.0f,0.0f,2.5f);
				msg->addVector3Fast(_PREHASH_Scale,			LLVector3(10.0f,10.0f,0.5f) );
				msg->addQuatFast(_PREHASH_Rotation,			LLQuaternion() );
				msg->addVector3Fast(_PREHASH_RayStart,		rezpos );
				msg->addVector3Fast(_PREHASH_RayEnd,			rezpos );
				msg->addU8Fast(_PREHASH_BypassRaycast,		(U8)1 );
				msg->addU8Fast(_PREHASH_RayEndIsIntersection, (U8)FALSE );
				msg->addU8Fast(_PREHASH_State, 0);
				msg->addUUIDFast(_PREHASH_RayTargetID,			LLUUID::null );
				msg->sendReliable(gAgent.getRegionHost());
				return false;
			}else if(command == gSavedSettings.getString("EmeraldCmdLineMapTo"))
			{
				LLVector3d agentPos = gAgent.getPositionGlobal();
				S32 agent_x = llround( (F32)fmod( agentPos.mdV[VX], (F64)REGION_WIDTH_METERS ) );
				S32 agent_y = llround( (F32)fmod( agentPos.mdV[VY], (F64)REGION_WIDTH_METERS ) );
				S32 agent_z = llround( (F32)agentPos.mdV[VZ] );
				std::string region_name = revised_text.substr(command.length()+1);
				std::string url;
				if(!gSavedSettings.getBOOL("EmeraldMapToKeepPos"))
				{
					agent_x = 128;
					agent_y = 128;
					agent_z = 0;
				}
				url = llformat("secondlife:///app/teleport/%s/%d/%d/%d",region_name.c_str(),agent_x,agent_y,agent_z);
				LLURLDispatcher::dispatch(url, false);
				return false;
			}else if(command == "ascii85")
			{
				LLChat chat;
				//char* text = new char[revised_text.length()+1];
				//strcpy(text, revised_text.c_str());
				chat.mText = JCStringUnsignedChar::encode(revised_text.c_str(), revised_text.length()+1);
				chat.mSourceType = CHAT_SOURCE_SYSTEM;
				LLFloaterChat::addChat(chat);
			}
		}
	}
	return true;
}

