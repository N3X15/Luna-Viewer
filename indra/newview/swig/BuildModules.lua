SWIGPATH="newview/swig/"

licensing=[[
/**
 * @brief Luna Lua Integration Framework
 * @author Luna Contributors
 *
 *  Copyright (C) 2008-2010 Luna Contributors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id$
 */

]]

function BuildModule(name)
	if not exists(SWIGPATH..name..".swig") then
		print(" * Not found.")
		return
	end
	local outfile = string.format("newview/_Lua%s",name)
	local cl = "swig -lua -c++ -o "..outfile..".cpp -oh "..outfile..".h "..SWIGPATH..name..".swig | tee "..SWIGPATH..name..".swig.log"
	print(cl)
	os.execute(cl);
	if not exists(outfile..".h") then
		io.open(outfile..".h","w"):write(licensing.."\n// This file is purposefully empty because swig is dumb.")
	end
end

function exists(filename)
	local file = io.open(filename)
	if file then
		io.close(file)
		return true
	else
		return false
	end
end

function Changed(name)
	local inf=SWIGPATH..name..".swig"
	local outf=SWIGPATH.."."..name..".swig"
	if not exists(inf) then
		return false
	end
	if not exists(outf) then
		return true
	end
	local in_c=io.open(inf):read("*a")
	local out_c=io.open(outf):read("*a")
	io.open(outf,"w"):write(in_c)
	return (in_c ~= out_c)
end

i=1
for i=1,#arg,1 do
	local modname = arg[i]
	if Changed(modname) then
		BuildModule(arg[i])
	end
end
