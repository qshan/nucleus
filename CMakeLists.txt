# AxoMem Nucleus - for developing Persistent Applications
# Copyright (C) 2018-2020 AxoMem Pte Ltd.

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
    add_definitions("-Wall" ) # "-Wextra" "-Werror"
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options("-stdlib=libc++")
    else()
        # nothing yet
    endif()
endif()

include(FindThreads)

find_package(PkgConfig QUIET)

set(LIBPMEMOBJ_CPP_REQUIRED_VERSION 1.8)
set(LIBPMEMOBJ_REQUIRED_VERSION 1.6)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBPMEMOBJ++ REQUIRED libpmemobj++>=${LIBPMEMOBJ_CPP_REQUIRED_VERSION})
else()
    find_package(LIBPMEMOBJ++ ${LIBPMEMOBJ_CPP_REQUIRED_VERSION} REQUIRED libpmemobj++)
    message(STATUS "libpmemobj++ found without pkg-config (legacy)")
endif()
# There are no include or library dirs - will be found via standard includes...

if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBPMEMOBJ REQUIRED libpmemobj>=${LIBPMEMOBJ_REQUIRED_VERSION})
else()
    find_package(LIBPMEMOBJ REQUIRED ${LIBPMEMOBJ_REQUIRED_VERSION})
    message(STATUS "libpmemobj found without pkg-config (legacy)")
endif()

message (STATUS "LIBPMEMOBJ libaries in ${LIBPMEMOBJ_LIBRARIES} " )

file(GLOB SOURCES_CORE ./src/*.cpp)
set (CORE_INCLUDE_DIRS ./src)

file(GLOB SOURCES_EXT ./ext/*.cpp ./ext/spdlog/*.cpp ./ext/http_parser/http_parser.c)
set (EXT_INCLUDE_DIRS  ./ext ./ext/http_parser)

add_executable(nucleus ${SOURCES_APP} ${SOURCES_CORE} ${SOURCES_EXT} )

target_include_directories(nucleus PUBLIC ${PMDK_INCLUDE_DIRS} ${CORE_INCLUDE_DIRS} ${EXT_INCLUDE_DIRS} ${APP_INCLUDE_DIRS} libpmemobj++)
target_link_libraries(nucleus ${PMDK_LIBPMEMOBJ_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${LIBPMEMOBJ++_LIBRARIES})

add_custom_command(TARGET nucleus POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:nucleus> ${CMAKE_SOURCE_DIR}/bin/
)


# === Install target ===================================================================================================
install(TARGETS nucleus DESTINATION /usr/local/bin COMPONENT binaries)
install(FILES nucleus.conf DESTINATION /etc/nucleus COMPONENT config)
install(FILES util/nucleus.service DESTINATION /usr/lib/systemd/system/ COMPONENT daemon)
install(CODE "MESSAGE(\"\n*** Ensure to edit /etc/nucleus/nucleus.conf and add relevant configuration settings.\")")
install(CODE "MESSAGE(\"*** Then run systemctl daemon-reload, systemctl start nucleus, systemctl status nucleus and systemctl enable nucleus\")")
