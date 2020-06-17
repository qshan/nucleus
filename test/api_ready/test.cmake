# TODO - set this to loop so it executes faster
include(${TEST_ROOT_DIR}/test_case_helpers.cmake)

test_case_start()

foreach(LOOP RANGE 30)
    execute_process(COMMAND sleep 1 COMMAND_ECHO STDOUT RESULTS_VARIABLE TEST_CASE_RESULT)
    execute_process(COMMAND curl http://127.0.0.1:20000/api/v1/ready COMMAND_ECHO STDOUT RESULT_VARIABLE TEST_CASE_RESULT)
    if(TEST_CASE_RESULT EQUAL 0)
        break()
    endif()
endforeach()

test_case_end()
