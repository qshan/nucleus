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

# If building your own app within tree, place it in ./MyApp. This will work for simple tests but for more complex
# projects its better to make Nucleus a subdirectory of your project and include it as git submodule (for example)

# To use nucleus in a subdirectory of your project, add your own CMakeLists.txt at the top level and include:
# set(APP_NAME your_app_name)
# set(APP_SOURCES ...)   # list of files
# set(APP_INCLUDE_DIRS ...)  # list of include dirs
# set(APP_LIBRARIES ...) # list of additional libs (optional)
# ADD_SUBDIRECTORY(nucleus)

if (APP_NAME)

    message (STATUS "Found preset APP_NAME ${APP_NAME}" )

    if (NOT APP_SOURCES)
        message (FATAL_ERROR "No APP_SOURCES found")
    endif()
    # message (STATUS "APP_SOURCES: ${APP_SOURCES}")

    if (NOT APP_INCLUDE_DIRS)
        message (FATAL_ERROR "No APP_INCLUDE_DIRS found")
    endif()
    # message (STATUS "APP_INCLUDE_DIRS: ${APP_INCLUDE_DIRS}")

    # message (STATUS "APP_LIBRARIES: ${APP_LIBRARIES}")

else()

    message (STATUS "Building app from sources within tree" )

    # Specify -DBUILD_EXAMPLE=example_name to build an example
    if (NOT BUILD_EXAMPLE)
        set (BUILD_EXAMPLE 1-hello_world)
    endif()

    if (EXISTS "${CMAKE_SOURCE_DIR}/MyApp/main.cpp")
        set (APP_DIR "${CMAKE_SOURCE_DIR}/MyApp")
    else()
        set (APP_DIR "${CMAKE_SOURCE_DIR}/examples/${BUILD_EXAMPLE}")
    endif()

    if (EXISTS "${APP_DIR}/main.cpp")
        message(STATUS "Using ${APP_DIR}/main.cpp for compilation.")
    else()
        message(FATAL_ERROR "Unable to file ${APP_DIR}/main.cpp needed for compiling the example")
    endif()

    file(GLOB APP_SOURCES ${APP_DIR}/*.cpp)
    set(APP_INCLUDE_DIRS  ${APP_DIR})

    # Set App Name. This is used as the target
    get_filename_component(APP_NAME ${APP_DIR} NAME)
    message(STATUS "App Name defaulted to ${APP_NAME} from last directory in APP_DIR")

endif()

if (NOT APP_NAME)
    set(APP_NAME nucleus)
    message(STATUS "App Name is defaulted to ${APP_NAME}")
endif()

# === Set compiler definitions =========================================================================================

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

option(COVERAGE "Install coverage options" OFF)

if (COVERAGE)
    # https://github.com/axomem/thingbase/issues/38 - can we simplify?
    message(STATUS "Setting up Coverage. Forcing build type to DEBUG")
    set(CMAKE_BUILD_TYPE "DEBUG")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_definitions("-O0" )  #-O0 no optimisations
        add_compile_options(--coverage)
        link_libraries(gcov) # in case of linker errors. Didnt happen with definitions eg -fprofile-arcs -ftest-coverage
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # See above - cannot use this compiler yet due to PSTL support
        # https://github.com/axomem/thingbase/issues/37
        # check missing optimisation =0
        add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
        add_link_options(-fprofile-instr-generate)
        link_libraries(clang_rt.profile-x86_64) # in case of linker errors
    endif ()
endif()

# Setting default build type
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting CMAKE_BUILD_TYPE to DEBUG")
    set(CMAKE_BUILD_TYPE "Debug")
endif (NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE EQUAL DEBUG)
    add_definitions("-g" )  #-g add GDB Native extras
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

set (LIBPMEMOBJ++_INCLUDE_DIRS libpmemobj++)  # Seems no auto set var for LIBPMEMOBJ++ include
# https://github.com/pmem/libpmemobj-cpp/issues/788
message (STATUS "LIBPMEMOBJ++_INCLUDE_DIRS = ${LIBPMEMOBJ++_INCLUDE_DIRS}")
message (STATUS "LIBPMEMOBJ++_LIBRARIES = ${LIBPMEMOBJ++_LIBRARIES} ")

# === Set up  ==========================================================================================================

ENABLE_TESTING()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")  # Requires CMAKE 3.17
# list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")  # Requires CMAKE 3.17
ADD_SUBDIRECTORY(test)

# === Build src vars and include directories for target ================================================================

file(GLOB NUCLEUS_SOURCES ./src/*.cpp)
set (NUCLEUS_INCLUDE_DIRS ./src)

file(GLOB NUCLEUS_EXT_SOURCES  ./ext/*.cpp ./ext/spdlog/*.cpp ./ext/http_parser/http_parser.c)
set (NUCLEUS_EXT_INCLUDE_DIRS  ./ext ./ext/http_parser)

add_executable(${APP_NAME} ${APP_SOURCES} 
                           ${NUCLEUS_SOURCES} 
                           ${NUCLEUS_EXT_SOURCES} )

target_include_directories(${APP_NAME} PUBLIC ${APP_INCLUDE_DIRS} 
                                              ${NUCLEUS_INCLUDE_DIRS} 
                                              ${NUCLEUS_EXT_INCLUDE_DIRS} 
                                              ${LIBPMEMOBJ++_INCLUDE_DIRS})

target_link_libraries(${APP_NAME} ${APP_LIBRARIES} 
                                  ${LIBPMEMOBJ++_LIBRARIES} 
                                  ${CMAKE_THREAD_LIBS_INIT} )

# Copy the target to the bin directory post build for easier use
add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${APP_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)


if (EXISTS "${APP_DIR}/test/CMakeLists.txt")
    ADD_SUBDIRECTORY(${APP_DIR}/test)
endif()


# === Install target ===================================================================================================

install(TARGETS ${APP_NAME} DESTINATION /usr/local/bin COMPONENT binaries)
install(FILES nucleus.conf DESTINATION /etc/${APP_NAME} RENAME ${APP_NAME}.conf COMPONENT config)
install(FILES util/nucleus.service DESTINATION /usr/lib/systemd/system RENAME ${APP_NAME}.service COMPONENT daemon)
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

