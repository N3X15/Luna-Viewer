
#include "llviewerprecompiledheaders.h"
#include "LuaWindlight.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"

#include "llwaterparamset.h"
#include "llwaterparammanager.h"

// Red Green Blue **GLOW**
void WLWater::SetFogColor(LLVector4 rgba)
{
	LLWaterParamManager::instance()->mFogColor=rgba;
}

void WLWater::GetFogColor()
{
	return LLWaterParamManager::instance()->mFogColor;
}

void WLWater::SetFogDensity(float density)
{
	LLWaterParamManager::instance()->mFogDensity=density;
}

float WLWater::GetFogDensity()
{
	return LLWaterParamManager::instance()->mFogDensity;
}

static void SetUnderwaterFogMod(float density)
{
	LLWaterParamManager::instance()->mUnderWaterFogMod=density;
}
static float GetUnderwaterFogMod()
{
	return LLWaterParamManager::instance()->mUnderWaterFogMod;
}

static void SetNormalScale(LLVector3 scale)
{
	LLWaterParamManager::instance()->mNormalScale=scale;
}
static LLVector3 GetNormalScale()
{
	return LLWaterParamManager::instance()->mNormalScale;
}

static void SetFresnelScale(float Scale)
{
	LLWaterParamManager::instance()->mFresnelScale=Scale;
}
static float GetFresnelScale()
{
	return LLWaterParamManager::instance()->mFresnelScale;
}

static void SetFresnelOffset(float offset);
static float GetFresnelOffset();

static void SetScaleAbove(float scale);
static float GetScaleAbove();

static void SetScaleBelow(float scale);
static float GetScaleBelow();

static void SetBlurMultiplier(float scale);
static float GetBlurMultiplier();

static void SetWave1Dir(float  x,float  y);
static void GetWave1Dir(float *x,float *y);

static void SetWave2Dir(float  x,float  y);
static void GetWave2Dir(float *x,float *y);

/*
	childSetCommitCallback("WaterUnderWaterFogMod", onFloatControlMoved, &param_mgr->mUnderWaterFogMod);

	// blue density
	childSetCommitCallback("WaterNormalScaleX", onVector3ControlXMoved, &param_mgr->mNormalScale);
	childSetCommitCallback("WaterNormalScaleY", onVector3ControlYMoved, &param_mgr->mNormalScale);
	childSetCommitCallback("WaterNormalScaleZ", onVector3ControlZMoved, &param_mgr->mNormalScale);

	// fresnel
	childSetCommitCallback("WaterFresnelScale", onFloatControlMoved, &param_mgr->mFresnelScale);
	childSetCommitCallback("WaterFresnelOffset", onFloatControlMoved, &param_mgr->mFresnelOffset);

	// scale above/below
	childSetCommitCallback("WaterScaleAbove", onFloatControlMoved, &param_mgr->mScaleAbove);
	childSetCommitCallback("WaterScaleBelow", onFloatControlMoved, &param_mgr->mScaleBelow);

	// blur mult
	childSetCommitCallback("WaterBlurMult", onFloatControlMoved, &param_mgr->mBlurMultiplier);

	// Load/save
	LLComboBox* comboBox = getChild<LLComboBox>("WaterPresetsCombo");

	//childSetAction("WaterLoadPreset", onLoadPreset, comboBox);
	childSetAction("WaterNewPreset", onNewPreset, comboBox);
	childSetAction("WaterSavePreset", onSavePreset, comboBox);
	childSetAction("WaterDeletePreset", onDeletePreset, comboBox);

	// wave direction
	childSetCommitCallback("WaterWave1DirX", onVector2ControlXMoved, &param_mgr->mWave1Dir);
	childSetCommitCallback("WaterWave1DirY", onVector2ControlYMoved, &param_mgr->mWave1Dir);
	childSetCommitCallback("WaterWave2DirX", onVector2ControlXMoved, &param_mgr->mWave2Dir);
	childSetCommitCallback("WaterWave2DirY", onVector2ControlYMoved, &param_mgr->mWave2Dir);
*/
