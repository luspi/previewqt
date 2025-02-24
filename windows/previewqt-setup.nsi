;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 2011-2024 Lukas Spies
; Contact: http://photoqt.org
;
; This file is part of PreviewQt.
;
; PreviewQt is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; PreviewQt is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; In order to use this file, the following files need to be placed in the
; SAME DIRECTORY AS THE APPLICATION FILES:
;
; - AdvUninstLog.nsh
; - FileAssociation.nsh
; - icon_install.ico
; - icon.ico
; - license.txt
; - previewqt-setup.nsi (this file)
;
; This will then create a new file in the application directory
; called previewqt-%version%.exe.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Unicode True

; modern ui
!include MUI2.nsh

; more flow control and logic
!include LogicLib.nsh

; allows creation of custom pages
!include nsDialogs.nsh

; macro for registering and unregistering file extensions
!include "FileAssociation.nsh"

; some main registry info, used for many things below
!define INSTDIR_REG_ROOT "HKLM"
!define INSTDIR_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\PreviewQt"

;include the Uninstall log header
!include AdvUninstLog.nsh

!define PREVIEWQT_VERSION "3.0"

; name of project and installer filename
Name "PreviewQt"
OutFile "previewqt-${PREVIEWQT_VERSION}.exe"

; this is a 64-bit program, thus install into 64-bit directory
InstallDir "$PROGRAMFILES64\PreviewQt"
InstallDirRegKey ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir"

; since we want to install into system location, request admin privileges
RequestExecutionLevel admin

; warn user on abort
!define MUI_ABORTWARNING

