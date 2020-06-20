#!/usr/bin/env bash

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


test_url=$'http://127.0.0.1:8080/api/v1/ping'

no_wrk=$'
This script requires \'wrk\' but it is not installed.
See https://github.com/wg/wrk/wiki/Installing-Wrk-on-Linux
Aborting.
'
command -v wrk >/dev/null 2>&1 || { echo >&2 "$no_wrk"; exit 1; }

echo "Running benchmark with wrk...  make sure nucleus is running on port 8080."
wrk -c 1 -t 1 --latency -d 15 ${test_url}
