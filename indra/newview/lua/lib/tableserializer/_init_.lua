function table.serialize(t,rl)
	if rl==nil then
		rl=0
	end
	local tmp="{\n"
	for k,v in pairs(t) do
		--print(k,v)
		tmp=tmp..indent(string.format("[%s]=%s,\n",serialize_data(k),serialize_data(v)))
	end
	return tmp.."}\n"
end

function serialize_data(d)
	if type(d)=="string" then
		return string.format("%q",d)
	elseif type(d)=="number" then
		return tostring(d)
	elseif type(d)=="table" then
		return table.serialize(d)
	else
		error("WHAT THE HELL IS A "..type(d).."?!")
		return "nil"
	end
end

function indent(str)
	t=""
	for ln,line in pairs(explode("\n",str)) do
		t=t.."\t"..line.."\n"
	end
	return t
end