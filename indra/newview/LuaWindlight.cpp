
#include "llviewerprecompiledheaders.h"
#include "LuaWindlight.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"

#include "llwaterparamset.h"
#include "llwaterparammanager.h"
#include "llwlparammanager.h"
#include "llwlparamset.h"


////////////////////////////////////////////////////////////////////
// Normal Map
////////////////////////////////////////////////////////////////////

void WLWater::SetNormalMap(const LLUUID& m)
{
	LLWaterParamManager::instance()->setNormalMapID(m);
}

LLUUID WLWater::GetNormalMap()
{
	return LLWaterParamManager::instance()->getNormalMapID();
}

////////////////////////////////////////////////////////////////////
//Fog Density
////////////////////////////////////////////////////////////////////
void WLWater::SetFogColor(const LLVector4& rgba)
{
	LLWaterParamManager::instance()->mCurParams.set("waterFogColor",rgba);
}
LLVector4 WLWater::GetFogColor()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getVector4("waterFogColor",err);
}

////////////////////////////////////////////////////////////////////
// Fog Density
////////////////////////////////////////////////////////////////////
void WLWater::SetFogDensity(float density)
{
	LLWaterParamManager::instance()->mCurParams.set("waterFogDensity",(F32)density);
}
float WLWater::GetFogDensity()
{
	bool err;
	return (float)LLWaterParamManager::instance()->mCurParams.getFloat("waterFogDensity",err);
}

////////////////////////////////////////////////////////////////////
// UnderWater Fog Modifier
////////////////////////////////////////////////////////////////////
void WLWater::SetUnderwaterFogMod(float density)
{
	LLWaterParamManager::instance()->mCurParams.set("underWaterFogMod",(F32)density);
}
float WLWater::GetUnderwaterFogMod()
{
	bool err;
	return 	LLWaterParamManager::instance()->mCurParams.getFloat("underWaterFogMod",err);
}

////////////////////////////////////////////////////////////////////
// Normal Scale
////////////////////////////////////////////////////////////////////
void WLWater::SetNormalScale(const LLVector3& scale)
{
	LLWaterParamManager::instance()->mCurParams.set("normScale",scale.mV[0],scale.mV[1],scale.mV[2]);
}
LLVector3 WLWater::GetNormalScale()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getVector3("normScale",err);
}

////////////////////////////////////////////////////////////////////
// Fresnel Scale
////////////////////////////////////////////////////////////////////
void WLWater::SetFresnelScale(float Scale)
{
	LLWaterParamManager::instance()->mCurParams.set("fresnelScale",(F32)Scale);
}
float WLWater::GetFresnelScale()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getFloat("fresnelScale",err);
}

////////////////////////////////////////////////////////////////////
// Fresnel Offset
////////////////////////////////////////////////////////////////////
void WLWater::SetFresnelOffset(float offset)
{
	LLWaterParamManager::instance()->mCurParams.set("fresnelOffset",(F32)offset);
}
float WLWater::GetFresnelOffset()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getFloat("fresnelOffset",err);
}

////////////////////////////////////////////////////////////////////
// Scale Above
////////////////////////////////////////////////////////////////////
void WLWater::SetScaleAbove(float scale)
{
	LLWaterParamManager::instance()->mCurParams.set("scaleAbove",(F32)scale);
}
float WLWater::GetScaleAbove()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getFloat("scaleAbove",err);
}

////////////////////////////////////////////////////////////////////
// Scale Below
////////////////////////////////////////////////////////////////////
void WLWater::SetScaleBelow(float scale)
{
	LLWaterParamManager::instance()->mCurParams.set("scaleBelow",(F32)scale);
}
float WLWater::GetScaleBelow()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getFloat("scaleBelow",err);
}

////////////////////////////////////////////////////////////////////
// Blur Multiplier
////////////////////////////////////////////////////////////////////
void WLWater::SetBlurMultiplier(float scale)
{
	LLWaterParamManager::instance()->mCurParams.set("blurMultiplier",(F32)scale);
}
float WLWater::GetBlurMultiplier()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getFloat("blurMultiplier",err);
}

////////////////////////////////////////////////////////////////////
// Wave1 Direction
////////////////////////////////////////////////////////////////////
void WLWater::SetWave1Dir(LLVector2 dir)
{
	LLWaterParamManager::instance()->mWave1Dir=dir;
}
LLVector2 WLWater::GetWave1Dir()
{
	return LLWaterParamManager::instance()->getWave1Dir();
}

////////////////////////////////////////////////////////////////////
// Wave2 Direction
////////////////////////////////////////////////////////////////////
void WLWater::SetWave2Dir(LLVector2 dir)
{
	LLWaterParamManager::instance()->mWave2Dir=dir;
}
LLVector2 WLWater::GetWave2Dir()
{
	return LLWaterParamManager::instance()->getWave2Dir();
}


/// Sun Delta Terrain tweak variables.
	// mSunDeltaYaw
//Static
void WLSky::SetSunDeltaYaw(float in)
{
	LLWLParamManager::instance()->mSunDeltaYaw=in;
}
float WLSky::GetSunDeltaYaw()
{
	return LLWLParamManager::instance()->mSunDeltaYaw;
}

	// mSceneLightStrength
