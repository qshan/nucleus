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

# To run a test case directly, first run `export PYTHONPATH=<path_to_nucleus_test_directory>`
# If this module is not being found in IDE, set Nucleus test directory as a sources directory

# This allows dictionary dot notation access, like dict.key = value
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
    for parm in sys.argv:
        parm_without_d = parm[2:]
        if "=" not in parm_without_d:
            continue
        parm_parts = parm_without_d.split("=")
        ctest[parm_parts[0]] = parm_parts[1]

    print(f"\n\nStarting {ctest.TEST_NAME}  ===============================================")
    print(f"CTEST Vars:" + str(ctest))


def test_case_end():
    print(f"Successfully completing {ctest.TEST_NAME}=====================================\n\n")
    return 0


class test_case_utils:
    # this can be extended for other util functions
    @staticmethod
    def get_server_url():
        return f"{ctest.TEST_SERVER_SCHEME}://{ctest.TEST_SERVER_ADDRESS}:{ctest.TEST_SERVER_PORT}"
