BUILDTYPE="Release"
#BUILDTYPE="RelWithDebInfo"
#BUILDTYPE="Debug"

export DISTCC_HOSTS="localhost/2 192.168.9.5/2 192.168.9.203/4"

MODULES=""
MODULES="${MODULES} SL" 	# Base module (handles types and stuff)
MODULES="${MODULES} GL" 	# GL module (rendering)
MODULES="${MODULES} Region"	# Region module

#Build the SL and GL Lua modules.
lua newview/swig/BuildModules.lua ${MODULES}

# Gather hooks in order to generate documentation.
find newview/ -name "*.cpp" -print | lua ../scripts/GetHooks.lua

CXX="distcc g++" ./develop.py --type=${BUILDTYPE} configure && ./develop.py --type=${BUILDTYPE} build #VERBOSE=1
