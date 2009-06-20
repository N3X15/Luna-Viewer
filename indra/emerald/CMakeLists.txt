# -*- cmake -*-

project(emerald)

include(00-Common)
include(Emerald)
include(OpenSSL)

include_directories(
    ${EMERALD_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    )

set(emerald_SOURCE_FILES
    emerald.cpp
    dsaparam.cpp
    dhparam.cpp
    )
    
set(emerald_HEADER_FILES
    CMakeLists.txt

    emerald.h
    )

set_source_files_properties(${emerald_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND emerald_SOURCE_FILES ${emerald_HEADER_FILES})

add_library (emerald ${emerald_SOURCE_FILES})
