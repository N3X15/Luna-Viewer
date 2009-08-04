# -*- cmake -*-

project(gpg.vs)

include_directories(
    inc.vs
    )

set(gpg.vs_SOURCE_FILES
    )

set(gpg.vs_HEADER_FILES
    CMakeLists.txt
    )

set_source_files_properties(${gpg.vs_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND gpg.vs_SOURCE_FILES ${gpg.vs_HEADER_FILES})
