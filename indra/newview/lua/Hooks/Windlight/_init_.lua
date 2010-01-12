
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


local function WLChatListener(from_id, owner_id, mesg)
	print("[WINDLIGHT]",from_id,key2name(owner_id),mesg)

	if string.starts(mesg,"&FLEXLIFE;WLWater^"..getMyID()) then
		local t = explode("^",mesg)
		local p = getCurrentParcel()
		if getParcelOwner(p)==owner_id and t[1]==tostring(getMyID()) then
			-- Set normalmap (unimplemented)
			--WLWater_SetNormalMapID		t[2]

			-- Set wave directions.
			WLWater_SetWave1Dir(LLVector2_parse(t[3]))
			WLWater_SetWave2Dir(LLVector2_parse(t[4]))
			
			-- Idk
			WLWater_SetScaleAbove(t[5])
			WLWater_SetScaleBelow(t[6])

			-- Normal texture scale.
			WLWater_SetNormalScale(LLVector3_parse(t[7]))

			WLWater_SetFresnelScale(t[8])
			WLWater_SetFresnelOffset(t[9])

			WLWater_SetBlurMultiplier(t[10])
		
			WLWater_SetFogDensity(t[11])
        		WLWater_SetFogColor(t[12])
		end
	end
end

-- Equivalent to llListen(CHANNEL_DEBUG,"","","")
SetHook("OnChatDebug",	WLChatListener)
