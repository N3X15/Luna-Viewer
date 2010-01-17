
--[[
1: \nLLPartSysData				Header/Datatag	
2: 1						CRC		
3: 2						Flags		
4:  -- wtf are these (0x02?)			Pattern		
5: 1.5625					Inner angle	
6: 1.5625					Outer Angle	
7: { 90, 0, 0 }					AngularVelocity	
8: 0.0976562					Burst Rate
9:  -- 0x01?					Burst Particle Count
10: 0						Burst Radius
11: 0.5						Speed minimum
12: 0.5						Speed maximum
13: 0						Maximum lifetime of system
14: a25afbff-bc7e-956c-1ff1-a6c91091a414	Target UUID
15: 0						Starting age?
16: { 0, 0, 0 }					Acceleration
17: 73b852f7-be1b-5b62-e671-c5f97f77327e	Image UUID
18: 371						Particle flags
19: 2						Max age for each particle
20: { 1, 1, 1, 0.0509804 }			
21: { 1, 1, 1, 0.0509804 }
22: { 1.03125, 1.03125 }
23: { 0.5, 1.03125 }
24: { 0, 0, 0 }
25: 0

		os << "\nLLPartSysData"; //1
		os << ":" << mCRC;//2
		os << ":" << mFlags;//3
		os << ":" << (int)mPattern;//4
		os << ":" << mInnerAngle;					// Inner angle for PATTERN_ANGLE
		os << ":" << mOuterAngle;					// Outer angle for PATTERN_ANGLE
		os << ":" << mAngularVelocity;
		os << ":" << mBurstRate;					// How often to do a burst of particles
		os << ":" << (int)mBurstPartCount;					// How many particles in a burst
		os << ":" << mBurstRadius;
		os << ":" << mBurstSpeedMin;					// Minimum particle velocity
		os << ":" << mBurstSpeedMax;	
		os << ":" << mMaxAge;						// Maximum lifetime of this particle source
		os << ":" << mTargetUUID;					// Target UUID for the particle system
		os << ":" << mStartAge;
		os << ":" << mPartAccel;
		os << ":" << mPartImageID;

		os << ":" << (int)mPartData.mFlags;					// Particle state/interpolators in effect
		os << ":" << mPartData.mMaxAge;					// Maximum age of the particle
		os << ":" << mPartData.mStartColor;
		os << ":" << mPartData.mEndColor;
		os << ":" << mPartData.mStartScale;
		os << ":" << mPartData.mEndScale;
		os << ":" << mPartData.mPosOffset;
		os << ":" << mPartData.mParameter;				// A single floating point parameter	
]]--
function parseParticleSystem(serialized)
	if not string.starts(serialized,"\nLLPartSysData:") then
		error("Invalid LLPartSysData.")
--		return nil
	end

	local t = explode(":",serialized)

--	DumpTable(t)

	psys=ParticleSystem()
	psys.CRC		= tonumber(	t[2])
	psys.SystemFlags	= tonumber(	t[3])
	psys.Pattern		= tonumber(	t[4])
	psys.InnerAngle		= tonumber(	t[5])
	psys.OuterAngle		= tonumber(	t[6])
	psys.AngularVelocity 	= parseVector3(	t[7])
	psys.BurstRate		= tonumber(	t[8])
	psys.BurstPartCount	= tonumber(	t[9])
	psys.BurstRadius	= tonumber(	t[10])
	psys.BurstSpeedMin	= tonumber(	t[11])
	psys.BurstSpeedMax	= tonumber(	t[12])
	psys.SystemMaxAge	= tonumber(	t[13])
	psys:SetTargetUUID(	  		t[14])
	psys.StartAge		= tonumber(	t[15])
	psys.PartAccel		= parseVector3(	t[16])
	psys:SetImageFromUUID(			t[17])

	psys.ParticleFlags	= tonumber(	t[18])
	psys.ParticleMaxAge	= tonumber(	t[19])

	psys.StartColor		= parseVector4(	t[20])
	psys.EndColor		= parseVector4(	t[21])

	psys.StartScale		= parseVector2(	t[22])
	psys.EndScale		= parseVector2(	t[23])
			
	psys.PosOffset		= parseVector3(	t[24])
	psys.Parameter		= tonumber(	t[25])

	return psys
end
