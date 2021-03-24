#!/usr/bin/env bash

[[ ! -z "$1" ]] && { echo "Changing to dir $1"; cd "$1";}

echo;echo "Starting sonar-run.sh. My current directory is $PWD."

[[ ! -f "CMakeCache.txt" ]] && { echo; echo "ERROR: Run from a build directory"; echo; exit 1;}
[[ ! -d "sonar-bw-output" ]] && { echo; echo "ERROR: Unable to find sonar build-wrapper output. Run wrapper first"; echo; exit 1;}

sonar_login=$(python3 -m keyring -b keyrings.alt.file.PlaintextKeyring get sonar login)

if [ -z "$sonar_login" ]; then

  if [ "$#" -ne 1 ]; then
      echo "Sonar Password not found in python keyring and not found on the command line, so exiting"
      echo "Use python3 -m keyring -b keyrings.alt.file.PlaintextKeyring get sonar login"
      echo "Get a token from the sonarcloud website under User Profile > Security"
      exit 1
  else
      sonar_login="$1"
  fi

fi

echo "Checking for git presence..."
git status
if [ $? -eq 0 ]; then

  # Get branch name
  git_ref=$(git symbolic-ref HEAD)
  replace="refs/heads/"
  git_branch=${git_ref/"$replace"/}
  sonar_scm_disabled="False"
else
  git_branch="temporary_scan"
  sonar_scm_disabled="True"
fi
echo; echo "Using Git branch $git_branch with SCM disabled setting $sonar_scm_disabled"; echo
read -t 5 -p "Hit CTRL-C if incorrect or ENTER to continue within 5 seconds..."

build_dir="$PWD"
cd ..
~/.sonar/sonar-scanner-4.4.0.2170-linux/bin/sonar-scanner \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.organization=axomem \
  -Dsonar.login="$sonar_login" \
  -Dsonar.cfamily.threads="$(nproc)" \
  -Dsonar.branch.name="$git_branch" \
  -Dsonar.scm.disabled="$sonar_scm_disabled" \
  -Dsonar.cfamily.gcov.reportsPath="$build_dir" \
  -Dsonar.cfamily.build-wrapper-output="$build_dir"/sonar-bw-output
  # -Dsonar.cfamily.cache.enabled=false
  # Note that some settings are in sonar-project.properities in root
