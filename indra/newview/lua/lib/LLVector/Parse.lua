
--[[
Parses <0,1> or (0,1) into the appropriate LLVector2 class.
]]--
function parseVector2(inp)
	-- Load up table.
	local f={}
	
	-- Parse all digits (0-9) and periods from the string
	for n in string.gmatch(inp, "[.%d]+") do
		-- Add to f table
		f[#f+1]=tonumber(n)
	end

	--DumpTable(f)
	
	-- Sanity check: Ensure there are at least two items in f.
	if #f<2 then
		error("parseVector2(): "..inp.." is not a valid input.")
		return
	end

	-- Create LLVector2.
	v2=LLVector2(0,0);
	
	-- Turn index 1 of table f into a number, and then assign it to the LLVector2's x field.
	v2.x=tonumber(f[1])

	-- Same thing, but with index 2 and field y.
	v2.y=tonumber(f[2])

	--print("ParseVector2:",inp,"->",v2);
	return v2
end

--[[
	Parses <0,1,2> or (0,1,2) or whatever into the appropriate LLVector3.
]]--
function parseVector3(inp)
	-- Load up table.
	local f={}
	
	-- Parse all digits (0-9) and periods from the string
	for n in string.gmatch(inp, "[.%d]+") do
		-- Add to f table
		f[#f+1]=tonumber(n)
	end

	--DumpTable(f)
	
	-- Sanity check: Ensure there are at least two items in f.
	if #f<3 then
		error("parseVector3(): "..inp.." is not a valid input.")
		return
	end

	-- Create LLVector3.
	v3=LLVector3(0,0,0);
	
	-- Turn index 1 of table f into a number, and then assign it to the LLVector2's x field.
	v3.x=tonumber(f[1])

	-- Same thing, but with index 2 and field y.
	v3.y=tonumber(f[2])

	v3.z=tonumber(f[3])

	--print("ParseVector3:",inp,"->",v2);
	return v3
end

--[[
	Parses <0,1,2,3> or {0,1,2,3} or whatever into the appropriate LLVector4.
]]--
function parseVector4(inp)
	-- Load up table.
	local f={}
	
	-- Parse all digits (0-9) and periods from the string
	for n in string.gmatch(inp, "[.%d]+") do
		-- Add to f table
		f[#f+1]=tonumber(n)
	end

	--DumpTable(f)
	
	-- Sanity check: Ensure there are at least two items in f.
	if #f<4 then
		error("parseVector4(): "..inp.." is not a valid input.")
		return
	end

	-- Create LLVector3.
	v4=LLVector4(0,0,0,0);
	
	-- Turn index 1 of table f into a number, and then assign it to the LLVector2's x field.
	v4.x=tonumber(f[1])

	-- Same thing, but with index 2 and field y.
	v4.y=tonumber(f[2])

	v4.z=tonumber(f[3])
	v4.w=tonumber(f[4])

	--print("ParseVector4:",inp,"->",v4);
	return v4
end
