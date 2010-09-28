--[[
	Ponger
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

print("[PING/PONG] Initializing hooks.")


--[02:49 AM]  RHSBot Head: &FLEXLIFE;PING^1284803368

local function LunaPonger(from_id, owner_id, mesg)
	local t = explode("^",mesg)
--	DumpTable(t)
	if (t[1]=="&FLEXLIFE;PING") then
		say("&FLEXLIFE;PONG^"..t[2],-50)
--		print("PING "..t[2].."... PONG!")
	end
end

SetHook("OnBridgeCommand",		LunaPonger)