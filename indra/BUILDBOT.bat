::@echo off
@rem %1 = VC version to use (VS2005/VS2008*/VS2010)

@rem Dick with settings
call ..\scripts\automated_build_scripts\update_settings.bat

@rem Clean installables cache
rm ../installed.xml

@rem Set your build type here.  Release = no debugging symbols.  RelWithDebInfo = Optimized with debugging symbols. Debug = FAT AND SLOW, MUCH LIKE YOUR MOTHER
set BUILDTYPE=RelWithDebInfo

@rem TODO: Turn this into a horrific python monstrosity.
@rem	Gather hooks in order to generate documentation.
@rem	Will fail without cygwin, so find a better way.
@rem find newview/ -name "*.cpp" -print | C:\lua5.1\lua ../scripts/GetHooks.lua

@rem set GEN=NMake Makefiles
if /i %1 == vs2005		set GEN=Visual Studio 8 2005
if /i %1 == vs2005-x64	set GEN=Visual Studio 8 2005 Win64
if /i %1 == vs2008		set GEN=Visual Studio 9 2008
if /i %1 == vs2008-x64	set GEN=Visual Studio 9 2008 Win64
if /i %1 == vs2010		set GEN=Visual Studio 10
if /i %1 == vs2010-x64	set GEN=Visual Studio 10 Win64


@rem Prep for nmake horseassery.
cmake -G "%GEN%" -DUNATTENDED:BOOL=OFF -DSTANDALONE:BOOL=OFF -DWORD_SIZE:STRING=32 -DOPENSIM_RULES:BOOL=OFF -DROOT_PROJECT_NAME:STRING=Luna .
@rem python ./develop.py --type=%BUILDTYPE% -G "%GEN%" build

(set GEN=)
(set BUILDTYPE=)