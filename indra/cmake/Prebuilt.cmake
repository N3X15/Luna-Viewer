# -*- cmake -*-

macro (use_prebuilt_binary _binary)
  if(NOT STANDALONE)
    get_property(PREBUILT_PACKAGES TARGET prepare PROPERTY PREBUILT)
    list(FIND PREBUILT_PACKAGES ${_binary} _index)
    if(_index LESS 0)
      set_property(TARGET prepare APPEND PROPERTY PREBUILT ${_binary})
    endif(_index LESS 0)
  endif(NOT STANDALONE)
endmacro (use_prebuilt_binary _binary)
