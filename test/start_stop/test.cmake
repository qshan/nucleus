
include(${TEST_CASE_HELPERS})

test_case_start()

prepare_pmem_dir(${TEST_NAME})

# By setting condition_path to a non-existing file, Nucleus will start and stop the app

# Run first time to create
execute_process(
    COMMAND ${TEST_EXE}
                --log_file=${TEST_OUT_DIR}/nucleus-create.log
                --log_level=trace
                --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
                --pool_main_size=10
                --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
                --condition_path=/tmp/thisfileshouldnotexist
                ${TEST_EXE_EXTRA_START_VARS}
    COMMAND_ECHO STDOUT
    TIMEOUT 60
    RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check()

# Run again to open existing file
execute_process(
        COMMAND ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus-openexisting.log
        --log_level=trace
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=10
        --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
        --condition_path=/tmp/thisfileshouldnotexist
        ${TEST_EXE_EXTRA_START_VARS}
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_end()
