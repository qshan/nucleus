#!/usr/bin/env bash

[[ ! -f "sonar-project.properties" ]] && { echo "Run from project root"; exit 1;}
[[ ! -d "build/sonar-bw-output" ]] && { echo "Unable to find sonar build-wrapper output. Run wrapper first"; exit 1;}

sonar_login=$(python3 -m keyring -b keyrings.alt.file.PlaintextKeyring get sonar login)

if [ -z "$sonar_login" ]; then

  if [ "$#" -ne 1 ]; then
      echo "Sonar Password not found in python keyring and not found on the command line, so exiting"
      echo "Use python3 -m keyring -b keyrings.alt.file.PlaintextKeyring get sonar login"
      exit 1
  else
      sonar_login="$1"
  fi

fi

# Get branch name
git_ref=$(git symbolic-ref HEAD)
replace="refs/heads/"
git_branch=${git_ref/"$replace"/}
echo "Using Git branch $git_branch"

~/.sonar/sonar-scanner-4.2.0.1873-linux/bin/sonar-scanner \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.organization=axomem \
  -Dsonar.login="$sonar_login" \
  -Dsonar.cfamily.threads="$(nproc)" \
  -Dsonar.branch.name="$git_branch"

# Dsonar.cfamily.cache.enabled=false
