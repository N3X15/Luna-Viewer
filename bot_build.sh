#!/bin/bash

# This is a the master build script - it is intended to be run by BuildBot
# It is called by a wrapper script in the shared repository which sets up
# the environment from the various BuildParams files and does all the build 
# result post-processing.
#
# PLEASE NOTE:
#
# Oh god what am I doing

export FUCKING_BUILDBOT=0
cd indra
. BUILD.sh
echo "Return code $?"
if ["$?" -ne "0"]
	then
		export FUCKING_BUILDBOT=$?
fi
cd ..
#Relay exit code from BUILD to BUILDBOT
if [$FUCKING_BUILDBOT -ne 1]
	then
		exit $FUCKING_BUILDBOT
fi