# -*- cmake -*-

project(llaudio)

include(00-Common)
include(Audio)
include(LLAudio)
include(FMOD)
include(OPENAL)
include(LLCommon)
include(LLMath)
include(LLMessage)
include(LLVFS)

include_directories(
    ${LLAUDIO_INCLUDE_DIRS}
    ${FMOD_INCLUDE_DIR}
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLMATH_INCLUDE_DIRS}
    ${LLMESSAGE_INCLUDE_DIRS}
    ${LLVFS_INCLUDE_DIRS}
    ${OGG_INCLUDE_DIRS}
    ${VORBISENC_INCLUDE_DIRS}
    ${VORBISFILE_INCLUDE_DIRS}
    ${VORBIS_INCLUDE_DIRS}
    ${OPENAL_LIB_INCLUDE_DIRS}
    ${FREEAULT_LIB_INCLUDE_DIRS}
    )

set(llaudio_SOURCE_FILES
    llaudioengine.cpp
    lllistener.cpp
    llaudiodecodemgr.cpp
    llvorbisdecode.cpp
    llvorbisencode.cpp
    )

set(llaudio_HEADER_FILES
    CMakeLists.txt

    llaudioengine.h
    lllistener.h
    llaudiodecodemgr.h
    llvorbisdecode.h
    llvorbisencode.h
    llwindgen.h
    )

if (FMOD)
    list(APPEND llaudio_SOURCE_FILES
         llaudioengine_fmod.cpp
         lllistener_fmod.cpp
         llstreamingaudio_fmod.cpp
         )

    list(APPEND llaudio_HEADER_FILES
         llaudioengine_fmod.h
         lllistener_fmod.h
         llstreamingaudio_fmod.h
         )

    if (LINUX)
	  # This check fails in Ubuntu 9.10 with g++-4.x.. We should be using > 4.1 now anyway.
	  # -Wno-unused-function should only be used once it is clear that the fmod instance is being created.
      #if (${CXX_VERSION} MATCHES "4.[234]")
        set_source_files_properties(llaudioengine_fmod.cpp
        							lllistener_fmod.cpp
        							llstreamingaudio_fmod.cpp
                                    PROPERTIES COMPILE_FLAGS "-Wno-write-strings -Wno-unused-function")
      #endif (${CXX_VERSION} MATCHES "4.[234]")
    endif (LINUX)
endif (FMOD)

if (OPENAL)
  list(APPEND llaudio_SOURCE_FILES
    llaudioengine_openal.cpp
    lllistener_openal.cpp
    )
  
  list(APPEND llaudio_HEADER_FILES
    llaudioengine_openal.h
    lllistener_openal.h
    )
endif (OPENAL)

set_source_files_properties(${llaudio_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND llaudio_SOURCE_FILES ${llaudio_HEADER_FILES})

add_library (llaudio ${llaudio_SOURCE_FILES})
add_dependencies(llaudio prepare)
target_link_libraries(
    llaudio
    ${VORBISENC_LIBRARIES}
    ${VORBISFILE_LIBRARIES}
    ${VORBIS_LIBRARIES}
    ${OGG_LIBRARIES}
    )
