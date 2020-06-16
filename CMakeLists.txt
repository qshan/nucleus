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


# === Choose which app to build ========================================================================================

 # Set App Dir to your App Source directory containing main.cpp
if (NOT APP_DIR)
    # Set the App directories to compile an example if no App dir specified
    # Specify -DBUILD_EXAMPLE=example_name to build an example
    if (NOT BUILD_EXAMPLE)
        set (BUILD_EXAMPLE 1-hello_world)
    endif()

    set (APP_DIR "${CMAKE_SOURCE_DIR}/examples/${BUILD_EXAMPLE}")
endif()

# Another option is to include Nucleus as a submodule in your own project.
# ------------------------------------------------------------------------------------
if (EXISTS "${APP_DIR}/main.cpp")
    message(STATUS "Using ${APP_DIR}/main.cpp for compilation. Be sure to delete your pmem file too.")
    # https://github.com/axomem/nucleus-alpha/issues/19 - layout issue
else()
    message(FATAL_ERROR "Unable to file ${APP_DIR}/main.cpp needed for compiling the example or your app.")
endif()

if (NOT APP_NAME)
    get_filename_component(APP_NAME ${APP_DIR} NAME)
    message(STATUS "App Name defaulted to ${APP_NAME} from last directory in APP_DIR")
else()
    message(STATUS "App Name is set to ${APP_NAME}")
endif()


# === Set compiler definitions =========================================================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

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


# === Set dependencies =================================================================================================

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
set (LIBPMEMOBJ++_INCLUDE_DIRS libpmemobj++)  # Seems no auto set vars for LIBPMEMOBJ++ yet.
message (STATUS "LIBPMEMOBJ++ INCLUDE DIRS = ${LIBPMEMOBJ++_INCLUDE_DIRS}")

if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBPMEMOBJ REQUIRED libpmemobj>=${LIBPMEMOBJ_REQUIRED_VERSION})
else()
    find_package(LIBPMEMOBJ REQUIRED ${LIBPMEMOBJ_REQUIRED_VERSION})
    message(STATUS "libpmemobj found without pkg-config (legacy)")
endif()
message (STATUS "LIBPMEMOBJ libaries in ${LIBPMEMOBJ_LIBRARIES} " )


# === Build src vars and include directories for target ================================================================

file(GLOB SOURCES_APP ${APP_DIR}/*.cpp)
set(APP_INCLUDE_DIRS ${APP_DIR})

file(GLOB SOURCES_NUCLEUS ./src/*.cpp)
set (NUCLEUS_INCLUDE_DIRS ./src)

file(GLOB SOURCES_NUCLEUS_EXT ./ext/*.cpp ./ext/spdlog/*.cpp ./ext/http_parser/http_parser.c)
set (NUCLEUS_EXT_INCLUDE_DIRS  ./ext ./ext/http_parser)

add_executable(${APP_NAME} ${SOURCES_APP} ${SOURCES_NUCLEUS} ${SOURCES_NUCLEUS_EXT} )

target_include_directories(${APP_NAME} PUBLIC ${APP_INCLUDE_DIRS} ${NUCLEUS_INCLUDE_DIRS} ${NUCLEUS_EXT_INCLUDE_DIRS} ${LIBPMEMOBJ++_INCLUDE_DIRS})
target_link_libraries(${APP_NAME} ${APP_LIBRARIES} ${LIBPMEMOBJ++_LIBRARIES} ${PMDK_LIBPMEMOBJ_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} )

# Copy the target to the bin directory post build for easier use
add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${APP_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)


# === Testing ==========================================================================================================

ENABLE_TESTING()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")  # Requires CMAKE 3.17
# list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")  # Requires CMAKE 3.17
ADD_SUBDIRECTORY(test)


# === Install target ===================================================================================================

install(TARGETS ${APP_NAME} DESTINATION /usr/local/bin COMPONENT binaries)
install(FILES nucleus.conf DESTINATION /etc/nucleus/${APP_NAME}.conf COMPONENT config)
install(FILES util/nucleus.service DESTINATION /usr/lib/systemd/system/${APP_NAME}.service COMPONENT daemon)
install(CODE "MESSAGE(\"\n*** Ensure to edit /etc/${APP_NAME}/${APP_NAME}.conf and add relevant configuration settings.\")")
install(CODE "MESSAGE(\"*** Then run systemctl daemon-reload, systemctl start ${APP_NAME}, systemctl status ${APP_NAME} and systemctl enable ${APP_NAME}\")")


# === RPM Packaging ====================================================================================================

set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME ${APP_NAME})
set(CPACK_PACKAGE_RELEASE 1)
# set(CPACK_PACKAGE_CONTACT "Name")
# set(CPACK_PACKAGE_VENDOR "Company")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr /usr/local /usr/local/bin /etc /etc/${APPNAME} /usr/lib /usr/lib/systemd /usr/lib/systemd/system)

#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup/thingbase.rpm.spec" "${CMAKE_CURRENT_BINARY_DIR}/thingbase.rpm.spec" @ONLY IMMEDIATE)
#set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/thingbase.rpm.spec")

include(CPack)

