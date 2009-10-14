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
 *   3. Neither the name Modular Systems Ltd nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS �AS IS�
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

//theGenius Indigo
//Currently, all this does is stream the avatar's eyeball rotation and blinking; useful for primed eyes on avatars

bool GUS::Enabled = false;
F32 GUS::Refresh = 0.001f;
bool GUS::FEEnabled = false;
F32 GUS::FERefresh = 0.001f;

//Constructor(s)
GUS::GUS()
{
	sMessage = genMessage();
	sFEMessage = genFEMessage();
	changed = true;
	FEchanged = true;
	initGUS();
}
//static
void GUS::initGUS()
{
	Enabled = gSavedSettings.getBOOL("EmeraldGUSEnabled");
	Refresh = gSavedSettings.getF32("EmeraldGUSRefresh");
	FEEnabled = gSavedSettings.getBOOL("EmeraldGUSFastEventsEnabled");
	FERefresh = gSavedSettings.getF32("EmeraldGUSFastEventsRefresh");
	gSavedSettings.getControl("EmeraldGUSEnabled")->getSignal()->connect(&gusEnabled);
	gSavedSettings.getControl("EmeraldGUSRefresh")->getSignal()->connect(&gusRefresh);
	gSavedSettings.getControl("EmeraldGUSFastEventsEnabled")->getSignal()->connect(&gusFEEnabled);
	gSavedSettings.getControl("EmeraldGUSFastEventsRefresh")->getSignal()->connect(&gusFERefresh);
}
//static
void GUS::gusEnabled(const LLSD &data)
{
	Enabled = (bool)data.asBoolean();
}
//static
void GUS::gusRefresh(const LLSD &data)
{
	Refresh = (F32)data.asReal();
}
//static
void GUS::gusFEEnabled(const LLSD &data)
{
	FEEnabled = (bool)data.asBoolean();
}
//static
void GUS::gusFERefresh(const LLSD &data)
{
	FERefresh = (F32)data.asReal();
}
bool GUS::streamData()
{
	std::string nMessage = genMessage();
	changed = changed || (sMessage != nMessage);
	if(!changed)return false;
	whisper(gSavedSettings.getS32("EmeraldGUSChannel"), nMessage);
	changed = false;
	sMessage = nMessage;
	return true;
}

bool GUS::fastEvent()
{
	std::string nMessage = genFEMessage();
	FEchanged = FEchanged || (sFEMessage != nMessage);
	if(!FEchanged)return false;
	F32 GUS_FE_freq = llclamp(gSavedSettings.getF32("EmeraldGUSFastEventsRefresh"), 0.0001f, 20.f);
	if(FELimiter < GUS_FE_freq)
	{
		whisper(gSavedSettings.getS32("EmeraldGUSChannel")+1, nMessage);
		sFEMessage = nMessage;
		FEchanged = false;
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
	
	{//Entering vector stacking scope of goodness
		//Stream the avatar's eyeball rotation
		if(gSavedSettings.getBOOL("EmeraldGUSEyeRot"))
		{
			cmds.push_back("EYEROT");
			cmds.push_back(sQuat(getEyeRot()));
		}
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
	
	{//Entering vector stacking scope of goodness
		//Stream the avatar's eyelid state
		if(gSavedSettings.getBOOL("EmeraldGUSEyelidState"))
		{
			cmds.push_back("EYELIDS");
			cmds.push_back((getEyelidState()?"SHUT":"OPEN"));
		}
	}
	
	for(p = cmds.begin(); p != cmds.end(); p++)
	{
		message << "::" << *p;
	}
	return message.str();
}

void GUS::whisper(S32 channel, std::string message)
{
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ChatFromViewer);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ChatData);
	msg->addStringFast(_PREHASH_Message, message.c_str());
	msg->addU8Fast(_PREHASH_Type, CHAT_TYPE_WHISPER);
	msg->addS32("Channel", channel);
	gAgent.sendReliableMessage();
	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CHAT_COUNT);
}

LLQuaternion GUS::getEyeRot()
{
	LLVOAvatar* avatarp;
	avatarp = gAgent.getAvatarObject();
	LLQuaternion eyerot;
	if(avatarp)
	{
		LLJoint* jointp = avatarp->getJoint("mEyeLeft");
		if(jointp)
		{
			eyerot = jointp->getRotation();
		}
	}
	return eyerot;
}

std::string GUS::sQuat(LLQuaternion quat)
{
	std::stringstream strstr;
	strstr << std::setiosflags(std::ios::fixed) << std::setprecision(6); // delicious iomanip
	strstr << "<" << quat.mQ[0] << ", " << quat.mQ[1] << ", "  << quat.mQ[2] << ", "  << quat.mQ[3] << ">";
	std::string squat = strstr.str(); // lol strstrstr
	return squat; // lol squat
}

bool GUS::getEyelidState() //FALSE == open, TRUE == shut
{
	LLVOAvatar* avatarp;
	avatarp = gAgent.getAvatarObject();
	F32 weight = 0.f;
	if(avatarp)
	{
		weight = avatarp->getVisualParam("Blink_Left")->getWeight();
	}
	return (weight!=0.f);
}
