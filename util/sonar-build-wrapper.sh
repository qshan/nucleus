#!/usr/bin/env bash

[[ ! -f "sonar-project.properties" ]] && { echo "Run from project root"; exit 1;}

[[ ! -d "build" ]] && mkdir build
cd build || exit 1

cmake .. -DCOVERAGE=ON
~/.sonar/build-wrapper-linux-x86/build-wrapper-linux-x86-64 --out-dir bw-output make
