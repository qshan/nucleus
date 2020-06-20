
# TODO - Add multiple interrupt. Switch around the background process. Launch a background kill process that waits 5 secs
#        then runs kill command. Check the return code from Nucleus is ok. Also support multiple.
# TODO - Also check SIGINT vs SIGTERM. Maybe split this case
# TODO - if pmempool exists, also create pool and check layout tag

include(${TEST_CASE_HELPERS})

test_case_start()

prepare_pmem_dir(${TEST_NAME})

execute_process(
        COMMAND ${TEST_CASE_DIR}/start_background.sh ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus.log
        --log_level=debug
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=10
        --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
        COMMAND_ECHO STDOUT
        TIMEOUT 30
        OUTPUT_VARIABLE PROCESS_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Started - Process ID is ${PROCESS_ID}")

execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)

# Now send CTRL-C
execute_process(
        COMMAND kill -SIGINT ${PROCESS_ID}
        COMMAND_ECHO STDOUT
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Process kill")

execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)

execute_process(
        COMMAND kill -0 ${PROCESS_ID}
        COMMAND_ECHO STDOUT
        RESULTS_VARIABLE EXIT_CODE
)

if(NOT EXIT_CODE)
    set(TEST_CASE_RESULT "KILL FAILED - Process with PID ${PROCESS_ID} is still running")
endif()

test_case_check("Confirm kill")

execute_process(
        COMMAND ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus.log
        --log_level=debug
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=10
        --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
        --condition_path=/tmp/thisfileshouldnotexist
        COMMAND_ECHO STDOUT
        TIMEOUT 30
        OUTPUT_VARIABLE PROCESS_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Confirm restart ok")

test_case_end()
