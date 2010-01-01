#ifndef LUAPARTICLES_H
#define LUAPARTICLES_H

#include "llviewerimagelist.h"
#include "llviewerobject.h"
#include "llviewerobjectlist.h"
/*
--- C++ PARTICLES ---
	LLPartSysData particle_parameters;

	// fancy particle cloud designed by Brent
	particle_parameters.mPartData.mMaxAge            = 4.f;
	particle_parameters.mPartData.mStartScale.mV[VX] = 0.8f;
	particle_parameters.mPartData.mStartScale.mV[VX] = 0.8f;
	particle_parameters.mPartData.mStartScale.mV[VY] = 1.0f;
	particle_parameters.mPartData.mEndScale.mV[VX]   = 0.02f;
	particle_parameters.mPartData.mEndScale.mV[VY]   = 0.02f;
	particle_parameters.mPartData.mStartColor        = LLColor4(1, 1, 1, 0.5f);
	particle_parameters.mPartData.mEndColor          = LLColor4(1, 1, 1, 0.0f);
	particle_parameters.mPartData.mStartScale.mV[VX] = 0.8f;
	LLViewerImage* cloud = gImageList.getImageFromFile("cloud-particle.j2c");
	particle_parameters.mPartImageID                 = cloud->getID();
	particle_parameters.mMaxAge                      = 0.f;
	particle_parameters.mPattern                     = LLPartSysData::LL_PART_SRC_PATTERN_ANGLE_CONE;
	particle_parameters.mInnerAngle                  = 3.14159f;
	particle_parameters.mOuterAngle                  = 0.f;
	particle_parameters.mBurstRate                   = 0.02f;
	particle_parameters.mBurstRadius                 = 0.0f;
	particle_parameters.mBurstPartCount              = 1;
	particle_parameters.mBurstSpeedMin               = 0.1f;
	particle_parameters.mBurstSpeedMax               = 1.f;
	particle_parameters.mPartData.mFlags             = ( 
		LLPartData::LL_PART_INTERP_COLOR_MASK | 
		LLPartData::LL_PART_INTERP_SCALE_MASK |
		LLPartData::LL_PART_EMISSIVE_MASK | 
		// LLPartData::LL_PART_FOLLOW_SRC_MASK |
		LLPartData::LL_PART_TARGET_POS_MASK );

	setParticleSource(particle_parameters, getID());

--- LUA ---

	local particle_parameters = ParticleSystem();

	-- fancy particle cloud designed by Brent
	particle_parameters.ParticleMaxAge = 4.0
	particle_parameters.StartScale.x = 0.8
	particle_parameters.StartScale.y = 1.0
	particle_parameters.EndScale.x   = 0.02
	particle_parameters.EndScale.y   = 0.02
	particle_parameters.StartColor = Color4(1.0, 1.0, 1.0, 0.5)
	particle_parameters.EndColor = Color4(1.0, 1.0, 1.0, 0.0)
	particle_parameters.SetImageFromFile("cloud-particle.j2c")
	particle_parameters.MaxAge = 0.0
	particle_parameters.Pattern = ParticlePattern:ANGLE_CONE
	particle_parameters.InnerAngle = 3.14159
	particle_parameters.OuterAngle = 0.0
	particle_parameters.BurstRate = 0.02
	particle_parameters.BurstRadius = 0.0
	particle_parameters.BurstPartCount = 1
	particle_parameters.BurstSpeedMin = 0.1f
	particle_parameters.BurstSpeedMax = 1.f
	particle_parameters.ParticleFlags = ( 
		ParticleFlags:INTERP_COLOR_MASK | 
		ParticleFlags:INTERP_SCALE_MASK |
		ParticleFlags:EMISSIVE_MASK | 
		-- LLPartData::LL_PART_FOLLOW_SRC_MASK |
		LLPartData::LL_PART_TARGET_POS_MASK );

	particle_parameters.AttachToObject(AvatarUUID);
*/

class ParticlePattern
{
public:
	const U8 DROP 		= 0x01; 
	const U8 EXPLODE 	= 0x02; 
	const U8 ANGLE 		= 0x04; 
	const U8 ANGLE_CONE 	= 0x08;
	const U8 CONE_EMPTY 	= 0x10;
};

class ParticleFlags
{
 	const U8 INTERP_COLOR_MASK	= 0x01; 
	const U8 INTERP_SCALE_MASK	= 0x02;
	const U8 BOUNCE_MASK		= 0x04;
	const U8 WIND_MASK 		= 0x08;
	const U8 FOLLOW_SRC_MASK 	= 0x10;
	const U8 FOLLOW_VELOCITY_MASK	= 0x20;
	const U8 TARGET_POS_MASK 	= 0x40;
	const U8 TARGET_LINEAR_MASK 	= 0x80;
	const U8 EMISSIVE_MASK 		= 0x100;
	const U8 BEAM_MASK 		= 0x200;
	//const U8 DEAD_MASK = 0x80000000; // DON'T FUCKING USE THIS
};

class ParticleSystem
{
public:
	unsigned int	ParticleFlags;	// mPartData.mFlags
	float		ParticleMaxAge;	// mPartData.mMaxAge
	LuaVector4 	StartColor;	// mPartData.mStartColor
	LuaVector4 	EndColor;	// mPartData.mEndColor
	LuaVector2 	StartScale;	// mPartData.mStartScale
	LuaVector2 	EndScale;	// mPartData.mEndScale
	LuaVector3 	PosOffset;	// mPartData.mPosOffset
	float 		Parameter;	// mPartData.mParameter

