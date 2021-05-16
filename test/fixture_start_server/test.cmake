# TODO - choose random REST Port and check
# TODO - set PID based on test ID or name of some form... so can run parallel tests on server
include(${TEST_CASE_HELPERS})

test_case_start()

prepare_pmem_dir(${TEST_NAME})

set_nucleus_condition_path()

get_background_task_vars()

execute_process(
    COMMAND ${BACKGROUND_TASK_SHELL} ${BACKGROUND_TASK_START} ${TEST_OUT_DIR} ${TEST_EXE}
                --log_file=${TEST_OUT_DIR}/nucleus.log
                --log_level=trace
                --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
                --pool_main_size=32
                --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
                --condition_path=${NUCLEUS_CONDITION_PATH}
                ${TEST_EXE_EXTRA_START_VARS}
    COMMAND_ECHO STDOUT
    OUTPUT_VARIABLE PROCESS_ID
    TIMEOUT 30
    RESULTS_VARIABLE TEST_CASE_RESULT
)

message(STATUS "Process ID is ${PROCESS_ID}")

file(WRITE "${TEST_OUT_DIR}/PID_PORT_${TEST_SERVER_PORT}.log" ${PROCESS_ID})

test_case_end()
