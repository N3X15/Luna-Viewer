--[[
	FlexLife Lua Event Handler
		by N3X15
	
	 Copyright (C) 2008-2009 FlexLife Contributors
	 
	 This program is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 2 of the License, or
	 (at your option) any later version.
	 
	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	 
	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 
	$Id$
	
	*** DO NOT MODIFY OR YOU WILL BREAK SHIT. ***
]]--

--[[ Coded events:	All arguments are received as strings. Types below indicate string format. 
			Eg: With 'integer MyVar' MyVar will be a string such as "2", which is 
			    safe to handle as an integer. Lua will even implicitly convert it in 
			    this case. For instance statements like if(MyVar==2) are valid.
			    
	 EventName:		( args ):
	 
	OnLuaInit 		( )
	OnAgentInit 		( string AvName,	integer GodMode	)
	EmeraldPhantomOn	( integer Phantom )
	OnObjectCreated 	( string ObjectID, 	string PrimCode )
	OnAttach 		( string ObjectID, 	string AvName	)
	
]]--

gEvents={};
gEventDescs={};

function SetHook(EventName,Func)
	if(gEvents[EventName]==nil) then RegisterHook(EventName,"Autoregistered by SetHook()") end
	table.insert(gEvents[EventName],Func);
end

-- Should only be called from FlexLife's C++ code, or from a package that initialized the event called.
function CallHook(EventName,...)
	if(gEvents[EventName]==nil) then return 0 end -- No hooks to call, so exit.
	val=0
	for _,hookedfunc in pairs(gEvents[EventName]) do hookedfunc(...) val=val + 1 end
	return val
end

function RegisterHook(EventName,Desc)
	if(gEvents[EventName]==nil) then
		gEvents[EventName]={};
		gEventDescs[EventName]=Desc
		return
	end
	error(EventName.." has already been registered!")
end

function DumpAllHooks()
	print "Activated Hooks:"
	for name,_ in pairs(gEvents) do
		print(name)
	end
end
