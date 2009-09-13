/** 
 * @file lldrawpoolwlsky.cpp
 * @brief LLDrawPoolWLSky class implementation
 *
 * $LicenseInfo:firstyear=2007&license=viewergpl$
 * 
 * Copyright (c) 2007-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "lldrawpoolwlsky.h"

#include "llerror.h"
#include "llgl.h"
#include "lldir.h"
#include "pipeline.h"
#include "llviewercamera.h"
#include "llimage.h"
#include "llwlparammanager.h"
#include "llsky.h"
#include "llvowlsky.h"
#include "llagent.h"
#include "llviewerregion.h"
#include "llface.h"
#include "llrender.h"


#include "SilverLining.h"
// All SilverLining objects are in the SilverLining namespace.
using namespace SilverLining;

// Statics and defines for a simple, self-contained demo application
//  Main interface to silverlining.
//static Atmosphere *atm = 0;

#define kVisibility 20000.0f

LLDrawPoolWLSky::LLDrawPoolWLSky(void) :
	LLDrawPool(POOL_WL_SKY)
{
	llinfos << "Initializing SilverLining..." << llendl;
	LLWLParamManager::instance()->propagateParameters();

	// PUT YOUR OWN DAMN LICENSE KEY HERE
	atm = new Atmosphere("Your Company Name", "Your License Code");
	atm->ShowFramerate(true);
	int err=atm->Initialize(Atmosphere::OPENGL, gDirUtilp->getExpandedFilename(FL_PATH_SKY,"").c_str(), true, 0);

	Location loc;
	loc.SetLatitude(45);
	loc.SetLongitude(-122);

	LocalTime tm;
	tm.SetYear(1971);
	tm.SetMonth(8);
	tm.SetDay(7);
	tm.SetHour(30);
	tm.SetMinutes(30);
	tm.SetSeconds(0);
	tm.SetObservingDaylightSavingsTime(true);
	tm.SetTimeZone(PST);

	atm->GetConditions()->SetTime(tm);
	atm->GetConditions()->SetLocation(loc);

	assert(atm);

	// Set up wind blowing south at 50 meters/sec
	WindVolume wv;
	wv.SetDirection(180);
	wv.SetMinAltitude(0);
	wv.SetMaxAltitude(10000);
	wv.SetWindSpeed(50);
	atm->GetConditions()->SetWind(wv);

	// Set up the desired cloud types.
	SetupCirrusClouds();
	SetupCumulusCongestusClouds();
	//SetupStratusClouds();
	//SetupCumulonimbusClouds();
	//SetupCumulusMediocrisClouds();

	// Set visibility
	atm->GetConditions()->SetVisibility(20000.0f);
}

LLDrawPoolWLSky::~LLDrawPoolWLSky()
{
	//llinfos << "destructing wlsky draw pool." << llendl;
}

LLViewerImage *LLDrawPoolWLSky::getDebugTexture()
{
	return NULL;
}

void LLDrawPoolWLSky::beginRenderPass( S32 pass )
{

}

void LLDrawPoolWLSky::endRenderPass( S32 pass )
{

}

void LLDrawPoolWLSky::renderLighting()
{
    llinfos << "renderLighting" << llendl;
    float x, y, z, r, g, b, ra, ga, ba;
    atm->GetSunOrMoonPosition(&x, &y, &z);
    atm->GetSunOrMoonColor(&r, &g, &b);
    atm->GetAmbientColor(&ra, &ga, &ba);

    GLfloat light_ambient[] = {ra, ga, ba, 1.0};
    GLfloat light_diffuse[] = {r, g, b, 1.0};
    GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light_position[] = {x, y, z, 0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);

    GLfloat mat_amb_diff[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat no_mat[] = {0, 0, 0, 0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_amb_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
}

void LLDrawPoolWLSky::DoAtmo()
{
	llinfos << "DoAtmo()" << llendl;
	assert(atm);

	// Set up wind blowing south at 50 meters/sec
	WindVolume wv;
	wv.SetDirection(180);
	wv.SetMinAltitude(0);
	wv.SetMaxAltitude(10000);
	wv.SetWindSpeed(50);
	atm->GetConditions()->SetWind(wv);

	// Set up the desired cloud types.
	SetupCirrusClouds();
	SetupCumulusCongestusClouds();
	//SetupStratusClouds();
	//SetupCumulonimbusClouds();
	//SetupCumulusMediocrisClouds();

	// Set visibility
	atm->GetConditions()->SetVisibility(kVisibility);
}

void LLDrawPoolWLSky::render(S32 pass)
{
	if (!gPipeline.hasRenderType(LLPipeline::RENDER_TYPE_SKY))
	{
		return;
	}
	llinfos << "Rendering..." << llendl;
	
	LLFastTimer ftm(LLFastTimer::FTM_RENDER_WL_SKY);
	llinfos << "BeginFrame(True)" << llendl;
	atm->BeginFrame(true);

        llinfos << "Camheight" << llendl;
	const F32 camHeightLocal = LLWLParamManager::instance()->getDomeOffset() * LLWLParamManager::instance()->getDomeRadius();

        llinfos << "Clipping" << llendl;
	LLGLDepthTest depth(GL_TRUE, GL_FALSE);
	LLGLDisable clip(GL_CLIP_PLANE0);

	LLGLClampToFarClip far_clip(glh_get_current_projection());
        llinfos << "Cam Origin" << llendl;
	LLVector3 const & origin = LLViewerCamera::getInstance()->getOrigin();
        llinfos << "glPushMatrix" << llendl;
	glPushMatrix();

        llinfos << "glTranslatef" << llendl;
		glTranslatef(origin.mV[0], origin.mV[1], origin.mV[2]);

	        llinfos << "RenderFog" << llendl;
		//renderHeavenlyBodies();
		renderFog();
		//renderStars();
		
        llinfos << "glPopMatrix" << llendl;
	glPopMatrix();

        llinfos << "EndFrame()" << llendl;
	atm->EndFrame();

}
//static 
bool LLDrawPoolWLSky::comp(ObjectHandle c1, ObjectHandle c2)
{
	LLVector3 pos=LLViewerCamera::getInstance()->getOrigin();
        double d1 = atm->GetObjectDistance(c1, c2, pos.mV[VX], pos.mV[VY], pos.mV[VZ]);
        double d2 = atm->GetObjectDistance(c2, c1, pos.mV[VX], pos.mV[VY], pos.mV[VZ]);

        return (d1 > d2);
}

void LLDrawPoolWLSky::renderClouds()
{
        std::vector<ObjectHandle>& objs = atm->GetObjects();

        sort(objs.begin(), objs.end(), comp);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(0);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);

        std::vector<ObjectHandle>::iterator it;
        for (it = objs.begin(); it != objs.end(); it++)
        {
                atm->DrawObject(*it);
        }

        glDepthMask(1);
}

void LLDrawPoolWLSky::renderFog()
{
	llinfos << "Rendering fog" << llendl;
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP);
	glHint(GL_FOG_HINT, GL_NICEST);

	// Need to light the fog, or it will glow in the dark...
	float ar, ag, ab;
	atm->GetSunOrMoonColor(&ar, &ag, &ab);

	if (atm->GetFogEnabled())
	{
		float density, r, g, b;
		atm->GetFogSettings(&density, &r, &g, &b);
		glFogf(GL_FOG_DENSITY, density);

		GLfloat fogColor[4] = {r * ar, g * ag, b * ab, 1.0};
		glFogfv(GL_FOG_COLOR, fogColor);
	}
	else
	{
		GLfloat fogColor[4];
		atm->GetHorizonColor(LLViewerCamera::getInstance()->getYaw(), &fogColor[0], &fogColor[1], &fogColor[2]);
		glFogfv(GL_FOG_COLOR, fogColor);

		float density = 1.0 / kVisibility;
		glFogf(GL_FOG_DENSITY, density);
	}
}

void LLDrawPoolWLSky::prerender()
{
	//llinfos << "wlsky prerendering pass." << llendl;
}

LLDrawPoolWLSky *LLDrawPoolWLSky::instancePool()
{
	return new LLDrawPoolWLSky();
}

LLViewerImage* LLDrawPoolWLSky::getTexture()
{
	return NULL;
}

void LLDrawPoolWLSky::resetDrawOrders()
{
}

//static
void LLDrawPoolWLSky::cleanupGL()
{
}

//static
void LLDrawPoolWLSky::restoreGL()
{

}


// Configure high cirrus clouds.
void LLDrawPoolWLSky::SetupCirrusClouds()
{
    llinfos << "Cirrus" << llendl;
    CloudLayer *cirrusCloudLayer;

    cirrusCloudLayer = CloudLayerFactory::Create(CIRRUS_FIBRATUS);
    cirrusCloudLayer->SetBaseAltitude(8000);
    cirrusCloudLayer->SetThickness(0);
    cirrusCloudLayer->SetBaseLength(100000);
    cirrusCloudLayer->SetBaseWidth(100000);
    cirrusCloudLayer->SetLayerPosition(0, 0);
    cirrusCloudLayer->SeedClouds(*atm);

    atm->GetConditions()->AddCloudLayer(cirrusCloudLayer);
}

// Add a cumulus congestus deck with 40% sky coverage.
void LLDrawPoolWLSky::SetupCumulusCongestusClouds()
{
    llinfos << "Cumulus Congestus" << llendl;
    CloudLayer *cumulusCongestusLayer;

    cumulusCongestusLayer = CloudLayerFactory::Create(CUMULUS_CONGESTUS);
    cumulusCongestusLayer->SetBaseAltitude(1500);
    cumulusCongestusLayer->SetThickness(100);
    cumulusCongestusLayer->SetBaseLength(30000);
    cumulusCongestusLayer->SetBaseWidth(30000);
    cumulusCongestusLayer->SetDensity(0.4);
    cumulusCongestusLayer->SetLayerPosition(0, 0);
    cumulusCongestusLayer->SeedClouds(*atm);
    cumulusCongestusLayer->GenerateShadowMaps(false);

    atm->GetConditions()->AddCloudLayer(cumulusCongestusLayer);
}

// Sets up a solid stratus deck.
void LLDrawPoolWLSky::SetupStratusClouds()
{
    llinfos << "Stratus" << llendl;
    CloudLayer *stratusLayer;

    stratusLayer = CloudLayerFactory::Create(NIMBOSTRATUS);
    stratusLayer->SetBaseAltitude(1000);
    stratusLayer->SetThickness(600);
    stratusLayer->SetDensity(0.5);
    stratusLayer->SetLayerPosition(0, 0);
    stratusLayer->SeedClouds(*atm);

    atm->GetConditions()->AddCloudLayer(stratusLayer);
}

// A thunderhead; note a Cumulonimbus cloud layer contains a single cloud.
void LLDrawPoolWLSky::SetupCumulonimbusClouds()
{
    llinfos << "Cumulonimus" << llendl;
    CloudLayer *cumulonimbusLayer;

    cumulonimbusLayer = CloudLayerFactory::Create(CUMULONIMBUS_CAPPILATUS);
    cumulonimbusLayer->SetBaseAltitude(1000);
    cumulonimbusLayer->SetThickness(3000);
    cumulonimbusLayer->SetBaseLength(3000);
    cumulonimbusLayer->SetBaseWidth(5000);
    cumulonimbusLayer->SetLayerPosition(5000, 5000);
    cumulonimbusLayer->SeedClouds(*atm);

    atm->GetConditions()->AddCloudLayer(cumulonimbusLayer);
}

// Cumulus mediocris are little, puffy clouds. Keep the density low for realism, otherwise
// you'll have a LOT of clouds because they are small.
void LLDrawPoolWLSky::SetupCumulusMediocrisClouds()
{
    llinfos << "Cumulus mediocris" << llendl;
    CloudLayer *cumulusMediocrisLayer;

    cumulusMediocrisLayer = CloudLayerFactory::Create(CUMULUS_MEDIOCRIS);
    cumulusMediocrisLayer->SetBaseAltitude(1000);
    cumulusMediocrisLayer->SetThickness(100);
    cumulusMediocrisLayer->SetBaseLength(20000);
    cumulusMediocrisLayer->SetBaseWidth(20000);
    cumulusMediocrisLayer->SetDensity(0.2);
    cumulusMediocrisLayer->SetLayerPosition(0, 0);
    cumulusMediocrisLayer->SeedClouds(*atm);
    cumulusMediocrisLayer->GenerateShadowMaps(false);

    atm->GetConditions()->AddCloudLayer(cumulusMediocrisLayer);
}
