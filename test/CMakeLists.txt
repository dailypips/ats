cmake_minimum_required(VERSION 3.1)

include_directories("../src/engine/base")
include_directories("../src/engine/core")
include_directories("framework")
include_directories("unittest")
include_directories("benchmark")
include_directories("test")

project(test C)

# unit test
if(BUILD_TEST_SEPARATION)
##  independent test
function(add_independent_test target)
  get_filename_component(exe_name ${target} NAME_WE)
  add_executable(${exe_name} ${target})
  target_link_libraries(${exe_name} core ${ATS_EXTERNAL_LIBRARIES})
  add_test(NAME ${exe_name} COMMAND ${exe_name})
endfunction()


add_definitions(-DBUILD_TEST_SEPARATION)
file(GLOB_RECURSE INDEPENDENT_TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/unittest/test_*.c)

foreach(item  ${INDEPENDENT_TEST_SRCS})
  add_independent_test(${item})
endforeach()

else() ## integrate test

file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/unittest/test_*.c)

set(ATS_TEST_FRAME_HEADER
    framework/runner.h
    framework/task.h
    unittest/test-list.h
)

if(WIN32)
  list(APPEND ATS_TEST_FRAME_HEADER framework/runner-win.h)
else(WIN32)
  list(APPEND ATS_TEST_FRAME_HEADER framework/runner-unix.h)
    list(APPEND ATS_EXTERNAL_LIBRARIES pthread)
endif()

set(ATS_TEST_FRAME_SRC
        framework/run-tests.c
        framework/runner.c
)

if(WIN32)
  list(APPEND ATS_TEST_FRAME_SRC framework/runner-win.c)
else(WIN32)
  list(APPEND ATS_TEST_FRAME_SRC framework/runner-unix.c)
endif()


add_executable(atest ${ATS_TEST_FRAME_HEADER} ${ATS_TEST_FRAME_SRC} ${TEST_SRCS})
target_link_libraries(atest core ${ATS_EXTERNAL_LIBRARIES})
endif() # integrate test

# benchmark
file(GLOB_RECURSE BENCHMARK_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/benchmark_*.c)

project(test C)

set(ATS_BENCHMARK_FRAME_HEADER
  framework/runner.h
  framework/task.h
)

if(WIN32)
  list(APPEND ATS_BENCHMARK_FRAME_HEADER framework/runner-win.h)
else(WIN32)
  list(APPEND ATS_BENCHMARK_FRAME_HEADER framework/runner-unix.h)
endif()

set(ATS_BENCHMARK_FRAME_SRC
        framework/run-benchmarks.c
        framework/runner.c
)

if(WIN32)
  list(APPEND ATS_BENCHMARK_FRAME_SRC framework/runner-win.c)
else(WIN32)
  list(APPEND ATS_BENCHMARK_FRAME_SRC framework/runner-unix.c)
endif()

add_executable(bench ${ATS_BENCHMARK_FRAME_HEADER} ${ATS_BENCHMARK_FRAME_SRC} ${BENCHMARK_SRCS})
target_link_libraries(bench core ${ATS_EXTERNAL_LIBRARIES})
