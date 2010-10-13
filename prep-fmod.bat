@echo off
rem Requires Cygwin with unzip and wget

wget -O fmod.zip http://www.fmod.org/files/fmod3/fmodapi375win.zip
unzip fmod.zip
rm fmod.zip

::cp -v fmodapi375win/api/inc/fmod.h		libraries/include/
::cp -v fmodapi375win/api/inc/fmod_errors.h	libraries/include/
cp -v fmodapi375win/api/inc/*.h	libraries/include/
cp -v fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib_release/
cp -v fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib_debug/
cp -v fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib/release/
cp -v fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib/debug/
cp -v fmodapi375win/api/fmod.dll		indra/newview/
cp -v fmodapi375win/api/fmod.dll		indra/BUILD-VC80/newview/RelWithDebInfo/
cp -v fmodapi375win/api/fmod.dll		indra/BUILD-VC80/newview/Release/
cp -v fmodapi375win/api/fmod.dll		indra/BUILD-VC80/newview/Debug/
cp -v fmodapi375win/api/fmod.dll		indra/BUILD-VC90/newview/RelWithDebInfo/
cp -v fmodapi375win/api/fmod.dll		indra/BUILD-VC90/newview/Release/
cp -v fmodapi375win/api/fmod.dll		indra/BUILD-VC90/newview/Debug/

rm -rf fmodapi375win 
