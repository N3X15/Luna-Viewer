rem SHOOPEDLIFE STAGING SYSTEM
rem 	$Id$

@echo off

set RELEASE_LOCATION=%CD%
set _=..\indra\newview
set s=..\indra\build-VC80\newview\RelWithDebInfo
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

xcopy /s /y     %_%\app_settings\*              %RELEASE_LOCATION%\staging\app_settings\
xcopy /s /y     %_%\help\*                      %RELEASE_LOCATION%\staging\help\
xcopy /s /y     %_%\skins\*                     %RELEASE_LOCATION%\staging\skins\
xcopy /s /y     %_%\fonts\*                     %RELEASE_LOCATION%\staging\fonts\
xcopy /s /y     %_%\mozilla-theme\*             %RELEASE_LOCATION%\staging\mozilla-theme\
xcopy /s /y     %_%\character\*                 %RELEASE_LOCATION%\staging\character\
xcopy /s /y     %_%\*.exe          		%RELEASE_LOCATION%\staging\
xcopy /s /y     %s%\*.exe          		%RELEASE_LOCATION%\staging\
xcopy /s /y     %s%\*.dll          		%RELEASE_LOCATION%\staging\
xcopy /s /y     %_%\*.txt          		%RELEASE_LOCATION%\staging\
xcopy /s /y     %_%\dronesettings.xml          	%RELEASE_LOCATION%\staging\
xcopy /s /y     %_%\lua\*          		%RELEASE_LOCATION%\staging\lua\
xcopy /s /y     %_%\loading\*          		%RELEASE_LOCATION%\staging\loading\

rem bhear
copy %_%\vivox-runtime\i686-win32\tntk.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\libeay32.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\SLVoice.exe	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\ssleay32.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\SLVoiceAgent.exe	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\srtp.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\alut.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\vivoxsdk.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\ortp.dll	%RELEASE_LOCATION%\staging\ /y
copy %_%\vivox-runtime\i686-win32\wrap_oal.dll	%RELEASE_LOCATION%\staging\ /y

del %RELEASE_LOCATION%\staging\volumesettings.xml

echo --- STAGING COMPLETE!

pause
