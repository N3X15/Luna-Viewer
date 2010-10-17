# Lua buildscript
# For Luna, by Luna devs
# STEAL AS DESIRED

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(lua)

# Do full dependency headers.
INCLUDE_REGULAR_EXPRESSION("^.*$")

#-----------------------------------------------------------------------------
# LUA version number, useful for packaging and doxygen doc:
SET(LUA_VERSION_MAJOR 1)
SET(LUA_VERSION_MINOR 5)
SET(LUA_VERSION_BUILD 4)
SET(LUA_VERSION_PATCH 2)
SET(LUA_VERSION
  "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_BUILD}-${LUA_VERSION_PATCH}")

# This setting of SOVERSION assumes that any API change
# will increment either the minor or major version number of LUA
SET(LUA_LIBRARY_PROPERTIES
  VERSION   "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_BUILD}"
  SOVERSION "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}"
)

IF(WIN32)
	ADD_DEFINITIONS(
		-DLUA_BUILD_AS_DLL
	)
ENDIF(WIN32)


#-----------------------------------------------------------------------------
# Test for some required system information.
# INCLUDE (${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityC.cmake)

#-----------------------------------------------------------------------------
# Compiler specific flags:
IF(CMAKE_COMPILER_IS_GNUCC)
	ADD_DEFINITIONS(
		-DLUA_USE_LINUX
	)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

include_directories(src/)

# Defines the source code for the library
SET(LUA_SRCS
	#Lua core
	src/lapi.c 
	src/lcode.c 
	src/ldebug.c 
	src/ldo.c 
	src/ldump.c 
	src/lfunc.c 
	src/lgc.c 
	src/llex.c 
	src/lmem.c
	src/lobject.c 
	src/lopcodes.c 
	src/lparser.c 
	src/lstate.c 
	src/lstring.c 
	src/ltable.c 
	src/ltm.c
	src/lundump.c 
	src/lvm.c 
	src/lzio.c

	# Lua standard library
	src/lauxlib.c 
	src/lbaselib.c 
	src/ldblib.c 
	src/liolib.c 
	src/lmathlib.c 
	src/loslib.c 
	src/ltablib.c
	src/lstrlib.c 
	src/loadlib.c 
	src/linit.c
)

# CLI (might as well get a list of hooks from it)
set (LUA_CLI_SRCS
	src/lua.c
)

# Pass proper definition to preprocessor to generate shared lib
IF(WIN32)
  IF(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DOPJ_EXPORTS)
  ELSE(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DOPJ_STATIC)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(WIN32)

# Create the library
ADD_LIBRARY(lua5.1 ${LUA_SRCS})
SET_TARGET_PROPERTIES(lua5.1 PROPERTIES
	${LUA_LIBRARY_PROPERTIES})
	

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(lua5.1 readline history ncurses)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
