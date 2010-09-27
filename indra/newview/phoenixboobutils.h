#ifndef __Ascentboobutils_h
#define __Ascentboobutils_h

#include <iostream>
#include <list>

#include "stdtypes.h"
#include "v3math.h"
#include "llquaternion.h"

struct AscentGlobalBoobConfig
{
	bool enabled;
	F32 mass;
	F32 hardness;
	F32 zMax;
	F32 velMin;
	F32 velMax;
	F32 zInfluence;
	F32 friction;
	F32 XYInfluence;

	AscentGlobalBoobConfig()
		: enabled(false),
			mass(6.4f),
			hardness(0.67f),
			zMax(1.29f),
			velMin(0.0027f*0.017f),
			velMax(0.0027f),
			zInfluence(0.0f),
			friction(0.35f),
			XYInfluence(0.3f)
	{
	}

	bool operator==(const AscentGlobalBoobConfig &other) const
	{
		return
			enabled == other.enabled &&
			mass == other.mass &&
			zMax == other.zMax &&
			velMax == other.velMax &&
			velMin == other.velMin &&
			zInfluence == other.zInfluence &&
			XYInfluence == other.XYInfluence &&
			friction == other.friction;
	}
};

std::ostream &operator<<(std::ostream &os, const AscentGlobalBoobConfig &v);

struct AscentAvatarLocalBoobConfig
{
	F32 actualBoobGrav;
	F32 actualButtGrav;
	F32 actualFatGrav;
	F32 boobSize;

	AscentAvatarLocalBoobConfig()
		: actualBoobGrav(0.0f),
		  actualButtGrav(0.0f),
		  actualFatGrav(0.0f),
			boobSize(0.0f)
	{
	}

	bool operator==(const AscentAvatarLocalBoobConfig &other) const
	{
		return
			actualBoobGrav == other.actualBoobGrav &&
			actualButtGrav == other.actualButtGrav &&
			actualFatGrav == other.actualFatGrav &&
			boobSize == other.boobSize;
	}

};

std::ostream &operator<<(std::ostream &os, const AscentAvatarLocalBoobConfig &v);

struct AscentBoobBounceState;

struct AscentBoobState
{
	F32 boobGrav;
	LLVector3 chestPosition;
	LLQuaternion chestRotation;
	F32 elapsedTime;
	F32 frameDuration;
	LLVector3 chestDisplacement;
	LLVector3 localChestDisplacement;
	LLVector3 displacementForce;
	F32 mysteryValue;
	std::list<AscentBoobBounceState> bounceStates;

	AscentBoobState()
		: boobGrav(0.0f),
			chestPosition(0.0f,0.0f,0.0f),
			chestRotation(0.0f,0.0f,0.0f,1.0f),
			elapsedTime(0.0f),
			frameDuration(0.0f),
			chestDisplacement(0.0f,0.0f,0.0f),
			localChestDisplacement(0.0f,0.0f,0.0f),
			displacementForce(0.0f,0.0f,0.0f),
			mysteryValue(0.0f)
	{
	}

	bool operator==(const AscentBoobState &other) const
	{
		return
			boobGrav == other.boobGrav &&
			chestPosition == other.chestPosition &&
			chestRotation == other.chestRotation &&
			elapsedTime == other.elapsedTime &&
			frameDuration == other.frameDuration &&
			chestDisplacement == other.chestDisplacement &&
			localChestDisplacement == other.localChestDisplacement &&
			displacementForce == other.displacementForce &&
			mysteryValue == other.mysteryValue &&
			bounceStates == other.bounceStates;
	}
};

std::ostream &operator<<(std::ostream &os, const AscentBoobState &v);

struct AscentBoobInputs
{
	LLVector3 chestPosition;
	LLQuaternion chestRotation;
	F32 elapsedTime;
	bool appearanceFlag;
	bool appearanceAnimating;
	S32 type;

	AscentBoobInputs()
		: chestPosition(0.0f,0.0f,0.0f),
			chestRotation(0.0f,0.0f,0.0f,1.0f),
			elapsedTime(0.0f),
			appearanceFlag(false),
			appearanceAnimating(false),
			type(0)
	{
	}

	bool operator==(const AscentBoobInputs &other) const
	{
		return
			chestPosition == other.chestPosition &&
			chestRotation == other.chestRotation &&
			elapsedTime == other.elapsedTime &&
			appearanceFlag == other.appearanceFlag &&
			appearanceAnimating == other.appearanceAnimating &&
			type == other.type;
	}
};

std::ostream &operator<<(std::ostream &os, const AscentBoobInputs &v);

struct AscentBoobBounceState
{
	F32 bounceStart;
	F32 bounceStartAmplitude;
	F32 bounceStartFrameDuration;

	AscentBoobBounceState()
		: bounceStart(0.0f),
			bounceStartAmplitude(0.0f),
			bounceStartFrameDuration(0.0f)
	{
	};

	bool operator==(const AscentBoobBounceState &other) const
	{
		return
			bounceStart == other.bounceStart &&
			bounceStartAmplitude == other.bounceStartAmplitude &&
			bounceStartFrameDuration == other.bounceStartFrameDuration;
	}
};

std::ostream &operator<<(std::ostream &os, const AscentBoobBounceState &v);


struct AscentBoobUtils
{
public:
	static AscentBoobState idleUpdate(const AscentGlobalBoobConfig &config, const AscentAvatarLocalBoobConfig &localConfig, const AscentBoobState &oldState, const AscentBoobInputs &inputs);

	static F32 convertMass(F32 displayMass);
	static F32 convertHardness(F32 displayHardness);
	static F32 convertVelMax(F32 displayVelMax);
	static F32 convertFriction(F32 displayFriction);
	static F32 convertVelMin(F32 displayVelMin);
};


#endif
