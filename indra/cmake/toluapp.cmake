# -*- cmake -*-
#
# Find ToLua++

include(Prebuilt)

SET(TOLUA_EXECUTABLE)
IF (WINDOWS)
	use_prebuilt_binary(toluapp)
	use_prebuilt_binary(Lua51)
	FIND_PROGRAM(TOLUA_EXECUTABLE NAMES tolua++ tolua++.exe PATHS ${CMAKE_SOURCE_DIR}/bin )
ELSE (WINDOWS)
	FIND_PROGRAM(TOLUA_EXECUTABLE NAMES tolua++ tolua++.exe)
ENDIF (WINDOWS)

IF (TOLUA_EXECUTABLE)
  SET(TOLUA_FOUND ON)
ELSE (TOLUA_EXECUTABLE)
  SET(TOLUA_FOUND OFF)
ENDIF (TOLUA_EXECUTABLE)

IF (TOLUA_FOUND)
  message(STATUS "tolua++ found: ${TOLUA_EXECUTABLE}")
  set(TOLUA_LIBRARIES 
      debug ${WINLIBS_PREBUILT_DEBUG_DIR}/toluapp.lib
      optimized ${WINLIBS_PREBUILT_RELEASE_DIR}/toluapp.lib
      )
  set(LUA_LIBRARIES
	  debug ${WINLIBS_PREBUILT_DEBUG_DIR}/lua5.1.lib
	  optimized ${WINLIBS_PREBUILT_RELEASE_DIR}/lua5.1.lib
	  )
  set(APR_INCLUDE_DIR ${LIBS_PREBUILT_DIR}/${LL_ARCH_DIR}/include/lua/)
ELSE (TOLUA_FOUND)
    MESSAGE(FATAL_ERROR "Could not find tolua++ executable")
ENDIF (TOLUA_FOUND)

MARK_AS_ADVANCED(TOLUA_EXECUTABLE TOLUA_FOUND)
