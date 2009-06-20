# -*- cmake -*-

include(Prebuilt)

use_prebuilt_binary(havok)

if (DARWIN)
    set(HAVOK_VERSION 460)
else (DARWIN)
    set(HAVOK_VERSION 461)
endif (DARWIN)

set(LLPHYSICS_INCLUDE_DIRS
    ${LIBS_SERVER_DIR}/llphysics
    ${LIBS_SERVER_DIR}/llphysics/hk${HAVOK_VERSION}
    ${LIBS_PREBUILT_DIR}/include/havok/hk${HAVOK_VERSION}/common
    ${LIBS_PREBUILT_DIR}/include/havok/hk${HAVOK_VERSION}/physics
    )

add_definitions(-DLL_CURRENT_HAVOK_VERSION=${HAVOK_VERSION})

if (LINUX OR DARWIN)
  if (DARWIN)
    link_directories(
        ${LIBS_PREBUILT_DIR}/universal-darwin/lib_release/havok/hk${HAVOK_VERSION}
        )
  else (DARWIN)
    link_directories(
        ${LIBS_PREBUILT_DIR}/${LL_ARCH_DIR}/lib_release/havok/hk${HAVOK_VERSION}
        )
  endif (DARWIN)

  set(LLPHYSICS_LIBRARIES 
      llphysics
      hkcompat 
      hkutilities 
      hkvisualize 
      hkdynamics 
      hkvehicle 
      hkcollide 
      hkinternal 
      hkconstraintsolver 
      hkmath 
      hkscenedata 
      hkserialize 
      hkgraphicsogl 
      hkgraphicsbridge 
      hkgraphics 
      hkdemoframework 
      hkbase
      )
elseif (WINDOWS)
  if (MSVC71)
    set(HK_DEBUG ${WINLIBS_PREBUILT_DEBUG_DIR}/havok/hk${HAVOK_VERSION})
    set(HK_RELEASE ${WINLIBS_PREBUILT_RELEASE_DIR}/havok/hk${HAVOK_VERSION})
  else (MSVC71)
    set(HK_DEBUG ${WINLIBS_PREBUILT_DEBUG_DIR}/havok/hk${HAVOK_VERSION}_net_8-0)
    set(HK_RELEASE ${WINLIBS_PREBUILT_RELEASE_DIR}/havok/hk${HAVOK_VERSION}_net_8-0)
  endif (MSVC71)

  set(HAVOK_LIBS
      hkbase
      hkdynamics
      hkmath
      hkcollide
      hkutilities
      hkvisualize
      hkinternal
      hkconstraintsolver
      hkcompat
      hkserialize
      hkvehicle
      hkscenedata
      )

  set(LLPHYSICS_LIBRARIES llphysics)

  foreach(lib ${HAVOK_LIBS})
    list(APPEND LLPHYSICS_LIBRARIES 
         debug ${HK_DEBUG}/${lib} optimized ${HK_RELEASE}/${lib})
  endforeach(lib)
endif (LINUX OR DARWIN)
