@echo off

: Dick with settings

:2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control AscentBuildAlwaysEnabled
:2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control PrivateLookAt
set PER_ACCT=../indra/newview/app_settings/settings_per_account.xml
echo *** Editing indra/newview/app_settings/settings.xml...
python.exe ../scripts/settings.py --set=LunaCmdLineLua=/lua;LunaCmdLineMacro=/m;AscentDoubleClickTeleport=1;RadarChatKeys=1;AscentBuildAlwaysEnabled=1;PrivateLookAt=1
echo *** Editing indra/newview/app_settings/settings_per_account.xml...
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set=LunaCmdLineLua=/lua;LunaCmdLineMacro=/m;AscentDoubleClickTeleport=1;RadarChatKeys=1;AscentBuildAlwaysEnabled=1

: Set your build type here.  Release = no debugging symbols.  RelWithDebInfo = Optimized with debugging symbols. Debug = FAT AND SLOW, MUCH LIKE YOUR MOTHER
set BUILDTYPE=Release


:TODO: Turn this into a horrific python monstrosity.
: Gather hooks in order to generate documentation.
: Will fail without cygwin, so find a better way.
:find newview/ -name "*.cpp" -print | C:\lua5.1\lua ../scripts/GetHooks.lua

: Generator: VC80 = Visual C++ 2005, VC90 = Visual C++ 2008 (use this)
set GEN="VC90"
python ./develop.py --type=%BUILDTYPE% -G "%GEN%" configure -DPACKAGE:BOOL=TRUE -DLL_TESTS:BOOL=FALSE
:python ./develop.py --type=%BUILDTYPE% -G "%GEN%" build
rem VERBOSE=1
pause