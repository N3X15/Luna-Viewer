function pixelfart(id,name,region_id)
	particle_parameters = ParticleSystem()

	-- fancy particle cloud designed by Brent
	particle_parameters.ParticleMaxAge = 4.0
	particle_parameters.StartScale.x = 0.8
	particle_parameters.StartScale.y = 1.0
	particle_parameters.EndScale.x   = 0.02
	particle_parameters.EndScale.y   = 0.02
	particle_parameters.StartColor = LLVector4(1.0, 1.0, 1.0, 0.5)
	particle_parameters.EndColor = LLVector4(1.0, 1.0, 1.0, 0.0)
	particle_parameters:SetImageFromUUID("6e1a3980-bf2d-4274-8970-91e60d85fb52")
	particle_parameters.MaxAge = 0.0
	particle_parameters.Pattern = ParticleSystem_ANGLE_CONE
	particle_parameters.InnerAngle = 3.14159
	particle_parameters.OuterAngle = 0.0
	particle_parameters.BurstRate = 0.02
	particle_parameters.BurstRadius = 0.0
	particle_parameters.BurstPartCount = 1
	particle_parameters.BurstSpeedMin = 0.1
	particle_parameters.BurstSpeedMax = 1.0
	particle_parameters.ParticleFlags = bit.bor(
		ParticleSystem_INTERP_COLOR_MASK,
		ParticleSystem_INTERP_SCALE_MASK,
		ParticleSystem_EMISSIVE_MASK,
		--ParticleSystem_FOLLOW_SRC_MASK,
		ParticleSystem_TARGET_POS_MASK )

	particle_parameters:AttachToObject(id, getMyID())
end

SetHook("OnAvatarLoading",pixelfart)