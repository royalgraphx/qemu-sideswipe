;!/usr/bin/makensis

; This NSIS script creates an installer for QEMU on Windows.

; Copyright (C) 2006-2021 Stefan Weil
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) version 3 or any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

; NSIS_WIN32_MAKENSIS

!define PRODUCT "QEMU"
!define URL     "https://www.qemu.org/"

!define UNINST_EXE "$INSTDIR\qemu-uninstall.exe"
!define UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"

!ifndef BINDIR
!define BINDIR nsis.tmp
!endif
!ifndef DATADIR
!define DATADIR "${BINDIR}\share"
!endif
!ifndef SRCDIR
!define SRCDIR .
!endif
!ifndef ICONSDIR
!ifdef W64
!define ICONSDIR /mingw64/share/icons
!else
!define ICONSDIR /mingw32/share/icons
!endif
!endif
!ifndef OUTFILE
!define OUTFILE "qemu-setup.exe"
!endif

; Build a unicode installer
Unicode true

; Use maximum compression.
SetCompressor /SOLID lzma

!include "MUI2.nsh"

; The name of the installer.
Name "QEMU"

; The file to write
OutFile "${OUTFILE}"

; The default installation directory.
!ifdef W64
InstallDir $PROGRAMFILES64\qemu
!else
InstallDir $PROGRAMFILES\qemu
!endif

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
!ifdef W64
InstallDirRegKey HKLM "Software\qemu64" "Install_Dir"
!else
InstallDirRegKey HKLM "Software\qemu32" "Install_Dir"
!endif

; Request administrator privileges for Windows Vista.
RequestExecutionLevel admin

;--------------------------------
; Interface Settings.
;!define MUI_HEADERIMAGE "qemu-nsis.bmp"
; !define MUI_SPECIALBITMAP "qemu.bmp"
!define MUI_ICON "${SRCDIR}\pc-bios\qemu-nsis.ico"
!define MUI_UNICON "${SRCDIR}\pc-bios\qemu-nsis.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${SRCDIR}\pc-bios\qemu-nsis.bmp"
; !define MUI_HEADERIMAGE_BITMAP "qemu-install.bmp"
; !define MUI_HEADERIMAGE_UNBITMAP "qemu-uninstall.bmp"
; !define MUI_COMPONENTSPAGE_SMALLDESC
; !define MUI_WELCOMEPAGE_TEXT "Insert text here.$\r$\n$\r$\n$\r$\n$_CLICK"

;--------------------------------
; Pages.

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${SRCDIR}\COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_LINK $(Visit_QEMU_Wiki_Link)
!define MUI_FINISHPAGE_LINK_LOCATION "${URL}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages.

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Spanish"

;--------------------------------
; Functions.

Function .onInit
    !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd


;--------------------------------

; The stuff to install.
;
; Remember to keep the "Uninstall" section in sync.

Section "${PRODUCT}" QEMU_System_File_Section_Description

    SectionIn RO

    ; Set output path to the installation directory.
    SetOutPath "$INSTDIR"

    File "${SRCDIR}\COPYING"
    File "${SRCDIR}\COPYING.LIB"
    File "${SRCDIR}\README.rst"
    File "${SRCDIR}\VERSION"

    File /r "${BINDIR}\share"

    SetOutPath "$INSTDIR\lib\gdk-pixbuf-2.0\2.10.0"
    FileOpen $0 "loaders.cache" w
    FileClose $0

!ifdef W64
    SetRegView 64
!endif

    ; Write the installation path into the registry
    WriteRegStr HKLM SOFTWARE\${PRODUCT} "Install_Dir" "$INSTDIR"

    ; Write the uninstall keys for Windows
    WriteRegStr HKLM "${UNINST_KEY}" "DisplayName" "QEMU"
!ifdef DISPLAYVERSION
    WriteRegStr HKLM "${UNINST_KEY}" "DisplayVersion" "${DISPLAYVERSION}"
!endif
    WriteRegStr HKLM "${UNINST_KEY}" "UninstallString" '"${UNINST_EXE}"'
    WriteRegDWORD HKLM "${UNINST_KEY}" "NoModify" 1
    WriteRegDWORD HKLM "${UNINST_KEY}" "NoRepair" 1
    WriteUninstaller "qemu-uninstall.exe"
SectionEnd

Section "$(Tools_Section_Name)" Tools_Section_Description
    SetOutPath "$INSTDIR"
    File "${BINDIR}\qemu-edid.exe"
    File "${BINDIR}\qemu-ga.exe"
    File "${BINDIR}\qemu-img.exe"
    File "${BINDIR}\qemu-io.exe"
    File "${BINDIR}\qemu-nbd.exe"
    File "${BINDIR}\qemu-storage-daemon.exe"
