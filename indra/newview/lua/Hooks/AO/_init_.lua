--[[
	Luna Lua AO Plugin
		by N3X15

	Copyright (C) 2008-2010 Luna Contributors

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
]]--

AO={}

-- For opening notecards
AO.Parsers={}

-- State={Replacements},
AO.AnimationOverrides={}

-- Anim=IDCache
AO.AnimUUIDs={}

-- Current Animations
AO.CurrentAnimation=UUID_nil
AO.CurrentState=""
AO.Disabled=false
AO.Debug=false -- TURN ON DEBUG MODE (/lua AO.Debug=true)

--OnAnimStart(avid,id,time_offset) For AOs.
function AO:OnAnimStart(av_id,id,time_offset)

	-- Ensure that we're only receiving AnimationStarts that come from our own avatar.
	if tostring(av_id) ~= tostring(getMyID()) then return end
	
	-- Also make sure that the AO is actually enabled.
	if self.Disabled==true then return end
	
	
	-- If it's a known viewer-generated animation, ignore it.
	if GeneratedAnims[id] ~= nil then
		return
	end
	
	-- Just in case.
	id=tostring(id)
	
	-- this is all for debugging.
	name=AnimationNames[id]
	if (name==nil) then
		name = "???"
	end
	
	st8=AnimationStates[id]
	if(st8==nil) then
		st8="???"
	end
	
	self:DebugInfo("Animation playing: "..id.." ("..name.." @ state "..st8..")")
	
	-- If the animation changes state of the avatar, 
	if name~="???" and AnimationStates[id] ~= nil and self.CurrentAnimation ~= id then
		state=AnimationStates[id]
		
		-- Debugging shit
		if self.CurrentState ~= state then
			self:DebugInfo("State changed to "..state)
			self.CurrentState=state
		end
		
		-- Check if the state has overrides assigned.  If not, abort and let the defaults play.
		if (self.AnimationOverrides==nil or self.AnimationOverrides[state]==nil) then return end
		
		-- Fetch the number and collection of overrides.
		numreplacements=table.getn(self.AnimationOverrides[state])
		replacements=self.AnimationOverrides[state]
		
		-- If we're already playing an override animation, stop it.
		if self.CurrentAnimation ~= UUID_nil or self.CurrentAnimation ~= nil then
			self:DebugInfo("Stopping AO motion "..self.CurrentAnimation)
			self.CurrentAnimation=self:Name2Key(self.CurrentAnimation)
			stopAnimation(getMyID(),self.CurrentAnimation)
		end
		
		-- If there are overrides assigned for this state...
		if numreplacements > 0 then
		
			-- Get a random one
			self.CurrentAnimation=replacements[math.random(1,numreplacements)]
			self:DebugInfo("Playing motion "..self.CurrentAnimation)
			
			self.CurrentAnimation=self:Name2Key(self.CurrentAnimation)
			
			-- Stop the animation being overridden
			--self:DebugInfo("Stopping natural motion "..id)
			--stopAnimation(getMyID(), id)
			-- And play the override.
			startAnimation(getMyID(), self.CurrentAnimation)
		end
	end
end

function AO:OnAnimStop(av_id,id)
-- Ensure that we're only receiving AnimationStarts that come from our own avatar.
	if tostring(av_id) ~= tostring(getMyID()) then return end
	
	-- Also make sure that the AO is actually enabled.
	if self.Disabled==true then return end
	
	-- If nothing's loaded, see if cached.lua exists
	-- Also set our cache folder
	if self.AnimationOverrides == {} then
		lfs.mkdir(getDataDir().."/AO/")
		self.CacheFile=getDataDir().."/AO/cached.lua"
		self:Load()
	end
	
	-- If it's a known viewer-generated animation, ignore it.
	if GeneratedAnims[id] ~= nil then
		return
	end
	
	-- Just in case.
	id=tostring(id)
	
	-- this is all for debugging.
	name=AnimationNames[id]
	if (name==nil) then
		name = "???"
	end
	
	st8=AnimationStates[id]
	if(st8==nil) then
		st8="???"
	end
	--self:DebugInfo("Animation playing: "..id.." ("..name.." @ state "..st8..")")
	
	-- If the animation changes state of the avatar, 
	if name~="???" and self.CurrentAnimation ~= nil and AnimationStates[id] ~= nil and self.CurrentAnimation ~= id then
		state=AnimationStates[id]
		
		if st8 == "Walking" then
			stopAnimation(getMyID(),self.CurrentAnimation)
			
			numreplacements=table.getn(self.AnimationOverrides["Standing"])
			replacements=self.AnimationOverrides["Standing"]
			if numreplacements > 0 then
			
				-- Get a random one
				self.CurrentAnimation=replacements[math.random(1,numreplacements)]
				self:DebugInfo("Playing motion "..self.CurrentAnimation)
				
				self.CurrentAnimation=self:Name2Key(self.CurrentAnimation)
				
				-- Stop the animation being overridden
				--self:DebugInfo("Stopping natural motion "..id)
				--stopAnimation(getMyID(), id)
				-- And play the override.
				startAnimation(getMyID(), self.CurrentAnimation)
			end
		end
	end
