Name ShoopedLife

SetCompressor /SOLID lzma

# Defines
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.3.0.75
!define COMPANY Anonymous
!define URL http://shoopedlife.nexisonline.net/

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_LICENSEPAGE_RADIOBUTTONS
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "branding.bmp"

# Included files
!include Sections.nsh
!include MUI2.nsh

# Reserved Files
ReserveFile "${NSISDIR}\Plugins\AdvSplash.dll"

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE license.txt
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile shoopedlife-w32-0.3.0.75.exe
InstallDir $PROGRAMFILES\ShoopedLife
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 0.3.0.75
VIAddVersionKey ProductName ShoopedLife
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription ""
VIAddVersionKey LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show

# Installer sections
Section -Main SEC0000
    RMDIR /r $INSTDIR
    SetOutPath $INSTDIR
    SetOverwrite on
    File /r staging\
    WriteRegStr HKLM "${REGKEY}\Components" Main 1
SectionEnd

SectionGroup /e Extras SECGRP0000
#   Section Copybot SEC0002
#       SetOutPath "C:\temp\"
#       SetOverwrite on
#       File "..\..\..\CopyBotSetup.exe"
#       Exec "C:\temp\CopyBotSetup.exe"
#   SectionEnd
SectionGroupEnd

Section -post SEC0003
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    RMDir /r $SMPROGRAMS\$StartMenuGroup
    SetOutPath $INSTDIR
	CreateDirectory $SMPROGRAMS\$StartMenuGroup
	CreateDirectory "$SMPROGRAMS\$StartMenuGroup\Other Grids"
	
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^Name).lnk" "$INSTDIR\ShoopedLife.exe" "-qa -god -multiple --notos=1 --grid=agni"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^Name) (No Goatse).lnk" "$INSTDIR\ShoopedLife.exe" "-qa -god -multiple --grid=agni"
    #SetOutPath "$SMPROGRAMS\$StartMenuGroup\Other Grids"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Other Grids\Second Life - Beta Grid.lnk" "$INSTDIR\ShoopedLife.exe" "-qa -god -multiple -notos --grid=aditi"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Other Grids\OSGrid.lnk" "$INSTDIR\ShoopedLife.exe" "-qa -god -multiple --notos=1 --loginuri=http://osgrid.org:8002"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Other Grids\Fort Longcat (Probably Down).lnk" "$INSTDIR\ShoopedLife.exe" "-qa -god -multiple --notos=1 --loginuri=http://longcat.patrioticnigras.org:9000/"     
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend



Section /o -un.Main UNSEC0000
    RmDir /r /REBOOTOK $INSTDIR
    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0003
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
    StrCpy $StartMenuGroup ShoopedLife
    Push $R1
    File /oname=$PLUGINSDIR\spltmp.bmp splash.bmp
    advsplash::show 1000 600 400 -1 $PLUGINSDIR\spltmp
    Pop $R1
    Pop $R1
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    StrCpy $StartMenuGroup ShoopedLife
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
    #!insertmacro SELECT_UNSECTION SaviorChan ${UNSEC0001}
    #!insertmacro SELECT_UNSECTION Copybot ${UNSEC0002}
FunctionEnd

# Section Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SECGRP0000} "Extra shit you can use."
!insertmacro MUI_FUNCTION_DESCRIPTION_END
