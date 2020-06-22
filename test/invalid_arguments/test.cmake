
message(STATUS "TEst case helper ${TEST_CASE_HELPERS}")
include(${TEST_CASE_HELPERS})

function(test_error_return value)
    if (NOT TEST_CASE_RESULT EQUAL ${value})
        set (TEST_CASE_RESULT "Got incorrect response code. Expected ${value}, got ${TEST_CASE_RESULT}" PARENT_SCOPE)
    else()
        set (TEST_CASE_RESULT 0 PARENT_SCOPE)
    endif()
endfunction()

test_case_start()

prepare_pmem_dir(${TEST_NAME})

execute_process(
        COMMAND ${TEST_EXE} --help
        COMMAND_ECHO STDOUT
        OUTPUT_VARIABLE OUTPUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check("Help is exit 0")

if (NOT OUTPUT MATCHES Usage)
    set (TEST_CASE_RESULT "Looking for Usage keyword, got ${OUTPUT}")
endif()

test_case_check("Got usage message")

# Unexpected parameter check
execute_process(
        COMMAND ${TEST_EXE} -cnfig --asdf
        COMMAND_ECHO STDOUT
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR_OUTPUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Unexpected parameter")

if (NOT ERROR_OUTPUT MATCHES Usage)
    set (TEST_CASE_RESULT "Unexpected parameter check. Looking for Usage keyword, got ${OUTPUT}")
endif()

test_case_check("Got usage message with unexpected parameter via STDERR")

execute_process(
        COMMAND ${TEST_EXE}
        --config_file=/dev/null/doesntexist.conf
        COMMAND_ECHO STDOUT
        TIMEOUT 30
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Invalid config file provided via command line")

execute_process(
        COMMAND ${TEST_EXE}
        --config_file=${TEST_CASE_DIR}/nucleus.conf
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=10
        --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        --condition_path=/tmp/thisfileshouldnotexist
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)
# Should return ok
test_case_check("Valid config file")


execute_process(
        COMMAND ${TEST_EXE}
        --config_file=${TEST_CASE_DIR}/nucleus-error.conf
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=10
        --rest_address=${TEST_SERVER_ADDRESS} --rest_port=${TEST_SERVER_PORT}
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        --condition_path=/tmp/thisfileshouldnotexist
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)
test_error_return(1)
test_case_check("Invalid config file")

execute_process(
        COMMAND ${TEST_EXE}
        --pool_main_file=/invalid/path/to/pool/file/name.pmem
        --pool_main_size=10
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Invalid pool file name")

execute_process(
        COMMAND ${TEST_EXE}
        --pool_main_file=/dev/null/pool_file.pmem
        --pool_main_size=10
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Invalid pool file location - dev/null")

execute_process(
        COMMAND ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        --pool_main_file=/pool_file.pmem
        --pool_main_size=10
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Invalid pool file locaton - permissions (should not have root access)")

execute_process(
        COMMAND ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=1
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Invalid pool size")

execute_process(
        COMMAND ${TEST_EXE}
        --log_file=${TEST_OUT_DIR}/nucleus${TEST_SUBCASE_NUMBER}.log
        --log_level=trace
        --pool_main_file=${TEST_PMEM_DIR}/${TEST_NAME}.pmem
        --pool_main_size=10
        --rest_port=100000
        COMMAND_ECHO STDOUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_error_return(1)
test_case_check("Invalid ReST Port")

test_case_end()
