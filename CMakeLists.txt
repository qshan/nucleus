# AxoMem Nucleus - for developing Persistent Applications
# Copyright (C) 2018-2019 AxoMem Pte Ltd.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License Version 2 (only)
# as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License Version 2 for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/licenses/

cmake_minimum_required(VERSION 3.3)
project(nucleus)

# ------------------------------------------------------------------------------------
# These set the App directories to compile with Nucleus
set (APP_DIR "${CMAKE_SOURCE_DIR}/examples/1-hello_world")
# set (APP_DIR "${CMAKE_SOURCE_DIR}/examples/2-subclass")

# Comment out the above and use this line for your own test app.
# set (APP_DIR "${CMAKE_SOURCE_DIR}/myapp")

# Another option is to include Nucleus as a submodule in your own project.
# ------------------------------------------------------------------------------------
if (EXISTS "${APP_DIR}/main.cpp")
    message(STATUS "Using ${APP_DIR}/main.cpp for compilation. Be sure to delete your pmem file too.")
    # https://github.com/axomem/nucleus-alpha/issues/19 - layout issue
else()
    message(FATAL_ERROR "Unable to file ${APP_DIR}/main.cpp needed for compiling the example or your app.")
endif()

file(GLOB SOURCES_APP ${APP_DIR}/*.cpp)
set(APP_INCLUDE_DIRS ${APP_DIR})


set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif (NOT CMAKE_BUILD_TYPE)

if(MSVC)
    add_definitions("/W3" "/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS" "/D _CRT_SECURE_NO_WARNINGS" "/D _WIN32_WINNT=0x0A00")
    add_compile_options( "$<$<CONFIG:RELEASE>:/O2>")
else()
    # add_definitions("-Wall" ) # "-Wextra" "-Werror"
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options("-stdlib=libc++")
    else()
        # nothing yet
    endif()
endif()

include(FindThreads)

find_package(PkgConfig QUIET)

find_path(PMDK_INCLUDE_DIRS libpmemobj.h)
find_library(PMDK_LIBPMEMOBJ_LIBRARY NAMES libpmemobj libpmemobj.so HINTS /usr/local/lib64/)

message (STATUS "Found PMDK include headers in ${PMDK_INCLUDE_DIRS}" )
message (STATUS "Found PMDK LIBPMEMOBJ library in ${PMDK_LIBPMEMOBJ_LIBRARY}" )

if(NOT PMDK_INCLUDE_DIRS OR NOT PMDK_LIBPMEMOBJ_LIBRARY)
    message(FATAL_ERROR "Unable to find PMDK LIBMEMOBJ - check messages above.")
endif()

file(GLOB SOURCES_CORE ./src/*.cpp)
set (CORE_INCLUDE_DIRS ./src)

file(GLOB SOURCES_EXT ./ext/*.cpp ./ext/spdlog/*.cpp ./ext/http_parser/http_parser.c)
set (EXT_INCLUDE_DIRS  ./ext ./ext/http_parser)

add_executable(nucleus ${SOURCES_APP} ${SOURCES_CORE} ${SOURCES_EXT} )

target_include_directories(nucleus PUBLIC ${PMDK_INCLUDE_DIRS} ${CORE_INCLUDE_DIRS} ${EXT_INCLUDE_DIRS} ${APP_INCLUDE_DIRS})
target_link_libraries(nucleus ${PMDK_LIBPMEMOBJ_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
