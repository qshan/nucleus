#!/usr/bin/env bash
git status
[[ ! $? -eq 0 ]] && { echo; echo 'ERROR: Not in git repo, exiting'; echo; exit 1;}
