
local i = 0
for n,f in pairs(UnitTests) do
	i=i+1
	val=UnitTests[n]()
	if val==true then
		print("Test "..i..": "..n.." [ OK ]")
	else
		print("Test "..i..": "..n.." [FAIL]")
		print(val)
	end
end
