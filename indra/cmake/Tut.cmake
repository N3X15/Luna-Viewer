# -*- cmake -*-
include(Prebuilt)

SET(TUT_FIND_REQUIRED TRUE)
SET(TUT_FIND_QUIETLY TRUE)

if (STANDALONE)
  include(FindTut)
  include_directories(${TUT_INCLUDE_DIR})
else (STANDALONE)
  use_prebuilt_binary(tut)
endif(STANDALONE)
