#ifndef LUAPARTICLES_H
#define LUAPARTICLES_H

#include "llviewerimagelist.h"
#include "llviewerobject.h"
#include "llviewerobjectlist.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"
#include "LunaLua.h"

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

inline LLColor4 Vector42LLColor4(const LLVector4& a)
{
	LLColor4 b;
	b.mV[VX]=a.mV[VX];
	b.mV[VY]=a.mV[VY];
	b.mV[VZ]=a.mV[VZ];
	b.mV[VW]=a.mV[VW];
	return b;
}
inline LLVector4 LLColor42Vector4(const LLColor4& a)
{
	LLVector4 b;
	b.mV[VX]=a.mV[VX];
	b.mV[VY]=a.mV[VY];
	b.mV[VZ]=a.mV[VZ];
	b.mV[VW]=a.mV[VW];
	return b;
}


inline void ParticleSystem_AttachToObject_Event(LLPartSysData &psys,LLUUID &objID,LLUUID &ownerID)
{
	LLViewerObject *o=gObjectList.findObject(objID);
	if(o==NULL)
	{
		return;
	}
	o->setParticleSource(psys,ownerID);
}

class ParticleSystem
{
public:
	enum ParticlePattern
	{
		DROP 		= 0x01, 
		EXPLODE 	= 0x02, 
		ANGLE 		= 0x04, 
		ANGLE_CONE 	= 0x08,
		CONE_EMPTY 	= 0x10
	};

	enum ParticleFlags
	{
	 	INTERP_COLOR_MASK	= 0x01,
		INTERP_SCALE_MASK	= 0x02,
		BOUNCE_MASK		= 0x04,
		WIND_MASK 		= 0x08,
		FOLLOW_SRC_MASK 	= 0x10,
		FOLLOW_VELOCITY_MASK	= 0x20,
		TARGET_POS_MASK 	= 0x40,
		TARGET_LINEAR_MASK 	= 0x80,
		EMISSIVE_MASK 		= 0x100,
		BEAM_MASK 		= 0x200
		//DEAD_MASK = 0x80000000; // DON'T FUCKING USE THIS
	};

	unsigned int	ParticleFlags;	// mPartData.mFlags
	float		ParticleMaxAge;	// mPartData.mMaxAge
	LLVector4 	StartColor;	// mPartData.mStartColor
	LLVector4 	EndColor;	// mPartData.mEndColor
	LLVector2 	StartScale;	// mPartData.mStartScale
	LLVector2 	EndScale;	// mPartData.mEndScale
	LLVector3 	PosOffset;	// mPartData.mPosOffset
	float 		Parameter;	// mPartData.mParameter

	unsigned int 	SystemFlags;
	int		Pattern;
	float	 	InnerAngle;
	float	 	OuterAngle;
	LLVector3 	AngularVelocity;
	float 		BurstRate;
	int		BurstPartCount;
	float 		BurstRadius;
	float 		BurstSpeedMin;
	float 		BurstSpeedMax;
	float 		SystemMaxAge;
	
	float 		StartAge;
	LLVector3 	PartAccel;

	ParticleSystem():
		ParticleFlags(0),
		SystemMaxAge(0.f),
		Parameter(0.f)
	{
		SystemFlags 	= 0;
		ParticleFlags	= 0;
		StartColor 	= LLVector4(1.f, 1.f, 1.f, 1.f);
		EndColor 	= LLVector4(1.f, 1.f, 1.f, 1.f);
		StartScale 	= LLVector2(1.f, 1.f);
		EndScale 	= LLVector2(1.f, 1.f);
		ParticleMaxAge 	= 10.f;
		SystemMaxAge 	= 0.f;
		StartAge 	= 0.f;
		Pattern 	= DROP;               // Pattern for particle velocity
		InnerAngle = 0.f;                                              // Inner angle of PATTERN_ANGLE_*
		OuterAngle = 0.f;                                              // Outer angle of PATTERN_ANGLE_*
		BurstRate = 0.1f;                                              // How often to do a burst of particles
		BurstPartCount = 1;                                            // How many particles in a burst
		BurstSpeedMin = 1.f;                                           // Minimum particle velocity
		BurstSpeedMax = 1.f;                                           // Maximum particle velocity
		BurstRadius = 0.f;
	}

	ParticleSystem(std::string in):
		ParticleFlags(0),
		SystemMaxAge(0.f),
		Parameter(0.f)
	{
		SystemFlags 	= 0;
		ParticleFlags	= 0;
		StartColor 	= LLVector4(1.f, 1.f, 1.f, 1.f);
		EndColor 	= LLVector4(1.f, 1.f, 1.f, 1.f);
		StartScale 	= LLVector2(1.f, 1.f);
		EndScale 	= LLVector2(1.f, 1.f);
		ParticleMaxAge 	= 10.f;
		SystemMaxAge 	= 0.f;
		StartAge 	= 0.f;
		Pattern 	= DROP;               // Pattern for particle velocity
		InnerAngle = 0.f;                                              // Inner angle of PATTERN_ANGLE_*
		OuterAngle = 0.f;                                              // Outer angle of PATTERN_ANGLE_*
		BurstRate = 0.1f;                                              // How often to do a burst of particles
		BurstPartCount = 1;                                            // How many particles in a burst
		BurstSpeedMin = 1.f;                                           // Minimum particle velocity
		BurstSpeedMax = 1.f;                                           // Maximum particle velocity
		BurstRadius = 0.f;
	}
	
