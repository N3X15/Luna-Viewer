/**
* Dunno what to license this yet.  Assume closed-source (AKA if you're not me, close this window kthx)
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

	typedef std::vector<VolumetricClouds*>::iterator CloudListIter;
	typedef std::vector<VolumetricClouds*> CloudList;
	
	static CloudList	mClouds;
	
};
#endif // FL_SKY_H
