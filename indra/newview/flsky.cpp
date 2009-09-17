
#include "llviewerprecompiledheaders.h"
#include "flsky.h"

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

#include "VolumeClouds.h"
VolumetricClouds* FLSky::mClouds = NULL;

void FLSky::Init()
{
	mClouds= new VolumetricClouds();
	// 10 clouds on a 5000x5000 plane @ 500 height (?)
	mClouds->Create(10,5000,500);
}

//void FLSky::UpdateSettings()
//{
//
//}

//static
void FLSky::UpdateCamera()
{
	LLVector3 sun = gSky->getSunDirection();
	LLVector3 camera;
	LLViewerCamera::getInstance()->lookDir(&camera);

	mClouds->Update(sun,camera);
}

void FLSky::Render()
{
	LLVector3 sun = gSky->getSunDirection();
	LLVector3 camera;
	LLViewerCamera::getInstance()->lookDir(&camera);

	mClouds->Render(sun,camera);
}
