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
#include "lggBeamMaps.h"
#include "llfile.h"
#include "llagent.h"
#include "llsdserialize.h"
#include "llviewercontrol.h"
#include "llhudeffecttrail.h"
#include "llhudmanager.h"
using namespace std;

lggBeamMaps gLggBeamMaps;

LLSD lggBeamMaps::getPic(std::string filename)
{
	LLSD data;
	llifstream importer(filename);
	LLSDSerialize::fromXMLDocument(data, importer);

	return data;
	
}
void lggBeamMaps::fireCurrentBeams(LLPointer<LLHUDEffectSpiral> mBeam, LLColor4U rgb)
{
	if(scale == 0.0f)return;
	
	for(int i = 0; i < (int)dots.size(); i++)
	{
					
		F32 distanceAdjust = dist_vec(mBeam->getPositionGlobal(),gAgent.getPositionGlobal()) ;
		F32 pulse = (F32)(.75f+sinf(gFrameTimeSeconds*1.0f)*0.25f);
		LLVector3d offset = dots[i];
		
		offset *= pulse * scale * distanceAdjust * 0.1;
		
		//llinfos << "dist is " << distanceAdjust << "scale is " << scale << llendl;
		LLVector3 beamLine = LLVector3( mBeam->getPositionGlobal() - gAgent.getPositionGlobal());
		beamLine.normalize();
		LLQuaternion change;
		change.shortestArc(LLVector3::x_axis,beamLine);
		offset.rotVec(change);
		LLPointer<LLHUDEffectSpiral> myBeam =  (LLHUDEffectSpiral *)LLHUDManager::getInstance()->createViewerEffect(LLHUDObject::LL_HUD_EFFECT_BEAM);
		myBeam->setPositionGlobal(mBeam->getPositionGlobal() + offset + (LLVector3d(beamLine) * sinf(gFrameTimeSeconds*2.0f) * 0.2f));
		myBeam->setColor(rgb);
		myBeam->setTargetObject(mBeam->getTargetObject());
		myBeam->setSourceObject(mBeam->getSourceObject());
		myBeam->setNeedsSendToSim(mBeam->getNeedsSendToSim());
		myBeam->setDuration(duration* 1.2f);
	}
}
void lggBeamMaps::forceUpdate()
{
	dots.clear();
	scale = 0.0f;
	lastFileName="";
}
F32 lggBeamMaps::setUpAndGetDuration()
{
	std::string settingName = gSavedSettings.getString("EmeraldBeamShape");
	if(settingName != lastFileName)
	{
		lastFileName=settingName;
		if(settingName.find (".xml") > 1 && settingName != "")
		{
			std::string filename =gDirUtilp->getAppRODataDir() 
						+gDirUtilp->getDirDelimiter()
						+"beams"
						+gDirUtilp->getDirDelimiter()
						+settingName;
			LLSD mydata = getPic(filename);
			scale = (F32)mydata["scale"].asReal()/10.0f;
			LLSD myPicture = mydata["data"];	
			dots.clear();
			for(int i = 0; i < myPicture.size(); i++)
			{
				LLVector3d dot = myPicture[i];
				
				dots.push_back(dot*(gSavedSettings.getF32("EmeraldBeamShapeScale")*2.0f));
			}
			
			F32 maxBPerQS = gSavedSettings.getF32("EmeraldMaxBeamsPerSecond") / 4.0f;
			duration = llceil((F32)(myPicture.size()) / maxBPerQS) * 0.25f;
			llinfos << "reading it all now size is " << myPicture.size() << " and duration is " << duration << llendl;
		
		}else
		{
			dots.clear();
			scale = 0.0f;//used as a flag too
			duration = 0.25f;
		}
		
	}
	//llinfos << "sent final dur of " << duration << llendl;
		
	return duration;
	
}

