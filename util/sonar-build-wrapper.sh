#!/usr/bin/env bash

[[ ! -z "$1" ]] && { echo "Changing to dir $1"; cd "$1";}

[[ ! -f "CMakeCache.txt" ]] && { echo "Run from a build directory. Current dir is $PWD"; exit 1;}

echo "Working directory is $PWD"

make clean
cmake .. -DCOVERAGE=ON
~/.sonar/build-wrapper-linux-x86/build-wrapper-linux-x86-64 --out-dir sonar-bw-output make -j "$(nproc)"
