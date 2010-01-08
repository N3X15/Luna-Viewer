--[[
	FlexLife Automatic Mute Plugin - Annoying Sounds List
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
]]--
local AnnoyingSounds={
	"6c68ecbb-1209-ad5e-18a5-1071bcd923b3", -- Batman cube.
	"2abb1ac6-0af7-da90-1ecb-8b81a449dfeb", -- Fucking helicopter alarm
}

gAutoMuteSounds	= array_concat(gAutoMuteSounds,	AnnoyingSounds)

print("[AutoMute]   + Loaded Annoying Sound/Texture UUID blocklist.");
