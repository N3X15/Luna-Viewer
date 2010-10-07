:: Shared settings configurator.

::2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control AscentBuildAlwaysEnabled
::2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control PrivateLookAt
set PER_ACCT=../indra/newview/app_settings/settings_per_account.xml
echo *** Editing indra/newview/app_settings/settings.xml...
python.exe ../scripts/settings.py --set=LunaCmdLineLua=/lua;LunaCmdLineMacro=/m;AscentDoubleClickTeleport=1;RadarChatKeys=1;AscentBuildAlwaysEnabled=1;PrivateLookAt=1
python.exe ../scripts/settings.py --set-type=AscentDoubleClickTeleport=BOOL;RadarChatKeys=BOOL;AscentBuildAlwaysEnabled=BOOL;PrivateLookAt=BOOL

:: LL fucking with the TPV again, let's take our business elsewhere.
python.exe ../scripts/settings.py --set=DefaultGrid=inworldz

echo *** Editing indra/newview/app_settings/settings_per_account.xml...
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set=LunaCmdLineLua=/lua;LunaCmdLineMacro=/m;AscentDoubleClickTeleport=1;RadarChatKeys=1;AscentBuildAlwaysEnabled=1
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set-type=AscentDoubleClickTeleport=BOOL;RadarChatKeys=BOOL;AscentBuildAlwaysEnabled=BOOL;PrivateLookAt=BOOL

