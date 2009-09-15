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
	FLSky();
	// @brief Destructor.
	~FLSky();
	
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
	static void		ConvertCart2Geo(int x,int y,double *_lat,double *_long);
private
	Atmosphere*	mAtm;
	void		SetupAtmosphericConditions();
	// @brief Big, fluffy clouds
	void 		SetupCumulusCongestusClouds();
	void 		SetupCirrusClouds();
	void		SetTimeAndLocation(int Lat=45,int Long=-122);
	void		SetupFog();
	
}
#endif // FL_SKY_H
