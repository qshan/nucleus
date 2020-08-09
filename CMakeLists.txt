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

cmake_minimum_required(VERSION 3.15)
project(nucleus)

set(NUCLEUS_SOURCE_DIR ${PROJECT_SOURCE_DIR})
include(cmake/utils.cmake)

if (NOT VERSION) # already set from higher-level project
    set(VERSION 0.3.beta)
endif()


# === Set compiler definitions =========================================================================================

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_definitions("/W3" "/D _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS" "/D _CRT_SECURE_NO_WARNINGS" "/D _WIN32_WINNT=0x0A00")
    add_compile_options( "$<$<CONFIG:RELEASE>:/O2>")
else()
    add_definitions("-Wall -Wno-sign-compare" ) # "-Wextra" "-Werror"
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

# Python used for testing
# FIX - add option to disable testing - https://github.com/axomem/nucleus/issues/46
find_package (Python3 COMPONENTS Interpreter)
if(${Python3_FOUND})
    message(STATUS "Found Python3 Interpreter at ${Python3_EXECUTABLE}")
else()
    message(FATAL_ERROR "Python3 Interpreter not found")
endif()

# === Set up testing ===================================================================================================

ENABLE_TESTING()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")  # Requires CMAKE 3.17
# list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")  # Requires CMAKE 3.17
ADD_SUBDIRECTORY(test)

if(${TEST_SERVER_PORT})  # Keep track of server port if its already defined
    set(TEST_SERVER_PORT_DEFINED)
endif()

nucleus_init(${PROJECT_SOURCE_DIR})

if (NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})  # if not nucleus building...
    if(NOT TEST_SERVER_PORT_DEFINED)
    # Push port up to top level
        set(TEST_SERVER_PORT ${TEST_SERVER_PORT} PARENT_SCOPE)
    endif()
endif()

# === Build src vars and include directories for target ================================================================

