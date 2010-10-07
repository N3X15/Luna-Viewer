@rem Shared settings configurator.
@rem	Sets up settings.xml and company.
@set BASEDIR=%~dp0\..\..\indra\newview\app_settings
@set PER_ACCT=%BASEDIR%\settings_per_account.xml
@set ALL_ACCT=%BASEDIR%\settings.xml

FuckingSettings=""
FuckingSettings=%FuckingSettings%LunaCmdLineLua=/lua;
FuckingSettings=%FuckingSettings%LunaCmdLineMacro=/m;
@rem 2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control AscentBuildAlwaysEnabled
@rem 2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control PrivateLookAt
FuckingSettings=%FuckingSettings%AscentDoubleClickTeleport=1;
FuckingSettings=%FuckingSettings%RadarChatKeys=1;
FuckingSettings=%FuckingSettings%AscentBuildAlwaysEnabled=1;
FuckingSettings=%FuckingSettings%PrivateLookAt=1;
FuckingSettings=%FuckingSettings%DefaultGrid=inworldz;

@REM set PER_ACCT=../indra/newview/app_settings/settings_per_account.xml

echo *** Editing %ALL_ACCT%...
python.exe ../scripts/settings.py --target-file=%ALL_ACCT% --set=%FuckingSettings%
python.exe ../scripts/settings.py --target-file=%ALL_ACCT% --set-type=AscentDoubleClickTeleport=BOOL;RadarChatKeys=BOOL;AscentBuildAlwaysEnabled=BOOL;PrivateLookAt=BOOL

echo *** Editing %PER_ACCT%...
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set=%FuckingSettings%
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set-type=AscentDoubleClickTeleport=BOOL;RadarChatKeys=BOOL;AscentBuildAlwaysEnabled=BOOL;PrivateLookAt=BOOL

@(set BASEDIR=)
@(set PER_ACCT=)
@(set ALL_ACCT=)