#!/bin/bash
# AxoMem Nucleus - for developing Persistent Applications
# Copyright (C) 2018-2019 AxoMem Pte Ltd.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License Version 2 (only)
# as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License Version 2 for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/licenses/

if [ ! -f ./bin/nucleus ]; then
    echo "NUCLEUS executable not found. See README.MD for how to build your app"
    exit 1
fi
while true
do
    ./bin/nucleus --config_file=./nucleus.conf "$@"
    retVal=$?
    if [ $retVal -eq 0 ]; then
        echo "Exiting normally - no loop"
        exit ${retVal}
    fi

    read -t 10 -p "Program has exited abnormally. Waiting 10s. Press CTRL-C to exit"

    # Comment out the following line if you want auto restart
    exit ${retVal}
done
