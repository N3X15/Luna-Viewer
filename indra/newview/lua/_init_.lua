--[[
	Luna Lua Loader
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
 
	$Id$
	
	*** DO NOT MODIFY UNLESS YOU KNOW WHAT THE FUCK YOU'RE DOING.  ***
]]--

-- dirtree()  Code by David Kastrup

require "lfs" -- Lua Filesystem API
require "bit" -- Bitwise operators

----------------------------------------------------------------------------------
--               IMPORTANT FUNCTIONS NEEDED TO LOAD LIBS, ETC                   --
----------------------------------------------------------------------------------


-- Locate files in directory
function dirtree(dir)
	if(dir and dir == "") then
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

function table.flip(t)
	r={}
	for v,k in ipairs(t) do
		r[k]=v
	end
	return r
end

-- String functions
function string.starts(String,Start)
   return string.sub(String,1,string.len(Start))==Start
end

function string.ends(String,End)
   return End=='' or string.sub(String,-string.len(End))==End
end

function trim (s)
	return (string.gsub(s, "^%s*(.-)%s*$", "%1"))
end


-- Filesystem stuff
function io.getDataDir()
	return getLuaFolder().."/data/"..getMyID()
end

function io.openDataFile(file,mode)
	return io.open(io.getDataDir().."/"..file,mode)
end

-- return a new array containing the concatenation of all of its 
-- parameters. Scaler parameters are included in place, and array 
-- parameters have their values shallow-copied to the final array.
-- Note that userdata and function values are treated as scalar.
--[[
function array_concat(...) 
    local t = {}
    for arg = 1,select("#",...) do
        if type(arg)=="table" then
            for _,v in ipairs(arg) do
                t[#t+1] = v
            end
        else
            t[#t+1] = arg
        end
    end
    return t
end
]]--

function array_concat(a,b)
	for k,v in pairs(a) do b[#b+1] = v end
	return b;
end
--Easily check contents of _G and SL
function DumpTable(tbl)
	for n,v in pairs(tbl) do
		print(n..": "..tostring(v))
	end
end


-- Load scripts in directory.
function LunaLoadDir(dir,silent)
	local dirlist = dirtree(dir)
	if(dirlist==nil) then
		error ("Dirlist = null ("..dir..")")
		return
	end
	for ent in dirlist do
		-- _init_.lua
		local ext=string.sub(ent,-10)
		--print(ext)
		if ext=="_init_.lua" then
			local hookname=string.sub(ent,string.len(dir)+1,-12)
			if not string.find(hookname,"/") then 
				if not silent then
					print(" * Loading plugin: "..hookname)
				end
				dofile(ent)
			end
		end
	end
end

----------------------------------------------------------------------------------
--                                 Load libs                                    --
----------------------------------------------------------------------------------

--LunaLoadDir(getCWD().."lib/") -- Luna Libraries

-- Copy SL functions into the global table
--  This is so one doesn't have to use SL.print to print to the window.
for k,v in pairs(SL) do
	-- Do not overwrite string table!
	if (k~="string") then
		_G[k]=v 
	end
end

dofile("lua/EventHandling.lua")
dofile("lua/HookDocs.lua")

LunaLoadDir("lua/lib/",true)  -- User Libraries
LunaLoadDir("lua/Hooks/",false) -- User Hooks

--DumpAllHooks();

local function SetUpUserdir(name, is_godlike)
	lfs.mkdir(io.getDataDir())
end

SetHook("OnAgentInit",SetUpUserdir)

-- Call OnAgentInit, just in case
if (getMyID()~=UUID_null) then
	CallHook("OnAgentInit",getMyName(),0)
end