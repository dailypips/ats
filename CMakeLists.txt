################################################################################
# Automated Trading System                                                     #
#                                                                              #
# Copyright (C) 2017 Xiaojun Gao                                               #
#                                                                              #
# Distributed under the terms and conditions of the MIT License.               #
################################################################################
cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME ats)
set(PROJECT_BRIEF "Automated Trading Library")

project(${PROJECT_NAME} C)

#set(CMAKE_C_STANDARD 11)

# be nice to vim users
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# silence policy CMP0042 warning by enabling RPATH explicitly
if(APPLE AND NOT DEFINED CMAKE_MACOSX_RPATH)
    set(CMAKE_MACOSX_RPATH true)
endif()

include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckStructHasMember)
include(CheckLibraryExists)
include(CheckCSourceCompiles)
include(GNUInstallDirs)
################################################################################
#                              set project options                             #
################################################################################
option(LIBATS_ENABLE_TESTING "Enable unit test" ON)
option(LIBATS_ENABLE_SHARED_BUILD "Enable build lib as shared library" ON)
option(BUILD_TEST_SEPARATION "Enable build separation test" OFF)
option(LIBATS_ENABLE_AMALG "Enable amalg build" OFF)
################################################################################
#                              get version                                     #
################################################################################
set(ATS_VERSION_MAJOR 1)
set(ATS_VERSION_MINOR 0)
set(ATS_VERSION_PATCH 0)
# create full version string
set(ATS_VERSION "${ATS_VERSION_MAJOR}.${ATS_VERSION_MINOR}.${ATS_VERSION_PATCH}")
# definitions
add_definitions(-DATS_VERSION_MAJOR=${ATS_VERSION_MAJOR})
add_definitions(-DATS_VERSION_MINOR=${ATS_VERSION_MINOR})
add_definitions(-DATS_VERSION_PATCH=${ATS_VERSION_PATCH})
add_definitions(-DATS_VERSION=${ATS_VERSION})
################################################################################
#   set output paths for binaries and libraries if not provided by the user    #
################################################################################
# prohibit in-source builds
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please use "
            "./configure to choose a build directory and "
            "initialize the build configuration.")
endif()
# set module path appropriately
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# set binary output path if not defined by user
if("${EXECUTABLE_OUTPUT_PATH}" STREQUAL "")
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()
# set library output path if not defined by user, but always set
# library output path to binary output path for Xcode projects
if("${CMAKE_GENERATOR}" STREQUAL "Xcode")
    set(LIBRARY_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}")
elseif("${LIBRARY_OUTPUT_PATH}" STREQUAL "")
    set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib")
endif()

################################################################################
#                                platform setup                                #
################################################################################
macro (ats_check_func SYM DEF)
    check_function_exists (${SYM} ${DEF})
    if (${DEF})
        add_definitions (-D${DEF}=1)
    endif ()
endmacro (ats_check_func)

macro (ats_check_sym SYM HDR DEF)
    check_symbol_exists (${SYM} ${HDR} ${DEF})
    if (${DEF})
        add_definitions (-D${DEF}=1)
    endif ()
