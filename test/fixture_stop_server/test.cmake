# TODO - check that process is still running before exiting, via PID. See start_interrupt for info
# TODO - include check to make sure process has exited... need to check pid file again

include(${TEST_CASE_HELPERS})

test_case_start()

file(READ "${TEST_OUT_DIR}/../fixture_start_server/PID_PORT_${TEST_SERVER_PORT}.log" PROCESS_ID)
string(STRIP "${PROCESS_ID}" PROCESS_ID)


get_background_task_vars()

# Send CTRL-C
execute_process(
        COMMAND ${BACKGROUND_TASK_SHELL} ${BACKGROUND_TASK_KILL} ${PROCESS_ID}
        COMMAND_ECHO STDOUT
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Process kill")

# Wait for program to exit
foreach(LOOP RANGE 30)
    execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1 )
    execute_process(COMMAND ${BACKGROUND_TASK_SHELL} ${BACKGROUND_TASK_CHECK} ${PROCESS_ID} COMMAND_ECHO STDOUT RESULTS_VARIABLE EXIT_CODE )
    if(NOT EXIT_CODE EQUAL 0)
        break()
    endif()
endforeach()

if(EXIT_CODE EQUAL 0)
    set(TEST_CASE_RESULT "KILL FAILED - Process with PID ${PROCESS_ID} is still running")
else()
    set(TEST_CASE_RESULT 0)
endif()

test_case_check("Confirm kill")

test_case_end()