void WLSky::SetSceneLightStrength(float in)
{
	LLWLParamManager::instance()->mSceneLightStrength=in;
}

float WLSky::GetSceneLightStrength()
{
	return LLWLParamManager::instance()->mSceneLightStrength;
}
	
	// mWLGamma(1.0f, "gamma"),
void WLSky::SetGamma(float in)
{
	LLWLParamManager::instance()->mCurParams.set("gamma",(F32)in);
}
float WLSky::GetGamma()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getFloat("gamma",err);
}

	//mBlueHorizon(0.25f, 0.25f, 1.0f, 1.0f, "blue_horizon", "WLBlueHorizon"),
void WLSky::SetBlueHorizon(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("blue_horizon",in);
}

LLVector4 WLSky::GetBlueHorizon()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("blue_horizon",err);
}

	//mHazeDensity(1.0f, 1.0f, 1.0f, 0.5f, "haze_density"),
void WLSky::SetHazeDensity(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("haze_density",in);
}

LLVector4 WLSky::GetHazeDensity()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("haze_density",err);
}

	//mBlueDensity(0.25f, 0.25f, 0.25f, 1.0f, "blue_density", "WLBlueDensity"),
void WLSky::SetBlueDensity(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("blue_density",in);
}

LLVector4 WLSky::GetBlueDensity()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("blue_density",err);
}

	//mDensityMult(1.0f, "density_multiplier", 1000),
void WLSky::SetDensityMultiplier(float in)
{
	LLWLParamManager::instance()->mCurParams.set("density_multiplier",(F32)in);
}
float WLSky::GetDensityMultiplier()
{
	bool err;
	return LLWaterParamManager::instance()->mCurParams.getFloat("density_multiplier",err);
}

	//mHazeHorizon(1.0f, 1.0f, 1.0f, 0.5f, "haze_horizon"),
void WLSky::SetHazeHorizon(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("haze_horizon",in);
}

LLVector4 WLSky::GetHazeHorizon()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("haze_horizon",err);
}

	//mMaxAlt(4000.0f, "max_y"),
void WLSky::SetMaxAltitude(float in)
{
	LLWLParamManager::instance()->mCurParams.set("max_y",(F32)in);
}
float WLSky::GetMaxAltitude()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getFloat("max_y",err);
}

	// Lighting
	//mLightnorm(0.f, 0.707f, -0.707f, 1.f, "lightnorm"),
void WLSky::SetLightNorm(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("lightnorm",in);
}

LLVector4 WLSky::GetLightNorm()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("lightnorm",err);
}

	//mSunlight(0.5f, 0.5f, 0.5f, 1.0f, "sunlight_color", "WLSunlight"),
void WLSky::SetSunlight(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("sunlight_color",in);
}

LLVector4 WLSky::GetSunlight()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("sunlight_color",err);
}

	//mAmbient(0.5f, 0.75f, 1.0f, 1.19f, "ambient", "WLAmbient"),
void WLSky::SetAmbient(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("ambient",in);
}

LLVector4 WLSky::GetAmbient()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("ambient",err);
}

	//mGlow(18.0f, 0.0f, -0.01f, 1.0f, "glow"),
void WLSky::SetGlow(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("glow",in);
}

LLVector4 WLSky::GetGlow()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("glow",err);
}

	// Clouds
	//mCloudColor(0.5f, 0.5f, 0.5f, 1.0f, "cloud_color", "WLCloudColor"),
void WLSky::SetCloudColor(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("cloud_color",in);
}

LLVector4 WLSky::GetCloudColor()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("cloud_color",err);
}

	//mCloudMain(0.5f, 0.5f, 0.125f, 1.0f, "cloud_pos_density1"),
void WLSky::SetCloudDensity1(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("cloud_pos_density1",in);
}

LLVector4 WLSky::GetCloudDensity1()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("cloud_pos_density1",err);
}

	//mCloudCoverage(0.0f, "cloud_shadow"),
void WLSky::SetCloudCoverage(float in)
{
	LLWLParamManager::instance()->mCurParams.set("cloud_shadow",(F32)in);
}
float WLSky::GetCloudCoverage()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getFloat("cloud_shadow",err);
}

	//mCloudDetail(0.0f, 0.0f, 0.0f, 1.0f, "cloud_pos_density2"),
void WLSky::SetCloudDensity2(const LLVector4& in)
{
	LLWLParamManager::instance()->mCurParams.set("cloud_pos_density2",in);
}

LLVector4 WLSky::GetCloudDensity2()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getVector("cloud_pos_density2",err);
}

	//mDistanceMult(1.0f, "distance_multiplier"),
void WLSky::SetDistanceMult(float in)
{
	LLWLParamManager::instance()->mCurParams.set("distance_multiplier",(F32)in);
}
float WLSky::GetDistanceMult()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getFloat("distance_multiplier",err);
}

	//mCloudScale(0.42f, "cloud_scale"),
void WLSky::SetCloudScale(float in)
{
	LLWLParamManager::instance()->mCurParams.set("cloud_scale",(F32)in);
}
float WLSky::GetCloudScale()
{
	bool err;
	return LLWLParamManager::instance()->mCurParams.getFloat("cloud_scale",err);
}
