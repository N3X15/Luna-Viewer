MACRO(LL_TEST_COMMAND OUTVAR LD_LIBRARY_PATH)
  # nat wonders how Kitware can use the term 'function' for a construct that
  # cannot return a value. And yet, variables you set inside a FUNCTION are
  # local. Try a MACRO instead.
  SET(value
    ${PYTHON_EXECUTABLE}
    "${CMAKE_SOURCE_DIR}/cmake/run_build_test.py")
  FOREACH(dir ${LD_LIBRARY_PATH})
    LIST(APPEND value "-l${dir}")
  ENDFOREACH(dir)
  LIST(APPEND value ${ARGN})
  SET(${OUTVAR} ${value})
##IF(LL_TEST_VERBOSE)
##  MESSAGE(STATUS "LL_TEST_COMMAND: ${value}")
##ENDIF(LL_TEST_VERBOSE)
ENDMACRO(LL_TEST_COMMAND)
