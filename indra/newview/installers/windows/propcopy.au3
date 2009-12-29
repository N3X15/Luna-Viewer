#NoTrayIcon
#RequireAdmin

$fmod = ""
$llkdu = ""
$SLVoice = ""
$found = ""

$SLdir = RegRead("HKLM\SOFTWARE\Linden Research, Inc.\SecondLife", "")

CheckFiles()

$SLdir = RegRead("HKLM\SOFTWARE\Linden Research, Inc.\SecondLifeReleaseCandidate", "")

CheckFiles()

If $fmod <> "" Then
	$found &= "FMOD "
EndIf

If $llkdu <> "" Then
	$found &= "KDU "
EndIf

If $SLVoice <> "" Then
	$found &= "SLVoice "
EndIf

If $found <> "" Then
	$result = MsgBox(0x124, "File Copy", "The components necissary for GreenLife to run optimally " & @CRLF & $found & @CRLF & "have been found." & @CRLF & "Do you want to copy these files?")
	If $result = 6 Then
		If $fmod <> "" Then
			FileCopy($fmod, @ScriptDir, 1)
		EndIf
		If $llkdu <> "" Then
			FileCopy($llkdu, @ScriptDir, 1)
		EndIf
		If $SLVoice <> "" Then
;			$result = MsgBox(0x24, "File Copy", "Do you want to copy SLVoice, too?")
;			If $result = 6 Then
				FileCopy($SLVoice & "\SLVoice.exe", @ScriptDir, 1)
				FileCopy($SLVoice & "\vivoxsdk.dll", @ScriptDir, 1)
				FileCopy($SLVoice & "\alut.dll", @ScriptDir, 1)
				FileCopy($SLVoice & "\ortp.dll", @ScriptDir, 1)
				FileCopy($SLVoice & "\wrap_oal.dll", @ScriptDir, 1)
;			EndIf
		EndIf
	EndIf
Else
	MsgBox(0x40, "File Copy", "No suitable components or Second Life installations could be found." & @CRLF & "The installation will work anyways though some features may be unavailable." & @CRLF & @CRLF & "To retry this copy process after installation of Second Life 1.22.11" & @CRLF & "you can choose ""Copy proprietary files"" from start menu")
EndIf

;---

Func CheckFiles()
	Local $ver
	Local $len
		
	If $fmod = "" Then
		$ver = FileGetVersion($SLdir & "\fmod.dll")
		If $ver = "3.7.4.0" Or $ver = "3.7.5.0" Then
			$fmod = $SLdir & "\fmod.dll"
		EndIf
	EndIf
	
	If $llkdu = "" Then
		$len = FileGetSize($SLdir & "\llkdu.dll")
		If $len = 753664 Then
			$llkdu = $SLdir & "\llkdu.dll"
		EndIf
	EndIf
	
	If $SLVoice = "" Then
		$ver = FileGetVersion($SLdir & "\vivoxsdk.dll")
		$len = FileGetSize($SLdir & "\SLVoice.exe")
		If $ver = "2.1.3010.6151" And $len = 946176 Then
			$SLVoice = $SLdir
		EndIf
	EndIf
EndFunc