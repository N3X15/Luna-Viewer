function table.serialize(t)
	local tmp="{"
	for k,v in pairs(t) do
		tmp=tmp..string.format("[%s]=%s,",serialize_data(k),serialize_data(v))
	end
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
	end
end