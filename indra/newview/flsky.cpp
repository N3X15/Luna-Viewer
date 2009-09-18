
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
	VolumetricClouds* Clouds= new VolumetricClouds();
	// 10 clouds on a 500x500 plane @ 192 height (?)
	Clouds->Create(10,256,192.f,region);
	mClouds.push_back(Clouds);
	llinfos << "Created cloud deck on region \"" << region->getName() << "\"." << llendl;
	return Clouds;
}

void FLSky::DestroyDeck(LLViewerRegion *region)
{
	CloudListIter i=mClouds.begin();
	for(;i<mClouds.end();i++)
	{
		if(((VolumetricClouds*)*i)->GetRegionHandle()==region->getHandle())
		{
			//((VolumetricClouds*)*i)->Destroy();
			delete *i;
			llinfos << "Destroyed cloud deck on region \"" << region->getName() << "\"." << llendl;
		}
	}
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
	int n=mClouds.size();
	int i;
	for(i=0;i<n;i++)
	{
		mClouds[i]->Update(LLV2V(sun),LLV2V(camera));
	}
}

void FLSky::Render()
{
	LLVector3 sun = gSky.getSunDirection();
	LLVector3 camera;
	LLViewerCamera::getInstance()->lookDir(camera);
	int n=mClouds.size();
	int i;
	for(i=0;i<n;i++)
	{
		mClouds[i]->Render(LLV2V(sun),LLV2V(camera));
	}
}
