// library includes
#include "llerror.h"
#include "llviewercontrol.h"
#include "llfasttimer.h"
#include "llmemory.h"
#include "llmemtype.h"
#include "llnamevalue.h"
#include "llvolume.h"
#include "material_codes.h"
#include "timing.h"
#include "v3color.h"
#include "llui.h" 
#include "llglheaders.h"
#include "llrender.h"
#include "lldir.h"

// SilverLining crap
#include "SilverLining.h"
using namespace SilverLining;

FLSky::FLSky() :
	mAtm("","")
{
	mAtm->ShowFramerate(true);

	// Tell SilverLining we're rendering in OpenGL, the Resources directory is 2 directories
	// above the working directory, and we're using a right-handed coordinate system.
	if (mAtm->Initialize(Atmosphere::OPENGL, gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS,"SilverLining")+"/", true, 0) == Atmosphere::E_NOERROR)
	{
		// Set up all the clouds
		SetupAtmosphericConditions();

		// Configure where and when we want to be
		SetTimeAndLocation();
	} else {
		llerrs << "SilverLining failed to load." << llendl;
	}

}
//static
void FLSky::UpdateCamera()
{
	if(gSavedSettings.getBool("SilverLiningDebug"))
		llinfos << "UpdateCamera()" << llendl;
	// Pass in the view and projection matrices to SilverLining.
	double mv[16], proj[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, mv);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	mAtm->SetCameraMatrix(mv);
	mAtm->SetProjectionMatrix(proj);
}


// Configure SilverLining for the desired wind, clouds, and visibility.
void FLSky::SetupAtmosphericConditions()
{
        // Set up wind blowing south at 50 meters/sec
        WindVolume wv;
        wv.SetDirection(180);
        wv.SetMinAltitude(0);
        wv.SetMaxAltitude(10000);
        wv.SetWindSpeed(50);
        mAtm->GetConditions()->SetWind(wv);

        // Set up the desired cloud types.
        SetupCirrusClouds();
        SetupCumulusCongestusClouds();

        // Set visibility in meters
        mAtm->GetConditions()->SetVisibility(100000);
}

// Add a cumulus congestus deck with 40% sky coverage, which stays centered around the camera position.
void FLSky::SetupCumulusCongestusClouds()
{
        CloudLayer *cumulusCongestusLayer;

        cumulusCongestusLayer = CloudLayerFactory::Create(CUMULUS_CONGESTUS_INFINITE);
        cumulusCongestusLayer->SetBaseAltitude(1500);
        cumulusCongestusLayer->SetThickness(100);
        cumulusCongestusLayer->SetBaseLength(30000);
        cumulusCongestusLayer->SetBaseWidth(30000);
        cumulusCongestusLayer->SetDensity(0.4);
        cumulusCongestusLayer->SetLayerPosition(0, 0);
        cumulusCongestusLayer->SeedClouds(*atm);
        cumulusCongestusLayer->GenerateShadowMaps(false);

        mAtm->GetConditions()->AddCloudLayer(cumulusCongestusLayer);
}

void FLSky::SetupCirrusClouds()
{
        CloudLayer *cirrusCloudLayer;

        cirrusCloudLayer = CloudLayerFactory::Create(CIRRUS_FIBRATUS);
        cirrusCloudLayer->SetBaseAltitude(8000);
        cirrusCloudLayer->SetThickness(0);
        cirrusCloudLayer->SetBaseLength(100000);
        cirrusCloudLayer->SetBaseWidth(100000);
        cirrusCloudLayer->SetLayerPosition(0, 0);
        cirrusCloudLayer->SeedClouds(*mAtm);

        mAtm->GetConditions()->AddCloudLayer(cirrusCloudLayer);
}
void FLSky::ConvertCart2Geo(int X,int Y,double *_lat,double *_long)
{
	FLGeoCoord point = new FLGeoCoord;
	// Sim Y = N, [0-1048576] -> [-90S,90N]
	// 	1.) Convert to fraction		(524288/1048576) 	= .5
	//	2.) Multiply by -180 [0,180]	
	//	    (Flips coords upside down)	(.5*-180)		= -90
	//	3.) Subtract 90 to get degrees
	//	    North/South of eq.		(90-90)			= 0
	// Latitude = 0, so it's
	_lat=(((double)Y/1048576.0)*-180.0)-90.0;

	// Convert from Sim X position [0-1048576] to Longitude ([-180W,+180E])
	//	524288/1048576		= .5
	//	.5*360			= 180	// No need to flip
	//	180-180			= 0	// Prime Meridian
	_long=(((double)X/1048576.0)*360.0)-180.0;
}
// Sets the simulated location and local time.
// Note, it's important that your longitude in the Location agrees with 
// the time zone in the LocalTime.
void FLSky::SetTimeAndLocation(int Lat,int Long)
{
        Location loc;
        loc.SetLatitude(Lat);
        loc.SetLongitude(Long); 

        LocalTime tm;
        tm.SetYear(1971);
        tm.SetMonth(8);
        tm.SetDay(5);
        tm.SetHour(14);
        tm.SetMinutes(0);
        tm.SetSeconds(0);
        tm.SetObservingDaylightSavingsTime(true);
        tm.SetTimeZone(PST);

        mAtm->GetConditions()->SetTime(tm);
        mAtm->GetConditions()->SetLocation(loc);
}

void FLSky::RenderStart()
{
	mAtm->BeginFrame(true);
}

void FLSky::RenderEnd()
{
	mAtm->EndFrame(true);
}

//static
void FLSky::GetLightingColor(double *r,double *g,double *b)
{
	mAtm->GetSunOrMoonColor(r,g,b);
}
