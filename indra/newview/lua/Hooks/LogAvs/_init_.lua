--[[
	Echo Avs to Chat
		Generates log of avatars that have appeared in your viewer.
	 
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

gAvatarNames={};

LogAvs = function (uuid,fullname,title)
	if(gAvatarNames[uuid]==nil) then
		gAvatarNames[uuid]=fullname;
--		if(gJLUList[uuid]==nil) then
			print ("Avatar detected: "..fullname.." (LLUUID: "..uuid..")")
--		else
--			error ("JLU DETECTED: "..fullname.." (LLUUID: "..uuid..")")
--		end
	end
end

SetHook("OnAvatarDetected",LogAvs)
--SetHook("OnObjectCreated",LogObjects)
