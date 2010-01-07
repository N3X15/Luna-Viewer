BUILDTYPE="Release"
#BUILDTYPE="RelWithDebInfo"

export DISTCC_HOSTS="localhost/2 192.168.9.5/2 192.168.9.185/4 192.168.9.203/4"


cd newview;swig -lua -c++ -o LuaBase.cpp -oh LuaBase.h LuaBase.swig;cd ..
CXX="distcc g++" ./develop.py --type=${BUILDTYPE} configure && ./develop.py --type=${BUILDTYPE} build #VERBOSE=1
