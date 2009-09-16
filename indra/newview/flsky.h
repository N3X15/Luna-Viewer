/**
* Dunno what to license this yet.  Assume closed-source (AKA if you're not me, close this window kthx)
*/
#ifndef FL_SKY_H
#define FL_SKY_H

#include "SilverLining.h"

#define SILVERLINING_COMPANY 	"YOUR COMPANY"
#define SILVERLINING_KEY	"LICENSE KEY"

using namespace SilverLining;

class FLSky
{
public:
	// @brief Sets up the class.
	static void Init();
	
	// @brief Get SilverLining::Atmosphere instance.
	Atmosphere* 	getAtmosphere();

	// @brief Get instance.
	//static FLSky* 		getInstance();

	// @brief Start rendering loop.
	static void RenderStart();

	// @brief End rendering loop (draw clouds etc)
	static void RenderEnd();

	static void	 	GetLightingColor(double *r,double *g,double *b);
	static void		UpdateCamera();
	static void		ConvertCart2Geo(double x,double y,double *_lat,double *_long);
	static Atmosphere*	mAtm;
private:
	static void		SetupAtmosphericConditions();
	// @brief Big, fluffy clouds
	static void 		SetupCumulusCongestusClouds();
	static void 		SetupCirrusClouds();
	static void		SetTimeAndLocation(int Lat=45,int Long=-122);
	static void		SetupFog();
	
};
#endif // FL_SKY_H
