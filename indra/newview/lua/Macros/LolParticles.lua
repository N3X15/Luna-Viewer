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