SectionEnd

SectionGroup "$(System_Emulation_Section_Name)" System_Emulation_Section_Description

!include "${BINDIR}\system-emulations.nsh"

SectionGroupEnd

Section "$(Desktop_Icon_Section_Name)" Desktop_Icon_Section_Description
    SetOutPath "$INSTDIR\share"
    File /r ${ICONSDIR}
SectionEnd

!ifdef DLLDIR
Section "$(DLL_Library_Section_Name)" DLL_Library_Section_Description
    SetOutPath "$INSTDIR"
    File "${DLLDIR}\*.dll"
SectionEnd
!endif

!ifdef CONFIG_DOCUMENTATION
Section "$(Documentation_Section_Name)" Documentation_Section_Description
    ; SetOutPath "$INSTDIR\share\doc"
    ; File /r "${BINDIR}\share\doc"
    SetOutPath "$INSTDIR"
    CreateDirectory "$SMPROGRAMS\${PRODUCT}"
    CreateShortCut "$SMPROGRAMS\${PRODUCT}\$(Link_Description_User_Documentation).lnk" "$INSTDIR\share\doc\index.html" "" "$INSTDIR\share\doc\index.html" 0
SectionEnd
!endif

; Optional section (can be disabled by the user)
Section "$(Start_Menu_Section_Name)" Start_Menu_Section_Description
    CreateDirectory "$SMPROGRAMS\${PRODUCT}"
    CreateShortCut "$SMPROGRAMS\${PRODUCT}\$(Link_Description_Uninstall).lnk" "${UNINST_EXE}" "" "${UNINST_EXE}" 0
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall" Uninstall_Section_Description
    ; Remove registry keys
!ifdef W64
    SetRegView 64
!endif
    DeleteRegKey HKLM "${UNINST_KEY}"
    DeleteRegKey HKLM SOFTWARE\${PRODUCT}

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\${PRODUCT}\$(Link_Description_User_Documentation).lnk"
    Delete "$SMPROGRAMS\${PRODUCT}\$(Link_Description_Technical_Documentation).lnk"
    Delete "$SMPROGRAMS\${PRODUCT}\$(Link_Description_Uninstall).lnk"
    RMDir "$SMPROGRAMS\${PRODUCT}"

    ; Remove files and directories used
    Delete "$INSTDIR\COPYING"
    Delete "$INSTDIR\COPYING.LIB"
    Delete "$INSTDIR\README.rst"
    Delete "$INSTDIR\VERSION"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\qemu-*.exe"
    RMDir /r "$INSTDIR\lib"
    RMDir /r "$INSTDIR\share"
    ; Remove generated files
    Delete "$INSTDIR\stderr.txt"
    Delete "$INSTDIR\stdout.txt"
    ; Remove uninstaller
    Delete "${UNINST_EXE}"
    RMDir "$INSTDIR"
SectionEnd

; Include files with language installer strings.
; Language ID table - https://www.science.co.il/language/Locale-codes.php
; Language ID 1033 - English
; Language ID 1031 - German
; Language ID 1034 - Spanish
; Language ID 1036 - French
; Language ID 1040 - Italian
!addincludedir ${SRCDIR}/installer
!include installer_strings_english.nsh
!include installer_strings_french.nsh
!include installer_strings_german.nsh
!include installer_strings_italian.nsh
!include installer_strings_spanish.nsh


; ---------------------

; Descriptions (mouse-over).
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${QEMU_System_File_Section_Description}  $(QEMU_System_File_Section_Description_Text)
    !insertmacro MUI_DESCRIPTION_TEXT ${Tools_Section_Description}             $(Tools_Section_Description_Text)
    !insertmacro MUI_DESCRIPTION_TEXT ${System_Emulation_Section_Description}  $(System_Emulation_Section_Description_Text)
    !insertmacro MUI_DESCRIPTION_TEXT ${Desktop_Icon_Section_Description}      $(Desktop_Icon_Section_Description_Text)
    !insertmacro MUI_DESCRIPTION_TEXT ${Uninstall_Section_Description}         $(Uninstall_Section_Description_Text)
!include "${BINDIR}\system-mui-text.nsh"
!ifdef DLLDIR
    !insertmacro MUI_DESCRIPTION_TEXT ${DLL_Library_Section_Description}       $(DLL_Library_Section_Description_Text)
!endif
!ifdef CONFIG_DOCUMENTATION
    !insertmacro MUI_DESCRIPTION_TEXT ${Documentation_Section_Description}     $(Documentation_Section_Description_Text)
!endif
    !insertmacro MUI_DESCRIPTION_TEXT ${Start_Menu_Section_Description}        $(Start_Menu_Section_Description_Text)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
