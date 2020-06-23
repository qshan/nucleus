#!/usr/bin/env bash
command -v gcov
# find . -name '*.o' -type f -exec gcov --all-blocks {} \;
find . -name '*.o' -type f -exec gcov {} \;
