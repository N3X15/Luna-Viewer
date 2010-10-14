--[[
Blah blah blah GPLv2, I don't care.

Rob N3X15 Nelson
]]--

AO={}

-- State={Replacements},
AO.AnimationOverrides={}

-- Current Animations
AO.CurrentAnimation=UUID_nil
AO.CurrentState=""
AO.Disabled=false

--OnAnimStart(is_self,id,time_offset) For AOs.
local function AOOnAnimStart(is_self,id,time_offset)
	if is_self == "0" then return end
	if AO.Disabled==true then return end
	
	id=tostring(id)
	
	--print("[AO] Animation playing "..id)
	
	if AnimationStates[id] ~= nil and AO.CurrentAnimation~= id then
		state=AnimationStates[id]
		if AO.CurrentState ~= state then
			print("[AO] State changed to "..state)
			AO.CurrentState=state
		end
		if (AO.AnimationOverrides==nil or AO.AnimationOverrides[id]==nil) then return end
		numreplacements=table.getn(AO.AnimationOverrides[id])
		replacements=AO.AnimationOverrides[id]
		if AO.CurrentAnimation ~= UUID_nil then
			stopAnimation(AO.CurrentAnimation)
		end
		if numreplacements > 0 then
			AO.CurrentAnimation=replacements[math.random(1,numreplacements)]
			stopAnimation(id)
			startAnimation(AO.CurrentAnimation)
		end
	end
end

function AO:AddOverride(state,anim)
	if self.AnimationOverrides[state] == nil then
		self.AnimationOverrides[state]={}
	end
	i=table.getn(AO.AnimationOverrides[state])+1
	self.AnimationOverrides[state][i]=anim
end
-- Lines
function AO:ReadZHAO(input)
--[[
#######################################################################################################################
## How to add/change animations in this notecard:
##
## 1. Find the line that matches the animation you're changing. For example, if you're adding a walk animation, find the line 
##      that starts with [ Walking ]
##     If the notecard already has walking animations, the line will look something like this:
##     [ Walking ]SexyWalk1|SexyWalk2
##     
## 2. Type the name of the new animation at the end of this line. If the line already contains some animations, type '|' before
##      typing the animation name. Once you're done, the line should look like this:
##      [ Walking ]NewWalkAnim
##      or
##     [ Walking ]SexyWalk1|SexyWalk2|NewWalkAnim
##
## 3. Once you're done, save the notecard, put it back in ZHAO-II's inventory, and load it. See the "Read Me First" document
##      for help on how to do that.
#######################################################################################################################

[ Standing ]A|B|C
[ Walking ]Walking0
[ Sitting ]sit knee up3
[ Sitting On Ground ]ground sit 1
[ Crouching ]
[ Crouch Walking ]
[ Landing ]land1.1
[ Standing Up ]
[ Falling ]
[ Flying Down ]
[ Flying Up ]up-up-and-away
[ Flying ]superhero-cruise
[ Flying Slow ]superhero-cruise
[ Hovering ]hover1
[ Jumping ]
[ Pre Jumping ]
[ Running ]
[ Turning Right ]
[ Turning Left ]
[ Floating ]
[ Swimming Forward ]
[ Swimming Up ]
[ Swimming Down ]

#######################################################################################################################
##
## For advanced users only
##
## Lines starting with a # are treated as comments and ignored. Blank lines are ignored. Valid lines look like this:
##
## [ Walking ]SexyWalk2|SexyWalk3
##
## The token (in this case, [ Walking ] - note the spaces inside the [ ]) identifies the animation to be overridden. The rest is a list of 
## animations, separated by the '|' (pipe) character. You can specify multiple animations for Stands, Walks, Sits, and GroundSits. 
## Multiple animations on any other line are invalid. You can have up to 12 animations each for Walks, Sits and GroundSits. 
## There is no hard limit on the number of stands, but adding too many stands will make the script run out of memory and crash, so be 
## careful. You can repeat tokens, so you can split the Stands up across multiple lines. Use the [ Standing ] token in each line, and 
## the script will add the animation lists together.
##
## Each 'animation name' can be a comma-separated list of animations, which will be played together. For example:
## [ Walking ]SexyWalk1UpperBody,SexyWalk1LowerBody|SexyWalk2|SexyWalk3
##
## Note the ',' between SexyWalk1UpperBody and SexyWalk1LowerBody - this tells ZHAO-II to treat these as a single 
## 'animation' and play them together. The '|' between this 'animation' and SexyWalk2 tells ZHAO-II to treat SexyWalk2 and
## SexyWalk3 as separate walk animations. You can use this to layer animations on top of each other.
##
## Do not add any spaces around animation names!!!
##
## If you have read and understood these instructions, feel free to delete these lines. A shorter notecard will load faster.
#######################################################################################################################
]]--
	for _,line in ipairs(input) do
		line=trim(line)
		if not string.starts(line,"#") and line~="" then 
			state=string.gsub(s, "^%s*\[ (.-) \]%s*$", "%1")
			chunks=explode("]",line)
			anims=explode("|",chunks[2])
			if self.AnimationOverrides[state] == nil then
				self.AninimationOverrides[state]={}
			end
			for _,a in ipairs(anims) do
				cats,items = findObjectsInInventory("#AO");
				i=getn(self.AnimationOverrides[state])+1
				items_r=table.flip(items)
				self.AnimationOverrides[state][i]=items_r[a]
			end
		end
	end
end
SetHook("OnAnimStart",AOOnAnimStart)