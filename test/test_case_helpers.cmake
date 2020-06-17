# This is run each time for each test case

set(TEST_CASE_DIR ${TEST_ROOT_DIR}/${TEST_NAME})
set(TEST_OUT_DIR ${CMAKE_BINARY_DIR}/${TEST_NAME})

message(STATUS "Running Test case ${TEST_NAME} | TEST_EXE ${TEST_EXE} | TEST_CASE_DIR ${TEST_CASE_DIR} | TEST_OUT_DIR ${TEST_OUT_DIR}")

function(test_case_start)
    message(STATUS "Clearing TEST Directories for ${TEST_OUT_DIR}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${TEST_OUT_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory   ${TEST_OUT_DIR})
endfunction()

function(test_case_check)
    message(STATUS "Checking Test case ${TEST_NAME} with result ${TEST_CASE_RESULT}")

    foreach(RESULT_ITEM ${TEST_CASE_RESULT})
        if (NOT ${RESULT_ITEM} EQUAL 0)
            message(FATAL_ERROR "TEST_CASE_RESULT was non-zero. Result was ${TEST_CASE_RESULT}")
        endif()
    endforeach()

endfunction()


function(test_case_end)
    message(STATUS "Finishing Test case ${TEST_NAME} with result ${TEST_CASE_RESULT}")

    test_case_check()

endfunction()

## @brief - Create a temporary directory for pmem files.
function(prepare_pmem_dir TEST_NAME)
    set(TEST_PMEM_DIR /tmp/pmem0/${TEST_NAME})
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${TEST_PMEM_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory   ${TEST_PMEM_DIR})
    if(NOT IS_DIRECTORY ${TEST_PMEM_DIR})
        message(FATAL_ERROR "Unable to create PMem directory ${TEST_PMEM_DIR}")
    endif()
    message(STATUS "Prepared PMEM dir for test ${TEST_NAME} with TEST_PMEM_DIR=${TEST_PMEM_DIR}")
    set(TEST_PMEM_DIR ${TEST_PMEM_DIR} PARENT_SCOPE)
endfunction()

# === Functions for setting and removing condition path file ===========================================================
# TODO - how to pass variable between different test runs. Tried a few approaches...

## @brief By setting the condition path, Nucleus will keep the app open until the file is cleared
function(set_nucleus_condition_path)

    #set(NUCLEUS_CONDITION_PATH ${TEST_PMEM_DIR}/condition_path_file)
    set(NUCLEUS_CONDITION_PATH /tmp/nucleus_condition_path_file)
    execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${NUCLEUS_CONDITION_PATH} RESULT_VARIABLE RESULT)
    if(NOT EXISTS ${NUCLEUS_CONDITION_PATH})
        message(FATAL_ERROR "Unable to set condition path file ${NUCLEUS_CONDITION_PATH} due to error ${RESULT}")
    endif()

    set(NUCLEUS_CONDITION_PATH ${NUCLEUS_CONDITION_PATH} PARENT_SCOPE) # push to parent to use in test, cleanup
    # set_property(GLOBAL PROPERTY NUCLEUS_CONDITION_PATH "${NUCLEUS_CONDITION_PATH}")
endfunction()


## @brief By setting condition_path to a non-existing file, Nucleus will start and stop the app
function(clear_nucleus_condition_path)

    #get_property(NUCLEUS_CONDITION_PATH GLOBAL PROPERTY NUCLEUS_CONDITION_PATH)
    set(NUCLEUS_CONDITION_PATH /tmp/nucleus_condition_path_file)

    if(NOT EXISTS ${NUCLEUS_CONDITION_PATH})
        message(FATAL_ERROR "Unable to clear condition path file as it is already cleared or doesn't exist - ${NUCLEUS_CONDITION_PATH}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${NUCLEUS_CONDITION_PATH} RESULT_VARIABLE RESULT)
    if(EXISTS ${NUCLEUS_CONDITION_PATH})
        message(FATAL_ERROR "Unable to clear condition path file due to error ${RESULT} - ${NUCLEUS_CONDITION_PATH}")
    endif()

endfunction()
