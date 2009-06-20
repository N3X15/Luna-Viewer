rem Requires Cygwin with unzip and wget

wget -O fmod.zip http://www.fmod.org/index.php/release/version/fmodapi375win.zip
unzip fmod.zip
rm fmod.zip

cp fmodapi375win/api/inc/fmod.h			libraries/include/
cp fmodapi375win/api/inc/fmod_errors.h	libraries/include/
cp fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib_release/
cp fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib_debug/
cp fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib/release/
cp fmodapi375win/api/lib/fmodvc.lib 	libraries/i686-win32/lib/debug/
cp fmodapi375win/api/fmod.dll			indra/newview/
cp fmodapi375win/api/fmod.dll			indra/BUILD-VC80/newview/RelWithDebInfo/
cp fmodapi375win/api/fmod.dll			indra/BUILD-VC80/newview/Release/
cp fmodapi375win/api/fmod.dll			indra/BUILD-VC80/newview/Debug/

rm -rf fmodapi375win 
