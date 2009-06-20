dofile("lua/VisualParamList.lua")

local avid=name2key(MACRO_ARGS[1].." "..MACRO_ARGS[2])

if(avid==nil) then
 error ("Avatar not found.")
else
	print ("Cloning avatar "..avid.."...")
	for _,v in pairs(gVisualParamList) do
		setParamOnSelf(v,getParamCurrentWeight(avid,v))
	end
end
