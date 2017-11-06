cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(base C)

set(ATS_BASE_HEADER
  atomic.h
  currency.h
  datetime.h
  hash.h
  mem.h
  mpscq.h
  spscq.h
  vec.h
  variant.h
)

set(ATS_BASE_SRC
  currency.c
  datetime.c
  hash.c
  mem.c
  mpscq.c
  spscq.c
  vec.c
  variant.c
)

#if(WIN32)
#  list(APPEND ATS_UTIL_SRC log_windows.c time_windows.c string_util_windows.c string_windows.c)
#else(WIN32)
#  list(APPEND ATS_UTIL_SRC log_posix.c time_posix.c string_posix.c)
#endif()

add_library(base SHARED ${ATS_BASE_HEADER} ${ATS_BASE_SRC})
target_link_libraries(base ${ATS_EXTERNAL_LIBRARIES})
