#ifndef LUAWINDLIGHT_H
#define LUAWINDLIGHT_H

#include "lluuid.h"
#include "v3math.h"
#include "v4math.h"
#include "v2math.h"

class WLWater
{
public:
	static void SetNormalMap(const LLUUID& map);
	static LLUUID GetNormalMap();

	static void SetFogColor(const LLVector4& rgba);
	static LLVector4 GetFogColor();

	static void SetFogDensity(float density);
	static float GetFogDensity();

	static void SetUnderwaterFogMod(float density);
	static float GetUnderwaterFogMod();	

	static void SetNormalScale(const LLVector3& scale);
	static LLVector3 GetNormalScale();

	static void SetFresnelScale(float Scale);
	static float GetFresnelScale();

	static void SetFresnelOffset(float offset);
	static float GetFresnelOffset();

	static void SetScaleAbove(float scale);
	static float GetScaleAbove();

	static void SetScaleBelow(float scale);
	static float GetScaleBelow();

	static void SetBlurMultiplier(float scale);
	static float GetBlurMultiplier();

	static void SetWave1Dir(LLVector2 dir);
	static LLVector2 GetWave1Dir();

	static void SetWave2Dir(LLVector2 dir);
	static LLVector2 GetWave2Dir();
};

class WLSky
{
public:

	/// Sun Delta Terrain tweak variables.
	// mSunDeltaYaw
	static void  SetSunDeltaYaw(float in);
	static float GetSunDeltaYaw();

	// mSceneLightStrength
	static void  SetSceneLightStrength(float in);
	static float GetSceneLightStrength();
	
	// mWLGamma(1.0f, "gamma"),
	static void  SetGamma(float in);
	static float GetGamma();

	//mBlueHorizon(0.25f, 0.25f, 1.0f, 1.0f, "blue_horizon", "WLBlueHorizon"),
	static void  		SetBlueHorizon(const LLVector4& in);
	static LLVector4	GetBlueHorizon();

	//mHazeDensity(1.0f, 1.0f, 1.0f, 0.5f, "haze_density"),
	static void  		SetHazeDensity(const LLVector4& in);
	static LLVector4	GetHazeDensity();

	//mBlueDensity(0.25f, 0.25f, 0.25f, 1.0f, "blue_density", "WLBlueDensity"),
	static void  		SetBlueDensity(const LLVector4& in);
	static LLVector4	GetBlueDensity();

	//mDensityMult(1.0f, "density_multiplier", 1000),
	static void		SetDensityMultiplier(float in);
	static float		GetDensityMultiplier();

	//mHazeHorizon(1.0f, 1.0f, 1.0f, 0.5f, "haze_horizon"),
	static void		SetHazeHorizon(const LLVector4& in);
	static LLVector4	GetHazeHorizon();

	//mMaxAlt(4000.0f, "max_y"),
	static void		SetMaxAltitude(float in);
	static float		GetMaxAltitude();

	// Lighting
	//mLightnorm(0.f, 0.707f, -0.707f, 1.f, "lightnorm"),
	static void		SetLightNorm(const LLVector4& in);
	static LLVector4	GetLightNorm();

	//mSunlight(0.5f, 0.5f, 0.5f, 1.0f, "sunlight_color", "WLSunlight"),
	static void		SetSunlight(const LLVector4& in);
	static LLVector4	GetSunlight();

	//mAmbient(0.5f, 0.75f, 1.0f, 1.19f, "ambient", "WLAmbient"),
	static void		SetAmbient(const LLVector4& in);
	static LLVector4	GetAmbient();

	//mGlow(18.0f, 0.0f, -0.01f, 1.0f, "glow"),
	static void		SetGlow(const LLVector4& in);
	static LLVector4	GetGlow();

	// Clouds
	//mCloudColor(0.5f, 0.5f, 0.5f, 1.0f, "cloud_color", "WLCloudColor"),
	static void		SetCloudColor(const LLVector4& in);
	static LLVector4	GetCloudColor();

	//mCloudMain(0.5f, 0.5f, 0.125f, 1.0f, "cloud_pos_density1"),
	static void		SetCloudDensity1(const LLVector4& in);
	static LLVector4	GetCloudDensity1();

	//mCloudCoverage(0.0f, "cloud_shadow"),
	static void		SetCloudCoverage(float in);
	static float		GetCloudCoverage();

	//mCloudDetail(0.0f, 0.0f, 0.0f, 1.0f, "cloud_pos_density2"),
	static void		SetCloudDensity2(const LLVector4& in);
	static LLVector4	GetCloudDensity2();

	//mDistanceMult(1.0f, "distance_multiplier"),
	static void		SetDistanceMult(float in);
	static float		GetDistanceMult();

	//mCloudScale(0.42f, "cloud_scale"),
	static void		SetCloudScale(float in);
	static float		GetCloudScale();
};

#endif
