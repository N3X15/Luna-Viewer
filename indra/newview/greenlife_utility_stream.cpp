/* Copyright (c) 2009
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
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
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

#include "greenlife_utility_stream.h"

#include <iostream>
#include <iomanip> // for setprecision
#include <vector>

#include "llagent.h"
#include "lljoint.h"
#include "llvoavatar.h"
#include "llviewerobject.h"
#include "llchat.h"
#include "llviewercontrol.h"
#include "llviewerstats.h"
#include "llfloaterchat.h"

#define GUS_PINGTIME 10.f;

const std::string GUS::ping_command = "GUS_ping"; //client-lsl-bridge command
const std::string GUS::sync_command = "GUS_sync"; //client-lsl-bridge command
const std::string GUS::change_channel = "GUS_chan"; //client-lsl-bridge command

LLFrameTimer GUS::ping_timer;
bool GUS::Enabled = false;
F32 GUS::Refresh = 0.001f;
bool GUS::FEEnabled = false;
F32 GUS::FERefresh = 0.001f;
S32 GUS::ChatChannel = -1;
unsigned char GUS::FELimiter = 0;
LLQuaternion GUS::lastEyeRot;
bool GUS::lastEyelidState = false;
LLVector3 GUS::lastLookAtVector;
const F32 NaN = std::numeric_limits<float>::quiet_NaN();
const LLVector3 GUS::NaNVector(NaN, NaN, NaN); //hack!
const LLQuaternion GUS::NaNRotation(NaN, NaN, NaN, NaN); //hack!

GUS::GUS()
{
	initGUS();
}
void GUS::initGUS()
{
	Enabled = gSavedSettings.getBOOL("EmeraldGUSEnabled");
	Refresh = gSavedSettings.getF32("EmeraldGUSRefresh");
	Refresh = llclamp(Refresh, 0.0001f, 1.f);
	FEEnabled = gSavedSettings.getBOOL("EmeraldGUSFastEventsEnabled");
	FERefresh = gSavedSettings.getF32("EmeraldGUSFastEventsRefresh");
	FERefresh = llclamp(Refresh, 0.0001f, 2.f);
	gSavedSettings.getControl("EmeraldGUSEnabled")->getSignal()->connect(&gusEnabled);
	gSavedSettings.getControl("EmeraldGUSRefresh")->getSignal()->connect(&gusRefresh);
	gSavedSettings.getControl("EmeraldGUSFastEventsEnabled")->getSignal()->connect(&gusFEEnabled);
	gSavedSettings.getControl("EmeraldGUSFastEventsRefresh")->getSignal()->connect(&gusFERefresh);
	ping_timer.reset();
}
void GUS::gusEnabled(const LLSD &data)
{
	Enabled = (bool)data.asBoolean();
}
void GUS::gusRefresh(const LLSD &data)
{
	Refresh = (F32)data.asReal();
}
void GUS::gusFEEnabled(const LLSD &data)
{
	FEEnabled = (bool)data.asBoolean();
}
void GUS::gusFERefresh(const LLSD &data)
{
	FERefresh = (F32)data.asReal();
}
bool GUS::streamData()
{
	if(!updateValues())return false;
	std::string nMessage = genMessage();
	whisper(ChatChannel, nMessage);
	return true;
}
bool GUS::fastEvent()
{
	if(!FEupdateValues())return false;
	F32 GUS_FE_freq = llclamp(FERefresh, 0.0001f, 2.f);
	if(FELimiter < GUS_FE_freq)
	{
		std::string nMessage = genFEMessage();
		whisper(ChatChannel+1, nMessage);
		FELimiter++;
		return true;
	}
	return false;
}
std::string GUS::genMessage()
{
	std::stringstream message;
	message << "GUS" << std::setiosflags(std::ios::fixed) << std::setprecision(3);
	std::vector<std::string> cmds;
	std::vector<std::string>::const_iterator p;
	if(gSavedSettings.getBOOL("EmeraldGUSEyeRot"))
	{
		cmds.push_back("EYEROT");
		cmds.push_back(sQuat(getEyeRot()));
	}
	//send look at pos maybeh
	//ok maybeh O:
	if(gSavedSettings.getBOOL("EmeraldGUSLookAt"))
	{
			cmds.push_back("LOOKAT");
			cmds.push_back(sVec3(getLookAtVector()));
	}
	for(p = cmds.begin(); p != cmds.end(); p++)
	{
		message << "::" << *p;
	}
	return message.str();
}
std::string GUS::genFEMessage()
{
	std::stringstream message;
	message << "GUS_FE" << std::setiosflags(std::ios::fixed) << std::setprecision(3);
	std::vector<std::string> cmds;
	std::vector<std::string>::const_iterator p;
	if(gSavedSettings.getBOOL("EmeraldGUSEyelidState"))
	{
		cmds.push_back("EYELIDS");
		cmds.push_back((getEyelidState()?"SHUT":"OPEN"));
	}
	for(p = cmds.begin(); p != cmds.end(); p++)
	{
		message << "::" << *p;
	}
	return message.str();
}

void GUS::whisper(S32 channel, std::string message, bool force)
{
	if(channel > 0 || force)
		chatmessage(channel, message, CHAT_TYPE_WHISPER);
}
void GUS::say(S32 channel, std::string message, bool force)
{
	if(channel > 0 || force)
		chatmessage(channel, message, CHAT_TYPE_NORMAL);
}
void GUS::chatmessage(S32 channel, std::string message, U8 type)
{
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ChatFromViewer);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ChatData);
	msg->addStringFast(_PREHASH_Message, message.c_str());
	msg->addU8Fast(_PREHASH_Type, type);
	msg->addS32("Channel", channel);
	gAgent.sendReliableMessage();
	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CHAT_COUNT);
}

LLQuaternion GUS::getEyeRot()
{
	LLVOAvatar* avatarp;
	avatarp = gAgent.getAvatarObject();
	LLQuaternion eyerot = NaNRotation;
	if(avatarp)
	{
		LLJoint* jointlp = avatarp->getJoint("mEyeLeft");
		LLJoint* jointrp = avatarp->getJoint("mEyeRight");
		if(jointlp && jointrp)
		{
			return nlerp(0.5, jointlp->getRotation(), jointrp->getRotation()); //Uchi wants Lerp 'n' Slerp o.o!
		}
	}
	return NaNRotation; //hack
}
std::string GUS::sQuat(const LLQuaternion &quat, bool nansmakesnull)
{
	if(nansmakesnull && quat == NaNRotation)return "NULL";
	std::stringstream strstr;
	strstr << std::setiosflags(std::ios::fixed) << std::setprecision(6); // delicious iomanip
	strstr << "<" << quat.mQ[VX] << ", " << quat.mQ[VY] << ", "  << quat.mQ[VZ] << ", "  << quat.mQ[VS] << ">";
	return strstr.str(); // lol strstrstr
}
std::string GUS::sVec3(const LLVector3 &vec, bool nansmakesnull)
{
	if(nansmakesnull && vec == NaNVector)return "NULL";
	std::stringstream strstr;
	strstr << std::setiosflags(std::ios::fixed) << std::setprecision(6); // delicious iomanip
	strstr << "<" << vec.mV[VX] << ", " << vec.mV[VY] << ", "  << vec.mV[VZ] << ">";
	return strstr.str(); // lol strstrstr
}
bool GUS::getEyelidState() //FALSE == open, TRUE == shut
{
	LLVOAvatar* avatarp;
	avatarp = gAgent.getAvatarObject();
	if(avatarp)
	{
		return avatarp->getVisualParam("Blink_Left")->getWeight() != 0.f;
	}
	return false;
}
LLVector3 GUS::getLookAtVector()
{
	LLVOAvatar* avatarp= gAgent.getAvatarObject();
	if(avatarp)
	{
		if( avatarp->getAnimationData("LookAtPoint"))
		{
			return *((LLVector3*) avatarp->getAnimationData("LookAtPoint"));
		}
	}
	return NaNVector; //hack!
}
//Why isn't this already in LLQuaternion?! (probably because I'm the first one to ever need to use this in the viewer...
//Just a heads-up; this is probably hidden somewhere in the bowels of the viewer already, but I can't seem to find it,
//so I've just reimiplimented it here:
//(compares to llAngleBetween());
S32 GUS::rotrad(LLQuaternion &a, LLQuaternion &b)
{
	return 2 * acos((a.mQ[VX] * b.mQ[VX] + a.mQ[VY] * b.mQ[VY] + a.mQ[VZ] * b.mQ[VZ] + a.mQ[VS] * b.mQ[VS]) / sqrt((a.mQ[VX] * a.mQ[VX] + a.mQ[VY] * a.mQ[VY] + a.mQ[VZ] * a.mQ[VZ] + a.mQ[VS] * a.mQ[VS]) * (b.mQ[VX] * b.mQ[VX] + b.mQ[VY] * b.mQ[VY] + b.mQ[VZ] * b.mQ[VZ] + b.mQ[VS] * b.mQ[VS]))); //crazy shxt, man
}
bool GUS::updateValues()
{
	bool changes = false;
	//get current values for stuff
	LLQuaternion eyerot = getEyeRot();
	LLVector3 lookat = getLookAtVector();
	//check to see if anything has changed...
	changes |= rotrad(eyerot, lastEyeRot) > 0.05f;
	changes |= LLVector3(lookat - lastLookAtVector).magVec() > 0.1;
	//dirty the saved values
	lastEyeRot = eyerot;
	lastLookAtVector = lookat;
	//return whether or not anything has changed
	return changes;
}
bool GUS::FEupdateValues()
{
	bool changes = false;
	//get current values for stuff
	bool eyestate = getEyelidState();
	//check to see if anything has changed...
	changes |= eyestate != lastEyelidState;
	//dirty the saved values
	lastEyelidState = eyestate;
	//return whether or not anything has changed
	return changes;
}
void GUS::ping()
{
	if(!Enabled)return;
	ping_timer.reset();
}
void GUS::sync(S32 channel)
{
	if(!Enabled)return;
	whisper(channel, llformat("GUS_chan::%i",ChatChannel));
	ping();
}
void GUS::chan(S32 channel)
{
	if(!Enabled)return;
	whisper(ChatChannel, llformat("GUS_chan::%i",channel));
	ChatChannel = channel;
	ping();
}
bool GUS::pinged()
{
	return ping_timer.getElapsedTimeF32() < GUS_PINGTIME;
}
