@echo off
@rem Shared settings configurator.
@rem	Sets up settings.xml and company.
@set BASEDIR=%~dp0\..\..\indra\newview\app_settings
@set PER_ACCT=%BASEDIR%\settings_per_account.xml
@set ALL_ACCT=%BASEDIR%\settings.xml

@rem Settings values
set Settings=""
set Settings=%Settings%LunaCmdLineLua=/lua;
set Settings=%Settings%LunaCmdLineMacro=/m;
set Settings=%Settings%AscentDoubleClickTeleport=1;
set Settings=%Settings%RadarChatKeys=1;
set Settings=%Settings%AscentBuildAlwaysEnabled=1;
set Settings=%Settings%PrivateLookAt=1;
set Settings=%Settings%DefaultGrid=inworldz;
set Settings=%Settings%RenderUseStreamVBO=False

@rem Fix types
set Types=""
set Types=%Types%AscentDoubleClickTeleport=Boolean;
set Types=%Types%RadarChatKeys=Boolean;
set Types=%Types%AscentBuildAlwaysEnabled=Boolean;
set Types=%Types%PrivateLookAt=Boolean;
set Types=%Types%RenderUseStreamVBO=Boolean

echo *** Editing %ALL_ACCT%...
python.exe ../scripts/settings.py --target-file=%ALL_ACCT% --set=%Settings%
python.exe ../scripts/settings.py --target-file=%ALL_ACCT% --set-type=%Types%

echo *** Editing %PER_ACCT%...
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set=%Settings%
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set-type=%Types%

@(set BASEDIR=)
@(set PER_ACCT=)
@(set ALL_ACCT=)
@(set Settings=)
@(set Types=)