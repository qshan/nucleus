# TODO - set this to loop so it executes faster
include(${TEST_CASE_HELPERS})

test_case_start()

get_server_url(SERVER_URL)

message(STATUS "SERVER URL IS ${SERVER_URL}")

foreach(LOOP RANGE 30)
    execute_process(COMMAND sleep 1 COMMAND_ECHO STDOUT RESULTS_VARIABLE TEST_CASE_RESULT)
    execute_process(COMMAND curl ${SERVER_URL}/api/v1/ready COMMAND_ECHO STDOUT RESULT_VARIABLE TEST_CASE_RESULT)
    if(TEST_CASE_RESULT EQUAL 0)
        break()
    endif()
endforeach()

test_case_end()
