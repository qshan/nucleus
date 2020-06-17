# TODO - choose random REST Port and check
# TODO - set PID based on test ID or name of some form... so can run parallel tests on server
include(${TEST_ROOT_DIR}/test_case_helpers.cmake)

test_case_start()

prepare_pmem_dir(${TEST_NAME})

set_nucleus_condition_path()

execute_process(
    COMMAND ${TEST_CASE_DIR}/start_background.sh ${TEST_EXE}
                --log_file=${TEST_OUT_DIR}/nucleus.log
                --log_level=debug
                --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
                --pool_main_size=10
                --rest_address=127.0.0.1 --rest_port=20000
                --condition_path=${NUCLEUS_CONDITION_PATH}
    COMMAND_ECHO STDOUT
    TIMEOUT 30
    RESULTS_VARIABLE TEST_CASE_RESULT
)

set(NUCLEUS_CONDITION_PATH ${NUCLEUS_CONDITION_PATH} PARENT_SCOPE)

test_case_end()
