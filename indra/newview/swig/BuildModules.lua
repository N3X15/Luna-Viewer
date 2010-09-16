SWIGPATH="newview/swig/"

licensing=[[
/**
 * @brief Luna Lua Integration Framework
 * @author N3X15
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
		print("touch "..outfile..".h")
		os.execute("touch "..outfile..".h")
		io.open(outfile..".h"):write(licensing.."\n// This file is purposefully empty because swig is dumb.")
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

local returncode = 256
if exists(SWIGPATH.."CHECKSUMS.md5") then
	returncode = os.execute("md5sum -c "..SWIGPATH.."CHECKSUMS.md5")
	print("Return code: "..returncode)
end

if returncode ~= 0 then
	i=1
	for i=1,#arg,1 do
--		print(i,arg[i])
		BuildModule(arg[i])
	end
	os.execute("md5sum "..SWIGPATH.."*.swig "..SWIGPATH.."*.i > "..SWIGPATH.."CHECKSUMS.md5")
else
	print "Skipping module build."
end
