

set(TEST_ROOT_DIR ${PROJECT_SOURCE_DIR}/test)

set(GLOBAL_TEST_ARGS -DTEST_ROOT_DIR=${TEST_ROOT_DIR}
)

find_package (Python3 COMPONENTS Interpreter)
if(${Python3_FOUND})
    message(STATUS "Found Python3 Interpreter at ${Python3_EXECUTABLE}")
else()
    message(FATAL_ERROR "Python3 Interpreter not found")
endif()

function(add_test_case name )
    add_test(NAME ${name}
            COMMAND ${CMAKE_COMMAND}
            ${GLOBAL_TEST_ARGS}
            -DTEST_NAME=${name}
            -DTEST_EXE=$<TARGET_FILE:${APP_NAME}>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/${name}/test.cmake
    )

endfunction()

function(add_test_case_python name )

    add_test(NAME ${name}
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/${name}/test.py
            WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} )

endfunction()