end

function AO:Name2Key(anim)
	-- If we're not looking at a UUID, it's probably a name. (God help us if they saved it as a UUID)
	if not UUID_validate(anim) then
		--print("Name2Key("..anim..")=false")
		-- Get the UUID associated with this name, if possible.
		if (self.AnimUUIDs[anim]~=nil) then
			-- It's cached, no need to search through inventory.
			return self.AnimUUIDs[anim]
		end
		
		anim_name=anim
		anim=getInventoryAssetUUID(anim,AssetType.ANIMATION)
		
		-- If not possible, ABORT ABORT
		if anim == UUID_null then
			error("Could not find UUID for item "..anim_name)
			return
		end
		
		self.AnimUUIDs[anim_name]=anim
		
		-- Debugging
		print("[AO] "..anim_name.." -> "..anim)
		return anim
	end
	--print("Name2Key("..anim..")=true")
	return anim
end

function AO:ClearOverrides()
	self.AnimationOverrides={}
end

function AO:AddOverride(state,anim)
	if UUID_validate(state) then
		-- What the hell are you doing
		state=AnimationStates[state]
	end
	
	-- Standardize names
	if state=="Stands" then
		state="Standing"
	end
	
	-- If there's no category in the overrides table for the desired state, add it.
	if self.AnimationOverrides == nil then
		self.AnimationOverrides={}
	end
	
	if self.AnimationOverrides[state] == nil then
		self.AnimationOverrides[state]={}
	end
	
	-- If we're not looking at a UUID, it's probably a name. (God help us if they saved it as a UUID)
	if not UUID_validate(anim) then
		-- Get the UUID associated with this name, if possible.
		anim_name=anim
		anim=getInventoryAssetUUID(anim,AssetType.ANIMATION)
		-- If not possible, ABORT ABORT
		if anim == UUID_null then return end
		
		-- Debugging
		--print("[AO] "..anim_name.." -> ["..state.."]="..anim)
	end
	-- Add animation override to the overrides table in the desired State category.
	i=table.getn(self.AnimationOverrides[state])+1
	self.AnimationOverrides[state][i]=anim
	
	-- SAVE.
	self:Save()
end

function AO:Save()
	if getMyID() ~= UUID_null and self.CacheFile ~= nil then
		data=io.openDataFile("/AO.lua","w")
		data:write("return "..table.serialize(self.AnimationOverrides))
		data:close()
	end
end

function AO:Load()
	if getMyID() ~= UUID_null and self.CacheFile ~= nil then
		if not exists(io.getDataDir().."/AO.lua") then return end
		tmptable=dofile(io.getDataDir().."/AO.lua")
		if err~=nil then
			error("[AO] Failed to load cache: "..err)
		else
			self.AnimationOverrides=tmptable
		end
	end
end

function AO:DebugInfo(msg)
	if self.Debug==true then
		print("[AO]: "..msg)
	end
end

-- Detect what kind of notecard we're dealing with.
function AO:DetectNotecardType(data)
	for n,p in pairs(self.Parsers) do
		if p:CanParse(data) then
			print("[AO] Parsing "..n.." notecard...")
			p:Parse(data)
			return
		end
	end
	error("Cannot read the specified notecard.")
end

------------------------------------------------------------------------------------------------------------------
--                                             EVENT HANDLING                                                   --
------------------------------------------------------------------------------------------------------------------
local function AO_OnAONotecard(id)
	-- Download the notecard and decode it
	AO.NotecardRequestKey=requestInventoryAsset(id,UUID_null)
	print ("[AO] Downloading notecard. (reqid: "..AO.NotecardRequestKey..")")
end

local function AO_OnAssetFailed(transfer_key)
	if AO.NotecardRequestKey == transfer_key then
		AO.NotecardRequestKey=nil
		error("Failed to download the notecard.  Please try again later.  You can also try from a different sim.")
	end
end

local function AO_OnAssetDownloaded(transfer_key,typ,data)
	if AO.NotecardRequestKey == transfer_key then
		f = io.open("_NotecardDump.txt","w")
		f:write(data)
		f:close()
		AO.NotecardRequestKey=nil
		print("[AO] Notecard successfully downloaded! Attempting to parse...")
		AO:DetectNotecardType(data)
	end
end

local function AO_OnAnimStart(av_id,id,time_offset)
	AO:OnAnimStart(av_id,id,time_offset)
end

local function AO_OnAnimStop(av_id,id)
	AO:OnAnimStop(av_id,id)
end

local function AO_OnAgentInit(name,godmode)
	lfs.mkdir(io.getDataDir().."/AO/")
	AO.CacheFile=io.getDataDir().."/AO/cached.lua"
	AO:Load()
end

SetHook("OnAnimStart",		AO_OnAnimStart)
SetHook("OnAnimStop",		AO_OnAnimStop)
SetHook("OnAONotecard",		AO_OnAONotecard)
SetHook("OnAssetDownloaded",AO_OnAssetDownloaded)
SetHook("OnAssetFailed",	AO_OnAssetFailed)
SetHook("OnAgentInit",	AO_OnAgentInit)

dofile("lua/Hooks/AO/ZHAOParser.lua")