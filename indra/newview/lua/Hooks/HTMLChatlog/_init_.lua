--[[
	HTML Chat Logs
		Generates an HTML Chat Log
	 
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

function chtmBegin(name,_)
	local htm=io.open(APPSETTINGS.."/"..name.."/chatlog.htm","w")
	htm:write([[
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
	<head>
		<title>FlexLife :: Your World, Evolved</title>
		<link href="http://flexlife.nexisonline.net/style.css" type="text/css" rel="stylesheet" />
	</head>
	<body>
		<div id="gWrap">
			<a href="http://flexlife.nexisonline.net/" id="header"><h1>FlexLife</h1></a>

			<h2 style="display:none;">
				Navigation
			</h2>
			<ul class="navbar">
				<li>
					<a href="/">
						Home
					</a>
				</li>
				<li>

					<a href="/about/">
						About
					</a>
				</li>
				<li>
					<a href="/downloads/">
						Download
					</a>
				</li>
				<li>

					<a href="/wiki/">
						Wiki
					</a>
				</li>
			</ul>
			<body>
				<h2>Chat log</h2>
	]])
	
	htm:close()
end

function chtmWrite(chatdata)
	local htm=io.open(APPSETTINGS.."/"..name.."/chatlog.htm","a")
	
	local msg= chatdata["message"]
	
	  -- Escape special HTML characters
	  msg = gsub(msg, "&", "&amp;")  -- Must be first!
	  msg = gsub(msg, "<", "&lt;")
	  msg = gsub(msg, ">", "&gt;")

	local line="<p class=\"chatline\"><span style=\"sender\">"..chatdata["sender"].."</span> "..msg.."</p>"
	htm:write(line)
	htm:close()
end
	
local htmlchatlogger = function (text,from,sourceid)
	if not exists(APPSETTINGS.."/"..name.."/chatlog.htm") then
		chtmBegin()
	end
	local chatdata = {message=text,sender=from}
	chtmWrite(chatdata)

end

SetHook("OnChatReceived",htmlchatlogger)
