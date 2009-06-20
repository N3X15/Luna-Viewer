local td="MACRO_ARGS={"
for _,v in pairs(MACRO_ARGS) do
	td=(td.."\""..v.."\",")
end
print(td.."};")
