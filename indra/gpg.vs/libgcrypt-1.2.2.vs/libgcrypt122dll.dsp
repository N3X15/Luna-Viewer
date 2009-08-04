# Microsoft Developer Studio Project File - Name="libgcrypt122dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libgcrypt122dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libgcrypt122dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgcrypt122dll.mak" CFG="libgcrypt122dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgcrypt122dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libgcrypt122dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libgcrypt122dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libgcrypt122dll___Win32_Release"
# PROP BASE Intermediate_Dir "libgcrypt122dll___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin.vs\libgcrypt-1.2.2\release\dll"
# PROP Intermediate_Dir "..\bin.vs\libgcrypt-1.2.2\release\dll\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /I ".\custom" /I "..\inc.vs" /I "..\..\libgcrypt-1.2.2\libgcrypt\cipher" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic" /I "..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "LIBGCRYPT122DLL_EXPORTS" /D "HAVE_CONFIG_H" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /I ".\custom" /I "..\inc.vs" /I "..\..\libgcrypt-1.2.2\libgcrypt\cipher" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic" /I "..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "LIBGCRYPT122DLL_EXPORTS" /D "HAVE_CONFIG_H" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib /nologo /dll /debug /machine:I386 /out:"..\bin.vs\libgcrypt-1.2.2\release\dll\libgcrypt.dll"
# SUBTRACT BASE LINK32 /map
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /dll /debug /machine:I386 /out:"..\bin.vs\libgcrypt-1.2.2\release\dll\libgcrypt.dll"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "libgcrypt122dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libgcrypt122dll___Win32_Debug"
# PROP BASE Intermediate_Dir "libgcrypt122dll___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin.vs\libgcrypt-1.2.2\debug\dll"
# PROP Intermediate_Dir "..\bin.vs\libgcrypt-1.2.2\debug\dll\obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I ".\custom" /I "..\inc.vs" /I "..\..\libgcrypt-1.2.2\libgcrypt\cipher" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic" /I "..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "LIBGCRYPT122DLL_EXPORTS" /D "HAVE_CONFIG_H" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I ".\custom" /I "..\inc.vs" /I "..\..\libgcrypt-1.2.2\libgcrypt\cipher" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi" /I "..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic" /I "..\..\libgcrypt-1.2.2\libgcrypt\src" /I "..\..\libgpg-error-1.1\libgpg-error\src" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "LIBGCRYPT122DLL_EXPORTS" /D "HAVE_CONFIG_H" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"..\bin.vs\libgcrypt-1.2.2\debug\dll/libgcrypt.dll" /pdbtype:sept
# SUBTRACT BASE LINK32 /map
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"..\bin.vs\libgcrypt-1.2.2\debug\dll/libgcrypt.dll" /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "libgcrypt122dll - Win32 Release"
# Name "libgcrypt122dll - Win32 Debug"
# Begin Group "cipher"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\ac.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\arcfour.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\bithelp.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\blowfish.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\cast5.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\cipher.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\crc.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\des.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\dsa.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\elgamal.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\md.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\md4.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\md5.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\primegen.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\pubkey.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rand-internal.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\random.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\random.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rfc2268.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rijndael.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rmd.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rmd160.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rndw32.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rsa.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\serpent.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\sha1.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\sha256.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\sha512.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\twofish.c"
# End Source File
# End Group
# Begin Group "mpi"

# PROP Default_Filter ""
# Begin Group "generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpi-asm-defs.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-add1.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-lshift.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-mul1.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-mul2.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-mul3.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-rshift.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\mpih-sub1.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\generic\udiv-w-sdiv.c"
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\longlong.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-add.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-asm-defs.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-bit.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-cmp.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-div.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-gcd.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-inline.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-inline.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-internal.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-inv.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-mpow.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-mul.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-pow.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpi-scan.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpicoder.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpih-div.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpih-mul.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\mpi\mpiutil.c"
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\ath.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\ath.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\cipher.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\g10lib.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\gcrypt-module.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\gcrypt.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\global.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\misc.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\missing-string.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\module.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\mpi.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\secmem.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\secmem.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\sexp.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\stdmem.c"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\stdmem.h"
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\src\types.h"
# End Source File
# End Group
# Begin Group "custom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\custom\config.h
# End Source File
# Begin Source File

SOURCE=".\custom\libgcrypt-1.2.2.def"
# End Source File
# End Group
# Begin Group "removed"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rndegd.c"
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rndlinux.c"
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\rndunix.c"
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="..\..\libgcrypt-1.2.2\libgcrypt\cipher\tiger.c"
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
