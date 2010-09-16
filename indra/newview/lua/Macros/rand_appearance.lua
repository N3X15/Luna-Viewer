print(string.format("Randomizing %s's appearance...",MACRO_ARGS[1]))

local t = MACRO_ARGS[1]
for _,v in pairs(gVisualParamList) do
	local min = getParamMin(t,v)
	local max = getParamMax(t,v)
	setParamOnTarget(t,v,math.random(min,max))
end
