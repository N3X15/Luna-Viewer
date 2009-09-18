
#include "llviewerprecompiledheaders.h"
#include "flsky.h"
#include "llsky.h" 		// gSky
#include "llviewercamera.h" 	// LLViewerCamera
#include "llviewerregion.h"

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

FLSky::CloudList FLSky::mClouds;

VolumetricClouds* FLSky::Create(LLViewerRegion *region)
{
	U64 i = region->getHandle();	
	VolumetricClouds* Clouds= new VolumetricClouds();
	// 10 clouds on a 500x500 plane @ 192 height (?)
	Clouds->Create(10,256,192.f,region);
	mClouds[i]=Clouds;
	llinfos << "Created cloud deck on region \"" << region->getName() << "\"." << llendl;
	return Clouds;
}

void FLSky::DestroyDeck(LLViewerRegion *region)
{
	U64 i = region->getHandle();	
	mClouds[i]->Destroy();
	delete [] mClouds[i];
	llinfos << "Destroyed cloud deck on region \"" << region->getName() << "\"." << llendl;
}

//void FLSky::UpdateSettings()
//{
//
//}

//static
void FLSky::UpdateCamera()
{
	LLVector3 sun = gSky.getSunDirection();
	LLVector3 camera;
	LLViewerCamera::getInstance()->lookDir(camera);
	for(CloudListIter i = mClouds.begin();i!=mClouds.end();i++)
	{
		i->second->Update(LLV2V(sun),LLV2V(camera));
	}
}

void FLSky::Render()
{
	LLVector3 sun = gSky.getSunDirection();
	LLVector3 camera;
	LLViewerCamera::getInstance()->lookDir(camera);
	for(CloudListIter i = mClouds.begin();i!=mClouds.end();i++)
	{
		i->second->Render(LLV2V(sun),LLV2V(camera));
	}
}
