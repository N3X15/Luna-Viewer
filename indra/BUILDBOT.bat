@echo off
@rem %1 = VC version to use (VS2005/VS2008*/VS2010)

@rem Dick with settings
call ..\scripts\automated_build_scripts\update_settings.bat

@rem Set your build type here.  Release = no debugging symbols.  RelWithDebInfo = Optimized with debugging symbols. Debug = FAT AND SLOW, MUCH LIKE YOUR MOTHER
set BUILDTYPE=Release

@rem TODO: Turn this into a horrific python monstrosity.
@rem	Gather hooks in order to generate documentation.
@rem	Will fail without cygwin, so find a better way.
@rem find newview/ -name "*.cpp" -print | C:\lua5.1\lua ../scripts/GetHooks.lua

set GEN=NMake Makefiles

@rem Prep for nmake horseassery.
cmake -G "%GEN%" -DUNATTENDED:BOOL=OFF -DSTANDALONE:BOOL=OFF -DWORD_SIZE:STRING=32 -DOPENSIM_RULES:BOOL=OFF -DROOT_PROJECT_NAME:STRING=Luna
@rem python ./develop.py --type=%BUILDTYPE% -G "%GEN%" build

set