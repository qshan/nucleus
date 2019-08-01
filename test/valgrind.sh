#!/usr/bin/env bash
#
# Copyright (c) 2018-2019 AxoMem Pte Ltd. All rights reserved.
#

# This is an example valgrind test

/usr/local/bin/valgrind --tool=memcheck --xml=yes --xml-file=/tmp/valgrind --gen-suppressions=all --leak-check=full --leak-resolution=med --track-origins=yes ../build/bin/nucleus
