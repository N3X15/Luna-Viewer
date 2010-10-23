/* Copyright (c) 2010
 *
 * Modular Systems All rights reserved.
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
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS �AS IS�
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

#include "llcalc.h"

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
#include "llworld.h"
#include "llworldmap.h"
#include "floateravatarlist.h"
#include "floaterao.h"
#include "llviewerobjectlist.h"
#include "llviewertexteditor.h"
#include "llvoavatar.h"
#include "lltooldraganddrop.h"
#include "llinventorymodel.h"
#include "llselectmgr.h"

#include <iosfwd>

#include <float.h>

#include "llchat.h"

#include "llfloaterchat.h"
#include "llviewerparcelmgr.h"
#include "llviewerparcelmedia.h"
#include "llparcel.h"
#include "llaudioengine.h"
#include "llviewerparcelmediaautoplay.h"
#include "lloverlaybar.h"
//#include "lggautocorrectfloater.h"
//#include "lggautocorrect.h"


void cmdline_printchat(std::string message);
void cmdline_rezplat(bool use_saved_value = true, F32 visual_radius = 30.0);
void cmdline_tp2name(std::string target);




LLViewerInventoryItem::item_array_t findInventoryInFolder(const std::string& ifolder)
{
	LLUUID folder = gInventory.findCategoryByName(ifolder);
	LLViewerInventoryCategory::cat_array_t cats;
	LLViewerInventoryItem::item_array_t items;
	//ObjectContentNameMatches objectnamematches(ifolder);
	gInventory.collectDescendents(folder,cats,items,FALSE);//,objectnamematches);

	return items;
}

class JCZface : public LLEventTimer
{
public:
	JCZface(std::stack<LLViewerInventoryItem*> stack, LLUUID dest, F32 pause) : LLEventTimer( pause )
	{
		cmdline_printchat("initialized");
		instack = stack;
		indest = dest;
	}
	~JCZface()
	{
		cmdline_printchat("deinitialized");
	}
	BOOL tick()
	{
		LLViewerInventoryItem* subj = instack.top();
		instack.pop();
		LLViewerObject *objectp = gObjectList.findObject(indest);
		if(objectp)
		{
			cmdline_printchat(std::string("dropping ")+subj->getName());
			LLToolDragAndDrop::dropInventory(objectp,subj,LLToolDragAndDrop::SOURCE_AGENT,gAgent.getID());
			return (instack.size() == 0);
		}else
		{
			cmdline_printchat("object lost");
			return TRUE;
		}	
	}


private:
	std::stack<LLViewerInventoryItem*> instack;
	LLUUID indest;
};

class JCZtake : public LLEventTimer
{
public:
	BOOL mRunning;

	JCZtake(const LLUUID& target) : LLEventTimer(1.25f), mTarget(target), mRunning(FALSE), mCountdown(5)
	{
		cmdline_printchat("Ztake initialised.");
	}
	~JCZtake()
	{
		cmdline_printchat("Ztake shutdown.");
	}
	BOOL tick()
	{
		{
			LLMessageSystem *msg = gMessageSystem;
			for(LLObjectSelection::iterator itr=LLSelectMgr::getInstance()->getSelection()->begin();
				itr!=LLSelectMgr::getInstance()->getSelection()->end();++itr)
			{
				LLSelectNode* node = (*itr);
				LLViewerObject* object = node->getObject();
				U32 localid=object->getLocalID();
				if(mDonePrims.find(localid) == mDonePrims.end())
				{
					mDonePrims.insert(localid);
					std::string name = llformat("%ix%ix%i",(int)object->getScale().mV[VX],(int)object->getScale().mV[VY],(int)object->getScale().mV[VZ]);
					msg->newMessageFast(_PREHASH_ObjectName);
					msg->nextBlockFast(_PREHASH_AgentData);
					msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
					msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
					msg->nextBlockFast(_PREHASH_ObjectData);
					msg->addU32Fast(_PREHASH_LocalID, localid);
					msg->addStringFast(_PREHASH_Name, name);
					gAgent.sendReliableMessage();
					mToTake.push_back(localid);
				}
			}

			if(mCountdown > 0) {
				cmdline_printchat(llformat("%i...", mCountdown--));
			}
			else if(mToTake.size() > 0) {
				msg->newMessageFast(_PREHASH_DeRezObject);
				msg->nextBlockFast(_PREHASH_AgentData);
				msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
				msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
				msg->nextBlockFast(_PREHASH_AgentBlock);
				msg->addUUIDFast(_PREHASH_GroupID, LLUUID::null);
				msg->addU8Fast(_PREHASH_Destination, 4);
				msg->addUUIDFast(_PREHASH_DestinationID, mTarget);
				LLUUID rand;
				rand.generate();
				msg->addUUIDFast(_PREHASH_TransactionID, rand);
				msg->addU8Fast(_PREHASH_PacketCount, 1);
				msg->addU8Fast(_PREHASH_PacketNumber, 0);
				msg->nextBlockFast(_PREHASH_ObjectData);
				msg->addU32Fast(_PREHASH_ObjectLocalID, mToTake[0]);
				gAgent.sendReliableMessage();
				mToTake.erase(mToTake.begin());
				if(mToTake.size() % 10 == 0) {
					if(mToTake.size() == 0) {
						cmdline_printchat("Ztake done! (You might want to try \"ztake off\")");
					} else {
						cmdline_printchat(llformat("Ztake: %i left to take.", mToTake.size()));
					}
				}
			}
				
		}
		return mRunning;
	}

private:
	std::set<U32> mDonePrims;
	std::vector<U32> mToTake;
	LLUUID mTarget;
	int mCountdown;
};

JCZtake *ztake;

void invrepair()
{
	LLViewerInventoryCategory::cat_array_t cats;
	LLViewerInventoryItem::item_array_t items;
	//ObjectContentNameMatches objectnamematches(ifolder);
	gInventory.collectDescendents(gAgent.getInventoryRootID(),cats,items,FALSE);//,objectnamematches);
}




bool cmd_line_chat(std::string revised_text, EChatType type, bool from_gesture)
{
	static BOOL *sPhoenixCmdLine = rebind_llcontrol<BOOL>("PhoenixCmdLine", &gSavedSettings, true);
	static std::string *sPhoenixCmdLinePos = rebind_llcontrol<std::string>("PhoenixCmdLinePos", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineDrawDistance = rebind_llcontrol<std::string>("PhoenixCmdLineDrawDistance", &gSavedSettings, true);
	static std::string *sPhoenixCmdTeleportToCam = rebind_llcontrol<std::string>("PhoenixCmdTeleportToCam", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineAO = rebind_llcontrol<std::string>("PhoenixCmdLineAO", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineKeyToName = rebind_llcontrol<std::string>("PhoenixCmdLineKeyToName", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineOfferTp = rebind_llcontrol<std::string>("PhoenixCmdLineOfferTp", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineGround = rebind_llcontrol<std::string>("PhoenixCmdLineGround", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineHeight = rebind_llcontrol<std::string>("PhoenixCmdLineHeight", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineTeleportHome = rebind_llcontrol<std::string>("PhoenixCmdLineTeleportHome", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineRezPlatform = rebind_llcontrol<std::string>("PhoenixCmdLineRezPlatform", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineMapTo = rebind_llcontrol<std::string>("PhoenixCmdLineMapTo", &gSavedSettings, true);
	static BOOL *sPhoenixCmdLineMapToKeepPos = rebind_llcontrol<BOOL>("PhoenixCmdLineMapToKeepPos", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineCalc = rebind_llcontrol<std::string>("PhoenixCmdLineCalc", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineTP2 = rebind_llcontrol<std::string>("PhoenixCmdLineTP2", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineClearChat = rebind_llcontrol<std::string>("PhoenixCmdLineClearChat", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineMedia = rebind_llcontrol<std::string>("PhoenixCmdLineMedia", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineMusic = rebind_llcontrol<std::string>("PhoenixCmdLineMusic", &gSavedSettings, true);
	static std::string *sPhoenixCmdLineAutocorrect = rebind_llcontrol<std::string>("PhoenixCmdLineAutocorrect", &gSavedSettings, true);
	static std::string *sLunaCmdLineLua = rebind_llcontrol<std::string>("LunaCmdLineLua", &gSavedSettings, true);
	static std::string *sLunaCmdLineMacro = rebind_llcontrol<std::string>("LunaCmdLineMacro", &gSavedSettings, true);
	static std::string *sLunaCmdLineShortMacro = rebind_llcontrol<std::string>("LunaCmdLineShortMacro", &gSavedSettings, true);
	//static std::string *sPhoenixCmdUndeform = rebind_llcontrol<std::string>("PhoenixCmdUndeform", &gSavedSettings, true);
	//gSavedSettings.getString("PhoenixCmdUndeform")
	
	if(*sPhoenixCmdLine)
	{
		std::istringstream i(revised_text);
		std::string command;
		i >> command;
		command = utf8str_tolower(command);
		if(command != "")
		{
			if(command == *sPhoenixCmdLinePos)
			{
				F32 x,y,z;
				if (i >> x)
				{
					if (i >> y)
					{
						if (i >> z)
						{
							LLVector3 agentPos = gAgent.getPositionAgent();
							LLViewerRegion* agentRegionp = gAgent.getRegion();
							if(agentRegionp)
							{
								LLVector3 targetPos(x,y,z);
								LLVector3d pos_global = from_region_handle(agentRegionp->getHandle());
								pos_global += LLVector3d((F64)targetPos.mV[0],(F64)targetPos.mV[1],(F64)targetPos.mV[2]);
								gAgent.teleportViaLocation(pos_global);
								return false;
							}
						}
					}
				}
			}
			else if(command == *sPhoenixCmdLineDrawDistance)
			{
				if(from_gesture)
				{
					cmdline_printchat("Due to the changes in code, it is no longer necessary to use this gesture.");
					gSavedSettings.setBOOL("RenderFarClipStepping",TRUE);
					return false;
				}
                int drawDist;
                if(i >> drawDist)
                {
                    gSavedSettings.setF32("RenderFarClip", drawDist);
                    gAgent.mDrawDistance=drawDist;
                    char buffer[DB_IM_MSG_BUF_SIZE * 2]; 
                    snprintf(buffer,sizeof(buffer),"Draw distance set to: %dm",drawDist);
					cmdline_printchat(std::string(buffer));
					return false;
                }
			}
			else if(command == *sPhoenixCmdTeleportToCam)
            {
				gAgent.teleportViaLocation(gAgent.getCameraPositionGlobal());
				return false;
            }
			/*else if(command == *sPhoenixCmdUndeform)
            {
				llinfos << "UNDEFORM: Do you feel your bones cracking back into place?" << llendl;
				gAgent.getAvatarObject()->undeform();
				return false;
            }*/ //what the fuck is this shit, thought it would be something useful like repairing the skeleton but its some shitty playing of inworld anims
			else if(command == *sPhoenixCmdLineMedia)
			{
				std::string url;
				std::string type;

				if(i >> url)
				{
					if(i >> type)
					{
						LLParcel *parcel = LLViewerParcelMgr::getInstance()->getAgentParcel();
						parcel->setMediaURL(url);
						parcel->setMediaType(type);
						LLViewerParcelMedia::play(parcel);
						LLViewerParcelMediaAutoPlay::playStarted();
						return false;
					}
				}
			}
			else if(command == *sPhoenixCmdLineMusic)
			{
				std::string status;
				if(i >> status)
				{
					if(!gOverlayBar->musicPlaying())
					{
						gOverlayBar->toggleMusicPlay(gOverlayBar);
					}
					gAudiop->startInternetStream(status);
					return false;
				}
			}
			else if(command == *sPhoenixCmdLineAO)
            {
				std::string status;
				int flags = gSavedPerAccountSettings.getS32("AO.Flags");
                if(i >> status)
                {
					if (status == "on" )
					{
						gSavedPerAccountSettings.setBOOL("AO.Enabled",TRUE);
					}
					else if (status == "off" )
					{
						gSavedPerAccountSettings.setBOOL("AO.Enabled",FALSE);
					}
					else if (status == "sit" )
					{
						// SIT: 1
						bool status=(flags&1)==1;
						std::string toggle;
						if(i >> toggle)
						{
							toggle=utf8str_tolower(toggle);
							status=(toggle=="on");
						} else {
							status=!status;
						}

						gSavedPerAccountSettings.setS32("AO.Flags",status);
					}
				}
				return false;
            }
			else if(command == *sPhoenixCmdLineKeyToName)
            {
                LLUUID targetKey;
                if(i >> targetKey)
                {
                    std::string object_name;
                    gCacheName->getFullName(targetKey, object_name);
                    char buffer[DB_IM_MSG_BUF_SIZE * 2]; 
                    snprintf(buffer,sizeof(buffer),"%s: (%s)",targetKey.asString().c_str(), object_name.c_str());
					cmdline_printchat(std::string(buffer));
                }
				return false;
            }
			else if(command == "/touch")
            {
                LLUUID targetKey;
                if(i >> targetKey)
                {
					LLViewerObject* myObject = gObjectList.findObject(targetKey);
					char buffer[DB_IM_MSG_BUF_SIZE * 2];

					if (!myObject)
					{
						snprintf(buffer,sizeof(buffer),"Object with key %s not found!",targetKey.asString().c_str());
						cmdline_printchat(std::string(buffer));
						return false;
					}

					LLMessageSystem	*msg = gMessageSystem;
					msg->newMessageFast(_PREHASH_ObjectGrab);
					msg->nextBlockFast( _PREHASH_AgentData);
					msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
					msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
					msg->nextBlockFast( _PREHASH_ObjectData);
					msg->addU32Fast(    _PREHASH_LocalID, myObject->mLocalID);
					msg->addVector3Fast(_PREHASH_GrabOffset, LLVector3::zero );
					msg->nextBlock("SurfaceInfo");
					msg->addVector3("UVCoord", LLVector3::zero);
					msg->addVector3("STCoord", LLVector3::zero);
					msg->addS32Fast(_PREHASH_FaceIndex, 0);
					msg->addVector3("Position", myObject->getPosition());
					msg->addVector3("Normal", LLVector3::zero);
					msg->addVector3("Binormal", LLVector3::zero);
					msg->sendMessage( myObject->getRegion()->getHost());

					// *NOTE: Hope the packets arrive safely and in order or else
					// there will be some problems.
					// *TODO: Just fix this bad assumption.
					msg->newMessageFast(_PREHASH_ObjectDeGrab);
					msg->nextBlockFast(_PREHASH_AgentData);
					msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
					msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
					msg->nextBlockFast(_PREHASH_ObjectData);
					msg->addU32Fast(_PREHASH_LocalID, myObject->mLocalID);
					msg->nextBlock("SurfaceInfo");
					msg->addVector3("UVCoord", LLVector3::zero);
					msg->addVector3("STCoord", LLVector3::zero);
					msg->addS32Fast(_PREHASH_FaceIndex, 0);
					msg->addVector3("Position", myObject->getPosition());
					msg->addVector3("Normal", LLVector3::zero);
					msg->addVector3("Binormal", LLVector3::zero);
					msg->sendMessage(myObject->getRegion()->getHost());
					snprintf(buffer,sizeof(buffer),"Touched object with key %s",targetKey.asString().c_str());
					cmdline_printchat(std::string(buffer));
                }
				return false;
            }
			else if(command == "/siton")
            {
                LLUUID targetKey;
                if(i >> targetKey)
                {
					LLViewerObject* myObject = gObjectList.findObject(targetKey);
					char buffer[DB_IM_MSG_BUF_SIZE * 2];

					if (!myObject)
					{
						snprintf(buffer,sizeof(buffer),"Object with key %s not found!",targetKey.asString().c_str());
						cmdline_printchat(std::string(buffer));
						return false;
					}
					LLMessageSystem	*msg = gMessageSystem;
					msg->newMessageFast(_PREHASH_AgentRequestSit);
					msg->nextBlockFast(_PREHASH_AgentData);
					msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
					msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
					msg->nextBlockFast(_PREHASH_TargetObject);
					msg->addUUIDFast(_PREHASH_TargetID, targetKey);
					msg->addVector3Fast(_PREHASH_Offset, LLVector3::zero);
					gAgent.getRegion()->sendReliableMessage();

					snprintf(buffer,sizeof(buffer),"Sat on object with key %s",targetKey.asString().c_str());
					cmdline_printchat(std::string(buffer));
                }
				return false;
            }
			else if(command == "/standup")
            {
				gAgent.setControlFlags(AGENT_CONTROL_STAND_UP);
				cmdline_printchat(std::string("Standing up"));
				return false;
            }
			else if(command == *sPhoenixCmdLineOfferTp)
            {
                std::string avatarKey;
//				llinfos << "CMD DEBUG 0 " << command << " " << avatarName << llendl;
                if(i >> avatarKey)
                {
//				llinfos << "CMD DEBUG 0 afterif " << command << " " << avatarName << llendl;
                    LLUUID tempUUID;
                    if(LLUUID::parseUUID(avatarKey, &tempUUID))
                    {
                        char buffer[DB_IM_MSG_BUF_SIZE * 2];
                        LLDynamicArray<LLUUID> ids;
                        ids.push_back(tempUUID);
                        std::string tpMsg="Join me!";
                        LLMessageSystem* msg = gMessageSystem;
                        msg->newMessageFast(_PREHASH_StartLure);
                        msg->nextBlockFast(_PREHASH_AgentData);
                        msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
                        msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
                        msg->nextBlockFast(_PREHASH_Info);
                        msg->addU8Fast(_PREHASH_LureType, (U8)0); 

                        msg->addStringFast(_PREHASH_Message, tpMsg);
                        for(LLDynamicArray<LLUUID>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
                        {
                            msg->nextBlockFast(_PREHASH_TargetData);
                            msg->addUUIDFast(_PREHASH_TargetID, *itr);
                        }
                        gAgent.sendReliableMessage();
                        snprintf(buffer,sizeof(buffer),"Offered TP to key %s",tempUUID.asString().c_str());
						cmdline_printchat(std::string(buffer));
						return false;
                    }
                }
            }
			
			else if(command == *sPhoenixCmdLineGround)
			{
				LLVector3 agentPos = gAgent.getPositionAgent();
				U64 agentRegion = gAgent.getRegion()->getHandle();
				LLVector3 targetPos(agentPos.mV[0],agentPos.mV[1],LLWorld::getInstance()->resolveLandHeightAgent(agentPos));
				LLVector3d pos_global = from_region_handle(agentRegion);
				pos_global += LLVector3d((F64)targetPos.mV[0],(F64)targetPos.mV[1],(F64)targetPos.mV[2]);
				gAgent.teleportViaLocation(pos_global);
				return false;
			}else if(command == *sPhoenixCmdLineHeight)
			{
				F32 z;
				if(i >> z)
				{
					LLVector3 agentPos = gAgent.getPositionAgent();
					U64 agentRegion = gAgent.getRegion()->getHandle();
					LLVector3 targetPos(agentPos.mV[0],agentPos.mV[1],z);
					LLVector3d pos_global = from_region_handle(agentRegion);
					pos_global += LLVector3d((F64)targetPos.mV[0],(F64)targetPos.mV[1],(F64)targetPos.mV[2]);
					gAgent.teleportViaLocation(pos_global);
					return false;
				}
			}else if(command == *sPhoenixCmdLineTeleportHome)
			{
				gAgent.teleportHome();
				return false;
            }else if(command == *sPhoenixCmdLineRezPlatform)
            {
				F32 width;
				if (i >> width) cmdline_rezplat(false, width);
				else cmdline_rezplat();
				return false;
			}else if(command == *sPhoenixCmdLineMapTo)
			{
				if (revised_text.length() > command.length() + 1) //Typing this command with no argument was causing a crash. -Madgeek
				{
					LLVector3d agentPos = gAgent.getPositionGlobal();
					S32 agent_x = llround( (F32)fmod( agentPos.mdV[VX], (F64)REGION_WIDTH_METERS ) );
					S32 agent_y = llround( (F32)fmod( agentPos.mdV[VY], (F64)REGION_WIDTH_METERS ) );
					S32 agent_z = llround( (F32)agentPos.mdV[VZ] );
					std::string region_name = LLWeb::escapeURL(revised_text.substr(command.length()+1));
					std::string url;

					if(!*sPhoenixCmdLineMapToKeepPos)
					{
						agent_x = 128;
						agent_y = 128;
						agent_z = 0;
					}

					url = llformat("secondlife:///app/teleport/%s/%d/%d/%d",region_name.c_str(),agent_x,agent_y,agent_z);
					LLURLDispatcher::dispatch(url, NULL, true);
				}
				return false;
			}else if(command == *sPhoenixCmdLineCalc)//Cryogenic Blitz
			{
				bool success;
				F32 result = 0.f;
				if(revised_text.length() > command.length() + 1)
				{

					std::string expr = revised_text.substr(command.length()+1);
					LLStringUtil::toUpper(expr);
					success = LLCalc::getInstance()->evalString(expr, result);

					std::string out;

					if (!success)
					{
						out =  "Calculation Failed";
					}
					else
					{
						// Replace the expression with the result
						std::ostringstream result_str;
						result_str << expr;
						result_str << " = ";
						result_str << result;
						out = result_str.str();
					}
					cmdline_printchat(out);
					return false;
				}
			}else if(command == *sPhoenixCmdLineTP2)
			{
				if (revised_text.length() > command.length() + 1) //Typing this command with no argument was causing a crash. -Madgeek
				{
					std::string name = revised_text.substr(command.length()+1);
					cmdline_tp2name(name);
				}
				return false;
			}else if (revised_text == "xyzzy")
			{
				//Zwag: I wonder how many people will actually get this?
				cmdline_printchat("Nothing happens.");
				return false;
			}
/*			else if (revised_text == "/pimps")
			{
				//Thanks to the pimp's horrible code for shutting down the site...
				cmdline_printchat("Pimps can't code.");
				return true;//dont block chat
			}
			else if(revised_text == "/ac")
			{
				lggAutoCorrectFloaterStart::show(TRUE,NULL);
				cmdline_printchat("Displaying AutoCorrection Floater.");
				return false;
			}
			else if(command == *sPhoenixCmdLineAutocorrect)
			{
				std::string info = revised_text.substr((*sPhoenixCmdLineAutocorrect).length()+1);
				//addac list name|wrong word|right word
				int bar = info.find("|");
				if (bar==std::string::npos)
				{
					cmdline_printchat("Wrong usage, correct usage is"+
				*sPhoenixCmdLineAutocorrect+" list Name|wrong word|right word.");
					return false;
				}
				

				std::string listName = info.substr(0,bar);
				info = info.substr(bar+1);
				
				bar = info.find("|");
				if (bar==std::string::npos)
				{
					cmdline_printchat("Wrong usage, correct usage is"+
						*sPhoenixCmdLineAutocorrect+" list Name|wrong word|right word.");
					return false;
				}

				std::string wrong = info.substr(0,bar);
				std::string right = info.substr(bar+1);
				if(LGGAutoCorrect::getInstance()->addEntryToList(wrong,right,listName))
				{
					cmdline_printchat("Added "+wrong+"=>"+right+" to the "+listName+" list.");
					LGGAutoCorrect::getInstance()->save();
					return false;
				}

 			}*/
//			else if (revised_text=="/reform")
// 			{
// 				cmdline_printchat("Reforming avatar.");
// 
// 				gAgent.getAvatarObject()->initClass();
// 				gAgent.getAvatarObject()->buildCharacter();
// 				//gAgent.getAvatarObject()->loadAvatar();
// 				return false;
// 			}
			else if(command == "typingstop")
			{
				std::string text;
				if(i >> text)
				{
					gChatBar->sendChatFromViewer(text, CHAT_TYPE_STOP, FALSE);
				}
			}
			else if(command == *sPhoenixCmdLineClearChat)
			{
				LLFloaterChat* chat = LLFloaterChat::getInstance(LLSD());
				if(chat)
				{
					LLViewerTextEditor*	history_editor = chat->getChild<LLViewerTextEditor>("Chat History Editor");
					LLViewerTextEditor*	history_editor_with_mute = chat->getChild<LLViewerTextEditor>("Chat History Editor with mute");
					history_editor->clear();
					history_editor_with_mute->clear();
					return false;
				}
			}
			else if(command == "zdrop")
			{
				cmdline_printchat("Zdrop running");
				std::string lolfolder;
				if(i >> lolfolder)
				{
					cmdline_printchat("Looking for folder");
					std::stack<LLViewerInventoryItem*> lolstack;
					LLDynamicArray<LLPointer<LLViewerInventoryItem> > lolinv = findInventoryInFolder(lolfolder);
					for(LLDynamicArray<LLPointer<LLViewerInventoryItem> >::iterator it = lolinv.begin(); it != lolinv.end(); ++it)
					{
						LLViewerInventoryItem* item = *it;
						lolstack.push(item);
					}

					if(lolstack.size())
					{
						std::string loldest;
						if(i >> loldest)
						{
							cmdline_printchat("Found destination");
							LLUUID sdest = LLUUID(loldest);
							new JCZface(lolstack, sdest, 2.5f);
						}
					}
					else
					{
						cmdline_printchat("Couldn't find folder.");
					}
				}
			}
			else if(command == "ztake")
			{
				std::string setting;
				if(i >> setting)
				{
					if(setting == "on")
					{
						if(ztake != NULL)
						{
							cmdline_printchat("You're already doing that, I think.");
						}
						else
						{
							std::string folder_name;
							if(i >> folder_name)
							{
								LLUUID folder = gInventory.findCategoryByName(folder_name);
								if(folder.notNull())
								{
									ztake = new JCZtake(folder);
								}
								else
								{
									cmdline_printchat(llformat("You can't see any %s here!", folder_name.c_str()));
								}
							}
							else
							{
								cmdline_printchat("What do you want to put the objects in?");
							}
						}
					}
					else if(setting == "off")
					{
						if(ztake == NULL)
						{
							cmdline_printchat("You weren't doing that anyway, were you?");
						}
						else
						{
							ztake->mRunning = TRUE;
							delete ztake;
							ztake = NULL;
						}
					}
					else
					{
						cmdline_printchat(llformat("I don't know the word \"%s\".", setting.c_str()));
					}
					return false;
				}
				else
				{
					cmdline_printchat("I beg your pardon?");
				}
				return false;
			}
			else if(command == "invrepair")
			{
				invrepair();
			}
			//
			// Luna
			//
			else if(command == *sLunaCmdLineLua)
			{
				// Luna:  Raw Lua Command from chatbar.
				// And yes, it currently just shoves the command into the same queue as a macro.
				int sz = 4;
				FLLua::callCommand(revised_text.substr(sz));
				return false;
			}
			else if(command == *sLunaCmdLineMacro || command==*sLunaCmdLineShortMacro)
			{
				// Luna:  Macros
				//  /m MacroName arg u ments

				FLLua::callCommand(revised_text); //revised_text already includes /m 
				return false;
			}
		}
	}
	return true;
}

