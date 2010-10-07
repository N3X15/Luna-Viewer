@echo off
:: %1 = VC version to use (VS2005/VS2008*/VS2010)

:: Dick with settings
call ..\scripts\automated_build_scripts\update_settings.bat

:: Set your build type here.  Release = no debugging symbols.  RelWithDebInfo = Optimized with debugging symbols. Debug = FAT AND SLOW, MUCH LIKE YOUR MOTHER
set BUILDTYPE=Release

::TODO: Turn this into a horrific python monstrosity.
::	Gather hooks in order to generate documentation.
::	Will fail without cygwin, so find a better way.
::find newview/ -name "*.cpp" -print | C:\lua5.1\lua ../scripts/GetHooks.lua

set GEN="NMake Buildscripts"

python ./develop.py --type=%BUILDTYPE% -G "%GEN%" configure -DPACKAGE:BOOL=TRUE -DLL_TESTS:BOOL=FALSE
::python ./develop.py --type=%BUILDTYPE% -G "%GEN%" build

:: Prep for nmake horseassery.
..\scripts\automated_build_scripts\env\%1.bat

pause