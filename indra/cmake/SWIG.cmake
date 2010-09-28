### SWIG SHIT
# Swig is used to generate these horrendous Lua bindings.

#swig -lua -c++ -o newview/_LuaSL.cpp -oh newview/_LuaSL.h newview/swig/SL.swig
FIND_PROGRAM(_swig swig [PATH])
IF(NOT ${_swig} MATCHES "-NOTFOUND")
	MESSAGE("-- Found Swig! Attempting bindings generation in ${PROJECT_SOURCE_DIR}")
	EXECUTE_PROCESS(COMMAND ${_swig} 
		-lua 
		-c++ 
		-o  _LuaSL.cpp 
		-oh _LuaSL.h 
		swig/SL.swig 
	WORKING_DIRECTORY
        "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE swig_release
        RESULT_VARIABLE swig_result
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE)

	IF(${swig_result} EQUAL 0)
		MESSAGE("-- Lua bindings generated!")
	ELSE()
		MESSAGE("-- Lua bindings failed!  You will see errors about _LuaSL.cpp.")
	ENDIF()
ELSE()
	MESSAGE("-- Could not find swig.  Please ensure it is within your operating system's PATH.")
	MESSAGE("-- Note, you will see _LuaSL.cpp-related errors.")
ENDIF()

#add_custom_command(
#        TARGET ${VIEWER_BINARY_NAME} PRE_BUILD
#        COMMAND swig
#        ARGS
#          -lua
#          -c++
#          #-includeall
#          -o  _LuaSL.cpp
#          -oh _LuaSL.h
#          ${CMAKE_CURRENT_SOURCE_DIR}/swig/SL.swig
#        COMMENT "Creating bindings."
#        )
#
# source_group("Swig Interfaces" FILES ${viewer_SWIG_FILES})
### END SWIG