//case insensative search for avatar in draw distance
//TODO: make this use the avatar list floaters list so we have EVERYONE
// even if they are out of draw distance.
LLUUID cmdline_partial_name2key(std::string partial_name)
{
	std::vector<LLUUID> avatars;
	std::string av_name;
	LLStringUtil::toLower(partial_name);
	LLWorld::getInstance()->getAvatars(&avatars);
	typedef std::vector<LLUUID>::const_iterator av_iter;
	bool has_avatarlist = (LLFloaterAvatarList::getInstance() ? true : false);
	if(has_avatarlist)
		LLFloaterAvatarList::getInstance()->updateAvatarList();
	for(av_iter i = avatars.begin(); i != avatars.end(); ++i)
	{
		if(has_avatarlist)
		{
			LLAvatarListEntry* entry = LLFloaterAvatarList::getInstance()->getAvatarEntry(*i);
			if(entry)
			{
				av_name = entry->getName();
			}
		}
		if (av_name.empty() && !gCacheName->getFullName(*i, av_name))
		{
			LLViewerObject *obj = gObjectList.findObject(*i);
			if(obj)
			{
				LLVOAvatar* avatarp = dynamic_cast<LLVOAvatar*>(obj);
				av_name = avatarp->getFullname();
			}
		}
		LLStringUtil::toLower(av_name);
		if(strstr(av_name.c_str(), partial_name.c_str()))
		{
			return *i;
		}
	}
	return LLUUID::null;
}
					


