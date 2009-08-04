# Microsoft Developer Studio Project File - Name="keygen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=keygen - Win32 DLL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "keygen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "keygen.mak" CFG="keygen - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "keygen - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "keygen - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "keygen - Win32 DLL Release" (based on "Win32 (x86) Console Application")
!MESSAGE "keygen - Win32 DLL Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "keygen - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin.vs\libgcrypt-1.2.2\release\static"
# PROP Intermediate_Dir "..\..\bin.vs\libgcrypt-1.2.2\release\static\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\inc.vs" /I "..\..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 user32.lib advapi32.lib libgcrypt.lib libgpg-error.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\bin.vs\libgcrypt-1.2.2\release\static" /libpath:"..\..\bin.vs\libgpg-error-1.1\release\static"

!ELSEIF  "$(CFG)" == "keygen - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin.vs\libgcrypt-1.2.2\debug\static"
# PROP Intermediate_Dir "..\..\bin.vs\libgcrypt-1.2.2\debug\static\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\inc.vs" /I "..\..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib advapi32.lib libgcrypt.lib libgpg-error.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\..\bin.vs\libgcrypt-1.2.2\debug\static" /libpath:"..\..\bin.vs\libgpg-error-1.1\debug\static"

!ELSEIF  "$(CFG)" == "keygen - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "keygen___Win32_DLL_Release"
# PROP BASE Intermediate_Dir "keygen___Win32_DLL_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin.vs\libgcrypt-1.2.2\release\dll"
# PROP Intermediate_Dir "..\..\bin.vs\libgcrypt-1.2.2\release\dll\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /I "..\..\inc.vs" /I "..\..\..\libgcrypt-1.2.2\src" /I "..\..\..\libgpg-error-1.1\src" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\inc.vs" /I "..\..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib libgcrypt-1.2.2.lib libgpg-error-1.1.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\bin.vs\libgcrypt-1.2.2\release\static" /libpath:"..\..\bin.vs\libgpg-error-1.1\release\static"
# ADD LINK32 user32.lib advapi32.lib libgcrypt.lib libgpg-error.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\bin.vs\libgcrypt-1.2.2\release\dll" /libpath:"..\..\bin.vs\libgpg-error-1.1\release\static"

!ELSEIF  "$(CFG)" == "keygen - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "keygen___Win32_DLL_Debug"
# PROP BASE Intermediate_Dir "keygen___Win32_DLL_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin.vs\libgcrypt-1.2.2\debug\dll"
# PROP Intermediate_Dir "..\..\bin.vs\libgcrypt-1.2.2\debug\dll\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\..\inc.vs" /I "..\..\..\libgcrypt-1.2.2\src" /I "..\..\..\libgpg-error-1.1\src" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "..\..\inc.vs" /I "..\..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib libgcrypt-1.2.2.lib libgpg-error-1.1.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\..\bin.vs\libgcrypt-1.2.2\debug\static" /libpath:"..\..\bin.vs\libgpg-error-1.1\debug\static"
# ADD LINK32 user32.lib advapi32.lib libgcrypt.lib libgpg-error.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\..\bin.vs\libgcrypt-1.2.2\debug\dll" /libpath:"..\..\bin.vs\libgpg-error-1.1\debug\static"

!ENDIF 

# Begin Target

# Name "keygen - Win32 Release"
# Name "keygen - Win32 Debug"
# Name "keygen - Win32 DLL Release"
# Name "keygen - Win32 DLL Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\..\libgcrypt-1.2.2\libgcrypt\tests\keygen.c"
# End Source File
# End Group
# End Target
# End Project
