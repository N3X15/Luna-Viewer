# -*- cmake -*-
#
# Find SWIG for Lua bindings.
#
# Input variables:
#   SWIG_FIND_REQUIRED - set this if configuration should fail without SWIG
#
# Output variables:
#
#   SWIG_FOUND - set if SWIG was found
#   SWIG_EXECUTABLE - path to SWIG or pSWIG executable
#   SWIG_BATCH_FLAG - how to put SWIG/pSWIG into batch mode
#  SWIG_VERSION   - the version number of the swig executable

SET(SWIG_EXECUTABLE)
IF (WINDOWS)
  FIND_PROGRAM(SWIG_EXECUTABLE NAMES swig swig.exe)
ELSE (WINDOWS)
  FIND_PROGRAM(SWIG_EXECUTABLE NAMES swig)
ENDIF (WINDOWS)

IF (SWIG_EXECUTABLE)
  SET(SWIG_FOUND ON)
ELSE (SWIG_EXECUTABLE)
  SET(SWIG_FOUND OFF)
ENDIF (SWIG_EXECUTABLE)

IF (SWIG_FOUND)
IF(SWIG_DIR)
  SET(SWIG_FOUND 1)
  SET(SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
  EXECUTE_PROCESS(COMMAND ${SWIG_EXECUTABLE} -version
    OUTPUT_VARIABLE SWIG_version_output
    ERROR_VARIABLE SWIG_version_output
	RESULT_VARIABLE SWIG_version_result)
    IF(SWIG_version_result)
	  MESSAGE(SEND_ERROR "Command \"${SWIG_EXECUTABLE} -version\" failed with output:\n${SWIG_version_output}")
    ELSE(SWIG_version_result)
	  STRING(REGEX REPLACE ".*SWIG Version[^0-9.]*\([0-9.]+\).*" "\\1"
	    SWIG_version_output "${SWIG_version_output}")
	  SET(SWIG_VERSION ${SWIG_version_output} CACHE STRING "Swig version" FORCE)
    ENDIF(SWIG_version_result)
  ENDIF(SWIG_DIR)
ELSE (SWIG_FOUND)
  IF (SWIG_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find SWIG executable")
  ENDIF (SWIG_FIND_REQUIRED)
ENDIF (SWIG_FOUND)

MARK_AS_ADVANCED(SWIG_EXECUTABLE SWIG_FOUND)
