include(Emerald)
include(LLCommon)
include(LLMath)

FILE(GLOB_RECURSE GTEST_SOURCES *.cpp)
FILE(GLOB_RECURSE GTEST_HEADERS *.h)

include(LLCommon)
include(LLMath)

include_directories(
    ${EMERALD_INCLUDE_DIRS}
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLMATH_INCLUDE_DIRS}
    )

ADD_EXECUTABLE(gtests
  ${GTEST_SOURCES}
  ${GTEST_HEADERS})

TARGET_LINK_LIBRARIES(gtests
  gtest
  gmock
  emerald
  llcommon
  llmath)
