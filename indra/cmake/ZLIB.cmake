# -*- cmake -*-

set(ZLIB_FIND_QUIETLY ON)
set(ZLIB_FIND_REQUIRED ON)

if (STANDALONE)
  include(FindZLIB)
else (STANDALONE)
	include(Prebuilt)
  use_prebuilt_binary(zlib)
    set(ZLIB_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/zlib)

#	You must have symlinks such as zlib.so -> zlib.so.1 or it doesn't get found.. HATE HATE HATE HATE
	find_library(Z_RELEASE
								NAMES z zlib zdll z1g llz
								PATHS ${ARCH_PREBUILT_DIRS_RELEASE}
								NO_DEFAULT_PATH
								)
	find_library(Z_DEBUG
								NAMES z zlibd zdll z1g llz
								PATHS ${ARCH_PREBUILT_DIRS_DEBUG}
								NO_DEFAULT_PATH
								)
	set(ZLIB_LIBRARIES optimized ${Z_RELEASE} debug ${Z_DEBUG}) 
#	message(STATUS "Zlib Library: ${ZLIB_LIBRARIES}")
endif (STANDALONE)
