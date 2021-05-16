
# TODO - Add multiple interrupt. Switch around the background process. Launch a background kill process that waits 5 secs
#        then runs kill command. Check the return code from Nucleus is ok. Also support multiple.
# TODO - Also check SIGINT vs SIGTERM. Maybe split this case
# TODO - if pmempool exists, also create pool and check layout tag

include(${TEST_CASE_HELPERS})

test_case_start()

prepare_pmem_dir(${TEST_NAME})

get_server_url(SERVER_URL)

get_background_task_vars()

execute_process(
        COMMAND ${BACKGROUND_TASK_SHELL} ${BACKGROUND_TASK_START} ${TEST_OUT_DIR} ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus-background.log
        --log_level=trace
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=32
        --rest_address=${TEST_SERVER_ADDRESS}  --rest_port=${TEST_SERVER_PORT}
        ${TEST_EXE_EXTRA_START_VARS}
        COMMAND_ECHO STDOUT
        TIMEOUT 30
        OUTPUT_VARIABLE PROCESS_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Started - Process ID is ${PROCESS_ID}")

# Wait for API up
foreach(LOOP RANGE 5)
    execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1 )
    execute_process(COMMAND curl ${SERVER_URL}/api/v1/ping COMMAND_ECHO STDOUT RESULT_VARIABLE TEST_CASE_RESULT)
    if(TEST_CASE_RESULT EQUAL 0)
        break()
    endif()
endforeach()

test_case_check("Waiting for API ready")

# execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)

# Now send CTRL-C
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

execute_process(
        COMMAND ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus-retest.log
        --log_level=debug
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=32
        --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
        --condition_path=/tmp/thisfileshouldnotexist
        ${TEST_EXE_EXTRA_START_VARS}
        COMMAND_ECHO STDOUT
        TIMEOUT 30
        OUTPUT_VARIABLE PROCESS_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Confirm restart ok")

test_case_end()
