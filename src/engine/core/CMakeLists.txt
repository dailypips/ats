cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(core C)

set(ATS_CORE_HEADER
  event.h
  event_bus.h
  event_queue.h
  event_dispatch.h
  context.h
)

set(ATS_CORE_SRC
  event.c
  event_bus.c
  event_queue.c
  event_dispatch.c
  context.c
)

add_library(core SHARED ${ATS_CORE_HEADER} ${ATS_CORE_SRC})
target_link_libraries(core base ${ATS_EXTERNAL_LIBRARIES})
