#ifndef LUAWINDLIGHT_H
#define LUAWINDLIGHT_H

#include "v3math.h"
#include "v4math.h"
#include "v2math.h"

class WLWater
{
public:
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

#endif
