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

message(STATUS "Including nucleus/test/test_framework_helpers.cmake")
#message(STATUS "TEST PSD ${PROJECT_SOURCE_DIR} CSD ${CMAKE_SOURCE_DIR} CCSD ${CMAKE_CURRENT_SOURCE_DIR}"
#                "CLD ${CMAKE_CURRENT_LIST_DIR} NSD ${NUCLEUS_SOURCE_DIR}")

# TODO - move these into a function and allow calling from top level project. Pass nucleus subdir if needed - done
# TODO - add function to detect if in app or nucleus build, and add test prefixes. See [1] below

# For external use, call this from your parent CMakeLists to set nucleus environment up. eg
# nucleus_init(${PROJECT_SOURCE_DIR}/nucleus)
function(nucleus_init nucleus_directory)

    if(NOT EXISTS ${nucleus_directory}/test/test_framework_helpers.cmake)
        message(FATAL_ERROR "Nucleus subdirectory not found - looking for test_framework_helpers.cmake in "
                "${nucleus_directory}/test/\n")
    endif()

    set(NUCLEUS_SOURCE_DIR ${nucleus_directory} PARENT_SCOPE)

    if (NOT TEST_SERVER_PORT)  # may have been set from parent. If so, dont change
        getrandomport(TEST_SERVER_PORT)
        message(STATUS "Nucleus::init allocating new server port ${TEST_SERVER_PORT} ")
    else()
        # message(STATUS "Nucleus::init using existing server port ${TEST_SERVER_PORT} ")
    endif()

    if (NOT TEST_SERVER_SCHEME)
        set(TEST_SERVER_SCHEME http PARENT_SCOPE)
    endif()

    if (NOT TEST_SERVER_ADDRESS)
        set(TEST_SERVER_ADDRESS 127.0.0.1 PARENT_SCOPE)
    endif()

    set(TEST_SERVER_PORT ${TEST_SERVER_PORT} PARENT_SCOPE)

    # message(STATUS "Nucleus CMake environment initialised in ${nucleus_directory}")

endfunction()

# ----------------------------------------------------------------------------------------------------------------------

