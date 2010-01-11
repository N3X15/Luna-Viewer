
#include "llviewerprecompiledheaders.h"
#include "LuaWindlight.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"

#include "llwaterparamset.h"
#include "llwaterparammanager.h"

//	LLUUID getNormalMapID(void);
//	LLVector2 getWave1Dir(void);
//	LLVector2 getWave2Dir(void);
//	F32 getScaleAbove(void);
//	F32 getScaleBelow(void);
//	LLVector3 getNormalScale(void);
//	F32 getFresnelScale(void);
//	F32 getFresnelOffset(void);
//	F32 getBlurMultiplier(void);
//	F32 getFogDensity(void);
//	LLColor4 getFogColor(void);


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

