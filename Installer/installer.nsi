!define NAME "SimFlight"
!define NAME_EXE "SimFlight.exe"
!define REGPATH_UNINSTSUBKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"

;--------------------------------
;General
Name "${NAME}"
OutFile "${NAME}-installer.exe"
Unicode True

InstallDir "" ; Don't set a default $InstDir so we can detect /D= and InstallDirRegKey
InstallDirRegKey HKCU "${REGPATH_UNINSTSUBKEY}" "UninstallString"
RequestExecutionLevel User

;--------------------------------
;Includes
!include LogicLib.nsh
!include WinCore.nsh
!include Integration.nsh
!include MUI2.nsh

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$InstDir\${NAME_EXE}"

;--------------------------------
;Pages
!insertmacro MUI_PAGE_LICENSE "LICENSE.md"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections
Function .onInit
  SetShellVarContext Current

  ${If} $InstDir == "" ; No /D= nor InstallDirRegKey?
    GetKnownFolderPath $InstDir ${FOLDERID_UserProgramFiles} ; This folder only exists on Win7+
    StrCmp $InstDir "" 0 +2 
    StrCpy $InstDir "$LocalAppData\Programs" ; Fallback directory

    StrCpy $InstDir "$InstDir\$(^Name)"
  ${EndIf}
FunctionEnd

Function un.onInit
  SetShellVarContext Current
FunctionEnd


Section "Program files (required)" SectionAppFiles
  SetOutPath $InstDir
  
  ;Uninstaller
  WriteUninstaller "$InstDir\Uninstall.exe"
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "DisplayName" "${NAME}"
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "DisplayIcon" "$InstDir\${NAME_EXE},0"
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "UninstallString" '"$InstDir\Uninstall.exe"'
  WriteRegStr HKCU "${REGPATH_UNINSTSUBKEY}" "QuietUninstallString" '"$InstDir\Uninstall.exe" /S'
  WriteRegDWORD HKCU "${REGPATH_UNINSTSUBKEY}" "NoModify" 1
  WriteRegDWORD HKCU "${REGPATH_UNINSTSUBKEY}" "NoRepair" 1

  ;Files
  File "${NAME_EXE}"
  File "config.ini"
  
  File "LICENSE.md"
  CreateDirectory "$InstDir\COPYING"
  SetOutPath "$InstDir\COPYING"
  File "COPYING\README.md"
  File "COPYING\GPL.md"
  File "COPYING\COMMITMENT"
  File "COPYING\SC.EULA.html"
  SetOutPath $InstDir
SectionEnd

Section "SimConnect (recommended)" SectionSimConnect
  SetOutPath $InstDir
  File "SimConnect.msi"
  ExecWait "msiexec /i $InstDir\SimConnect.msi"
SectionEnd

Section "Start Menu shortcut" SectionStartMenuShortcut
  SetOutPath $InstDir
  CreateShortcut "$SMPrograms\${NAME}.lnk" "$InstDir\${NAME_EXE}"
SectionEnd

Section "Desktop shortcut" SectionDesktopShortcut
  SetOutPath $InstDir
  CreateShortcut "$Desktop\${NAME}.lnk" "$InstDir\${NAME_EXE}"
SectionEnd

;--------------------------------
;Section descriptions
LangString DESC_SectionAppFiles ${LANG_ENGLISH} "Base program files"
LangString DESC_SectionSimConnect ${LANG_ENGLISH} "Recommended. SimConnect 10.0.63003.0 is needed by this program"
LangString DESC_SectionStartMenuShortcut ${LANG_ENGLISH} "Start Menu shortcut to SimFlight"
LangString DESC_SectionDesktopShortcut ${LANG_ENGLISH} "Desktop shortcut to SimFlight"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionAppFiles} $(DESC_SectionAppFiles)
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionSimConnect} $(DESC_SectionSimConnect)
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionStartMenuShortcut} $(DESC_SectionStartMenuShortcut)
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionDesktopShortcut} $(DESC_SectionDesktopShortcut)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller
Section -Uninstall
  Delete "$InstDir\${NAME_EXE}"
  Delete "$InstDir\config.ini"
  Delete "$InstDir\SimConnect.msi"
  Delete "$InstDir\log.txt"
  
  Delete "$InstDir\LICENSE.md"
  Delete "$InstDir\COPYING\README.md"
  Delete "$InstDir\COPYING\GPL.md"
  Delete "$InstDir\COPYING\COMMITMENT"
  Delete "$InstDir\COPYING\SC.EULA.html"
  RMDir "$InstDir\COPYING"
  
  Delete "$InstDir\Uninstall.exe"
  RMDir "$InstDir"
  
  DeleteRegKey HKCU "${REGPATH_UNINSTSUBKEY}"
  Delete "$Desktop\${NAME}.lnk"
  ${UnpinShortcut} "$SMPrograms\${NAME}.lnk"
  Delete "$SMPrograms\${NAME}.lnk"
SectionEnd
