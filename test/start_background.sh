#!/usr/bin/env bash
# This file must be blank output except for the pid. Its used to kill later...
$2 "${@:3}" 2>"$1/stderr.log" 1>"$1/stdout.log" 0</dev/null &
echo "$!"