	unsigned int 	SystemFlags;
	U8 		Pattern;
	float	 	InnerAngle;
	float	 	OuterAngle;
	LuaVector3 	AngularVelocity;
	float 		BurstRate;
	U8 		BurstPartCount;
	float 		BurstRadius;
	float 		BurstSpeedMin;
	float 		BurstSpeedMax;
	float 		MaxAge;
	
	float 		StartAge;
	LuaVector3 	PartAccel;

	ParticleSystem():
		ParticleFlags(0),
		MaxAge(0.f),
		Parameter(0.f)
	{
		SystemFlags 	= 0;
		Flags 		= 0;
		StartColor 	= LuaVector4(1.f, 1.f, 1.f, 1.f);
		EndColor 	= LuaVector4(1.f, 1.f, 1.f, 1.f);
		StartScale 	= LuaVector2(1.f, 1.f);
		EndScale 	= LuaVector2(1.f, 1.f);
		ParticleMaxAge 	= 10.f;
		SystemMaxAge 	= 0.f;
		StartAge = 0.f;
		Pattern = LL_PART_SRC_PATTERN_DROP;                    // Pattern for particle velocity
		InnerAngle = 0.f;                                                              // Inner angle of PATTERN_ANGLE_*
		OuterAngle = 0.f;                                                              // Outer angle of PATTERN_ANGLE_*
		BurstRate = 0.1f;                                                              // How often to do a burst of particles
		BurstPartCount = 1;                                                    // How many particles in a burst
		BurstSpeedMin = 1.f;                                           // Minimum particle velocity
		BurstSpeedMax = 1.f;                                           // Maximum particle velocity
		BurstRadius = 0.f;
	}
	
	void 		SetImageFromFile(std::string Filename){
		LLViewerImage* cloud = gImageList.getImageFromFile(FileName);
		mPartImageID                 = cloud->getID();
	}

	void 		SetImageFromUUID(std::string UUID)
	{
		mPartImageID=LLUUID(UUID);
	}
	std::string 	GetImageUUID() { return mPartImageID.asString();}

	std::string 	GetTargetUUID(){ return mTargetID.asString();}
	void		SetTargetUUID(std::string target){ mTargetID=LLUUID(target);}

	void AttachToObject(std::string UUID);

	ParticleSystem&	operator=(const LLPartSysData& psys)
	{
		ParticleSystem lpsys;

		lpsys.ParticleFlag	= psys.mPartData.mFlags;
		lpsys.ParticleMaxAge	= psys.mPartData.mMaxAge;
		lpsys.StartColor	= psys.mPartData.mStartColor;
		lpsys.EndColor		= psys.mPartData.mEndColor;
		lpsys.StartScale	= psys.mPartData.mStartScale;
		lpsys.EndScale		= psys.mPartData.mEndScale;
		lpsys.PosOffset		= psys.mPartData.mPosOffset;
		lpsys.Parameter		= psys.mPartData.mParameter;
		
		lpsys.SystemFlags	= psys.mFlags;
		lpsys.Pattern		= psys.mPattern;
		lpsys.InnerAngle	= psys.mInnerAngle;
		lpsys.OuterAngle	= psys.mOuterAngle;
		lpsys.AngularVelocity	= psys.mAngularVelocity;
		lpsys.BurstRate		= psys.mBurstRate;
		lpsys.BurstPartCount	= psys.mBurstPartCount;
		lpsys.BurstRadius	= psys.mBurstRadius;
		lpsys.BurstSpeedMin	= psys.mBurstSpeedMin;
		lpsys.BurstSpeedMax	= psys.mBurstSpeedMax;
		lpsys.MaxAge		= psys.mMaxAge;
		lpsys.StartAge		= psys.mStartAge;
		lpsys.PartAccel		= psys.mPartAccel;

		lpsys.SetImageUUID(psys.mPartImageID);
		lpsys.SetTargetUUID(psys.mTargetID);

		return lpsys;
	}
	
	operator LLPartSysData()
	{
		LLPartSysData psys;
		
		psys.mPartData.mFlags		= ParticleFlags
		psys.mPartData.mMaxAge		= ParticleMaxAge;
		psys.mPartData.mStartColor	= (LLColor4)StartColor;
		psys.mPartData.mEndColor	= (LLColor4)EndColor;
		psys.mPartData.mStartScale	= (LLVector3)StartScale;
		psys.mPartData.mEndScale	= (LLVector3)EndScale;
		psys.mPartData.mPosOffset	= (LLVector3)PosOffset;
		psys.mPartData.mParameter	= Parameter;

		psys.mFlags			= SystemFlags;
		psys.mPattern			= Pattern;
		psys.mInnerAngle		= InnerAngle;
		psys.mOuterAngle		= OuterAngle;
		psys.mAngularVelocity		= AngularVelocity;
		psys.mBurstRate			= BurstRate;
		psys.mBurstPartCount		= BurstCount;
		psys.mBurstRadius		= BurstRadius;
		psys.mBurstSpeedMin		= BurstSpeedMin;
		psys.mBurstSpeedMax		= BurstSpeedMax;
		psys.mMaxAge			= MaxAge;
		psys.mStartAge			= StartAge;
		psys.mPartAccel;		= (LLVector3)PartAccel;

		psys.mPartImageID		= mPartImageID;
		psys.mTargetID			= mTargetID;
		return psys;
	}
private:
	LLUUID 		mPartImageID;
	LLUUID		mTargetID;
};

inline void ParticleSystem::AttachToObject(std::string ObjUUID,std::string OwnerUUID)
{
	LLUUID objid(ObjUUID);
	LLUUID ownid(OwnerUUID);
	LLViewerObject *o=gObjectList.findObject(objid);
	if(o==NULL)
	{
		return;
	}
	o->setParticleSource((LLPartSysData)this,ownid);
}
#endif