	void SetImageFromFile(std::string FileName)
	{
		LLViewerImage* cloud = gImageList.getImageFromFile(FileName);
		mPartImageID = cloud->getID();
	}

	void SetImageFromUUID(std::string UUID)
	{
		mPartImageID=LLUUID(UUID);
	}
	std::string 	GetImageUUID() { return mPartImageID.asString();}

	std::string 	GetTargetUUID(){ return mTargetID.asString();}
	void		SetTargetUUID(std::string target){ mTargetID=LLUUID(target);}

	void AttachToObject(LLUUID ObjectUUID,LLUUID OwnerUUID);

	void AddFlag(int flag)
	{
		ParticleFlags|=flag;
	}

	void RemoveFlag(int flag)
	{
		ParticleFlags&=~flag;
	}

	const ParticleSystem&	operator=(const LLPartSysData& psys)
	{
		ParticleFlags	= (int)psys.mPartData.mFlags;
		ParticleMaxAge	= psys.mPartData.mMaxAge;
		StartColor	= LLColor42Vector4(psys.mPartData.mStartColor);
		EndColor	= LLColor42Vector4(psys.mPartData.mEndColor);
		StartScale	= psys.mPartData.mStartScale;
		EndScale	= psys.mPartData.mEndScale;
		PosOffset	= psys.mPartData.mPosOffset;
		Parameter	= psys.mPartData.mParameter;
		
		SystemFlags	= (int)psys.mFlags;
		Pattern		= (int)psys.mPattern;
		InnerAngle	= psys.mInnerAngle;
		OuterAngle	= psys.mOuterAngle;
		AngularVelocity	= psys.mAngularVelocity;
		BurstRate	= psys.mBurstRate;
		BurstPartCount	= psys.mBurstPartCount;
		BurstRadius	= psys.mBurstRadius;
		BurstSpeedMin	= psys.mBurstSpeedMin;
		BurstSpeedMax	= psys.mBurstSpeedMax;
		SystemMaxAge	= psys.mMaxAge;
		StartAge	= psys.mStartAge;
		PartAccel	= psys.mPartAccel;

		SetImageFromUUID(psys.mPartImageID.asString());
		SetTargetUUID(psys.mTargetUUID.asString());

		return *this;
	}
	
	const LLPartSysData asParticleSystem()
	{
		LLPartSysData psys;
		
		psys.mPartData.mFlags		= (U8)ParticleFlags;
		psys.mPartData.mMaxAge		= ParticleMaxAge;
		psys.mPartData.mStartColor	= Vector42LLColor4(StartColor);
		psys.mPartData.mEndColor	= Vector42LLColor4(EndColor);
		psys.mPartData.mStartScale	= StartScale;
		psys.mPartData.mEndScale	= EndScale;
		psys.mPartData.mPosOffset	= PosOffset;
		psys.mPartData.mParameter	= Parameter;

		psys.mFlags			= (U8)SystemFlags;
		psys.mPattern			= (U8)Pattern;
		psys.mInnerAngle		= InnerAngle;
		psys.mOuterAngle		= OuterAngle;
		psys.mAngularVelocity		= AngularVelocity;
		psys.mBurstRate			= BurstRate;
		psys.mBurstPartCount		= BurstPartCount;
		psys.mBurstRadius		= BurstRadius;
		psys.mBurstSpeedMin		= BurstSpeedMin;
		psys.mBurstSpeedMax		= BurstSpeedMax;
		psys.mMaxAge			= SystemMaxAge;
		psys.mStartAge			= StartAge;
		psys.mPartAccel			= PartAccel;

		psys.mPartImageID		= mPartImageID;
		psys.mTargetUUID		= mTargetID;

		return psys;
	}

	operator LLPartSysData() { return asParticleSystem(); }
private:
	LLUUID 		mPartImageID;
	LLUUID		mTargetID;
};

inline void ParticleSystem::AttachToObject(LLUUID ObjUUID,LLUUID OwnerUUID)
{
	CB_Args3<LLPartSysData,LLUUID,LLUUID>(&ParticleSystem_AttachToObject_Event,asParticleSystem(),ObjUUID,OwnerUUID);
}

#ifdef SWIG
%ignore ClearParticlesFromObject_Event(std::string,std::string);
%ignore ParticleSystem_AttachToObject_Event(LLPartSysData,LLUUID,LLUUID);
#endif

inline void ClearParticlesFromObject_Event(std::string &ObjUUID,std::string &OwnerUUID)
{
	LLUUID objid(ObjUUID);
	LLUUID ownid(OwnerUUID);
	LLViewerObject *o=gObjectList.findObject(objid);
	if(o==NULL)
	{
		return;
	}
	o->deleteParticleSource();
}

inline void ClearParticlesFromObject(std::string ObjUUID,std::string OwnerUUID)
{
	CB_Args2<std::string,std::string>(&ClearParticlesFromObject_Event,ObjUUID,OwnerUUID);
}




#endif
