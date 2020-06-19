#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    echo "Include the sonar password on the command line"
fi


git_ref=$(git symbolic-ref HEAD)
replace="refs/heads/"
git_branch=${git_ref/"$replace"/}
echo "Using Git branch $git_branch"

~/.sonar/sonar-scanner-4.2.0.1873-linux/bin/sonar-scanner \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.organization=axomem \
  -Dsonar.login="$1" \
  -Dsonar.branch.name="$git_branch" \
  -Dsonar.cfamily.threads=4


#  -Dsonar.cfamily.build-wrapper-output=bw-output \