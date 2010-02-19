/* Copyright (c) 2009
 *
 * Greg Hendrickson (LordGregGreg Back) All rights reserved.
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
#include "lggbeammaps.h"
#include "lggbeamscolors.h"
#include "llappviewer.h"
#include "llfile.h"
#include "llagent.h"
#include "llsdserialize.h"
#include "llviewercontrol.h"
#include "llhudeffecttrail.h"
#include "llhudmanager.h"
//using namespace std;

lggBeamMaps gLggBeamMaps;
F32 hueToRgb ( F32 val1In, F32 val2In, F32 valHUeIn )
{
	while ( valHUeIn < 0.0f ) valHUeIn += 1.0f;
	while ( valHUeIn > 1.0f ) valHUeIn -= 1.0f;
	if ( ( 6.0f * valHUeIn ) < 1.0f ) return ( val1In + ( val2In - val1In ) * 6.0f * valHUeIn );
	if ( ( 2.0f * valHUeIn ) < 1.0f ) return ( val2In );
	if ( ( 3.0f * valHUeIn ) < 2.0f ) return ( val1In + ( val2In - val1In ) * ( ( 2.0f / 3.0f ) - valHUeIn ) * 6.0f );
	return ( val1In );
}

void hslToRgb ( F32 hValIn, F32 sValIn, F32 lValIn, F32& rValOut, F32& gValOut, F32& bValOut )
{
	if ( sValIn < 0.00001f )
	{
		rValOut = lValIn;
		gValOut = lValIn;
		bValOut = lValIn;
	}
	else
	{
		F32 interVal1;
		F32 interVal2;

		if ( lValIn < 0.5f )
			interVal2 = lValIn * ( 1.0f + sValIn );
		else
			interVal2 = ( lValIn + sValIn ) - ( sValIn * lValIn );

		interVal1 = 2.0f * lValIn - interVal2;

		rValOut = hueToRgb ( interVal1, interVal2, hValIn + ( 1.f / 3.f ) );
		gValOut = hueToRgb ( interVal1, interVal2, hValIn );
		bValOut = hueToRgb ( interVal1, interVal2, hValIn - ( 1.f / 3.f ) );
	}
}



LLSD lggBeamMaps::getPic(std::string filename)
{
	LLSD data;
	llifstream importer(filename);
	LLSDSerialize::fromXMLDocument(data, importer);

	return data;
	
}
LLColor4U lggBeamMaps::getCurrentColor(LLColor4U agentColor)
{
	std::string settingName = gSavedSettings.getString("EmeraldBeamColorFile");

	if(settingName=="===OFF===") return agentColor;

	if(settingName != lastColorFileName)
	{
		lastColorFileName=settingName;
	
		std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "beamsColors", ""));
		std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beamsColors", ""));
		std::string filename =path_name +settingName+".xml";
		if(gDirUtilp->fileExists(filename))
		{
		}else
		{
			filename =path_name2 +settingName+".xml";
			if(!gDirUtilp->fileExists(filename))
			{
				return agentColor;
			}
		}

		lastColorsData=lggBeamsColors::fromLLSD(getPic(filename));
	}
	agentColor = beamColorFromData(lastColorsData);
	
	return agentColor;
}

static LLFrameTimer timer;
LLColor4U lggBeamMaps::beamColorFromData(lggBeamsColors data)
{

	F32 r, g, b;
	LLColor4 output;
	LLColor4U toReturn;
	F32 timeinc =  timer.getElapsedTimeF32()*0.3f*((data.rotateSpeed+.01f)) * (360/(data.endHue-data.startHue));

	S32 diference = llround(data.endHue  - data.startHue);
	if(diference == 360 || diference == 720)
	{
		//full rainbow
		//liner one
		hslToRgb(fmod(timeinc,1.0f), 1.0f, 0.5f, r, g, b);

	}else
	{
		F32 variance = ((data.endHue/360.0f)-(data.startHue/360.0f))/2.0f;
		hslToRgb((data.startHue/360.0f) + variance + (sinf(timeinc)*(variance)), 1.0f, 0.5f, r, g, b);
	}
	output.set(r, g, b);

	toReturn.setVecScaleClamp(output);
	return toReturn;

}
void lggBeamMaps::fireCurrentBeams(LLPointer<LLHUDEffectSpiral> mBeam, LLColor4U rgb)
{
	if(scale == 0.0f)return;
	
	for(int i = 0; i < (int)dots.size(); i++)
	{
		LLColor4U myColor = rgb;
		if(	gSavedSettings.getString("EmeraldBeamColorFile")=="===OFF===")
			myColor = dots[i].c;

		F32 distanceAdjust = dist_vec(mBeam->getPositionGlobal(),gAgent.getPositionGlobal()) ;
		F32 pulse = (F32)(.75f+sinf(gFrameTimeSeconds*1.0f)*0.25f);
		LLVector3d offset = dots[i].p;
		offset.mdV[VY] *= -1;
		offset *= pulse * scale * distanceAdjust * 0.1;
		
		//llinfos << "dist is " << distanceAdjust << "scale is " << scale << llendl;
		LLVector3 beamLine = LLVector3( mBeam->getPositionGlobal() - gAgent.getPositionGlobal());
		LLVector3 beamLineFlat = beamLine;
		beamLineFlat.mV[VZ]= 0.0f;

		LLVector3 newDirFlat = LLVector3::x_axis;
		beamLine.normalize();
		beamLineFlat.normalize();
		LLQuaternion change;
		change.shortestArc(newDirFlat,beamLineFlat);
		offset.rotVec(change);
		newDirFlat.rotVec(change);
		change.shortestArc(newDirFlat,beamLine);
		offset.rotVec(change);

		LLPointer<LLHUDEffectSpiral> myBeam =  (LLHUDEffectSpiral *)LLHUDManager::getInstance()->createViewerEffect(LLHUDObject::LL_HUD_EFFECT_BEAM);
		myBeam->setPositionGlobal(mBeam->getPositionGlobal() + offset + (LLVector3d(beamLine) * sinf(gFrameTimeSeconds*2.0f) * 0.2f));
		
		myBeam->setColor(myColor);
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
		if( settingName != "===OFF===" && settingName != "")
		{

			std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "beams", ""));
			std::string path_name2(gDirUtilp->getExpandedFilename( LL_PATH_USER_SETTINGS , "beams", ""));
			std::string filename =path_name +settingName+".xml";
			if(gDirUtilp->fileExists(filename))
			{
			}else
			{
				filename =path_name2 +settingName+".xml";
			}
			LLSD mydata = getPic(filename);
			scale = (F32)mydata["scale"].asReal()/10.0f;
			LLSD myPicture = mydata["data"];	
			dots.clear();
			for(int i = 0; i < myPicture.size(); i++)
			{
				LLSD beamData = myPicture[i];
				lggBeamData dot;
				dot.p = beamData["offset"];
				dot.p *= (gSavedSettings.getF32("EmeraldBeamShapeScale")*2.0f);
				LLColor4 color = beamData["color"];
				
				dot.c = LLColor4U(color);
				
				dots.push_back(dot);
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


std::vector<std::string> lggBeamMaps::getFileNames()
{
	
	std::vector<std::string> names;	
	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "beams", ""));
	bool found = true;			
	while(found) 
	{
		std::string name;
		found = gDirUtilp->getNextFileInDir(path_name, "*.xml", name, false);
		if(found)
		{

			name=name.erase(name.length()-4);

			// bugfix for SL-46920: preventing filenames that break stuff.
			char * curl_str = curl_unescape(name.c_str(), name.size());
			std::string unescaped_name(curl_str);
			curl_free(curl_str);
			curl_str = NULL;

			names.push_back(name);
			
			//LL_DEBUGS2("AppInit", "Shaders") << "name: " << name << LL_ENDL;
			//loadPreset(unescaped_name,FALSE);
		}
	}
	std::string path_name2(gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "beams", ""));
	found = true;			
	while(found) 
	{
		std::string name;
		found = gDirUtilp->getNextFileInDir(path_name2, "*.xml", name, false);
		if(found)
		{

			name=name.erase(name.length()-4);

			// bugfix for SL-46920: preventing filenames that break stuff.
			char * curl_str = curl_unescape(name.c_str(), name.size());
			std::string unescaped_name(curl_str);
			curl_free(curl_str);
			curl_str = NULL;

			names.push_back(name);
			
			//LL_DEBUGS2("AppInit", "Shaders") << "name: " << name << LL_ENDL;
			//loadPreset(unescaped_name,FALSE);
		}
	}
	return names;

	

}

std::vector<std::string> lggBeamMaps::getColorsFileNames()
{

	std::vector<std::string> names;	
	std::string path_name(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "beamsColors", ""));
	bool found = true;			
	while(found) 
	{
		std::string name;
		found = gDirUtilp->getNextFileInDir(path_name, "*.xml", name, false);
		if(found)
		{

			name=name.erase(name.length()-4);

			// bugfix for SL-46920: preventing filenames that break stuff.
			char * curl_str = curl_unescape(name.c_str(), name.size());
			std::string unescaped_name(curl_str);
			curl_free(curl_str);
			curl_str = NULL;

			names.push_back(name);

			//LL_DEBUGS2("AppInit", "Shaders") << "name: " << name << LL_ENDL;
			//loadPreset(unescaped_name,FALSE);
		}
	}
	std::string path_name2(gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "beamsColors", ""));
	found = true;			
	while(found) 
	{
		std::string name;
		found = gDirUtilp->getNextFileInDir(path_name2, "*.xml", name, false);
		if(found)
		{

			name=name.erase(name.length()-4);

			// bugfix for SL-46920: preventing filenames that break stuff.
			char * curl_str = curl_unescape(name.c_str(), name.size());
			std::string unescaped_name(curl_str);
			curl_free(curl_str);
			curl_str = NULL;

			names.push_back(name);

			//LL_DEBUGS2("AppInit", "Shaders") << "name: " << name << LL_ENDL;
			//loadPreset(unescaped_name,FALSE);
		}
	}
	return names;



}