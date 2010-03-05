BUILDTYPE="Release"
#BUILDTYPE="RelWithDebInfo"
#BUILDTYPE="Debug"

export DISTCC_HOSTS="localhost/2 192.168.9.5/2 192.168.9.203/4"

#Build the SL and GL Lua modules.
lua newview/swig/BuildModules.lua SL GL

CXX="distcc g++" ./develop.py --type=${BUILDTYPE} configure && ./develop.py --type=${BUILDTYPE} build #VERBOSE=1
