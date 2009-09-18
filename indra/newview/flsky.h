/**
* GPL v2.0 here
*/
#ifndef FL_SKY_H
#define FL_SKY_H

#include "VolumeClouds.h"
#include "llviewerregion.h"

class FLSky
{
public:
	// @brief Sets up the class.
	static VolumetricClouds* 	Create(		LLViewerRegion *region);
	static void 			DestroyDeck(	LLViewerRegion *region);
	

	// @brief Get instance.
	//static FLSky* 		getInstance();

	// @brief Render clouds.
	static void Render();
	static void UpdateCamera();

	typedef std::map<U64,VolumetricClouds*>::iterator CloudListIter;
	typedef std::map<U64,VolumetricClouds*> CloudList;
	
	static CloudList	mClouds;
	
};
#endif // FL_SKY_H
