#!/bin/bash
#
# Copyright (c) 2018-2019 AxoMem Pte Ltd. All rights reserved.
#
if [ ! -f ./build/bin/nucleus ]; then
    echo "NUCLEUS executable not found. See README.MD for how to build your app"
    exit 1
fi
while true
do
    ./build/bin/nucleus --config_file=./nucleus.conf "$@"
    retVal=$?
    if [ $retVal -eq 0 ]; then
        echo "Exiting normally - no loop"
        exit ${retVal}
    fi

    read -t 10 -p "Program has exited abnormally. Waiting 10s. Press CTRL-C to exit"

    # Comment out the following line if you want auto restart
    exit ${retVal}
done
