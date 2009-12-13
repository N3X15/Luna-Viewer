# Install script for directory: /root/FlexLife/trunk/indra

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/root/FlexLife/trunk/indra/cmake/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llaudio/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llcharacter/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llcommon/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llimage/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llimagej2coj/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llinventory/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llmath/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llmedia/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llmessage/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llprimitive/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llrender/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llvfs/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llwindow/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llxml/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/lscript/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/emerald/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llcrashlogger/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/llui/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/linux_crash_logger/cmake_install.cmake")
  INCLUDE("/root/FlexLife/trunk/indra/newview/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/root/FlexLife/trunk/indra/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/root/FlexLife/trunk/indra/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
