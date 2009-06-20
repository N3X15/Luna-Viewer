--[[
	Hello World Extension
		Displays Lua startup banner.
	 
	 Copyright (C) 2008 Patriotic Nigras
	 
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
	print (_VERSION.." on ShoopedLife ".._SHL_VERSION.." loaded!")
	print "_________________________________________"
	print "Congratulations, all Lua scripts compiled and hooks are set correctly."
	print "ShoopedLife is now ready for use."
	print "If you need any assistance, please visit http://www.patrioticnigras.org/ or fag around on our IRC (irc.patrioticnigras.org #pn)"
	print "http://wiki.patrioticnigras.org has a crapload of useful scripts and dox, too."
	print "_________________________________________"
	--DumpAllHooks()
	return true
end;

SetHook("OnLuaInit",helloworld)