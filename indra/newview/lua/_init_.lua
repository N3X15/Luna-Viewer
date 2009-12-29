--[[
	FlexLife Lua Loader
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
	
	*** DO NOT MODIFY UNLESS YOU KNOW WHAT THE FUCK YOU'RE DOING.  ***
]]--

-- dirtree()  Code by David Kastrup

require "lfs"
----------------------------------------------------------------------------------
--               IMPORTANT FUNCTIONS NEEDED TO LOAD LIBS, ETC                   --
----------------------------------------------------------------------------------

-- Load scripts in directory.
function flLoadDir(dir)
	local dirlist = dirtree(dir)
	if(dirlist==nil) then
		error ("Dirlist = null ("..dir..")")
		return
	end
	for ent in dirlist do
		if(string.ends(ent,".lua")) then
			print("Loading "..ent)
			dofile(ent)
		end
	end
end

-- Locate files in directory
function dirtree(dir)
	if(dir and dir ~= "") then
		error("directory parameter is missing or empty")
		return
	end
	
	if string.sub(dir, -1) == "/" then
		dir=string.sub(dir, 1, -2)
	end

	local function yieldtree(dir)
		for entry in lfs.dir(dir) do
			if entry ~= "." and entry ~= ".." then
				entry=dir.."/"..entry
				local attr=lfs.attributes(entry)
				coroutine.yield(entry,attr)
				if attr.mode == "directory" then
				  yieldtree(entry)
				end
			end
	    end
	end

	return coroutine.wrap(function() yieldtree(dir) end)
end

-- String functions
function string.starts(String,Start)
   return string.sub(String,1,string.len(Start))==Start
end

function string.ends(String,End)
   return End=='' or string.sub(String,-string.len(End))==End
end

--Easily check contents of _G and SL
function DumpTable(tbl)
	for n,v in pairs(tbl) do
		print (n,v)
	end
end

--Override print.
-- This will always print to chat, which allows the Lua subsystem to give the 
--   user a heads-up immediately, instead of waiting for them to dick with a
--   seperate window.  Eventually, I'd like to place hooks in the settings 
--   system so we can allow the user to specify if they'd like to use another
--   window, but this will suffice for now, and it conforms with the original
--   ShoopedLife implementation (users hate change). -- n3x15
function print(str) 
	SL.print(str)
end

--Easily check contents of _G and SL
function DumpTable(tbl)
      for n,v in pairs(tbl) do
              print (n,v)
      end
end

--Override print.
function print(str)
      --Need to decide if this goes to chat or if we need to
      --create a seprate function for that.
     SL.print(str) --Always prints to chat. Do we want this?
end

----------------------------------------------------------------------------------
--                                 Load libs                                    --
----------------------------------------------------------------------------------

--flLoadDir(getCWD().."lib/") -- FlexLife Libraries
--flLoadDir(getCWD().."/lua/Hooks/") -- User Hooks

dofile("lua/EventHandling.lua")

dofile("lua/Hooks/HelloWorld/_init_.lua")
dofile("lua/Hooks/InitLuaAgent/_init_.lua")
dofile("lua/Hooks/LogAvs/_init_.lua")

DumpAllHooks();
