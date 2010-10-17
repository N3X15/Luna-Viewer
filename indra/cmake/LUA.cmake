# -*- cmake -*-
#include(Prebuilt)
#find_package(Lua51)
#
#IF (LUA51_FOUND)
#	set(LUA_LIBRARY ${LUA_LIBRARY})
#	set(LUA_INCLUDES ${LUA_INCLUDE_DIR})
#ELSE (LUA51_FOUND)
#	#set(LUA_LIBRARY liblua5.1.so)
#	set(LUA_LIBRARY lua5.1)
#ENDIF (LUA51_FOUND)

set (LUA_LIBRARY lua5.1)
set (LUA_INCLUDE_DIR ${LIBS_OPEN_DIR}/lua/src)