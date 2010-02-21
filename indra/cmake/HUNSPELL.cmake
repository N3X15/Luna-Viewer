# -*- cmake -*-

if (STANDALONE)
	include(FindHunSpell)
else (STANDALONE)
# Uncomment this if ever added to install.xml
#	include(Prebuilt)
#	use_prebuilt_binary(hunspell)
	
	set(HUNSPELL_INCLUDE_DIR ${LIBS_PREBUILT_DIR}/include/hunspell)
	
	find_library(HUNSPELL_RELEASE NAMES libhunspell hunspell hunspell-1.2 PATHS ${ARCH_PREBUILT_DIRS_RELEASE})
#	MESSAGE(STATUS "Hunspell library: ${HUNSPELL_RELEASE}")
	if (HUNSPELL_RELEASE)
		set(HUNSPELL_LIBRARY ${HUNSPELL_RELEASE})
	else (HUNSPELL_RELEASE)
		MESSAGE(FATAL_ERROR "Could not find HunSpell library")
	endif (HUNSPELL_RELEASE)
endif (STANDALONE)
