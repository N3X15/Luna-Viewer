# -*- cmake -*-

# In-source library only need these variables set.
set(OPENJPEG_LIBRARIES openjpeg)
if (LIBOPENJPEG2)
  set(OPENJPEG_INCLUDE_DIR ${LIBS_OPEN_DIR}/libopenjpeg2)
else (LIBOPENJPEG2)
  set(OPENJPEG_INCLUDE_DIR ${LIBS_OPEN_DIR}/libopenjpeg)
endif (LIBOPENJPEG2)
