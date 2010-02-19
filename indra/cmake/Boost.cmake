# -*- cmake -*-

include(Prebuilt)

set(Boost_FIND_QUIETLY ON)
set(Boost_FIND_REQUIRED ON)

if (STANDALONE)
  include(FindBoost)

  set(BOOST_PROGRAM_OPTIONS_LIBRARY boost_program_options-mt)
  set(BOOST_REGEX_LIBRARY boost_regex-mt)
  set(BOOST_SIGNALS_LIBRARY boost_signals-mt)
else (STANDALONE)
  use_prebuilt_binary(boost)
  set(Boost_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include)

  if (WINDOWS)
    set(BOOST_VERSION 1_36)
    if (MSVC71)
      set(BOOST_PROGRAM_OPTIONS_LIBRARY 
          optimized libboost_program_options-vc71-mt-${BOOST_VERSION}
          debug libboost_program_options-vc71-mt-gd-${BOOST_VERSION})
      set(BOOST_REGEX_LIBRARY
          optimized libboost_regex-vc71-mt-${BOOST_VERSION}
          debug libboost_regex-vc71-mt-gd-${BOOST_VERSION})
      set(BOOST_SIGNALS_LIBRARY 
          optimized libboost_signals-vc71-mt-${BOOST_VERSION}
          debug libboost_signals-vc71-mt-gd-${BOOST_VERSION})
    elseif (MSVC80)
      set(BOOST_PROGRAM_OPTIONS_LIBRARY 
          optimized libboost_program_options-vc80-mt-${BOOST_VERSION}
          debug libboost_program_options-vc80-mt-gd-${BOOST_VERSION})
      set(BOOST_REGEX_LIBRARY
          optimized libboost_regex-vc80-mt-${BOOST_VERSION}
          debug libboost_regex-vc80-mt-gd-${BOOST_VERSION})
      set(BOOST_SIGNALS_LIBRARY 
          optimized libboost_signals-vc80-mt-${BOOST_VERSION}
          debug libboost_signals-vc80-mt-gd-${BOOST_VERSION})
    elseif (MSVC90)
      set(BOOST_PROGRAM_OPTIONS_LIBRARY 
          optimized libboost_program_options-vc90-mt-${BOOST_VERSION}
          debug libboost_program_options-vc90-mt-gd-${BOOST_VERSION})
      set(BOOST_REGEX_LIBRARY
          optimized libboost_regex-vc90-mt-${BOOST_VERSION}
          debug libboost_regex-vc90-mt-gd-${BOOST_VERSION})
      set(BOOST_SIGNALS_LIBRARY 
          optimized libboost_signals-vc90-mt-${BOOST_VERSION}
          debug libboost_signals-vc90-mt-gd-${BOOST_VERSION})
    endif (MSVC71)
  else (WINDOWS)
  	find_library(PROGRAM_LIB NAMES boost_program_options-mt boost_program_options PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(REGEX_LIB NAMES boost_regex-mt boost_regex PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(PYTHON_LIB NAMES boost_python-mt boost_python PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(SIGNALS_LIB NAMES boost_signals-mt boost_signals PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(WAVE_LIB NAMES boost_wave-mt boost_wave PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(SYSTEM_LIB NAMES boost_system-mt boost_system PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(FILESYSTEM_LIB NAMES boost_filesystem-mt boost_filesystem PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(IOSTREAMS_LIB NAMES boost_iostreams-mt boost_iostreams PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(DATE_LIB NAMES boost_date_time-mt boost_date_time PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)
  	find_library(THREAD_LIB NAMES boost_thread-mt boost_thread PATHS ${ARCH_PREBUILT_DIRS_RELEASE} NO_DEFAULT_PATH)

  	set(BOOST_PROGRAM_OPTIONS_LIBRARY ${PROGRAM_LIB})  	
  	set(BOOST_REGEX_LIBRARY ${REGEX_LIB})
  	set(BOOST_PYTHON_LIBRARY ${PYTHON_LIB})
  	set(BOOST_SIGNALS_LIBRARY ${SIGNALS_LIB})
  	set(BOOST_WAVE_LIBRARY ${WAVE_LIB})
  	set(BOOST_SYSTEM_LIBRARY ${SYSTEM_LIB})
  	set(BOOST_FILESYSTEM_LIBRARY ${FILESYSTEM_LIB})
  	set(BOOST_IOSTREAMS_LIBRARY ${IOSTREAMS_LIB})
  	set(BOOST_DATE_TIME_LIBRARY ${DATE_LIB})
  	set(BOOST_THREAD_LIBRARY ${THREAD_LIB})
  endif (WINDOWS)
endif (STANDALONE)
