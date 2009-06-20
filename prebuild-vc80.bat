@echo off

echo 	PYTHON	scripts\install.py kdu-binaries
python scripts\install.py kdu-binaries > install.log

echo 	PYTHON	scripts\install.py toluapp
python scripts\install.py toluapp >> install.log

echo 	CD	indra\newview
cd indra\newview\

echo 	TOLUA++	LuaBase.h
..\bin\tolua++.exe -o LuaBase.h LuaBase.pkg >> install.log

echo 	CD	..
cd ..

echo 	PYTHON	develop.py -G VC80 configure
python develop.py -G VC80 configure >> install.log

echo 	CD ..
cd ..

echo	CALL	prep-fmod.bat
call prep-fmod.bat

echo 	CALL	scripts\Prerun.bat
call scripts\prerun.bat VC80

pause
echo.
echo.
echo.
echo	Setup complete.
echo.
echo	You are now ready to compile.  If you are running an express version of Visual Studio, set secondlife-bin as the startup project and RelWithDebInfo as your configuration.
echo.
echo	In the event of an error during load that appears to have no meaning, check permissions on fmod.dll etc.  If that doesn't work, check with Dependancy Walker.
echo.
echo	Anything else is just typical shit found on SL's Common Compile Issues wiki page.
echo.
pause