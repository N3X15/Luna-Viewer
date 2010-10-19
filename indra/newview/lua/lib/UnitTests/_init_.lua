UnitTests={}

function AddUnitTest(name,func)
	UnitTests[name]=func
end

--[[ Basic Types, functions ]]--


-- Testing UUID_validate with valid UUID
local function UUIDValidateTest1()
	if UUID_validate("c31a831e-b42f-4e85-8a8b-177191e1beed") then
		--print("UUID_validate(\"c31a831e-b42f-4e85-8a8b-177191e1beed\")="..UUID_validate("c31a831e-b42f-4e85-8a8b-177191e1beed"))
		return true
	end
	return false
end

-- Testing UUID_validate with invalid UUID
local function UUIDValidateTest2()
	if UUID_validate("ERRONEOUS") then
		--print("UUID_validate(\"ERRONEOUS\")="..UUID_validate("ERRONEOUS"))
		return false
	end
	return true
end

local function GetInventoryAssetUUIDTest1()
	id = getInventoryAssetUUID("sway",AssetType.ANIMATION)
	if id ~= "bf3675c7-6973-4c47-809f-3cb13bc606f7" then
		return id
	end
	return true
end

AddUnitTest("Testing UUID_validate with valid UUID",	UUIDValidateTest1)
AddUnitTest("Testing UUID_validate with invalid UUID",	UUIDValidateTest2)

AddUnitTest("Testing getInventoryAssetUUID with animation",	GetInventoryAssetUUIDTest1)