endmacro (ats_check_sym)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions (-DATS_HAVE_LINUX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_definitions (-DATS_HAVE_OSX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions (-DATS_HAVE_WINDOWS)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    # Target Windows Vista and later
    add_definitions (-D_WIN32_WINNT=0x0600)
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    add_definitions (-DATS_HAVE_FREEBSD)
elseif (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    add_definitions (-DATS_HAVE_NETBSD)
elseif (CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    add_definitions (-DATS_HAVE_OPENBSD)
elseif (CMAKE_SYSTEM_NAME MATCHES "Solaris|SunOS")
    add_definitions (-DATS_HAVE_SOLARIS)
elseif (CMAKE_SYSTEM_NAME MATCHES "HP-UX")
    add_definitions (-DATS_HAVE_HPUX)
elseif (CMAKE_SYSTEM_NAME MATCHES "QNX")
    add_definitions (-DATS_HAVE_QNX)
else ()
    message (AUTHOR_WARNING "WARNING: This platform may or may not be supported: ${CMAKE_SYSTEM_NAME}")
endif ()

if (NOT WIN32)
    # Unconditionally declare the following feature test macros.  These are
    # needed for some platforms (glibc and SunOS/illumos) and should be harmless
    # on the others.
    add_definitions (-D_GNU_SOURCE)
    add_definitions (-D_REENTRANT)
    add_definitions (-D_THREAD_SAFE)
    add_definitions (-D_POSIX_PTHREAD_SEMANTICS)

    #ats_check_func (gethrtime ATS_HAVE_GETHRTIME)
    #ats_check_sym (CLOCK_MONOTONIC time.h ATS_HAVE_CLOCK_MONOTONIC)
endif ()

################################################################################
#                                compiler setup                                #
################################################################################

# set build default build type to RelWithDebInfo if not set
if(NOT CMAKE_BUILD_TYPE)
  if(LIBATS_ENABLE_TESTING)
    set(CMAKE_BUILD_TYPE Debug)
  else()
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
  endif()
endif()

# add extra flags
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS})
################################################################################
#                                Doxygen setup                                 #
################################################################################
# check for doxygen and add custom "doc" target to Makefile
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(PROJECT_VERSION ATS_VERSION)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            @ONLY)
    add_custom_target(doc "${DOXYGEN_EXECUTABLE}"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_HOME_DIRECTORY}"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif(DOXYGEN_FOUND)
################################################################################
#                                Boost setup                                   #
################################################################################
find_package(libuv REQUIRED)
if(LIBUV_FOUND)
    include_directories(${LIBUV_INCLUDE_DIRS})
endif()

find_package(Jemalloc REQUIRED)
if(JEMALLOC_FOUND)
  include_directories(${JEMALLOC_INCLUDE_DIRS})
endif()

find_package(LuaJIT REQUIRED)
if(LUAJIT_FOUND)
  include_directories(${LUAJIT_INCLUDE_DIRES})
endif()

set(ATS_EXTERNAL_LIBRARIES ${LIBUV_LIBRARIES} ${JEMALLOC_LIBRARIES} ${LUAJIT_LIBRARIES})
################################################################################
#                       set inclue paths for subprojects                       #
################################################################################
# all projects need the headers of the core components
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Build the targets
add_subdirectory(src)

# enable tests if not disabled
if (LIBATS_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
################################################################################
#                                print summary                                 #
################################################################################
# collect all compiler flags
string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPER_BUILD_TYPE)
set(ALL_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${UPPER_BUILD_TYPE}}")

# done
if(NOT LIBATS_NO_SUMMARY)
    message(STATUS
            "\n====================|  Build Summary  |===================="
            "\n"
            "\nLib version:       ${ATS_VERSION}"
            "\n"
            "\nBuild type:        ${CMAKE_BUILD_TYPE}"
            "\nC Compiler:        ${CMAKE_C_COMPILER}"
            "\nCFLAGS:            ${ALL_C_FLAGS}"
            "\nLIBRARIES:         ${LD_FLAGS} ${ATS_EXTERNAL_LIBRARIES}"
            "\n"
            "\nSource directory:  ${CMAKE_CURRENT_SOURCE_DIR}"
            "\nBuild directory:   ${CMAKE_CURRENT_BINARY_DIR}"
            "\nExecutable path:   ${EXECUTABLE_OUTPUT_PATH}"
            "\nLibrary path:      ${LIBRARY_OUTPUT_PATH}"
            "\nInstall prefix:    ${CMAKE_INSTALL_PREFIX}"
            "\nGenerator:         ${CMAKE_GENERATOR}"
            "\n"
            "\n===========================================================\n")
endif()
################################################################################
#                                THE END                                       #
################################################################################
