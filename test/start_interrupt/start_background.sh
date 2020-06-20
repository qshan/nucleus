#!/usr/bin/env bash
$1 "${@:2}" 2>/dev/null 1>/dev/null 0</dev/null &
echo "$!"
