--[[
	Luna Automatic Mute Plugin
		by N3X15
	
	Copyright (C) 2008-2009 Luna Contributors

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	 
	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 
	$Id$
]]--

gAutoMuted={}

local PluginName="AutoMute";

function isInTable(items,snd)
	for _,v in pairs(items) do
		if v == snd then
			return true
		end
	end
	return false
end
	

local AMCheckSoundTrigger = function (sound_id, owner_id, gain, object_id, parent_id)

	-- Collision sounds;  Not important.
	if 	sound_id=="be582e5d-b123-41a2-a150-454c39e961c8" or 
		sound_id=="dce5fdd4-afe4-4ea1-822f-dd52cac46b08" or 
		sound_id=="cbe75eb2-3375-41d8-9e3f-2ae46b4164ed" or 
		sound_id=="063c97d3-033a-4e9b-98d8-05c8074922cb" then return end

	if(sound_id=="3d09f582-3851-c0e0-f5ba-277ac5c73fb4") then
		--error(key2name(owner_id).." took a snapshot.")
	elseif isInTable(gAutoMuteSounds,tostring(sound_id))==true then
		if gAutoMuted[owner_id] then return end
		gAutoMuted[owner_id]=true
		if muteAvatar(owner_id) then
			error("AutoMute has automatically muted "..key2name(owner_id).." for playing a sound on Luna's automute list ("..sound_id..").")
		end
	else
		--print(key2name(owner_id).." is playing "..sound_id.."")
	end
end

--getID() << audio_uuid << owner_id << gain << flags
local AMCheckAttachedSound = function (object_id,audio_uuid,owner_id,gain,flags)
	if owner_id=="00000000-0000-0000-0000-000000000000" then return end

	if isInTable(gAutoMuteSounds,tostring(audio_uuid)) then
		if gAutoMuted[owner_id] then return end
		gAutoMuted[owner_id]=true
		if muteAvatar(owner_id) then
			error("AutoMute has automatically muted "..key2name(owner_id).." for playing a sound on Luna's automute list ("..audio_uuid..").")
		end
	else
		--print("[OBJECT] "..key2name(owner_id).." is playing "..audio_uuid)
	end
end

local AMCheckAttachedParticles = function(object_id,owner_id,texture_id,particle_data)
	-- Deserialize particle data
	psys = parseParticleSystem(particle_data)

	texture_id=psys:GetImageUUID()

	if owner_id=="00000000-0000-0000-0000-000000000000" or texture_id==UUID_null then return end
	
	--print("PARTICLE:",key2name(owner_id),texture_id,particle_data)
	if isInTable(gAutoMuteTextures,tostring(texture_id))==true then
		error("Muting particlesystem in object "..object_id.." owned by "..key2name(owner_id).." for using the texture "..texture_id..".")
		ClearParticlesFromObject(object_id,owner_id)
	end	
end

local AMCheckSetText = function(text,object_id)
	if(object_id=="00000000-0000-0000-0000-000000000000" or text=="") then return end
	
	if isInTable(gAutoMuteText,text)==true then
		if isInTable(gAutoMuted,owner_id) then return end
		if muteAvatar(owner_id) then
			error("AutoMute has automatically muted "..key2name(owner_id).." for playing a sound on Luna's automute list.")
		end
	end	
end

gAutoMuteSounds={}
gAutoMuteTextures={}

LunaLoadDir("lua/Hooks/"..PluginName.."/DB/",true)

print("[AutoMute] Loaded "..tostring(#gAutoMuteSounds).." sounds for automuting.")
print("[AutoMute] Loaded "..tostring(#gAutoMuteTextures).." textures for automuting.")

SetHook("OnSoundTriggered",	AMCheckSoundTrigger)
SetHook("OnAttachedSound",	AMCheckAttachedSound)
SetHook("OnAttachedParticles",	AMCheckAttachedParticles)
--SetHook("OnSetText",		AMCheckSetText)
