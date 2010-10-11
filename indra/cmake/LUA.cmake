# -*- cmake -*-
include(Prebuilt)
find_package(liblua REQUIRED)

IF (LibLua_FOUND)
	set(LUA_LIBRARY ${LibLua_LIBRARIES})
	set(LUA_INCLUDES ${LibLua_INCLUDE_DIRS})
ELSE (LibLua_FOUND)
	#set(LUA_LIBRARY liblua5.1.so)
	set(LUA_LIBRARY lua5.1)
ENDIF (LibLua_FOUND)