# add a CMAKE test case
function(add_test_case name )

    message(STATUS "Adding CMAKE test case ${name} for ${APP_NAME} in ${APP_DIR}")

    # TODO - move detection to a function and back up cmake_source_dir, prefix (nucleus/app)
    if (EXISTS ${ARGV1}/test/${name}/test.cmake)
        set (cmake_source_dir ${ARGV1})
    else()
        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/${name}/test.cmake)
            # being added from nucleus
            set (cmake_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
        else()
            if (EXISTS ${CMAKE_SOURCE_DIR}/test/${name}/test.cmake)
                # being added from higher level project
                set (cmake_source_dir ${CMAKE_SOURCE_DIR})
            else()
                message(FATAL_ERROR "Cannot find CMAKE file for test case ${name} at"
                        " ${CMAKE_CURRENT_SOURCE_DIR}/test/${name}/test.cmake"
                        " or ${CMAKE_SOURCE_DIR}/test/${name}/test.cmake")
            endif()
        endif()
    endif()
    #message(STATUS "TEST PSD ${PROJECT_SOURCE_DIR} CSD ${CMAKE_SOURCE_DIR} CCSD ${CMAKE_CURRENT_SOURCE_DIR}"
    #               " CLD ${CMAKE_CURRENT_LIST_DIR} NSD ${NUCLEUS_SOURCE_DIR}")
    #message(STATUS "TEST_EXE_EXTRA_START_VARS = ${TEST_EXE_EXTRA_START_VARS}")

    set(TEST_NAME "${APP_NAME}__${name}")

    add_test(NAME "${TEST_NAME}"
            COMMAND ${CMAKE_COMMAND}
            -DTEST_ROOT_DIR=${NUCLEUS_SOURCE_DIR}/test
            -DTEST_APP_DIR=${APP_DIR}
            -DTEST_PMEM_DIRS=${APP_PMEM_DIRS}
            -DTEST_CASE_HELPERS=${NUCLEUS_SOURCE_DIR}/test/test_case_helpers.cmake
            -DTEST_CASE_DIR=${cmake_source_dir}/test/${name}
            -DTEST_NAME=${TEST_NAME}
            -DTEST_EXE=$<TARGET_FILE:${APP_NAME}>
            -DTEST_OUT_DIR=${CMAKE_BINARY_DIR}/test/${APP_NAME}/${name}
            -DTEST_COVERAGE_ENABLED=${COVERAGE}
            -DTEST_COVERAGE_DIR=${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${APP_NAME}.dir
            -DTEST_SERVER_SCHEME=${TEST_SERVER_SCHEME}
            -DTEST_SERVER_ADDRESS=${TEST_SERVER_ADDRESS}
            -DTEST_SERVER_PORT=${TEST_SERVER_PORT}
            -DTEST_EXE_EXTRA_START_VARS=${TEST_EXE_EXTRA_START_VARS}
            -P ${cmake_source_dir}/test/${name}/test.cmake
            ${EXTRA_TEST_ARGS}
    )

    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT PMEM_IS_PMEM_FORCE=1)

endfunction()

# Add a python test case. ARGV1 is optional app_home directory (eg for xPatient), otherwise it will try to find it
function(add_test_case_python name)

    message(STATUS "Adding PYTHON test case ${name} for ${APP_NAME}")

    # message(STATUS "Directories ${APP_DIR} ${PROJECT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    #               ${NUCLEUS_SOURCE_DIR} V1 ${ARGV1}")

    if (EXISTS ${ARGV1}/test/${name}/test.py)
        set (cmake_source_dir ${ARGV1})
    else()
        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/${name}/test.py)
            # being added from nucleus
            set (cmake_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
        else()
            if (EXISTS ${CMAKE_SOURCE_DIR}/test/${name}/test.py)
                # being added from higher level project
                set (cmake_source_dir ${CMAKE_SOURCE_DIR})
            else()
                message(FATAL_ERROR "Cannot find Python file for ${name} at"
                                    " ${CMAKE_CURRENT_SOURCE_DIR}/test/${name}/test.py"
                                    " or ${CMAKE_SOURCE_DIR}/test/${name}/test.py")
            endif()
        endif()
    endif()

    set(TEST_NAME  "${APP_NAME}__${name}")

    add_test(NAME "${TEST_NAME}"
            COMMAND
            ${CMAKE_COMMAND} -E env PYTHONPATH=${NUCLEUS_SOURCE_DIR}/test
            ${Python3_EXECUTABLE} ${cmake_source_dir}/test/${name}/test.py
            -DTEST_ROOT_DIR=${NUCLEUS_SOURCE_DIR}/test
            -DTEST_APP_DIR=${APP_DIR}
            -DTEST_PMEM_DIRS=${APP_PMEM_DIRS}
            -DTEST_CASE_DIR=${cmake_source_dir}/test/${name}
            -DTEST_NAME=${TEST_NAME}
            -DTEST_EXE=$<TARGET_FILE:${APP_NAME}>
            -DTEST_OUT_DIR=${CMAKE_BINARY_DIR}/test/${APP_NAME}/${name}
            -DTEST_SERVER_SCHEME=${TEST_SERVER_SCHEME}
            -DTEST_SERVER_ADDRESS=${TEST_SERVER_ADDRESS}
            -DTEST_SERVER_PORT=${TEST_SERVER_PORT}
            -DTEST_EXE_EXTRA_START_VARS="${TEST_EXE_EXTRA_START_VARS}"
            ${EXTRA_TEST_ARGS}
            WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} )

    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT PMEM_IS_PMEM_FORCE=1)

endfunction()

# Wraps set_tests_properties to expand the test name to match above
function(add_test_case_property name placeholder property property_value)
    # `placeholder` argument is there so we support same syntax as set_tests_properties
    # TODO - Do we need to support lists in property_value?
    if("${property_value}" MATCHES ";")
        message(FATAL_ERROR "Lists in add_test_case_property not yet supported")
    endif()

    set(TEST_NAME "${APP_NAME}__${name}")
    set(PROPERTY_VALUE_EX "${APP_NAME}__${property_value}")

    set_tests_properties("${TEST_NAME}" PROPERTIES "${property}" "${PROPERTY_VALUE_EX}")

endfunction()


# Used for getting next IP Port so we dont conflict
function(getnextport result currentport)

    if (currentport STREQUAL "")
        message(FATAL_ERROR "Set port to a numerical value before calling getnextport")
    else()
        math(EXPR nextport "${currentport} + 1")
    endif()
    set(${result} ${nextport} PARENT_SCOPE)
    # message(STATUS "Nucleus::getnextport is ${nextport}")

endfunction()

function(getrandomport result)

    # Come up with port somewhere between 20000 and 60000 with 100 part breaks
    string(RANDOM LENGTH 2 ALPHABET 0123456789 number) # lower part
    math(EXPR number "${number} * 100")  # 100 part breaks
    string(RANDOM LENGTH 1 ALPHABET 0123 higher_number) # higher part of number to add to front
    set(full_number ${higher_number}${number})    # concat together
    math(EXPR final_number "${full_number} + 20000")
    set(${result} "${final_number}" PARENT_SCOPE)

    message(STATUS "Nucleus::getrandomport provided ${final_number}")

endfunction()
