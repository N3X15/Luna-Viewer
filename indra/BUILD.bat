set BUILDTYPE="Release"
rem BUILDTYPE="RelWithDebInfo"
rem BUILDTYPE="Debug"

set GEN="Visual Studio 8 2005"
set GEN="VC90"
cd newview
swig -lua -c++ -o LuaBase.cpp -oh LuaBase.h LuaBase.swig
cd ..
python ./develop.py --type=%BUILDTYPE% -G "%GEN%" configure
python ./develop.py --type=%BUILDTYPE% -G "%GEN%" build
rem VERBOSE=1
pause