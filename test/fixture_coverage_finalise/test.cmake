
include(${TEST_CASE_HELPERS})

test_case_start()

IF(TEST_COVERAGE_ENABLED)
    # Seems to work but any files in directories end up at root - is this ok for Sonar?
    if (EXISTS ${TEST_COVERAGE_DIR})

        execute_process(COMMAND ${TEST_CASE_DIR}/run_gcov.sh
                        WORKING_DIRECTORY ${TEST_COVERAGE_DIR}
                        COMMAND_ECHO STDOUT

                        RESULT_VARIABLE TEST_CASE_RESULT)

        message(STATUS "Coverage executed in ${TEST_COVERAGE_DIR}")

        if(NOT EXISTS ${TEST_COVERAGE_DIR}/main.cpp.gcov)
            message(FATAL_ERROR "No GCOV results - can't find ${TEST_COVERAGE_DIR}/main.cpp.gcov"
                    " - are you sure you ran in DEBUG mode?")
        endif()

    else()
        message(FATAL_ERROR "Coverage directory not found - looking in ${TEST_COVERAGE_DIR}")
    endif()

else()

    set(TEST_CASE_RESULT 0)
    message(STATUS "TEST_COVERAGE_ENABLED not set")

endif()

test_case_end()
