--[[
	FlexLife Lua Automatic Mute
		by N3X15
	
	 Copyright (C) 2008-2009 FlexLife Contributors
	 
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

local PluginName="AutoMute";

dofile(getLuaFolder().."/Hooks/"..PluginName.."/AutoMuteSounds.lua")

local Check4Snapshot = function (sound_id, owner_id, gain, object_id, parent_id)
	local isInTable= function(items,sound)
		for _,v in pairs(items) do
  			if v == snd then
    				return true
  			end
  		end
		return false
	end
	if sound_id=="be582e5d-b123-41a2-a150-454c39e961c8" or sound_id=="dce5fdd4-afe4-4ea1-822f-dd52cac46b08" then return end
	

	if(sound_id=="3d09f582-3851-c0e0-f5ba-277ac5c73fb4") then
		error(key2name(owner_id).." took a snapshot.")
	elseif isInTable(gAutoMuteSounds,sound_id) and not isMuted(owner_id,key2name(owner_id)) then
		error(key2name(owner_id).." is playing really damn loud noises.")
		whisper("/me has automatically muted "..key2name(owner_id).." for playing a sound on FlexLife's automute list.")
		muteAvatar(owner_id)
	else
		print(key2name(owner_id).." is playing "..sound_id)
	end
end

SetHook("OnSoundTriggered",Check4Snapshot)
