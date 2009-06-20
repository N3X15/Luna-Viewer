print "_______________________________________"
print "Lua Unit Testing"
print ""

dofile "tests/unittest.lua"
dofile "tests/base.lua"

gSuccessCount	= 0
gFailCount		= 0

function utAssert(funcname, test)
	if(test==true) then
		gSuccessCount=gSuccessCount+1
		print(" - "..funcname..": [ OK ]")
	else
		gFailCount=gFailCount+1
		error(" - "..funcname..": [FAIL]")
	end
end

function utNotAssert(funcname, test)
	if (test==true) then
		gFailCount=gFailCount+1
		error(" - "..funcname..": [FAIL]")
	else
		gSuccessCount=gSuccessCount+1
		print(" - "..funcname..": [ OK ]")
	end
end

print ("[BASIC FUNCTIONALITY]")
utSendChat()
utSay()
utShout()
utWhisper()

SendChat ("[REGION FUNCTIONS]")
region = getCurrentRegion()
utAssert("getCurrentRegion()",(region == nil))
utAssert("region.getPacketsLost()",(region:getPacketsLost()>0))
--utAssert("region.getPacketsLost()",(region.getPacketsLost()>0))
