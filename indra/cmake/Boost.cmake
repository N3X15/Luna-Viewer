# -*- cmake -*-
include(Prebuilt)

set(Boost_FIND_QUIETLY ON)
set(Boost_FIND_REQUIRED ON)

if (STANDALONE)
  find_package(Boost COMPONENTS system filesystem iostreams python regex signals thread wave program_options date_time REQUIRED)
else (STANDALONE)
  use_prebuilt_binary(boost)
  set(Boost_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include)

  if (WINDOWS)
    set(BOOST_VERSION 1_43)
    if (MSVC80)
      set(Boost_PROGRAM_OPTIONS_LIBRARY 
          optimized libboost_program_options-vc80-mt-${BOOST_VERSION}
          debug libboost_program_options-vc80-mt-gd-${BOOST_VERSION})
      set(Boost_REGEX_LIBRARY
          optimized libboost_regex-vc80-mt-${BOOST_VERSION}
          debug libboost_regex-vc80-mt-gd-${BOOST_VERSION})
      set(Boost_SIGNALS_LIBRARY 
          optimized libboost_signals-vc80-mt-${BOOST_VERSION}
          debug libboost_signals-vc80-mt-gd-${BOOST_VERSION})
      set(Boost_WAVE_LIBRARY 
          optimized libboost_wave-vc80-mt-${BOOST_VERSION}
          debug libboost_wave-vc80-mt-gd-${BOOST_VERSION})
    else (MSVC90)
      set(Boost_PROGRAM_OPTIONS_LIBRARY 
          optimized libboost_program_options-vc90-mt-${BOOST_VERSION}
          debug libboost_program_options-vc90-mt-gd-${BOOST_VERSION})
      set(Boost_REGEX_LIBRARY
          optimized libboost_regex-vc90-mt-${BOOST_VERSION}
          debug libboost_regex-vc90-mt-gd-${BOOST_VERSION})
      set(Boost_SIGNALS_LIBRARY 
          optimized libboost_signals-vc90-mt-${BOOST_VERSION}
          debug libboost_signals-vc90-mt-gd-${BOOST_VERSION})
      set(Boost_WAVE_LIBRARY 
          optimized libboost_wave-vc90-mt-${BOOST_VERSION}
          debug libboost_wave-vc90-mt-gd-${BOOST_VERSION})
    endif (MSVC80)
  elseif (LINUX)
  	set(Boost_PROGRAM_OPTIONS_LIBRARY boost_program_options-mt)  	
  	set(Boost_REGEX_LIBRARY boost_regex-mt)
  	set(Boost_PYTHON_LIBRARY boost_python-mt)
  	set(Boost_SIGNALS_LIBRARY boost_signals-mt)
  	set(Boost_WAVE_LIBRARY boost_wave-mt)
  	set(Boost_SYSTEM_LIBRARY boost_system-mt)
  	set(Boost_FILESYSTEM_LIBRARY boost_filesystem-mt)
  	set(Boost_IOSTREAMS_LIBRARY boost_iostreams-mt)
  	set(Boost_DATE_TIME_LIBRARY boost_date_time-mt)
  	set(Boost_THREAD_LIBRARY boost_thread-mt)
  elseif (DARWIN)
  	set(Boost_PROGRAM_OPTIONS_LIBRARY boost_program_options)  	
  	set(Boost_REGEX_LIBRARY boost_regex)
  	set(Boost_PYTHON_LIBRARY boost_python)
  	set(Boost_SIGNALS_LIBRARY boost_signals)
  	set(Boost_WAVE_LIBRARY boost_wave)
  	set(Boost_SYSTEM_LIBRARY boost_system)
  	set(Boost_FILESYSTEM_LIBRARY boost_filesystem)
  	set(Boost_IOSTREAMS_LIBRARY boost_iostreams)
  	set(Boost_DATE_TIME_LIBRARY boost_date_time)
  	set(Boost_THREAD_LIBRARY boost_thread)
  endif (WINDOWS)
endif (STANDALONE)
