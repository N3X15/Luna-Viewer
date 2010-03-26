Luna Viewer GIT

Luna is a new Second Life viewer designed to become one of the most 
extendable viewers available.  It makes use of the Emerald viewer codebase
(with a new customized Lua engine based off of a de-griefed ShoopedLife Lua engine) 
and adds in some security patches.

*** 3PVP STATEMENT ***

This viewer has been modified to connect to OSGrid by default.  IF YOU DECIDE TO TRY TO CONNECT TO THE SECOND LIFE GRID, YOU DO SO AT YOUR OWN RISK. WE WILL NOT BE HELD LIABLE IF YOU DO SO, AND WE WILL NOT SUPPORT LUNA IF IT IS USED ON ANY LINDEN LAB GRIDS.


== Licensing ==

Same GPL/FLOSS license as SL itself.

== How to Build ==

=== Debian (Lenny) ===

Same deal as the usual LL build, but there are additional packages you will need.

  # For Lua 5.1 and friends
  aptitude install Lua5.1 liblua5.1-0-dev luarocks swig

  # LFS extension for Lua (dynamic loading of macros and hooks)
  luarocks install luafilesystem
  cp /usr/share/lib/luarocks/luafilesystem*/lib/lfs.so indra/newview/lua/

It should now build without issue.

== Windoze ==

No support for Windows yet, it's coming soon.
