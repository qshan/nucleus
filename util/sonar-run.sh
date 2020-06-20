#!/usr/bin/env bash

[[ ! -f "sonar-project.properties" ]] && { echo "Run from project root"; exit 1;}
[[ ! -d "build/sonar-bw-output" ]] && { echo "Unable to find sonar build-wrapper output. Run wrapper first"; exit 1;}

sonar_login=$(secret-tool lookup sonar login)

if [ -z "$sonar_login" ]; then

  if [ "$#" -ne 1 ]; then
      echo "Sonar Password not found in secret-tool and not found on the command line, exiting"
      echo "use secret-tool store label='sonar_login' sonar login"
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
  -Dsonar.cfamily.cache.enabled=false \
  -Dsonar.cfamily.threads="$(nproc)" \
  -Dsonar.cfamily.build-wrapper-output=build/sonar-bw-output \
  -Dsonar.branch.name="$git_branch" \
  -Dsonar.cfamily.gcov.reportsPath=build/CMakeFiles/1-hello_world.dir