file(GLOB NUCLEUS_SOURCES ./src/*.cpp)
set (NUCLEUS_INCLUDE_DIRS ./src)

file(GLOB NUCLEUS_EXT_SOURCES  ./ext/*.cpp ./ext/spdlog/*.cpp ./ext/http_parser/http_parser.c)
set (NUCLEUS_EXT_INCLUDE_DIRS  ./ext ./ext/http_parser)

set (NUCLEUS_ALL_TARGETS "")

function(add_target APP_NAME APP_DIR APP_SOURCES APP_INCLUDE_DIRS APP_LIBRARIES)

    message(STATUS "Adding target ${APP_NAME} =================================")
    # message(STATUS "${APP_NAME}" "\n" "${APP_DIR}" "\n" "${APP_SOURCES}" "\n" "${APP_INCLUDE_DIRS}" "\n" "${APP_LIBRARIES}")

    add_executable(${APP_NAME} ${APP_SOURCES}
                               ${NUCLEUS_SOURCES}
                               ${NUCLEUS_EXT_SOURCES}
                               nucleus.conf
                               util/nucleus.service)  # last two included due to RPM Packaging. Renamed on install

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

    add_common_tests()

    # Add the additional app tests, if exist

    if (EXISTS "${APP_DIR}/test/CMakeLists.txt")
        if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
            #set(NUCLEUS_SOURCE_DIR ${PROJECT_SOURCE_DIR})
            include(${APP_DIR}/test/CMakeLists.txt)
        else()
            #set(NUCLEUS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/nucleus)
            message(STATUS "Out of tree build detected. "
                           "Include your common tests in your top-level test folder.")
            include(${APP_DIR}/test/CMakeLists.txt)
        endif()
    endif()

    # Get next server port
    getnextport(TEST_SERVER_PORT ${TEST_SERVER_PORT})
    set(TEST_SERVER_PORT ${TEST_SERVER_PORT} PARENT_SCOPE)

    message(STATUS "Target configuration done =================================")

endfunction()

# === Choose which app to build ========================================================================================

# If building your own app within tree, place it in ./HelloWorld. This will work for simple tests but for more complex
# projects its better to make Nucleus a subdirectory of your project and include it as git submodule (for example)

# To use nucleus in a subdirectory of your project, add your own CMakeLists.txt at the top level and include:
# set(APP_NAME your_app_name)
# set(APP_SOURCES ...)                         # list of files
# set(APP_INCLUDE_DIRS ...)                    # list of include dirs
# set(APP_LIBRARIES ...)                       # list of additional libs (optional)
# ADD_SUBDIRECTORY(nucleus)                    # or other location
# nucleus_init(${PROJECT_SOURCE_DIR}/nucleus)  # or other location

if (APP_NAME)

    message (STATUS "Found preset APP_NAME ${APP_NAME} from higher-level inclusion" )

    if (NOT APP_DIR)
        message (FATAL_ERROR "No APP_DIR found - this should point to your root directory")
    endif()

    if (NOT APP_SOURCES)
        message (FATAL_ERROR "No APP_SOURCES found")
    endif()
    # message (STATUS "APP_SOURCES: ${APP_SOURCES}")

    if (NOT APP_INCLUDE_DIRS)
        message (FATAL_ERROR "No APP_INCLUDE_DIRS found")
    endif()
    # message (STATUS "APP_INCLUDE_DIRS: ${APP_INCLUDE_DIRS}")

    # message (STATUS "APP_LIBRARIES: ${APP_LIBRARIES}")

    add_target("${APP_NAME}" "${APP_DIR}" "${APP_SOURCES}" "${APP_INCLUDE_DIRS}" "${APP_LIBRARIES}")

    include(cmake/install.cmake)
    include(cmake/package.cmake)

    list(APPEND NUCLEUS_ALL_TARGETS ${APP_NAME})

elseif(EXISTS "${CMAKE_SOURCE_DIR}/myapp/main.cpp")

    message (STATUS "Building MyApp from within tree" )

    set (APP_NAME myapp)

    set (APP_DIR "${CMAKE_SOURCE_DIR}/myapp")

    if (EXISTS "${APP_DIR}/main.cpp")
        message(STATUS "Using ${APP_DIR}/main.cpp for compilation.")
    else()
        message(FATAL_ERROR "Unable to file ${APP_DIR}/main.cpp needed for compiling your code")
    endif()

    file(GLOB APP_SOURCES ${APP_DIR}/*.cpp)
    set(APP_INCLUDE_DIRS  ${APP_DIR})

    add_target("${APP_NAME}" "${APP_DIR}" "${APP_SOURCES}" "${APP_INCLUDE_DIRS}" "${APP_LIBRARIES}")

    list(APPEND NUCLEUS_ALL_TARGETS ${APP_NAME})

    include(cmake/install.cmake)
    include(cmake/package.cmake)

elseif(APP_BUILD_DIRECTORY)

    message(STATUS "Using app provided build directory ${APP_BUILD_DIRECTORY}")

else()

    set(APP_BUILD_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)
    message(STATUS "Using Nucleus example build directory ${APP_BUILD_DIRECTORY}")

endif()

if (APP_BUILD_DIRECTORY)
    # later could move this to a function and call with both examples and test directories.
    # make a list function parm so upper level projects can call with list to iterate.

    getsubdirlist(build_subdirs ${APP_BUILD_DIRECTORY})

    # Take a copy as we'll add to these in each loop (also libs?)
    set (APP_SOURCES_INITIAL ${APP_SOURCES})
    set (APP_INCLUDE_DIRS_INITIAL ${APP_INCLUDE_DIRS})

    foreach(subdir ${build_subdirs})

        if (NOT EXISTS ${APP_BUILD_DIRECTORY}/${subdir}/main.cpp)
            break()
        endif()

        set (APP_NAME ${subdir})
        set (APP_DIR "${APP_BUILD_DIRECTORY}/${subdir}")
        list(APPEND NUCLEUS_ALL_TARGETS ${APP_NAME})

        # Reset these to initial values
        set (APP_SOURCES ${APP_SOURCES_INITIAL})
        set (APP_INCLUDE_DIRS ${APP_INCLUDE_DIRS_INITIAL})

        # add additional target config
        if (EXISTS ${APP_BUILD_DIRECTORY}/${subdir}/CMakeLists.txt)
            include (${APP_BUILD_DIRECTORY}/${subdir}/CMakeLists.txt)
        endif()

        file(GLOB APP_SOURCES ${APP_SOURCES} ${APP_DIR}/*.cpp)
        set(APP_INCLUDE_DIRS  ${APP_INCLUDE_DIRS} ${APP_DIR})

        add_target("${APP_NAME}" "${APP_DIR}" "${APP_SOURCES}" "${APP_INCLUDE_DIRS}" "${APP_LIBRARIES}")

    endforeach()

    # Set up install and package only for the last built app - mainly for testing
    if (NOT NUCLEUS_INSTALL_INCLUDED)
        include(cmake/install.cmake)
    endif()

    if (NOT NUCLEUS_PACKAGE_INCLUDED)
        include(cmake/package.cmake)
    endif()

    message(STATUS "MAKE CHECK targets: " "${NUCLEUS_ALL_TARGETS}")

endif()

# Run test twice to try resolve https://github.com/axomem/nucleus/issues/58
add_custom_target(analyze
    COMMAND ${NUCLEUS_SOURCE_DIR}/util/sonar-build-wrapper.sh ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build . --target test
    COMMAND ${CMAKE_COMMAND} --build . --target test
    COMMAND ${NUCLEUS_SOURCE_DIR}/util/sonar-run.sh ${CMAKE_BINARY_DIR}
    COMMENT "Running SONAR analysis. If sonar not installed, use util/sonar-install.sh"
)

add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E env DOXYGEN_OUTPUT_DIRECTORY=${CMAKE_BINARY_DIR}/doc doxygen doc/main.Doxyfile.in
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/..
)

add_custom_target(check
        # issue here - parallel builds
        # message(STATUS "Building and Testing targets" "${NUCLEUS_ALL_TARGETS}")
        # COMMAND ${CMAKE_COMMAND} --parallel --build .
        COMMAND ${CMAKE_COMMAND} --build . --target test
        DEPENDS ${NUCLEUS_ALL_TARGETS}
)
