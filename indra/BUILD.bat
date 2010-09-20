:Something's wrong with this, but I don't know what.
set BUILDTYPE=Release
:set BUILDTYPE=RelWithDebInfo
:set BUILDTYPE=Debug

: Clean out unused _Lua* files
rm newview/_Lua*

set MODULES=
set MODULES=%MODULES% SL
:set MODULES=%MODULES% GL
:set MODULES=%MODULES% Region
:set MODULES=%MODULES% Object
:set MODULES=%MODULES% HUDEffects

:Build the SL and GL Lua modules.
lua5.1 newview/swig/BuildModules.lua %MODULES%

set GEN="VC90"
python ./develop.py --type=%BUILDTYPE% -G "%GEN%" configure -DPACKAGE:BOOL=TRUE -DLL_TESTS:BOOL=FALSE
:python ./develop.py --type=%BUILDTYPE% -G "%GEN%" build
rem VERBOSE=1
pause