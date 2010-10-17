/* Copyright (c) 2009
 *
 * Modular Systems All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "llviewerprecompiledheaders.h"

#include "phoenixboobutils.h"

std::ostream &operator<<(std::ostream &os, const PhoenixGlobalBoobConfig &v)
{
	os << "PhoenixBoobConfig" << std::endl;
	os << "enabled: " << v.enabled << std::endl;
	os << "mass: " << v.mass << std::endl;
	os << "hardness: " << v.hardness << std::endl;
	os << "zMax: " << v.zMax << std::endl;
	os << "velMin: " << v.velMin << std::endl;
	os << "velMax: " << v.velMax << std::endl;
	os << "zInfluence: " << v.zInfluence << std::endl;
	os << "friction: " << v.friction << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const PhoenixAvatarLocalBoobConfig &v)
{
	os << "PhoenixAvatarLocalBoobConfig" << std::endl;
	os << "actualBoobGrav: " << v.actualBoobGrav << std::endl;
	os << "boobSize: " << v.boobSize << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const PhoenixBoobState &v)
{
	os << "PhoenixBoobState" << std::endl;
	os << "boobGrav: " << v.boobGrav << std::endl;
	os << "chestPosition: " << v.chestPosition << std::endl;
	os << "chestRotation: " << v.chestRotation << std::endl;
	os << "elapsedTime: " << v.elapsedTime << std::endl;
	os << "frameDuration: " << v.frameDuration << std::endl;
	os << "chestDisplacement: " << v.chestDisplacement << std::endl;
	os << "localChestDisplacement: " << v.localChestDisplacement << std::endl;
	os << "displacementForce: " << v.displacementForce << std::endl;
	os << "mysteryValue: " << v.mysteryValue << std::endl;
	os << "Number of bounceStates: " << v.bounceStates.size() << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const PhoenixBoobInputs &v)
{
	os << "PhoenixBoobInputs" << std::endl;
	os << "chestPosition: " << v.chestPosition << std::endl;
	os << "chestRotation: " << v.chestRotation << std::endl;
	os << "elapsedTime: " << v.elapsedTime << std::endl;
	os << "appearanceFlag: " << v.appearanceFlag << std::endl;
	os << "appearanceAnimating: " << v.appearanceAnimating << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const PhoenixBoobBounceState &v)
{
	os << "PhoenixBoobBounceState" << std::endl;
	os << "bounceStart: " << v.bounceStart << std::endl;
	os << "bounceStartAmplitude: " << v.bounceStartAmplitude << std::endl;
	os << "bounceStartFrameDuration: " << v.bounceStartFrameDuration << std::endl;
	return os;
}

F32 PhoenixBoobUtils::convertMass(F32 displayMass)
{ return displayMass/100.f*150.f; };

F32 PhoenixBoobUtils::convertHardness(F32 displayHardness)
{ return displayHardness/100.f*50; };

F32 PhoenixBoobUtils::convertVelMax(F32 displayVelMax)
{ return displayVelMax/100.f*0.01f; };

F32 PhoenixBoobUtils::convertFriction(F32 displayFriction)
{ return displayFriction/100.f*1.0f; };

F32 PhoenixBoobUtils::convertVelMin(F32 displayVelMin)
{ return displayVelMin/100.f; };

PhoenixBoobState PhoenixBoobUtils::idleUpdate(const PhoenixGlobalBoobConfig &config, const PhoenixAvatarLocalBoobConfig &localConfig, const PhoenixBoobState &oldState, const PhoenixBoobInputs &inputs)
{
	PhoenixBoobState newState;
	F32 avatarLocalMass = 0.0f;
	F32 partMod = 1.f;

	if(!config.enabled || inputs.appearanceFlag || inputs.appearanceAnimating)
		return newState;

	if(inputs.type == 0)
	{
		newState.boobGrav = localConfig.actualBoobGrav;
		avatarLocalMass = (llclamp(localConfig.boobSize, 0.0f, 0.5f) / 0.5f);
	}
	if(inputs.type == 1)
	{
		newState.boobGrav = localConfig.actualButtGrav;
		partMod = 1.5f;
		avatarLocalMass = llclamp(localConfig.actualButtGrav, 0.0f, 0.5f) / 0.5f;
	}
	if(inputs.type == 2)
	{
		newState.boobGrav = localConfig.actualFatGrav;
		partMod = 1.3f;
		avatarLocalMass = localConfig.actualFatGrav;
	}


	newState.elapsedTime = inputs.elapsedTime;
	//  seemed to create incorrect amounts of velocity when FPS varied
	newState.frameDuration = inputs.elapsedTime - oldState.elapsedTime;
	newState.chestPosition = inputs.chestPosition;
	newState.chestRotation = inputs.chestRotation;
	newState.chestDisplacement = inputs.chestPosition - oldState.chestPosition;
	newState.localChestDisplacement = newState.chestDisplacement * ~inputs.chestRotation;


	std::list<PhoenixBoobBounceState> bounceStates = oldState.bounceStates;

	if(fabs(newState.localChestDisplacement.length()) > 0.f)
	{
		F32 boobVel = 0.f;
		boobVel = newState.localChestDisplacement.mV[VZ];
		boobVel +=	newState.localChestDisplacement[VX] * config.XYInfluence;
		boobVel +=	newState.localChestDisplacement.mV[VY] * config.XYInfluence;
		boobVel *= newState.frameDuration * 0.3f * 100.f;
		boobVel =	llclamp(boobVel, -config.velMax, config.velMax);
		 if(fabs(boobVel) <= config.velMax * config.velMin * newState.frameDuration * 100.f)
			 boobVel = 0.0f;
		 else
		 {
			PhoenixBoobBounceState bounceState;
			bounceState.bounceStart = inputs.elapsedTime;
			bounceState.bounceStartFrameDuration = newState.frameDuration;
			bounceState.bounceStartAmplitude = boobVel;
			bounceState.bounceStartAmplitude *= avatarLocalMass;
			bounceState.bounceStartAmplitude *= config.mass;
			bounceStates.push_front(bounceState);
		 }
	}

	/*if(fabs(newState.localChestDisplacement.length()) >= 0.f) {
		LLVector3 displacementInfluence = newState.localChestDisplacement;
		displacementInfluence *= LLVector3(0.3f, 0.3f, 1.0f);
		F32 clampedDisplacementInfluenceLength = llclamp(displacementInfluence.length(), 0.0f, config.velMax);
		if(displacementInfluence[VZ]<0.f)
			clampedDisplacementInfluenceLength= -clampedDisplacementInfluenceLength;
		PhoenixBoobBounceState bounceState;
		bounceState.bounceStart = inputs.elapsedTime;
		bounceState.bounceStartFrameDuration = newState.frameDuration;
		bounceState.bounceStartAmplitude = clampedDisplacementInfluenceLength;
		if(fabs(bounceState.bounceStartAmplitude) < config.velMin * config.velMax)
			bounceState.bounceStartAmplitude = 0.0f;
		else
		{
			bounceState.bounceStartAmplitude *= config.mass;
			bounceStates.push_front(bounceState);
		}
	}
	*/

	F32 totalNewAmplitude = 0.0f;
	//std::cout << "Beginning bounce State processing at time " << inputs.elapsedTime << std::endl;
	while(!bounceStates.empty()) {
		PhoenixBoobBounceState bounceState = bounceStates.front();
		//std::cout << "Now processing " << bounceState;
		bounceStates.pop_front();
		F32 bounceTime = newState.elapsedTime-bounceState.bounceStart;
		F32 newAmplitude = bounceState.bounceStartAmplitude*pow(60.f*config.friction, -bounceTime)*cos(config.hardness*partMod*bounceTime);
		if(fabs(newAmplitude) < 0.005f) {
			newAmplitude = 0.0f;
		} else {
			newState.bounceStates.push_front(bounceState);
		}
		totalNewAmplitude+=newAmplitude;
	}
	//std::cout << "Total new amplitude: " << totalNewAmplitude << std::endl;
	/*
	if(inputs.type == 0)
		newState.boobGrav = localConfig.actualBoobGrav + totalNewAmplitude;
	if(inputs.type == 1)
		newState.boobGrav = localConfig.actualButtGrav + totalNewAmplitude;
	if(inputs.type == 2)
		newState.boobGrav = localConfig.actualFatGrav + totalNewAmplitude;
	*/

	newState.boobGrav = totalNewAmplitude;


	newState.boobGrav = llclamp(newState.boobGrav, -1.5f, 2.0f);

	return newState;
}
