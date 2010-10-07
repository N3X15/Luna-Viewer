:: Shared settings configurator.

set FuckingSettings=""
set FuckingSettings=%FuckingSettings%LunaCmdLineLua=/lua;
set FuckingSettings=%FuckingSettings%LunaCmdLineMacro=/m;
::2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control AscentBuildAlwaysEnabled
::2010-09-26T22:06:21Z WARNING: LLControlGroup::getBOOL: Invalid BOOL control PrivateLookAt
set FuckingSettings=%FuckingSettings%AscentDoubleClickTeleport=1;
set FuckingSettings=%FuckingSettings%RadarChatKeys=1;
set FuckingSettings=%FuckingSettings%AscentBuildAlwaysEnabled=1;
set FuckingSettings=%FuckingSettings%PrivateLookAt=1;
set FuckingSettings=%FuckingSettings%DefaultGrid=inworldz;

set PER_ACCT=../indra/newview/app_settings/settings_per_account.xml

echo *** Editing indra/newview/app_settings/settings.xml...
python.exe ../scripts/settings.py --set=%FuckingSettings%
python.exe ../scripts/settings.py --set-type=AscentDoubleClickTeleport=BOOL;RadarChatKeys=BOOL;AscentBuildAlwaysEnabled=BOOL;PrivateLookAt=BOOL

echo *** Editing indra/newview/app_settings/settings_per_account.xml...
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set=%FuckingSettings%
python.exe ../scripts/settings.py --target-file=%PER_ACCT% --set-type=AscentDoubleClickTeleport=BOOL;RadarChatKeys=BOOL;AscentBuildAlwaysEnabled=BOOL;PrivateLookAt=BOOL

