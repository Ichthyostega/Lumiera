#!/bin/sh
#
# docker_open-lumiera-buildenv.sh  -  launch Docker container with interactive build shell
#
#
#------------------------------------------------------Setup
#
# Working directory path on the host
WORKDIR_HOST="/home/$USER/devel/lumi"
#
# bind-mount of the working dir in the container
WORKDIR_GUEST="/lumi"
#
# Docker image-ID to launch
DOCKER_IMAGE="ubuntu:bionic"
#
# Script code to inject into interactive shell
COMMANDS=$(cat) <<RUN_IN_CONTAINER
. /etc/bash.bashrc
. ~/.bashrc

alias la='ls -latr'

$WORKDIR_GUEST/admin/build_lumiera-build-dependencies.sh

RUN_IN_CONTAINER
#
#------------------------------------------------------Setup(End)
#

# Launch...
# (1) launch the docker container
# (2) start a bash there to process a bash commandline
# (3) this commandline in turn launches an interactive bash
# (4) and this interactive bash gets a startup-Shellscript to perform
# (5) and this shellscript is read from an temporary named pipe to another shellscript
# (6) which in turn prints the script code from the setup-variable $COMMANDS
#
# bash magic thanks to Jonathan Potter; see https://serverfault.com/a/586272
#
#
docker run -v $WORKDIR_HOST:$WORKDIR_GUEST -it $DOCKER_IMAGE bash -c "bash --rcfile <(echo \"${COMMANDS}\" )"


