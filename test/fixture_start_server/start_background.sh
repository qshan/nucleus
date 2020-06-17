#!/usr/bin/env bash
echo "Starting $1 in background with " "${@:2}"
$1 "${@:2}" 2>/dev/null 1>/dev/null 0</dev/null &
echo "Process ID is $!"
