@echo off
echo 	PYTHON	scripts\install.py toluapp
python scripts\install.py toluapp >> install.log
echo	CD	indra\newview
cd indra\newview\
echo	TOLUA++	LuaBase.h
..\bin\tolua++.exe -o LuaBase.h LuaBase.pkg >> install.log
echo	CD	..
cd ..
echo	PYTHON	develop.py -G VS2003
python develop.py -G VS2003 configure
echo	CREATING COMPILE BATCH FILE
echo cd indra > ..\COMPILE.bat
echo python develop.py -G VS2003 build >> ..\COMPILE.bat
pause