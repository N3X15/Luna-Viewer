@echo off

set RELEASE_LOCATION="C:\ShoopedLife"
mkdir %RELEASE_LOCATION%

deltree %RELEASE_LOCATION%\staging\

Echo *** Making folders in %RELEASE_LOCATION%\staging\

mkdir %RELEASE_LOCATION%\staging\
mkdir %RELEASE_LOCATION%\staging\app_settings\
mkdir %RELEASE_LOCATION%\staging\help\
mkdir %RELEASE_LOCATION%\staging\skins\
mkdir %RELEASE_LOCATION%\staging\fonts\
mkdir %RELEASE_LOCATION%\staging\mozilla-theme\
mkdir %RELEASE_LOCATION%\staging\character\
mkdir %RELEASE_LOCATION%\staging\loading\
mkdir %RELEASE_LOCATION%\staging\lua\

ECHO *** Copying stuff

xcopy /s /y     .\app_settings\*                %RELEASE_LOCATION%\staging\app_settings\
xcopy /s /y     .\help\*                        %RELEASE_LOCATION%\staging\help\
xcopy /s /y     .\skins\*                       %RELEASE_LOCATION%\staging\skins\
xcopy /s /y     .\fonts\*                       %RELEASE_LOCATION%\staging\fonts\
xcopy /s /y     .\mozilla-theme\*               %RELEASE_LOCATION%\staging\mozilla-theme\
xcopy /s /y     .\character\*                   %RELEASE_LOCATION%\staging\character\
xcopy /s /y     .\Release\*          %RELEASE_LOCATION%\staging\
xcopy /s /y     .\lua\*          		%RELEASE_LOCATION%\staging\lua\
xcopy /s /y     .\loading\*          		%RELEASE_LOCATION%\staging\loading\

rem bhear
copy .\vivox-runtime\i686-win32\tntk.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\libeay32.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\SLVoice.exe	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\ssleay32.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\SLVoiceAgent.exe	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\srtp.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\alut.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\vivoxsdk.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\ortp.dll	%RELEASE_LOCATION%\staging\ /y
copy .\vivox-runtime\i686-win32\wrap_oal.dll	%RELEASE_LOCATION%\staging\ /y

echo Copying feature tables.
copy /y .\FeatureTable* %RELEASE_LOCATION%\staging\
copy /y .\lfs.dll %RELEASE_LOCATION%\staging\
copy /y .\Lua5.1.dll %RELEASE_LOCATION%\staging\
del %RELEASE_LOCATION%\staging\*.obj
del %RELEASE_LOCATION%\staging\*.res
del %RELEASE_LOCATION%\staging\*.idb
del %RELEASE_LOCATION%\staging\*.pch
del %RELEASE_LOCATION%\staging\*.pdb
del %RELEASE_LOCATION%\staging\*.dep
del %RELEASE_LOCATION%\staging\BuildLog.*
del %RELEASE_LOCATION%\staging\volumesettings.xml

echo --- STAGING COMPLETE!

pause
