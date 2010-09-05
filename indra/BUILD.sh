echo "*************************************************************"
echo "* WARNING: LUNA IS NOT TPV COMPLIANT. USE AT YOUR OWN RISK! *"
echo "*************************************************************"
echo ""
echo "See NOT_TPV_COMPLIANT.txt for more details."
echo ""
echo "Press any key to continue, CTRL+C to exit. >"
read

BUILDTYPE="Release"
#BUILDTYPE="RelWithDebInfo"
#BUILDTYPE="Debug"

export DISTCC_HOSTS="localhost/2 192.168.9.5/2 192.168.9.203/4"

MODULES=""
MODULES="${MODULES} SL" 	# Base module (handles types and stuff)
MODULES="${MODULES} GL" 	# GL module (rendering)
MODULES="${MODULES} Region"	# Region module
MODULES="${MODULES} Object"	# Object manipulation module.
MODULES="${MODULES} HUDEffects"	# HUD Effects module.

#Build the SL and GL Lua modules.
lua newview/swig/BuildModules.lua ${MODULES}

# Gather hooks in order to generate documentation.
find newview/ -name "*.cpp" -print | lua ../scripts/GetHooks.lua

CXX="distcc g++" ./develop.py --type=${BUILDTYPE} configure && ./develop.py --type=${BUILDTYPE} build #VERBOSE=1
