Var DesktopIconCheckbox
Var StartMenuEntryCheckbox
Var DefaultAppsButton

Function OptionsSilent

    !include "scripts\RegisterCapabilities.nsh"
    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'

    ; in silent mode we simply create the start menu entry and nothing else
    ${If} $MultiUser.InstallMode == "AllUsers"
        SetShellVarContext all
        CreateDirectory "$SMPROGRAMS\PreviewQt"
        CreateShortcut \
            "$SMPROGRAMS\PreviewQt\PreviewQt.lnk" \
            "$INSTDIR\files\previewqt.exe"
    ${Else}
        SetShellVarContext current
        CreateDirectory "$SMPROGRAMS\PreviewQt"
        CreateShortcut \
            "$SMPROGRAMS\PreviewQt\PreviewQt.lnk" \
            "$INSTDIR\files\previewqt.exe"
    ${EndIf}

FunctionEnd

; --------------------------------------------------------------------------
; Custom page
; --------------------------------------------------------------------------

Function OptionsPage

    !include "scripts\RegisterCapabilities.nsh"
    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'

    !insertmacro MUI_HEADER_TEXT "PreviewQt options" "Configure shortcuts and associations"

    nsDialogs::Create 1018
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}

    ; intro
    ${NSD_CreateLabel} 0 0 100% 35u \
        "PreviewQt is registered as possible default program for most of its supported formats in the Windows Settings. To have files open automatically in PreviewQt, open the Settings, navigate to 'Apps' and then 'Default Apps'. Find and click on PreviewQt, then you can set it as default for any file format you want."
    Pop $0

    ; Button-like link
    ${NSD_CreateButton} 5% 40u 90% 14u "Directly open Settings at its 'Default Apps' page"
    Pop $DefaultAppsButton
    ${NSD_OnClick} $DefaultAppsButton OpenDefaultApps

    ; Separator
    ${NSD_CreateHLine} 0 68u 100% 1u HLineBeforeDesktop

    ; Desktop icon
    ${NSD_CreateCheckbox} 0 75u 100% 12u "Create start menu entry"
    Pop $StartMenuEntryCheckbox
    ${NSD_Check} $StartMenuEntryCheckbox

    ; Desktop icon
    ${NSD_CreateCheckbox} 0 90u 100% 12u "Create desktop icon"
    Pop $DesktopIconCheckbox

    nsDialogs::Show
FunctionEnd


Function OpenDefaultApps
    ; Windows Settings URI for Default Apps.
    ExecShell "open" "ms-settings:defaultapps"
FunctionEnd


Function OptionsPageLeave

    ; Create desktop icon (if selected)
    ${NSD_GetState} $DesktopIconCheckbox $0
    ${If} $0 == ${BST_CHECKED}
        CreateShortcut "$desktop\PreviewQt.lnk" "$INSTDIR\files\previewqt.exe" "" "$INSTDIR\previewqt.ico" 0
        System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'
    ${EndIf}

    ; Create start menu entry (if selected)
    ${NSD_GetState} $StartMenuEntryCheckbox $0
    ${If} $0 == ${BST_CHECKED}
        ${If} $MultiUser.InstallMode == "AllUsers"
            SetShellVarContext all
            CreateDirectory "$SMPROGRAMS\PreviewQt"
            CreateShortcut \
                "$SMPROGRAMS\PreviewQt\PreviewQt.lnk" \
                "$INSTDIR\files\previewqt.exe"
        ${Else}
            SetShellVarContext current
            CreateDirectory "$SMPROGRAMS\PreviewQt"
            CreateShortcut \
                "$SMPROGRAMS\PreviewQt\PreviewQt.lnk" \
                "$INSTDIR\files\previewqt.exe"
        ${EndIf}
    ${EndIf}

FunctionEnd
