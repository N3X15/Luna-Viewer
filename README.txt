Luna Viewer SVN

Luna is a new Second Life viewer designed to become one of the most 
extendable viewers available.  It makes use of the Emerald viewer codebase
(with a new customized Lua engine based off of a de-griefed ShoopedLife Lua engine) 
and adds in some security patches.

== Licensing ==

Same GPL/FLOSS license as SL itself.

== How to Build ==

=== Debian (Lenny) ===

Same deal as the usual LL build, but there are additional packages you will need.

  # For Lua 5.1 and friends
  aptitude install Lua5.1 liblua5.1-0-dev luarocks

  # LFS extension for Lua (dynamic loading of macros and hooks)
  luarocks install luafilesystem
  cp /usr/share/lib/luarocks/luafilesystem*/lib/lfs.so indra/newview/lua/

It should now build without issue.

== Windoze ==

No support for Windows yet, it's coming soon.
