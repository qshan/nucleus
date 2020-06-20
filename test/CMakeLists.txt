# AxoMem Nucleus - for developing Persistent Applications
# Copyright (C) 2018-2020 AxoMem Pte Ltd.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License Version 2 (only)
# as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License Version 2 for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/licenses/

# cmake_minimum_required(VERSION 3.17)
# v17 due to using CMAKE_CTEST_ARGUMENTS on root CMakeLists.txt file. Otherwise test result doesnt show if error.
# https://crascit.com/2016/10/18/test-fixtures-with-cmake-ctest/
# https://cmake.org/cmake/help/v3.7/prop_test/FIXTURES_REQUIRED.html#prop_test:FIXTURES_REQUIRED

message(STATUS "Executing Nucleus tests/CMakeLists.txt")

include(test_framework_helpers.cmake)

function (add_common_tests)

    message(STATUS "Adding common tests for ${APP_NAME}")

    add_test_case(invalid_arguments)
    add_test_case(start_stop)
    add_test_case(start_interrupt)
    add_test_case(fixture_start_server)
    add_test_case(fixture_stop_server)
    add_test_case(fixture_coverage_finalise)
    add_test_case(api_ready)

    add_test_case_property(start_stop                PROPERTIES FIXTURES_SETUP pre_api_server)
    add_test_case_property(start_interrupt           PROPERTIES FIXTURES_SETUP pre_api_server)
    add_test_case_property(invalid_arguments         PROPERTIES FIXTURES_SETUP pre_api_server)
    add_test_case_property(fixture_start_server      PROPERTIES FIXTURES_SETUP api_server)
    add_test_case_property(api_ready                 PROPERTIES FIXTURES_SETUP api_server)
    add_test_case_property(api_ready                 PROPERTIES DEPENDS fixture_start_server)
    add_test_case_property(api_ready                 PROPERTIES FIXTURES_REQUIRED pre_api_server)

    add_test_case_property(fixture_start_server      PROPERTIES FIXTURES_REQUIRED pre_api_server)
    add_test_case_property(fixture_stop_server       PROPERTIES FIXTURES_CLEANUP api_server)
    add_test_case_property(fixture_stop_server       PROPERTIES FIXTURES_REQUIRED pre_api_server)
    add_test_case_property(fixture_coverage_finalise PROPERTIES DEPENDS fixture_stop_server)  # final steps


    add_test_case_python(api_basic_nucleus)
    add_test_case_property(api_basic_nucleus       PROPERTIES FIXTURES_REQUIRED api_server)

endfunction()
