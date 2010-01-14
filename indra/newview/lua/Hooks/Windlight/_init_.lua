
print("[WINDLIGHT] Initializing hooks.")

function explode(d,p)
  local t, ll
  t={}
  ll=0
  if(#p == 1) then return p end
    while true do
      l=string.find(p,d,ll+1,true) -- find the next d in the string
      if l~=nil then -- if "not not" found then..
        table.insert(t, string.sub(p,ll,l-1)) -- Save it in our array.
        ll=l+1 -- save just after where we found it for searching next time.
      else
        table.insert(t, string.sub(p,ll)) -- Save what's left in our array.
        break -- Break at end, as it should be, according to the lua manual.
      end
    end
  return t
end

function DumpEnv()
	local env = tostring(WLWater_GetNormalMap()).."^"
	env = env.. tostring(WLWater_GetWave1Dir()).."^"
	env = env.. tostring(WLWater_GetWave2Dir()).."^"
	env = env.. tostring(WLWater_GetScaleAbove()).."^"
	env = env.. tostring(WLWater_GetScaleBelow()).."^"
	env = env.. tostring(WLWater_GetNormalScale()).."^"
	env = env.. tostring(WLWater_GetFresnelScale()).."^"
	env = env.. tostring(WLWater_GetFresnelOffset()).."^"
	env = env.. tostring(WLWater_GetBlurMultiplier()).."^"
	env = env.. tostring(WLWater_GetFogDensity()).."^"
	env = env.. tostring(WLWater_GetFogColor())
	print("string WLWater=\""..env.."\";")

	env=""
	env = env.. tostring(WLSky_GetAmbient()).."^"
	env = env.. tostring(WLSky_GetBlueDensity()).."^"
	env = env.. tostring(WLSky_GetBlueHorizon()).."^"
	env = env.. tostring(WLSky_GetCloudColor()).."^"
	env = env.. tostring(WLSky_GetCloudCoverage()).."^"
	env = env.. tostring(WLSky_GetCloudDensity1()).."^"
	env = env.. tostring(WLSky_GetCloudDensity2()).."^"
	env = env.. tostring(WLSky_GetCloudScale()).."^"
	env = env.. tostring(WLSky_GetDensityMultiplier()).."^"
	env = env.. tostring(WLSky_GetDistanceMult()).."^"
	env = env.. tostring(WLSky_GetGamma()).."^"
	env = env.. tostring(WLSky_GetGlow()).."^"
	env = env.. tostring(WLSky_GetHazeDensity()).."^"
	env = env.. tostring(WLSky_GetHazeHorizon()).."^"
	env = env.. tostring(WLSky_GetLightNorm()).."^"
	env = env.. tostring(WLSky_GetMaxAltitude()).."^"
	env = env.. tostring(WLSky_GetSceneLightStrength()).."^"
	env = env.. tostring(WLSky_GetSunDeltaYaw()).."^"
	env = env.. tostring(WLSky_GetSunlight())
	print("string WLSky=\""..env.."\";")
end

local function WLChatListener(from_id, owner_id, mesg)
	--print("[WINDLIGHT]",from_id,key2name(owner_id),mesg)

	if string.starts(mesg,"&FLEXLIFE;WLWater^"..getMyID()) then
		local t = explode("^",mesg)
		--DumpTable(t)
		local p = getCurrentParcel()
		--print(getParcelOwner(p),owner_id.."\n",getMyID(),t[2])
		-- If I am the owner or the landowner is the owner, and I am being addressed, then
		if (tostring(getMyID())==owner_id or getParcelOwner(p)==owner_id) and t[2]==tostring(getMyID()) then
--[[
1: &FLEXLIFE;WLWater
2: a556e37a-6c2f-486b-8ab9-d08a55ff9301
3: 00000000-0000-0000-0000-000000000000
4: (1.05, -0.42)
5: (1.11, -1.16)
6: 0.029999999329448
7: 0.20000000298023
8: <2, 2, 2>
9: 0.39999997615814
10: 0.5
11: 0.040000002831221
12: 16
13: <0.0156863, 0.14902, 0.25098, 1>
]]--
			-- Set normalmap (unimplemented)
			WLWater_SetNormalMap(t[3])

			-- Set wave directions.
			WLWater_SetWave1Dir(parseVector2(t[4]))
			WLWater_SetWave2Dir(parseVector2(t[5]))
			
			-- Idk
			WLWater_SetScaleAbove(t[6])
			WLWater_SetScaleBelow(t[7])

			-- Normal texture scale.
			WLWater_SetNormalScale(parseVector3(t[8]))

			WLWater_SetFresnelScale(t[9])
			WLWater_SetFresnelOffset(t[10])

			WLWater_SetBlurMultiplier(t[11])
		
			WLWater_SetFogDensity(t[12])
        		WLWater_SetFogColor(parseVector4(t[13]))

			print("Windlight water settings have been set by the landowner.")
		end
	elseif string.starts(mesg,"&FLEXLIFE;WLSky^"..getMyID()) then
		local t = explode("^",mesg)
		DumpTable(t)
		local p = getCurrentParcel()
		--print(getParcelOwner(p),owner_id.."\n",getMyID(),t[2])
		-- If I am the owner or the landowner is the owner, and I am being addressed, then
		if (tostring(getMyID())==owner_id or getParcelOwner(p)==owner_id) and t[2]==tostring(getMyID()) then
--[[
1: &FLEXLIFE;WLSky
2: a556e37a-6c2f-486b-8ab9-d08a55ff9301
3: <0.204047, 0.242463, 0.330004, 0.110012>
4: <0.449991, 0.449999, 0.45001, 1>
5: <0.239996, 0.239999, 0.24, 1>
6: <0.226154, 0.226154, 0.226154, 1>
7: 0.26999998092651
8: <1.68841, 0.526097, 0.88, 1>
9: <1.68841, 0.526097, 0.125, 1>
10: 0.41999998688698
11: 0
12: 7.7576471085194e-05
13: 1
14: <5, 0.001, -0.48, 1>
15: <3.99991, 0, 0, 1>
16: <4.59712e-06, 0.199156, 0.199156, 1>
17: <-0, 0.711613, 0.702571, 1>
18: 906.19012451172
19: 2
20: 180
21: <0.34879, 0.355765, 0.660036, 0.220012>
]]--
			WLSky_SetAmbient(		parseVector4(t[3]))
			WLSky_SetBlueDensity(		parseVector4(t[4]))
			WLSky_SetBlueHorizon(		parseVector4(t[5]))
			WLSky_SetCloudColor(		parseVector4(t[6]))
			WLSky_SetCloudCoverage(		t[7])
			WLSky_SetCloudDensity1(		parseVector4(t[8]))
			WLSky_SetCloudDensity2(		parseVector4(t[9]))
			WLSky_SetCloudScale(		t[10])
			WLSky_SetDensityMultiplier(	t[11])
			WLSky_SetDistanceMult(		t[12])
			WLSky_SetGamma(			t[13])
			WLSky_SetGlow(			parseVector4(t[14]))
			WLSky_SetHazeDensity(		parseVector4(t[15]))
			WLSky_SetHazeHorizon(		parseVector4(t[16]))
			WLSky_SetLightNorm(		parseVector4(t[17]))
			WLSky_SetMaxAltitude(		t[18])
			WLSky_SetSceneLightStrength(	t[19])
			WLSky_SetSunDeltaYaw(		t[20])
			WLSky_SetSunlight(		parseVector4(t[21]))
			print("Windlight sky settings have been set by the landowner.")
		end
	end
end

local function WLBridgeListener(Channel, from_name,source_id,owner_id,message)
	print(message)
end
-- Equivalent to llListen(CHANNEL_DEBUG,"","","")
SetHook("OnChatDebug",		WLChatListener)
SetHook("OnBridgeMessage",	WLBridgeTranslator)
