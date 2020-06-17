
message(STATUS "Adding Hello World Test")

include(${CMAKE_SOURCE_DIR}/test/test_framework_helpers.cmake)

add_test_case_python(hello_world)
set_tests_properties(hello_world       PROPERTIES FIXTURES_REQUIRED "api_server;api_ready")
