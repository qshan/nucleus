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

import sys
import json
import requests

# https://github.com/psf/requests/blob/master/requests/status_codes.py

# To run a test case directly, first run `export PYTHONPATH=<path_to_nucleus_test_directory>`
# If this module is not being found in IDE, set Nucleus test directory as a sources directory

# This allows dictionary dot notation access, like dict.key = value

test_banner = "============================================================="

class ObjDict(dict):
    def __getattr__(self, name):
        if name in self:
            return self[name]
        else:
            raise AttributeError("No such attribute: " + name)

    def __setattr__(self, name, value):
        self[name] = value

    def __delattr__(self, name):
        if name in self:
            del self[name]
        else:
            raise AttributeError("No such attribute: " + name)


# Global variable for holding CTEST vars
ctest = ObjDict({})


def test_case_start():
    global ctest
    ctest.TEST_NAME = "TEMPORARY_TEST"
    for parm in sys.argv:
        parm_without_d = parm[2:]
        if "=" not in parm_without_d:
            continue
        parm_parts = parm_without_d.split("=")
        ctest[parm_parts[0]] = parm_parts[1]

    print(f"Received CTEST Vars:" + str(ctest))
    print(f"\n=== Starting {ctest.TEST_NAME} {test_banner[:len(test_banner) - len(ctest.TEST_NAME)]}")


def test_case_end():
    print(f"=== PASS {ctest.TEST_NAME} {test_banner[:len(test_banner) - len(ctest.TEST_NAME) + 3]} \n\n")
    return 0


def test_case_check(msg, passed = True):
    status = "PASS" if passed else "FAIL"
    banner = "----------------------------------------------------------"
    if msg:
        print(f"--- {status} {msg} {banner[:len(banner)-len(msg)-1]}")
    else:
        print(f"--- {status} {banner}")


class test_case_utils:
    # this can be extended for other util functions
    @staticmethod
    def get_server_url():
        scheme = "http" if "TEST_SERVER_SCHEME" not in ctest else ctest.TEST_SERVER_SCHEME
        address = "localhost" if "TEST_SERVER_ADDRESS" not in ctest else ctest.TEST_SERVER_ADDRESS
        port = "8080" if "TEST_SERVER_PORT" not in ctest else ctest.TEST_SERVER_PORT
        return f"{scheme}://{address}:{port}"

    """ @copyright - https://stackoverflow.com/questions/25851183/how-to-compare-two-json-objects-with-the-same-elements-in-a-different-order-equa """
    # Note also issue re should lists be strictly ordered?
    @staticmethod
    def ordered(obj):
        if isinstance(obj, dict):
            return sorted((k, test_case_utils.ordered(v)) for k, v in obj.items())
        if isinstance(obj, list):  # should we disable this?
            return sorted(test_case_utils.ordered(x) for x in obj)
        else:
            return obj

    @staticmethod
    def json_equal(first, second):
        if test_case_utils.ordered(first) == test_case_utils.ordered(second):
            return True
        else:
            raise ValueError("ERROR different dicts:\n" + json.dumps(first, indent=2) + "\n------\n" + json.dumps(second, indent=2))

    @staticmethod
    def status_codes_equal(first, second):
        if first == second:
            return True
        else:
            print(f"\nERROR Response code different: First: {first} {requests.status_codes._codes[first][0]}"
                  f" | Second: {second} {requests.status_codes._codes[second][0]}\n")
            return False
