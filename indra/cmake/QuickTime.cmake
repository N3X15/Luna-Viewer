# -*- cmake -*-

if(INSTALL_PROPRIETARY)
  include(Prebuilt)
  use_prebuilt_binary(quicktime)
endif(INSTALL_PROPRIETARY)

if (DARWIN)
  include(CMakeFindFrameworks)
  find_library(QUICKTIME_LIBRARY QuickTime)
elseif (WINDOWS)
  set(QUICKTIME_SDK_DIR "$ENV{PROGRAMFILES}/QuickTime SDK"
      CACHE PATH "Location of the QuickTime SDK.")
  find_library(QUICKTIME_LIBRARY qtmlclient
               PATHS
               ${LIBS_PREBUILT_DIR}/${LL_ARCH_DIR}/lib/release
				"C:\\Program Files (x86)\\Quicktime SDK\\libraries"
               "${QUICKTIME_SDK_DIR}\\libraries"
               )
  include_directories(
    ${LIBS_PREBUILT_DIR}/${LL_ARCH_DIR}/include/quicktime
	"C:\\Program Files (x86)\\Quicktime SDK\\CIncludes"
    "${QUICKTIME_SDK_DIR}\\CIncludes"
    )
endif (DARWIN)

mark_as_advanced(QUICKTIME_LIBRARY)

if (QUICKTIME_LIBRARY)
  set(QUICKTIME ON CACHE BOOL "Build with QuickTime streaming media support.")
endif (QUICKTIME_LIBRARY)

if (QUICKTIME)
  add_definitions(-DLL_QUICKTIME_ENABLED=1)
endif (QUICKTIME)