; a custom installer icon`
!define MUI_ICON "icon_install.ico"

; we have an interactive uninstall
!insertmacro INTERACTIVE_UNINSTALL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Welcome page

!define MUI_WELCOMEPAGE_TITLE "Welcome to the installer of PreviewQt"

!define MUI_WELCOMEPAGE_TEXT "This installer will guide you through the installation of the PreviewQt. It is recommended that you close all other applications before starting the installer. $\r$\n$\r$\nIf you have any questions or concerns, please contact the developer through his website:$\r$\n$\r$\nhttps://previewqt.org$\r$\n$\r$\n$\r$\n Click Next to continue."


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Final page

!define MUI_FINISHPAGE_RUN "$INSTDIR/previewqt.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Open PreviewQt"

!define MUI_FINISHPAGE_LINK "PreviewQt website: https://previewqt.org"
!define MUI_FINISHPAGE_LINK_LOCATION "https://previewqt.org"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; The order of pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
Page custom FinalStepsInit FinalStepsLeave
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


; Set the language if the installer to English
; This has to come AFTER the list of pages above
!insertmacro MUI_LANGUAGE "English"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; List all the files and store some meta info

Section "PreviewQt" SecDummy

    ;The output path for where to install files to.
    SetOutPath "$INSTDIR"

    ;We start by removing existing files.
    !insertmacro UNINSTALL.NEW_PREUNINSTALL "$INSTDIR"

    ;Open the uninstall log file.
    !insertmacro UNINSTALL.LOG_OPEN_INSTALL

    ;Recursively add all the files.
    File /r /x *nsh /x *nsi /x *qmlc /x previewqt-*.exe ".\"

    ;Close the uninstall log.
    !insertmacro UNINSTALL.LOG_CLOSE_INSTALL

    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir" "$INSTDIR"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "DisplayName" "PreviewQt"
    ;Same as create shortcut you need to use ${UNINST_EXE} instead of anything else.
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "UninstallString" "${UNINST_EXE} /S"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "QuietUninstallString" "${UNINST_EXE} /S"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "DisplayVersion" "${PREVIEWQT_VERSION}"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "Publisher" "Lukas Spies"

    IfSilent 0 +2
    Call FinalStepsLeave

SectionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Function .onInit

        ;prepare log always within .onInit function
        !insertmacro UNINSTALL.LOG_PREPARE_INSTALL

FunctionEnd

Function .onInstSuccess

         ;create/update log always within .onInstSuccess function
         !insertmacro UNINSTALL.LOG_UPDATE_INSTALL

FunctionEnd


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Ask the user whether to register for any filetypes

Var Dialog

Var LabelFiletypeDesc

Var RadioButtonNone
Var RadioButtonAll
Var RadioButtonAll_State

Var CheckboxPdfPs
Var CheckboxPdfPs_State
Var CheckboxPsdXcf
Var CheckboxPsdXcf_State

Var CheckboxStartMenu
Var CheckboxStartMenu_State
Var CheckboxDesktop
Var CheckboxDesktop_State

Function FinalStepsInit
    !insertmacro MUI_HEADER_TEXT "Finishing up" "$SMPROGRAMS\$StartMenuFolder"

    nsDialogs::Create 1018
    Pop $Dialog
    ${If} $Dialog == error
        Abort
    ${EndIf}

    ${NSD_CreateLabel} 0 0 100% 18u "Here you can set PreviewQt as default application for all supported file formats. If you decide against this, then you can always still open any file from inside PreviewQt"
    Pop $LabelFiletypeDesc

    ${NSD_CreateRadioButton} 0 20u 100% 12u "Do not set as default for any file formats"
    Pop $RadioButtonNone
    ${NSD_OnClick} $RadioButtonNone FinalStepsDisEnable

    ${NSD_CreateRadioButton} 0 33u 100% 12u "Set as default for all supported file formats"
    Pop $RadioButtonAll
    ${NSD_Check} $RadioButtonAll
    ${NSD_OnClick} $RadioButtonAll FinalStepsDisEnable

    ${NSD_CreateCheckbox} 0 48u 100% 12u "Include PDF and PS"
    Pop $CheckboxPdfPs

    ${NSD_CreateCheckbox} 0 61u 100% 12u "Include PSD and XCF"
    Pop $CheckboxPsdXcf

    ${NSD_CreateHLine} 0 83u 100% 1u HLineBeforeDesktop

    ${NSD_CreateCheckbox} 0 88u 100% 12u "Create Desktop Icon"
    Pop $CheckboxDesktop

    ${NSD_CreateCheckbox} 0 103u 100% 12u "Create Start menu entry"
    Pop $CheckboxStartMenu
    ${NSD_Check} $CheckboxStartMenu

    nsDialogs::Show

FunctionEnd

; pdf/psd/xcf checkboxes are only enabled when PreviewQt is to be set as default
Function FinalStepsDisEnable

    ${NSD_GetState} $RadioButtonAll $RadioButtonAll_State
    ${If} $RadioButtonAll_State == ${BST_CHECKED}
        EnableWindow $CheckboxPdfPs 1
        EnableWindow $CheckboxPsdXcf 1
    ${Else}
        EnableWindow $CheckboxPdfPs 0
        EnableWindow $CheckboxPsdXcf 0
    ${EndIf}

FunctionEnd

; perform actions based on user choices
Function FinalStepsLeave

    SetShellVarContext all

    ${NSD_GetState} $RadioButtonAll $RadioButtonAll_State
    ${NSD_GetState} $CheckboxPdfPs $CheckboxPdfPs_State
    ${NSD_GetState} $CheckboxPsdXcf $CheckboxPsdXcf_State
    ${NSD_GetState} $CheckboxDesktop $CheckboxDesktop_State
    ${NSD_GetState} $CheckboxStartMenu $CheckboxStartMenu_State

    IfSilent 0 +3
    StrCpy $RadioButtonAll_State ${BST_CHECKED}
    StrCpy $CheckboxStartMenu_State ${BST_CHECKED}

    ; The supported file formats can change between installs
    ; Thus we need to unregister all previous formats and re-register them below
    ${UnRegisterExtension} ".3fr" "Hasselblad Raw Image Format"
    ${UnRegisterExtension} ".aai" "AAI Dune image"
    ${UnRegisterExtension} ".ani" "Animated Windows cursors"
    ${UnRegisterExtension} ".apng" "Animated Portable Network Graphics"
    ${UnRegisterExtension} ".ari" "ARRIFLEX Raw Image Format"
    ${UnRegisterExtension} ".art" "1st Publisher"
    ${UnRegisterExtension} ".arw" "Sony Digital Camera Alpha Raw Image Format"
    ${UnRegisterExtension} ".asf" "Advanced Systems Format"
    ${UnRegisterExtension} ".avif" "AV1 Image File Format"
    ${UnRegisterExtension} ".avifs" "AV1 Image File Format"
    ${UnRegisterExtension} ".avs" "AVS X image"
    ${UnRegisterExtension} ".x" "AVS X image"
    ${UnRegisterExtension} ".mbfavs" "AVS X image"
    ${UnRegisterExtension} ".bay" "Casio Raw Image Format"
    ${UnRegisterExtension} ".bmp" "Microsoft Windows bitmap"
    ${UnRegisterExtension} ".dib" "Microsoft Windows bitmap"
    ${UnRegisterExtension} ".bpg" "Better Portable Graphics"
    ${UnRegisterExtension} ".cals" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".ct1" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".ct2" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".ct3" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".ct4" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".c4" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".cal" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".nif" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".ras" "Continuous Acquisition and Life-cycle Support Type 1 image"
    ${UnRegisterExtension} ".cap" "Phase One Raw Image Format"
    ${UnRegisterExtension} ".eip" "Phase One Raw Image Format"
    ${UnRegisterExtension} ".liq" "Phase One Raw Image Format"
    ${UnRegisterExtension} ".cb7" "Comic book archive"
    ${UnRegisterExtension} ".cbr" "Comic book archive"
    ${UnRegisterExtension} ".cbt" "Comic book archive"
    ${UnRegisterExtension} ".cbz" "Comic book archive"
    ${UnRegisterExtension} ".cg3" "CCITT Group 3"
    ${UnRegisterExtension} ".g3" "CCITT Group 3"
    ${UnRegisterExtension} ".crw" "Canon Digital Camera Raw Image Format"
    ${UnRegisterExtension} ".crr" "Canon Digital Camera Raw Image Format"
    ${UnRegisterExtension} ".cr2" "Canon Digital Camera Raw Image Format"
    ${UnRegisterExtension} ".cr3" "Canon Digital Camera Raw Image Format"
    ${UnRegisterExtension} ".cube" "Cube Color lookup table converted to a HALD image"
    ${UnRegisterExtension} ".cur" "Microsoft Windows cursor format"
    ${UnRegisterExtension} ".cut" "Dr. Halo"
    ${UnRegisterExtension} ".pal" "Dr. Halo"
    ${UnRegisterExtension} ".dcr" "Kodak Cineon Raw Image Format"
    ${UnRegisterExtension} ".kdc" "Kodak Cineon Raw Image Format"
    ${UnRegisterExtension} ".drf" "Kodak Cineon Raw Image Format"
    ${UnRegisterExtension} ".k25" "Kodak Cineon Raw Image Format"
    ${UnRegisterExtension} ".dcs" "Kodak Cineon Raw Image Format"
    ${UnRegisterExtension} ".dcx" "ZSoft IBM PC multi-page Paintbrush image"
    ${UnRegisterExtension} ".dds" "DirectDraw Surface"
    ${UnRegisterExtension} ".dfont" "Multi-face font package"
    ${UnRegisterExtension} ".dic" "Digital Imaging and Communications in Medicine (DICOM) image"
    ${UnRegisterExtension} ".dcm" "Digital Imaging and Communications in Medicine (DICOM) image"
    ${UnRegisterExtension} ".djvu" "DjVu digital document format "
    ${UnRegisterExtension} ".djv" "DjVu digital document format "
    ${UnRegisterExtension} ".dng" "Adobe Digital Negative Raw Image Format"
    ${UnRegisterExtension} ".dpx" "Digital Moving Picture Exchange"
    ${UnRegisterExtension} ".erf" "Epson Raw Image Format"
    ${UnRegisterExtension} ".exr" "OpenEXR"
    ${UnRegisterExtension} ".ff" "farbfeld"
    ${UnRegisterExtension} ".fits" "Flexible Image Transport System"
    ${UnRegisterExtension} ".fit" "Flexible Image Transport System"
    ${UnRegisterExtension} ".fts" "Flexible Image Transport System"
    ${UnRegisterExtension} ".fl32" "FilmLight floating point image format"
    ${UnRegisterExtension} ".ftx" "FAKK 2"
    ${UnRegisterExtension} ".gif" "Graphics Interchange Format"
    ${UnRegisterExtension} ".gpr" "GoPro GPR Raw Image Format"
    ${UnRegisterExtension} ".heif" "High Efficiency Image Format"
    ${UnRegisterExtension} ".heic" "High Efficiency Image Format"
    ${UnRegisterExtension} ".hrz" "Slow-scan television"
    ${UnRegisterExtension} ".icns" "Apple Icon Image"
    ${UnRegisterExtension} ".ico" "Microsoft Windows icon format"
    ${UnRegisterExtension} ".iff" "Interchange File Format"
    ${UnRegisterExtension} ".jbig" "Joint Bi-level Image experts Group file interchange format (JBIG)"
    ${UnRegisterExtension} ".jbg" "Joint Bi-level Image experts Group file interchange format (JBIG)"
    ${UnRegisterExtension} ".bie" "Joint Bi-level Image experts Group file interchange format (JBIG)"
    ${UnRegisterExtension} ".jng" "JPEG Network Graphics"
    ${UnRegisterExtension} ".jpeg" "Joint Photographic Experts Group JFIF format"
    ${UnRegisterExtension} ".jpg" "Joint Photographic Experts Group JFIF format"
    ${UnRegisterExtension} ".jpe" "Joint Photographic Experts Group JFIF format"
    ${UnRegisterExtension} ".jif" "Joint Photographic Experts Group JFIF format"
    ${UnRegisterExtension} ".jpeg2000" "JPEG-2000"
    ${UnRegisterExtension} ".j2k" "JPEG-2000"
    ${UnRegisterExtension} ".jp2" "JPEG-2000"
    ${UnRegisterExtension} ".jpc" "JPEG-2000"
    ${UnRegisterExtension} ".jpx" "JPEG-2000"
    ${UnRegisterExtension} ".jxl" "JPEG XL"
    ${UnRegisterExtension} ".jxr" "JPEG-XR"
    ${UnRegisterExtension} ".hdp" "JPEG-XR"
    ${UnRegisterExtension} ".wdp" "JPEG-XR"
    ${UnRegisterExtension} ".koa" "KOALA files"
    ${UnRegisterExtension} ".gg" "KOALA files"
    ${UnRegisterExtension} ".gig" "KOALA files"
    ${UnRegisterExtension} ".kla" "KOALA files"
    ${UnRegisterExtension} ".kra" "Krita Document"
    ${UnRegisterExtension} ".lbm" "Interlaced Bitmap"
    ${UnRegisterExtension} ".mat" "MATLAB image format"
    ${UnRegisterExtension} ".mdc" "Minolta/Agfa Raw Image Format"
    ${UnRegisterExtension} ".mef" "Mamiya Raw Image Format"
    ${UnRegisterExtension} ".miff" "Magick image file format"
    ${UnRegisterExtension} ".mif" "Magick image file format"
    ${UnRegisterExtension} ".mng" "Multiple-image Network Graphics"
    ${UnRegisterExtension} ".mos" "Leaf Raw Image Format"
    ${UnRegisterExtension} ".mpc" "Magick Persistent Cache image file format"
    ${UnRegisterExtension} ".mtv" "MTV ray tracer bitmap"
    ${UnRegisterExtension} ".pic" "MTV ray tracer bitmap"
    ${UnRegisterExtension} ".mvg" "Magick Vector Graphics"
    ${UnRegisterExtension} ".nef" "Nikon Digital SLR Camera Raw Image Format"
    ${UnRegisterExtension} ".nrw" "Nikon Digital SLR Camera Raw Image Format"
    ${UnRegisterExtension} ".ora" "OpenRaster"
    ${UnRegisterExtension} ".orf" "Olympus Digital Camera Raw Image Format"
    ${UnRegisterExtension} ".otb" "On-the-air Bitmap"
    ${UnRegisterExtension} ".otf" "OpenType font file"
    ${UnRegisterExtension} ".otc" "OpenType font file"
    ${UnRegisterExtension} ".ttf" "OpenType font file"
    ${UnRegisterExtension} ".ttc" "OpenType font file"
    ${UnRegisterExtension} ".p7" "Xv Visual Schnauzer thumbnail format"
    ${UnRegisterExtension} ".palm" "Palm pixmap"
    ${UnRegisterExtension} ".pam" "Portable Arbitrary Map format"
    ${UnRegisterExtension} ".pbm" "Portable bitmap format (black and white)"
    ${UnRegisterExtension} ".pcd" "Photo CD"
    ${UnRegisterExtension} ".pcds" "Photo CD"
    ${UnRegisterExtension} ".pcx" "ZSoft PiCture eXchange"
    ${UnRegisterExtension} ".pdb" "Palm Database ImageViewer Format"
    ${UnRegisterExtension} ".pef" "Pentax Raw Image Format"
    ${UnRegisterExtension} ".ptx" "Pentax Raw Image Format"
    ${UnRegisterExtension} ".pes" "Embrid Embroidery Format"
    ${UnRegisterExtension} ".pfb" "Postscript Type 1 font "
    ${UnRegisterExtension} ".pfm" "Postscript Type 1 font "
    ${UnRegisterExtension} ".afm" "Postscript Type 1 font "
    ${UnRegisterExtension} ".inf" "Postscript Type 1 font "
    ${UnRegisterExtension} ".pfa" "Postscript Type 1 font "
    ${UnRegisterExtension} ".ofm" "Postscript Type 1 font "
    ${UnRegisterExtension} ".pfm" "Portable Float Map"
    ${UnRegisterExtension} ".pgm" "Portable graymap format (gray scale)"
    ${UnRegisterExtension} ".pgx" "JPEG 2000 uncompressed format"
    ${UnRegisterExtension} ".phm" "Portable float map format 16-bit half"
    ${UnRegisterExtension} ".pic" "Softimage PIC"
    ${UnRegisterExtension} ".picon" "Personal Icon"
    ${UnRegisterExtension} ".pict" "QuickDraw/PICT"
    ${UnRegisterExtension} ".pct" "QuickDraw/PICT"
    ${UnRegisterExtension} ".pic" "QuickDraw/PICT"
    ${UnRegisterExtension} ".pix" "Alias/Wavefront RLE image format"
    ${UnRegisterExtension} ".als" "Alias/Wavefront RLE image format"
    ${UnRegisterExtension} ".alias" "Alias/Wavefront RLE image format"
    ${UnRegisterExtension} ".png" "Portable Network Graphics"
    ${UnRegisterExtension} ".ppm" "Portable pixmap format (color)"
    ${UnRegisterExtension} ".pnm" "Portable pixmap format (color)"
    ${UnRegisterExtension} ".ptiff" "Pyramid encoded TIFF"
    ${UnRegisterExtension} ".ptif" "Pyramid encoded TIFF"
    ${UnRegisterExtension} ".pxn" "Logitech Raw Image Format"
    ${UnRegisterExtension} ".qoi" "Quite OK image format"
    ${UnRegisterExtension} ".raf" "Fuji CCD Raw Image Format"
    ${UnRegisterExtension} ".raw" "Leica Raw Image Format"
    ${UnRegisterExtension} ".rwl" "Leica Raw Image Format"
    ${UnRegisterExtension} ".rgba" "SGI images"
    ${UnRegisterExtension} ".rgb" "SGI images"
    ${UnRegisterExtension} ".sgi" "SGI images"
    ${UnRegisterExtension} ".bw" "SGI images"
    ${UnRegisterExtension} ".rgbe" "Radiance RGBE image format"
    ${UnRegisterExtension} ".hdr" "Radiance RGBE image format"
    ${UnRegisterExtension} ".rad" "Radiance RGBE image format"
    ${UnRegisterExtension} ".rgf" "LEGO Mindstorms EV3 Robot Graphics File"
    ${UnRegisterExtension} ".rla" "Wavefront RLA File Format"
    ${UnRegisterExtension} ".rle" "Utah Run length encoded image file"
    ${UnRegisterExtension} ".rw2" "Panasonic Raw Image Format"
    ${UnRegisterExtension} ".scr" "ZX-Spectrum SCREEN"
    ${UnRegisterExtension} ".sct" "Scitex Continuous Tone Picture"
    ${UnRegisterExtension} ".ch" "Scitex Continuous Tone Picture"
    ${UnRegisterExtension} ".ct" "Scitex Continuous Tone Picture"
    ${UnRegisterExtension} ".sfw" "Seattle File Works image"
    ${UnRegisterExtension} ".alb" "Seattle File Works image"
    ${UnRegisterExtension} ".pwm" "Seattle File Works image"
    ${UnRegisterExtension} ".pwp" "Seattle File Works image"
    ${UnRegisterExtension} ".sixel" "DEC SIXEL Graphics Format"
    ${UnRegisterExtension} ".srf" "Sony (Minolta) Raw Image Format"
    ${UnRegisterExtension} ".mrw" "Sony (Minolta) Raw Image Format"
    ${UnRegisterExtension} ".sr2" "Sony (Minolta) Raw Image Format"
    ${UnRegisterExtension} ".srw" "Samsung Raw Image Format"
    ${UnRegisterExtension} ".sun" "SUN Rasterfile"
    ${UnRegisterExtension} ".ras" "SUN Rasterfile"
    ${UnRegisterExtension} ".sr" "SUN Rasterfile"
    ${UnRegisterExtension} ".im1" "SUN Rasterfile"
    ${UnRegisterExtension} ".im24" "SUN Rasterfile"
    ${UnRegisterExtension} ".im32" "SUN Rasterfile"
    ${UnRegisterExtension} ".im8" "SUN Rasterfile"
    ${UnRegisterExtension} ".rast" "SUN Rasterfile"
    ${UnRegisterExtension} ".rs" "SUN Rasterfile"
    ${UnRegisterExtension} ".scr" "SUN Rasterfile"
    ${UnRegisterExtension} ".svg" "Scalable Vector Graphics"
    ${UnRegisterExtension} ".svgz" "Scalable Vector Graphics"
    ${UnRegisterExtension} ".tar" "TAR file format"
    ${UnRegisterExtension} ".tga" "Truevision Targa image"
    ${UnRegisterExtension} ".icb" "Truevision Targa image"
    ${UnRegisterExtension} ".vda" "Truevision Targa image"
    ${UnRegisterExtension} ".vst" "Truevision Targa image"
    ${UnRegisterExtension} ".tiff" "Tagged Image File Format"
    ${UnRegisterExtension} ".tif" "Tagged Image File Format"
    ${UnRegisterExtension} ".tim" "PSX TIM (PlayStation Graphics)"
    ${UnRegisterExtension} ".ttf" "TrueType font file"
    ${UnRegisterExtension} ".vicar" "VICAR rasterfile format"
    ${UnRegisterExtension} ".vic" "VICAR rasterfile format"
    ${UnRegisterExtension} ".img" "VICAR rasterfile format"
    ${UnRegisterExtension} ".viff" "Khoros Visualization Image File Format"
    ${UnRegisterExtension} ".xv" "Khoros Visualization Image File Format"
    ${UnRegisterExtension} ".vtf" "Valve Texture Format"
    ${UnRegisterExtension} ".wbmp" "Wireless Bitmap"
    ${UnRegisterExtension} ".webp" "Google web image format"
    ${UnRegisterExtension} ".wmf" "Windows Metafile"
    ${UnRegisterExtension} ".wmz" "Windows Metafile"
    ${UnRegisterExtension} ".apm" "Windows Metafile"
    ${UnRegisterExtension} ".wpg" "Word Perfect Graphics File"
    ${UnRegisterExtension} ".xbm" "X BitMap"
    ${UnRegisterExtension} ".bm" "X BitMap"
    ${UnRegisterExtension} ".xpm" "X PixMap"
    ${UnRegisterExtension} ".pm" "X PixMap"
    ${UnRegisterExtension} ".xwd" "X Windows system window dump"
    ${UnRegisterExtension} ".eps" "Encapsulated PostScript"
    ${UnRegisterExtension} ".epsf" "Encapsulated PostScript"
    ${UnRegisterExtension} ".epsi" "Encapsulated PostScript"
    ${UnRegisterExtension} ".pdf" "Adobe Portable Document Format"
    ${UnRegisterExtension} ".ps" "Adobe Level III PostScript file"
    ${UnRegisterExtension} ".ps2" "Adobe Level III PostScript file"
    ${UnRegisterExtension} ".ps3" "Adobe Level III PostScript file"
    ${UnRegisterExtension} ".psd" "Adobe PhotoShop"
    ${UnRegisterExtension} ".psb" "Adobe PhotoShop"
    ${UnRegisterExtension} ".xcf" "Gimp XCF"


    ${If} $RadioButtonAll_State == ${BST_CHECKED}

        WriteRegStr HKCU "Software\PreviewQt" "fileformats" "all"

        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".3fr" "Hasselblad Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".aai" "AAI Dune image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ai" "Adobe Illustrator (PDF compatible)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ani" "Animated Windows cursors"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".apng" "Animated Portable Network Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ari" "ARRIFLEX Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".art" "1st Publisher"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".arw" "Sony Digital Camera Alpha Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".asf" "Advanced Systems Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".avif" "AV1 Image File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".avifs" "AV1 Image File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".avs" "AVS X image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".x" "AVS X image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mbfavs" "AVS X image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".bay" "Casio Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".bmp" "Microsoft Windows bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dib" "Microsoft Windows bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".bpg" "Better Portable Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cals" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ct1" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ct2" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ct3" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ct4" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".c4" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cal" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".nif" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ras" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cap" "Phase One Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".eip" "Phase One Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".liq" "Phase One Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cb7" "Comic book archive"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cbr" "Comic book archive"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cbt" "Comic book archive"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cbz" "Comic book archive"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cg3" "CCITT Group 3"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".g3" "CCITT Group 3"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".crw" "Canon Digital Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".crr" "Canon Digital Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cr2" "Canon Digital Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cr3" "Canon Digital Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cube" "Cube Color lookup table converted to a HALD image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cur" "Microsoft Windows cursor format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".cut" "Dr. Halo"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pal" "Dr. Halo"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dcr" "Kodak Cineon Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".kdc" "Kodak Cineon Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".drf" "Kodak Cineon Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".k25" "Kodak Cineon Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dcs" "Kodak Cineon Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dcx" "ZSoft IBM PC multi-page Paintbrush image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dds" "DirectDraw Surface"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dfont" "Multi-face font package"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dic" "Digital Imaging and Communications in Medicine (DICOM) image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dcm" "Digital Imaging and Communications in Medicine (DICOM) image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".djvu" "DjVu digital document format "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".djv" "DjVu digital document format "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dng" "Adobe Digital Negative Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".dpx" "Digital Moving Picture Exchange"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".erf" "Epson Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".exr" "OpenEXR"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ff" "farbfeld"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".fits" "Flexible Image Transport System"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".fit" "Flexible Image Transport System"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".fts" "Flexible Image Transport System"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".fl32" "FilmLight floating point image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ftx" "FAKK 2"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".gif" "Graphics Interchange Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".gpr" "GoPro GPR Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".heif" "High Efficiency Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".heic" "High Efficiency Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".hrz" "Slow-scan television"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".icns" "Apple Icon Image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ico" "Microsoft Windows icon format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".iff" "Interchange File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jbig" "Joint Bi-level Image experts Group file interchange format (JBIG)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jbg" "Joint Bi-level Image experts Group file interchange format (JBIG)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".bie" "Joint Bi-level Image experts Group file interchange format (JBIG)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jng" "JPEG Network Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jpeg" "Joint Photographic Experts Group JFIF format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jpg" "Joint Photographic Experts Group JFIF format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jpe" "Joint Photographic Experts Group JFIF format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jif" "Joint Photographic Experts Group JFIF format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jpeg2000" "JPEG-2000"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".j2k" "JPEG-2000"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jp2" "JPEG-2000"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jpc" "JPEG-2000"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jpx" "JPEG-2000"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jxl" "JPEG XL"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".jxr" "JPEG-XR"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".hdp" "JPEG-XR"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".wdp" "JPEG-XR"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".koa" "KOALA files"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".gg" "KOALA files"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".gig" "KOALA files"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".kla" "KOALA files"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".kra" "Krita Document"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".lbm" "Interlaced Bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mat" "MATLAB image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mdc" "Minolta/Agfa Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mef" "Mamiya Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".miff" "Magick image file format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mif" "Magick image file format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mng" "Multiple-image Network Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mos" "Leaf Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mpc" "Magick Persistent Cache image file format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mtv" "MTV ray tracer bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pic" "MTV ray tracer bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mvg" "Magick Vector Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".nef" "Nikon Digital SLR Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".nrw" "Nikon Digital SLR Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ora" "OpenRaster"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".orf" "Olympus Digital Camera Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".otb" "On-the-air Bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".otf" "OpenType font file"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".otc" "OpenType font file"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ttf" "OpenType font file"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ttc" "OpenType font file"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".p7" "Xv Visual Schnauzer thumbnail format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".palm" "Palm pixmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pam" "Portable Arbitrary Map format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pbm" "Portable bitmap format (black and white)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pcd" "Photo CD"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pcds" "Photo CD"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pcx" "ZSoft PiCture eXchange"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pdb" "Palm Database ImageViewer Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pef" "Pentax Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ptx" "Pentax Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pes" "Embrid Embroidery Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pfb" "Postscript Type 1 font "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pfm" "Postscript Type 1 font "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".afm" "Postscript Type 1 font "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".inf" "Postscript Type 1 font "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pfa" "Postscript Type 1 font "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ofm" "Postscript Type 1 font "
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pfm" "Portable Float Map"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pgm" "Portable graymap format (gray scale)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pgx" "JPEG 2000 uncompressed format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".phm" "Portable float map format 16-bit half"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pic" "Softimage PIC"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".picon" "Personal Icon"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pict" "QuickDraw/PICT"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pct" "QuickDraw/PICT"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pic" "QuickDraw/PICT"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pix" "Alias/Wavefront RLE image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".als" "Alias/Wavefront RLE image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".alias" "Alias/Wavefront RLE image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".png" "Portable Network Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ppm" "Portable pixmap format (color)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pnm" "Portable pixmap format (color)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ptiff" "Pyramid encoded TIFF"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ptif" "Pyramid encoded TIFF"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pxn" "Logitech Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".qoi" "Quite OK image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".raf" "Fuji CCD Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".raw" "Leica Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rwl" "Leica Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rgba" "SGI images"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rgb" "SGI images"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sgi" "SGI images"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".bw" "SGI images"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rgbe" "Radiance RGBE image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".hdr" "Radiance RGBE image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rad" "Radiance RGBE image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rgf" "LEGO Mindstorms EV3 Robot Graphics File"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rla" "Wavefront RLA File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rle" "Utah Run length encoded image file"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rw2" "Panasonic Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".scr" "ZX-Spectrum SCREEN"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sct" "Scitex Continuous Tone Picture"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ch" "Scitex Continuous Tone Picture"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ct" "Scitex Continuous Tone Picture"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sfw" "Seattle File Works image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".alb" "Seattle File Works image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pwm" "Seattle File Works image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pwp" "Seattle File Works image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sixel" "DEC SIXEL Graphics Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".srf" "Sony (Minolta) Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".mrw" "Sony (Minolta) Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sr2" "Sony (Minolta) Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".srw" "Samsung Raw Image Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sun" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ras" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".sr" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".im1" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".im24" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".im32" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".im8" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rast" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".rs" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".scr" "SUN Rasterfile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".svg" "Scalable Vector Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".svgz" "Scalable Vector Graphics"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".tar" "TAR file format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".tga" "Truevision Targa image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".icb" "Truevision Targa image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".vda" "Truevision Targa image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".vst" "Truevision Targa image"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".tiff" "Tagged Image File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".tif" "Tagged Image File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".tim" "PSX TIM (PlayStation Graphics)"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ttf" "TrueType font file"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".vicar" "VICAR rasterfile format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".vic" "VICAR rasterfile format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".img" "VICAR rasterfile format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".viff" "Khoros Visualization Image File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".xv" "Khoros Visualization Image File Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".vtf" "Valve Texture Format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".wbmp" "Wireless Bitmap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".webp" "Google web image format"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".wmf" "Windows Metafile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".wmz" "Windows Metafile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".apm" "Windows Metafile"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".wpg" "Word Perfect Graphics File"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".xbm" "X BitMap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".bm" "X BitMap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".xpm" "X PixMap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pm" "X PixMap"
        ${RegisterExtension} "$INSTDIR\previewqt.exe" ".xwd" "X Windows system window dump"

        ${If} $CheckboxPdfPs_State == ${BST_CHECKED}

            WriteRegStr HKCU "Software\PreviewQt" "fileformats_pdfps" "registered"

            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".eps" "Encapsulated PostScript"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".epsf" "Encapsulated PostScript"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".epsi" "Encapsulated PostScript"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".pdf" "Adobe Portable Document Format"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ps" "Adobe Level III PostScript file"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ps2" "Adobe Level III PostScript file"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".ps3" "Adobe Level III PostScript file"

        ${Else}

            ; if it was registered in a previous install, we need to de-register it here
            Var /GLOBAL fileformats_pdfps
            ReadRegStr $fileformats_pdfps HKCU "Software\PreviewQt" "fileformats_pdfps"
            ${If} $fileformats_pdfps == "registered"

                ${UnRegisterExtension} ".eps" "Encapsulated PostScript"
                ${UnRegisterExtension} ".epsf" "Encapsulated PostScript"
                ${UnRegisterExtension} ".epsi" "Encapsulated PostScript"
                ${UnRegisterExtension} ".pdf" "Adobe Portable Document Format"
                ${UnRegisterExtension} ".ps" "Adobe Level III PostScript file"
                ${UnRegisterExtension} ".ps2" "Adobe Level III PostScript file"
                ${UnRegisterExtension} ".ps3" "Adobe Level III PostScript file"

            ${EndIf}

            WriteRegStr HKCU "Software\PreviewQt" "fileformats_pdfps" ""

        ${EndIf}

        ${If} $CheckboxPsdXcf_State == ${BST_CHECKED}

            WriteRegStr HKCU "Software\PreviewQt" "fileformats_psdxcf" "registered"

            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".psd" "Adobe PhotoShop"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".psb" "Adobe PhotoShop"
            ${RegisterExtension} "$INSTDIR\previewqt.exe" ".xcf" "Gimp XCF"

        ${Else}

            ; if it was registered in a previous install, we need to de-register it here
            Var /GLOBAL fileformats_psdxcf
            ReadRegStr $fileformats_psdxcf HKCU "Software\PreviewQt" "fileformats_psdxcf"
            ${If} $fileformats_psdxcf == "registered"
                ${UnRegisterExtension} ".psd" "Adobe PhotoShop"
                ${UnRegisterExtension} ".psb" "Adobe PhotoShop"
                ${UnRegisterExtension} ".xcf" "Gimp XCF"
            ${EndIf}

            WriteRegStr HKCU "Software\PreviewQt" "fileformats_psdxcf" ""

        ${EndIf}

    ${EndIf}

    ${If} $CheckboxDesktop_State == ${BST_CHECKED}

        ; create desktop shortcut
        CreateShortcut "$desktop\PreviewQt.lnk" "$instdir\previewqt.exe" "" "$INSTDIR\icon.ico" 0

    ${Else}

        Delete "$desktop\PreviewQt.lnk"

    ${EndIf}

    ${If} $CheckboxStartMenu_State == ${BST_CHECKED}

        ; create start menu entry in top level, no need for a subdirectory
        CreateShortcut "$SMPROGRAMS\PreviewQt.lnk" "$INSTDIR\previewqt.exe" "" "" 0

    ${Else}

        Delete "$SMPROGRAMS\PreviewQt.lnk"

    ${EndIf}

    WriteRegStr HKLM "${INSTDIR_REG_KEY}" "DisplayIcon" "$INSTDIR\icon.ico"

    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'

FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; The uninstaller

Section "Uninstall"

    Var /GLOBAL un_fileformats
    Var /GLOBAL un_fileformats_pdfps
    Var /GLOBAL un_fileformats_psdxcf
    ReadRegStr $un_fileformats HKCU "Software\PreviewQt" "fileformats"
    ReadRegStr $un_fileformats_pdfps HKCU "Software\PreviewQt" "fileformats_pdfps"
    ReadRegStr $un_fileformats_psdxcf HKCU "Software\PreviewQt" "fileformats_psdxcf"

    ${If} $un_fileformats == "all"

        ; ... DE-register file formats ...
        ${UnRegisterExtension} ".3fr" "Hasselblad Raw Image Format"
        ${UnRegisterExtension} ".aai" "AAI Dune image"
        ${UnRegisterExtension} ".ai" "Adobe Illustrator (PDF compatible)"
        ${UnRegisterExtension} ".ani" "Animated Windows cursors"
        ${UnRegisterExtension} ".apng" "Animated Portable Network Graphics"
        ${UnRegisterExtension} ".ari" "ARRIFLEX Raw Image Format"
        ${UnRegisterExtension} ".art" "1st Publisher"
        ${UnRegisterExtension} ".arw" "Sony Digital Camera Alpha Raw Image Format"
        ${UnRegisterExtension} ".asf" "Advanced Systems Format"
        ${UnRegisterExtension} ".avif" "AV1 Image File Format"
        ${UnRegisterExtension} ".avifs" "AV1 Image File Format"
        ${UnRegisterExtension} ".avs" "AVS X image"
        ${UnRegisterExtension} ".x" "AVS X image"
        ${UnRegisterExtension} ".mbfavs" "AVS X image"
        ${UnRegisterExtension} ".bay" "Casio Raw Image Format"
        ${UnRegisterExtension} ".bmp" "Microsoft Windows bitmap"
        ${UnRegisterExtension} ".dib" "Microsoft Windows bitmap"
        ${UnRegisterExtension} ".bpg" "Better Portable Graphics"
        ${UnRegisterExtension} ".cals" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".ct1" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".ct2" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".ct3" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".ct4" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".c4" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".cal" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".nif" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".ras" "Continuous Acquisition and Life-cycle Support Type 1 image"
        ${UnRegisterExtension} ".cap" "Phase One Raw Image Format"
        ${UnRegisterExtension} ".eip" "Phase One Raw Image Format"
        ${UnRegisterExtension} ".liq" "Phase One Raw Image Format"
        ${UnRegisterExtension} ".cb7" "Comic book archive"
        ${UnRegisterExtension} ".cbr" "Comic book archive"
        ${UnRegisterExtension} ".cbt" "Comic book archive"
        ${UnRegisterExtension} ".cbz" "Comic book archive"
        ${UnRegisterExtension} ".cg3" "CCITT Group 3"
        ${UnRegisterExtension} ".g3" "CCITT Group 3"
        ${UnRegisterExtension} ".crw" "Canon Digital Camera Raw Image Format"
        ${UnRegisterExtension} ".crr" "Canon Digital Camera Raw Image Format"
        ${UnRegisterExtension} ".cr2" "Canon Digital Camera Raw Image Format"
        ${UnRegisterExtension} ".cr3" "Canon Digital Camera Raw Image Format"
        ${UnRegisterExtension} ".cube" "Cube Color lookup table converted to a HALD image"
        ${UnRegisterExtension} ".cur" "Microsoft Windows cursor format"
        ${UnRegisterExtension} ".cut" "Dr. Halo"
        ${UnRegisterExtension} ".pal" "Dr. Halo"
        ${UnRegisterExtension} ".dcr" "Kodak Cineon Raw Image Format"
        ${UnRegisterExtension} ".kdc" "Kodak Cineon Raw Image Format"
        ${UnRegisterExtension} ".drf" "Kodak Cineon Raw Image Format"
        ${UnRegisterExtension} ".k25" "Kodak Cineon Raw Image Format"
        ${UnRegisterExtension} ".dcs" "Kodak Cineon Raw Image Format"
        ${UnRegisterExtension} ".dcx" "ZSoft IBM PC multi-page Paintbrush image"
        ${UnRegisterExtension} ".dds" "DirectDraw Surface"
        ${UnRegisterExtension} ".dfont" "Multi-face font package"
        ${UnRegisterExtension} ".dic" "Digital Imaging and Communications in Medicine (DICOM) image"
        ${UnRegisterExtension} ".dcm" "Digital Imaging and Communications in Medicine (DICOM) image"
        ${UnRegisterExtension} ".djvu" "DjVu digital document format "
        ${UnRegisterExtension} ".djv" "DjVu digital document format "
        ${UnRegisterExtension} ".dng" "Adobe Digital Negative Raw Image Format"
        ${UnRegisterExtension} ".dpx" "Digital Moving Picture Exchange"
        ${UnRegisterExtension} ".erf" "Epson Raw Image Format"
        ${UnRegisterExtension} ".exr" "OpenEXR"
        ${UnRegisterExtension} ".ff" "farbfeld"
        ${UnRegisterExtension} ".fits" "Flexible Image Transport System"
        ${UnRegisterExtension} ".fit" "Flexible Image Transport System"
        ${UnRegisterExtension} ".fts" "Flexible Image Transport System"
        ${UnRegisterExtension} ".fl32" "FilmLight floating point image format"
        ${UnRegisterExtension} ".ftx" "FAKK 2"
        ${UnRegisterExtension} ".gif" "Graphics Interchange Format"
        ${UnRegisterExtension} ".gpr" "GoPro GPR Raw Image Format"
        ${UnRegisterExtension} ".heif" "High Efficiency Image Format"
        ${UnRegisterExtension} ".heic" "High Efficiency Image Format"
        ${UnRegisterExtension} ".hrz" "Slow-scan television"
        ${UnRegisterExtension} ".icns" "Apple Icon Image"
        ${UnRegisterExtension} ".ico" "Microsoft Windows icon format"
        ${UnRegisterExtension} ".iff" "Interchange File Format"
        ${UnRegisterExtension} ".jbig" "Joint Bi-level Image experts Group file interchange format (JBIG)"
        ${UnRegisterExtension} ".jbg" "Joint Bi-level Image experts Group file interchange format (JBIG)"
        ${UnRegisterExtension} ".bie" "Joint Bi-level Image experts Group file interchange format (JBIG)"
        ${UnRegisterExtension} ".jng" "JPEG Network Graphics"
        ${UnRegisterExtension} ".jpeg" "Joint Photographic Experts Group JFIF format"
        ${UnRegisterExtension} ".jpg" "Joint Photographic Experts Group JFIF format"
        ${UnRegisterExtension} ".jpe" "Joint Photographic Experts Group JFIF format"
        ${UnRegisterExtension} ".jif" "Joint Photographic Experts Group JFIF format"
        ${UnRegisterExtension} ".jpeg2000" "JPEG-2000"
        ${UnRegisterExtension} ".j2k" "JPEG-2000"
        ${UnRegisterExtension} ".jp2" "JPEG-2000"
        ${UnRegisterExtension} ".jpc" "JPEG-2000"
        ${UnRegisterExtension} ".jpx" "JPEG-2000"
        ${UnRegisterExtension} ".jxl" "JPEG XL"
        ${UnRegisterExtension} ".jxr" "JPEG-XR"
        ${UnRegisterExtension} ".hdp" "JPEG-XR"
        ${UnRegisterExtension} ".wdp" "JPEG-XR"
        ${UnRegisterExtension} ".koa" "KOALA files"
        ${UnRegisterExtension} ".gg" "KOALA files"
        ${UnRegisterExtension} ".gig" "KOALA files"
        ${UnRegisterExtension} ".kla" "KOALA files"
        ${UnRegisterExtension} ".kra" "Krita Document"
        ${UnRegisterExtension} ".lbm" "Interlaced Bitmap"
        ${UnRegisterExtension} ".mat" "MATLAB image format"
        ${UnRegisterExtension} ".mdc" "Minolta/Agfa Raw Image Format"
        ${UnRegisterExtension} ".mef" "Mamiya Raw Image Format"
        ${UnRegisterExtension} ".miff" "Magick image file format"
        ${UnRegisterExtension} ".mif" "Magick image file format"
        ${UnRegisterExtension} ".mng" "Multiple-image Network Graphics"
        ${UnRegisterExtension} ".mos" "Leaf Raw Image Format"
        ${UnRegisterExtension} ".mpc" "Magick Persistent Cache image file format"
        ${UnRegisterExtension} ".mtv" "MTV ray tracer bitmap"
        ${UnRegisterExtension} ".pic" "MTV ray tracer bitmap"
        ${UnRegisterExtension} ".mvg" "Magick Vector Graphics"
        ${UnRegisterExtension} ".nef" "Nikon Digital SLR Camera Raw Image Format"
        ${UnRegisterExtension} ".nrw" "Nikon Digital SLR Camera Raw Image Format"
        ${UnRegisterExtension} ".ora" "OpenRaster"
        ${UnRegisterExtension} ".orf" "Olympus Digital Camera Raw Image Format"
        ${UnRegisterExtension} ".otb" "On-the-air Bitmap"
        ${UnRegisterExtension} ".otf" "OpenType font file"
        ${UnRegisterExtension} ".otc" "OpenType font file"
        ${UnRegisterExtension} ".ttf" "OpenType font file"
        ${UnRegisterExtension} ".ttc" "OpenType font file"
        ${UnRegisterExtension} ".p7" "Xv Visual Schnauzer thumbnail format"
        ${UnRegisterExtension} ".palm" "Palm pixmap"
        ${UnRegisterExtension} ".pam" "Portable Arbitrary Map format"
        ${UnRegisterExtension} ".pbm" "Portable bitmap format (black and white)"
        ${UnRegisterExtension} ".pcd" "Photo CD"
        ${UnRegisterExtension} ".pcds" "Photo CD"
        ${UnRegisterExtension} ".pcx" "ZSoft PiCture eXchange"
        ${UnRegisterExtension} ".pdb" "Palm Database ImageViewer Format"
        ${UnRegisterExtension} ".pef" "Pentax Raw Image Format"
        ${UnRegisterExtension} ".ptx" "Pentax Raw Image Format"
        ${UnRegisterExtension} ".pes" "Embrid Embroidery Format"
        ${UnRegisterExtension} ".pfb" "Postscript Type 1 font "
        ${UnRegisterExtension} ".pfm" "Postscript Type 1 font "
        ${UnRegisterExtension} ".afm" "Postscript Type 1 font "
        ${UnRegisterExtension} ".inf" "Postscript Type 1 font "
        ${UnRegisterExtension} ".pfa" "Postscript Type 1 font "
        ${UnRegisterExtension} ".ofm" "Postscript Type 1 font "
        ${UnRegisterExtension} ".pfm" "Portable Float Map"
        ${UnRegisterExtension} ".pgm" "Portable graymap format (gray scale)"
        ${UnRegisterExtension} ".pgx" "JPEG 2000 uncompressed format"
        ${UnRegisterExtension} ".phm" "Portable float map format 16-bit half"
        ${UnRegisterExtension} ".pic" "Softimage PIC"
        ${UnRegisterExtension} ".picon" "Personal Icon"
        ${UnRegisterExtension} ".pict" "QuickDraw/PICT"
        ${UnRegisterExtension} ".pct" "QuickDraw/PICT"
        ${UnRegisterExtension} ".pic" "QuickDraw/PICT"
        ${UnRegisterExtension} ".pix" "Alias/Wavefront RLE image format"
        ${UnRegisterExtension} ".als" "Alias/Wavefront RLE image format"
        ${UnRegisterExtension} ".alias" "Alias/Wavefront RLE image format"
        ${UnRegisterExtension} ".png" "Portable Network Graphics"
        ${UnRegisterExtension} ".ppm" "Portable pixmap format (color)"
        ${UnRegisterExtension} ".pnm" "Portable pixmap format (color)"
        ${UnRegisterExtension} ".ptiff" "Pyramid encoded TIFF"
        ${UnRegisterExtension} ".ptif" "Pyramid encoded TIFF"
        ${UnRegisterExtension} ".pxn" "Logitech Raw Image Format"
        ${UnRegisterExtension} ".qoi" "Quite OK image format"
        ${UnRegisterExtension} ".raf" "Fuji CCD Raw Image Format"
        ${UnRegisterExtension} ".raw" "Leica Raw Image Format"
        ${UnRegisterExtension} ".rwl" "Leica Raw Image Format"
        ${UnRegisterExtension} ".rgba" "SGI images"
        ${UnRegisterExtension} ".rgb" "SGI images"
        ${UnRegisterExtension} ".sgi" "SGI images"
        ${UnRegisterExtension} ".bw" "SGI images"
        ${UnRegisterExtension} ".rgbe" "Radiance RGBE image format"
        ${UnRegisterExtension} ".hdr" "Radiance RGBE image format"
        ${UnRegisterExtension} ".rad" "Radiance RGBE image format"
        ${UnRegisterExtension} ".rgf" "LEGO Mindstorms EV3 Robot Graphics File"
        ${UnRegisterExtension} ".rla" "Wavefront RLA File Format"
        ${UnRegisterExtension} ".rle" "Utah Run length encoded image file"
        ${UnRegisterExtension} ".rw2" "Panasonic Raw Image Format"
        ${UnRegisterExtension} ".scr" "ZX-Spectrum SCREEN"
        ${UnRegisterExtension} ".sct" "Scitex Continuous Tone Picture"
        ${UnRegisterExtension} ".ch" "Scitex Continuous Tone Picture"
        ${UnRegisterExtension} ".ct" "Scitex Continuous Tone Picture"
        ${UnRegisterExtension} ".sfw" "Seattle File Works image"
        ${UnRegisterExtension} ".alb" "Seattle File Works image"
        ${UnRegisterExtension} ".pwm" "Seattle File Works image"
        ${UnRegisterExtension} ".pwp" "Seattle File Works image"
        ${UnRegisterExtension} ".sixel" "DEC SIXEL Graphics Format"
        ${UnRegisterExtension} ".srf" "Sony (Minolta) Raw Image Format"
        ${UnRegisterExtension} ".mrw" "Sony (Minolta) Raw Image Format"
        ${UnRegisterExtension} ".sr2" "Sony (Minolta) Raw Image Format"
        ${UnRegisterExtension} ".srw" "Samsung Raw Image Format"
        ${UnRegisterExtension} ".sun" "SUN Rasterfile"
        ${UnRegisterExtension} ".ras" "SUN Rasterfile"
        ${UnRegisterExtension} ".sr" "SUN Rasterfile"
        ${UnRegisterExtension} ".im1" "SUN Rasterfile"
        ${UnRegisterExtension} ".im24" "SUN Rasterfile"
        ${UnRegisterExtension} ".im32" "SUN Rasterfile"
        ${UnRegisterExtension} ".im8" "SUN Rasterfile"
        ${UnRegisterExtension} ".rast" "SUN Rasterfile"
        ${UnRegisterExtension} ".rs" "SUN Rasterfile"
        ${UnRegisterExtension} ".scr" "SUN Rasterfile"
        ${UnRegisterExtension} ".svg" "Scalable Vector Graphics"
        ${UnRegisterExtension} ".svgz" "Scalable Vector Graphics"
        ${UnRegisterExtension} ".tar" "TAR file format"
        ${UnRegisterExtension} ".tga" "Truevision Targa image"
        ${UnRegisterExtension} ".icb" "Truevision Targa image"
        ${UnRegisterExtension} ".vda" "Truevision Targa image"
        ${UnRegisterExtension} ".vst" "Truevision Targa image"
        ${UnRegisterExtension} ".tiff" "Tagged Image File Format"
        ${UnRegisterExtension} ".tif" "Tagged Image File Format"
        ${UnRegisterExtension} ".tim" "PSX TIM (PlayStation Graphics)"
        ${UnRegisterExtension} ".ttf" "TrueType font file"
        ${UnRegisterExtension} ".vicar" "VICAR rasterfile format"
        ${UnRegisterExtension} ".vic" "VICAR rasterfile format"
        ${UnRegisterExtension} ".img" "VICAR rasterfile format"
        ${UnRegisterExtension} ".viff" "Khoros Visualization Image File Format"
        ${UnRegisterExtension} ".xv" "Khoros Visualization Image File Format"
        ${UnRegisterExtension} ".vtf" "Valve Texture Format"
        ${UnRegisterExtension} ".wbmp" "Wireless Bitmap"
        ${UnRegisterExtension} ".webp" "Google web image format"
        ${UnRegisterExtension} ".wmf" "Windows Metafile"
        ${UnRegisterExtension} ".wmz" "Windows Metafile"
        ${UnRegisterExtension} ".apm" "Windows Metafile"
        ${UnRegisterExtension} ".wpg" "Word Perfect Graphics File"
        ${UnRegisterExtension} ".xbm" "X BitMap"
        ${UnRegisterExtension} ".bm" "X BitMap"
        ${UnRegisterExtension} ".xpm" "X PixMap"
        ${UnRegisterExtension} ".pm" "X PixMap"
        ${UnRegisterExtension} ".xwd" "X Windows system window dump"

        WriteRegStr HKCU "Software\PreviewQt" "fileformats" ""

    ${EndIf}

    ${If} $un_fileformats_pdfps == "registered"

        ${UnRegisterExtension} ".eps" "Encapsulated PostScript"
        ${UnRegisterExtension} ".epsf" "Encapsulated PostScript"
        ${UnRegisterExtension} ".epsi" "Encapsulated PostScript"
        ${UnRegisterExtension} ".pdf" "Adobe Portable Document Format"
        ${UnRegisterExtension} ".ps" "Adobe Level III PostScript file"
        ${UnRegisterExtension} ".ps2" "Adobe Level III PostScript file"
        ${UnRegisterExtension} ".ps3" "Adobe Level III PostScript file"

        WriteRegStr HKCU "Software\PreviewQt" "fileformats_pdfps" ""

    ${EndIf}

    ${If} $un_fileformats_psdxcf == "registered"

        ${UnRegisterExtension} ".psd" "Adobe PhotoShop"
        ${UnRegisterExtension} ".psb" "Adobe PhotoShop"
        ${UnRegisterExtension} ".xcf" "Gimp XCF"

        WriteRegStr HKCU "Software\PreviewQt" "fileformats_psdxcf" ""

    ${EndIf}

    SetShellVarContext all
    Delete "$SMPROGRAMS\PreviewQt.lnk"
    Delete "$desktop\PreviewQt.lnk"

    ;begin uninstall
    !insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL

    ;uninstall from path, must be repeated for every install logged path individual
    !insertmacro UNINSTALL.LOG_UNINSTALL "$INSTDIR"

    ;end uninstall, after uninstall from all logged paths has been performed
    !insertmacro UNINSTALL.LOG_END_UNINSTALL

    DeleteRegKey ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}"

    ; Remove environment variables
    EnVar::Delete "PHOTOQT_MAGICK_CODER_MODULE_PATH"
    EnVar::Delete "PHOTOQT_MAGICK_FILTER_MODULE_PATH"

    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'

SectionEnd
