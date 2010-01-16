--[[
	Create documentation template for FlexLife wiki.
	 
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

-- Initialize variables.
local i = 0;
local txt="The following is a Lua _G table dump from a \"vanilla\" build of ".._SLUA_VERSION.."."

local functions={}
local tables={}
local variables={}

-- Loop through global table
for n,v in pairs(_G) do
	-- Sort types into categories
	if type(v)=="function" then
		table.insert(functions,n)
	elseif type(v)=="table" then
		table.insert(tables,n)
	else
		table.insert(variables,n)
	end		
end

-- Sort each category
table.sort(functions)
table.sort(tables)
table.sort(variables)

txt=txt.."\n\n== Functions ==\n"
for _,n in pairs(functions) do
	txt=txt.."\n* [["..n.."]]"
end

txt=txt.."\n\n== Tables ==\n"
for _,n in pairs(tables) do
	txt=txt.."\n* [["..n.."]]"
end

txt=txt.."\n\n== Variables/Other types ==\n"
for _,n in pairs(variables) do
	txt=txt.."\n* [["..n.."]]"
end

txt=txt.."\n\n== Hooks ==\n"..DumpAllHooks2Wiki()


print ("Dumping all functions/tables to FuncDump.txt...")
local f = io.open("LuaFuncDump.txt","w")
f:write(txt)
f:close()
print "Done."
