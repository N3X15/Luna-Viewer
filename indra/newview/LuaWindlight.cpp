
#include "llviewerprecompiledheaders.h"
#include "LuaWindlight.h"
#include "LuaTypes.h";

#include "llwaterparamset.h"
#include "llwaterparammanager.h"

void WLWater::SetFogColor(LuaVector3 rgb, float a)
{
	LLWaterParamManager::instance()->mFogColor=new LLVector4(rgb.mV[VX],rgb.mV[VY],rgb.mV[VZ],a);
}

void WLWater::GetFogColor(LuaVector3 *rgb, float *a)
{
	LLVector4 rgba=LLWaterParamManager::instance()->mFogColor;
	rgb=new LLVector3(rgba);
	a=rgba.mV[VW];
}

void WLWater::SetFogDensity(float density)
{
	LLWaterParamManager::instance()->mFogDensity=density;
}

float WLWater::GetFogDensity()
{
	return LLWaterParamManager::instance()->mFogDensity;
}

static void SetUnderwaterFogMod(float density);
static float GetUnderwaterFogMod();	

static void SetNormalScale(LuaVector3 scale);
static LuaVector3 GetNormalScale();

static void SetFresnelScale(float Scale);
static float GetFresnelScale();

static void SetFresnelOffset(float offset);
static float GetFresnelOffset();

static void SetScaleAbove(float scale);
static float GetScaleAbove();

static void SetScaleBelow(float scale);
static float GetScaleBelow();

static void SetBlurMultiplier(float scale);
static float GetBluMultiplier();

static void SetWave1Dir(float  x,float  y);
static void GetWave1Dir(float *x,float *y);

static void SetWave2Dir(float  x,float  y);
static void GetWave2Dir(float *x,float *y);
