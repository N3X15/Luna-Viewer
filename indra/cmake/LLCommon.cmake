# -*- cmake -*-
#include(Python)

#execute_process(
#      COMMAND ${PYTHON_EXECUTABLE} ${SCRIPTS_DIR}/svnversion.py
#      OUTPUT_VARIABLE SVN_VERSION
#      OUTPUT_STRIP_TRAILING_WHITESPACE
#      )
#add_definitions(-DEMERALD_SVN_VERSION=${SVN_VERSION})
#
#execute_process(
#      COMMAND ${PYTHON_EXECUTABLE} ${SCRIPTS_DIR}/svnbranch.py
#      OUTPUT_VARIABLE SVN_BRANCH
#      OUTPUT_STRIP_TRAILING_WHITESPACE
#      )
#add_definitions(-DEMERALD_BRANCH="${SVN_BRANCH}")

include(APR)
include(Boost)
include(EXPAT)
include(ZLIB)

set(LLCOMMON_INCLUDE_DIRS
    ${LIBS_OPEN_DIR}/llcommon
    ${CMAKE_BINARY_DIR}/llcommon
    ${APRUTIL_INCLUDE_DIR}
    ${APR_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    )

set(LLCOMMON_LIBRARIES llcommon)
