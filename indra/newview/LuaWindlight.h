#ifndef LUAWINDLIGHT_H
#define LUAWINDLIGHT_H

#include "LuaTypes.h"

class WLWater
{
	static void SetFogColor(LuaVector3 rgb, float glow);
	static void GetFogColor(LuaVector3 *rgb,float *glow);

	static void SetFogDensity(float density);
	static float GetFogDensity();

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

/*
	childSetCommitCallback("WaterFogColor", onWaterFogColorMoved, &param_mgr->mFogColor);

	// 
	childSetCommitCallback("WaterGlow", onColorControlAMoved, &param_mgr->mFogColor);

	// fog density
	childSetCommitCallback("WaterFogDensity", onExpFloatControlMoved, &param_mgr->mFogDensity);
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
	childSetCommitCallback("WaterWave2DirY", onVector2ControlYMoved, &param_mgr->mWave2Dir);*/
}

#endif
