CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(openjpeg)

# Do full dependency headers.
INCLUDE_REGULAR_EXPRESSION("^.*$")

#-----------------------------------------------------------------------------
# OPENJPEG version number, useful for packaging and doxygen doc:
SET(OPENJPEG_VERSION_MAJOR 2)
SET(OPENJPEG_VERSION_MINOR 0)
SET(OPENJPEG_VERSION_BUILD 0)
SET(OPENJPEG_VERSION
  "${OPENJPEG_VERSION_MAJOR}.${OPENJPEG_VERSION_MINOR}.${OPENJPEG_VERSION_BUILD}")

# This setting of SOVERSION assumes that any API change
# will increment either the minor or major version number of openjpeg
SET(OPENJPEG_LIBRARY_PROPERTIES
  VERSION   "${OPENJPEG_VERSION_MAJOR}.${OPENJPEG_VERSION_MINOR}.${OPENJPEG_VERSION_BUILD}"
  SOVERSION "${OPENJPEG_VERSION_MAJOR}.${OPENJPEG_VERSION_MINOR}"
)

#-----------------------------------------------------------------------------
# Compiler specific flags:
IF(CMAKE_COMPILER_IS_GNUCC)
  # For all builds, make sure openjpeg is std99 compliant:
  # SET(CMAKE_C_FLAGS "-Wall -std=c99 ${CMAKE_C_FLAGS}") # FIXME: this setting prevented us from setting a coverage build.
  # Do not use ffast-math for all build, it would produce incorrect results, only set for release:
  SET(CMAKE_C_FLAGS_RELEASE "-ffast-math ${CMAKE_C_FLAGS_RELEASE}")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Defines the source code for the library
SET(OPENJPEG_SRCS
  tcd.c
  bio.c
  cio.c
  dwt.c
  event.c
  image.c
  j2k.c
  j2k_lib.c
  jp2.c
  jpt.c
  mct.c
  mqc.c
  openjpeg.c
  pi.c
  raw.c
  t1.c
  tgt.c
  profile.c
  invert.c
  function_list.c
  t2.c
  bio.h
  cio.h
  dwt.h
  event.h
  image.h
  j2k.h
  j2k_lib.h
  jp2.h
  jpt.h
  mct.h
  mqc.h
  openjpeg.h
  pi.h
  int.h
  raw.h
  t1.h
  t2.h
  tcd.h
  tgt.h
  profile.h
  invert.h
  function_list.h
)

# Pass proper definition to preprocessor to generate shared lib
IF(WIN32)
  IF(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DOPJ_EXPORTS)
  ELSE(BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DOPJ_STATIC)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(WIN32)

IF(ENABLE_PROFILING)
    ADD_DEFINITIONS(-D_PROFILE)
ENDIF(ENABLE_PROFILING)
  
# Create the library
ADD_LIBRARY(openjpeg ${OPENJPEG_SRCS})
SET_TARGET_PROPERTIES(openjpeg PROPERTIES
	${OPENJPEG_LIBRARY_PROPERTIES})
IF(UNIX)
  TARGET_LINK_LIBRARIES(${OPENJPEG_LIBRARY_NAME} m)
ENDIF(UNIX)

