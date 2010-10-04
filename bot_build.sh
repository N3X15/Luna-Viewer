#!/bin/sh

# This is a the master build script - it is intended to be run by BuildBot
# It is called by a wrapper script in the shared repository which sets up
# the environment from the various BuildParams files and does all the build 
# result post-processing.
#
# PLEASE NOTE:
#
# Oh god what am I doing

cd indra
. BUILD.sh
cd ..