void cmdline_tp2name(std::string target)
{
	LLUUID avkey = cmdline_partial_name2key(target);
	if(avkey.isNull())
	{
		cmdline_printchat("Avatar not found.");
		return;
	}
	LLFloaterAvatarList* avlist = LLFloaterAvatarList::getInstance();
	LLViewerObject* obj = gObjectList.findObject(avkey);
	if(obj)
	{
		LLVector3d pos = obj->getPositionGlobal();
		pos.mdV[VZ] += 2.0;
		gAgent.teleportViaLocation(pos);
	}
	else if(avlist)
	{
		LLAvatarListEntry* entry = avlist->getAvatarEntry(avkey);
		if(entry)
		{
			LLVector3d pos = entry->getPosition();
			pos.mdV[VZ] += 2.0;
			gAgent.teleportViaLocation(pos);
		}
	}
}

void cmdline_rezplat(bool use_saved_value, F32 visual_radius) //cmdline_rezplat() will still work... just will use the saved value
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
    msg->addU8Fast(_PREHASH_Material,    LL_MCODE_METAL);

    if(agentPos.mV[2] > 4096.0)msg->addU32Fast(_PREHASH_AddFlags, FLAGS_CREATE_SELECTED);
    else msg->addU32Fast(_PREHASH_AddFlags, 0);

    LLVolumeParams    volume_params;

    volume_params.setType( LL_PCODE_PROFILE_CIRCLE, LL_PCODE_PATH_CIRCLE_33 );
    volume_params.setRatio    ( 2, 2 );
    volume_params.setShear    ( 0, 0 );
    volume_params.setTaper(2.0f,2.0f);
    volume_params.setTaperX(0.f);
    volume_params.setTaperY(0.f);

    LLVolumeMessage::packVolumeParams(&volume_params, msg);
    LLVector3 rezpos = agentPos - LLVector3(0.0f,0.0f,2.5f);
    LLQuaternion rotation;
    rotation.setQuat(90.f * DEG_TO_RAD, LLVector3::y_axis);

	static F32 *sPhoenixCmdLinePlatformSize = rebind_llcontrol<F32>("PhoenixCmdLinePlatformSize", &gSavedSettings, true);

	if (use_saved_value) visual_radius = *sPhoenixCmdLinePlatformSize;
	F32 realsize = visual_radius / 3.0f;
	if (realsize < 0.01f) realsize = 0.01f;
	else if (realsize > 10.0f) realsize = 10.0f;

    msg->addVector3Fast(_PREHASH_Scale,            LLVector3(0.01f,realsize,realsize) );
    msg->addQuatFast(_PREHASH_Rotation,            rotation );
    msg->addVector3Fast(_PREHASH_RayStart,        rezpos );
    msg->addVector3Fast(_PREHASH_RayEnd,            rezpos );
    msg->addU8Fast(_PREHASH_BypassRaycast,        (U8)1 );
    msg->addU8Fast(_PREHASH_RayEndIsIntersection, (U8)FALSE );
    msg->addU8Fast(_PREHASH_State, 0);
    msg->addUUIDFast(_PREHASH_RayTargetID,            LLUUID::null );
    msg->sendReliable(gAgent.getRegionHost());
}

void cmdline_printchat(std::string message)
{
    LLChat chat;
    chat.mText = message;
	chat.mSourceType = CHAT_SOURCE_SYSTEM;
    LLFloaterChat::addChat(chat, FALSE, FALSE);
}

