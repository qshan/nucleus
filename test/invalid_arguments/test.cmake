
message(STATUS "TEst case helper ${TEST_CASE_HELPERS}")
include(${TEST_CASE_HELPERS})

test_case_start()

execute_process(
        COMMAND ${TEST_EXE} --help
        OUTPUT_VARIABLE OUTPUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

test_case_check()  # should be ok

if (NOT OUTPUT MATCHES Usage)
    set (TEST_CASE_RESULT "Looking for Usage keyword, got ${OUTPUT}")
endif()

test_case_check()

# Unexpected parameter check
execute_process(
        COMMAND ${TEST_EXE} -cnfig --asdf
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR_OUTPUT
        TIMEOUT 60
        RESULTS_VARIABLE TEST_CASE_RESULT
)

if (NOT TEST_CASE_RESULT EQUAL 1)
    set (TEST_CASE_RESULT "Got incorrect response code. Expected 1, got ${TEST_CASE_RESULT}")
else()
    set (TEST_CASE_RESULT 0)
endif()

test_case_check()

if (NOT ERROR_OUTPUT MATCHES Usage)
    set (TEST_CASE_RESULT "Unexpected parameter check. Looking for Usage keyword, got ${OUTPUT}")
endif()

test_case_check()


test_case_end()
