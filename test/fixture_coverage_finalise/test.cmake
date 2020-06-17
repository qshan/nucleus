
include(${TEST_ROOT_DIR}/test_case_helpers.cmake)
# WIP
test_case_start()

if(COVERAGE)
    # Seems to work but any files in directories end up at root - is this ok for Sonar?
    execute_process(COMMAND find . -name '*.o' -type f -exec gcov --all-blocks {}\;
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles/${APP_NAME}.dir/src
                    COMMAND_ECHO STDOUT RESULT_VARIABLE TEST_CASE_RESULT)
else()

    set(TEST_CASE_RESULT 0)

endif()

test_case_end()
