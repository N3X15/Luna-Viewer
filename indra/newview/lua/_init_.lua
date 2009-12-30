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

----------------------------------------------------------------------------------
--                                 Load libs                                    --
----------------------------------------------------------------------------------

--flLoadDir(getCWD().."lib/") -- FlexLife Libraries
--flLoadDir(getCWD().."/lua/Hooks/") -- User Hooks

-- Copy SL functions into the global table
--  This is so one doesn't have to use SL.print to print to the window.
for k,v in pairs(SL) do _G[k]=v end

dofile("lua/EventHandling.lua")

dofile("lua/Hooks/HelloWorld/_init_.lua")
dofile("lua/Hooks/InitLuaAgent/_init_.lua")
dofile("lua/Hooks/LogAvs/_init_.lua")

DumpAllHooks();
