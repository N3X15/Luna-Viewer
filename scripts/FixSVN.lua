--[[ written for Lua 5.1
split a string by a pattern, take care to create the "inverse" pattern 
yourself. default pattern splits by white space.
]]
string.split = function(str, pattern)
  pattern = pattern or "[^%s]+"
  if pattern:len() == 0 then pattern = "[^%s]+" end
  local parts = {__index = table.insert}
  setmetatable(parts, parts)
  str:gsub(pattern, parts)
  setmetatable(parts, nil)
  parts.__index = nil
  return parts
end

for line in io.lines() do
	local chunks = string.split(line,"[^%s]+")
	local c = 2
	local filename = chunks[2]
	if (#chunks>2) then
		while c<#chunks+1 do
			filename = filename.."\\ "..chunks[c]
			c=c+1
		end
	end
	if chunks[1]=="!" then
		print ("DELETE",filename)
		os.execute("svn delete "..filename)
	elseif chunks[1]=="?" then
		if filename and not filename:find("libraries/") then
			print ("ADD",filename)
			os.execute("svn add "..filename)
		end
	end
end

