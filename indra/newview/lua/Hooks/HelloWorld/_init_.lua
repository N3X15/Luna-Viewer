--[[
	Hello World Extension
		Displays Lua startup banner.
	 
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

local helloworld = function ()
	print (_VERSION.." on Luna ".._SLUA_VERSION.." loaded!")
	print "_________________________________________"
	print "Congratulations, all Lua scripts compiled and hooks are set correctly."
	print "Luna is now ready for use."
	print "If you need any assistance, please visit http://luna.nexisonline.net/"
	print "_________________________________________"
	return true
end;

SetHook("OnLuaInit",helloworld)
