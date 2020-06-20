# TODO - check that process is still running before exiting, via PID. See start_interrupt for info
# TODO - include check to make sure process has exited... need to check pid file again

include(${TEST_CASE_HELPERS})

test_case_start()

clear_nucleus_condition_path()

test_case_end()
