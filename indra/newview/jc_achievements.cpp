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

#include "jc_achievements.h"

#include "llcontrol.h"

#include "llviewercontrol.h"

#include "llagent.h"

#include "llbufferstream.h"

#include "llsdserialize.h"

LLSD JCAchievementTracker::AchievementData;

std::string JCAchievementTracker::AchievementPath;

JCAchievementTracker* JCAchievementTracker::sInstance;

std::string JCAchievementTracker::avatar_secret;

JCAchievementTracker::JCAchievementTracker() : LLEventTimer( (F32)1.0 )
{
	if(sInstance)
	{
		delete this;
	}else
	{
		sInstance = this;
	}
}
JCAchievementTracker::~JCAchievementTracker()
{
	sInstance = NULL;
}

BOOL JCAchievementTracker::tick()
{
	//lo;
	return FALSE;
}

void JCAchievementTracker::loadAchievementData()
{
	std::string path = gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "achievements.xml", "");
	AchievementPath = path;
	if(!gDirUtilp->fileExists(path))
	{
		path = gDirUtilp->getExpandedFilename( LL_PATH_APP_SETTINGS , "achievements.dat", "");
	}
	LLSD data;
	llifstream importer(path);
	LLSDSerialize::fromXMLDocument(AchievementData, importer);
}

void JCAchievementTracker::saveAchievementData()
{
	if(AchievementPath == "")return;

	llofstream export_file(AchievementPath);
	LLSDSerialize::toPrettyXML(AchievementData, export_file);
	export_file.close();
}

void JCAchievementTracker::AchievementSecretLoaded()
{
	std::string name;
	gAgent.getName(name);
	AchievementData[name+"_secret"] = avatar_secret;


	std::ostringstream lol;

	std::string test(lol.str());
	//std::string
	//stuff
}

void JCAchievementTracker::completedRaw(U32 status,
						const std::string& reason,
						const LLChannelDescriptors& channels,
							const LLIOPipe::buffer_ptr_t& buffer)
{

}

