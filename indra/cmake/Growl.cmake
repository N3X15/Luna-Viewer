# -*- cmake -*-

# Growl is actually libnotify on linux systems.
if (LINUX)
    set(LIBNOTIFY_FIND_REQUIRED ON)
    include(FindLibnotify)
    set(GROWL_INCLUDE_DIRS ${LIBNOTIFY_INCLUDE_DIR})
    set(GROWL_LIBRARY ${LIBNOTIFY_LIBRARIES})
else (LINUX)
    include(Prebuilt)
    use_prebuilt_binary(Growl)
    if (DARWIN)
	set(GROWL_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/Growl)
	set(GROWL_LIBRARY growl)
    elseif (WINDOWS)
	set(GROWL_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/Growl)
	set(GROWL_LIBRARY lgggrowl++)
    endif (DARWIN)
endif (LINUX)
