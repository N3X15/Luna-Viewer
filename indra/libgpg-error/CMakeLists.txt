# -*- cmake -*-

project(libgpg-error)

if (WINDOWS)
    include_directories(
	../gpg.vs/inc.vs
	../gpg.vs/libgpg-error-1.1.vs/custom
	libgpg-error-1.0/src
	)
else (WINDOWS)
    include_directories(
	libgpg-error-1.0/src
	)
endif (WINDOWS)

set(libgpg-error_SOURCE_FILES
    libgpg-error-1.0/src/code-from-errno.c
    libgpg-error-1.0/src/code-to-errno.c
    libgpg-error-1.0/src/gpg-error.c
    libgpg-error-1.0/src/mkerrcodes.c
    libgpg-error-1.0/src/strerror-sym.c
    libgpg-error-1.0/src/strerror.c
    libgpg-error-1.0/src/strsource-sym.c
    libgpg-error-1.0/src/strsource.c
    )

set(libgpg-error_HEADER_FILES
    CMakeLists.txt

    libgpg-error-1.0/src/err-codes.h
    libgpg-error-1.0/src/err-sources.h
    libgpg-error-1.0/src/gettext.h
    libgpg-error-1.0/src/gpg-error.h
    )

set_source_files_properties(${libgpg-error_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

list(APPEND libgpg-error_SOURCE_FILES ${libgpg-error_HEADER_FILES})

add_library (libgpg-error ${libgpg-error_SOURCE_FILES})
target_link_libraries(
    libgpg-error
    )
