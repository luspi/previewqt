;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 2024-2026 Lukas Spies
; Contact: http://previewqt.org
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
; - icon_portable.ico
; - previewqt-portable-setup.nsi (this file)
;
; IN ADDITION THE EXECUTABLE NEEDS TO BE BUILT WITH PORTABLE TWEAKS !!
;
; This will then create a new file in the application directory
; called previewqt-portable-%version%.exe.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Unicode True

SilentInstall silent

; this is supposed to be the fastest to uncompress
SetCompressor ZLIB

!define PREVIEWQT_VERSION "3.0"

Icon "icon_portable.ico"
OutFile "photoqt-portable-${PREVIEWQT_VERSION}.exe"

; only one (invisible) page
Page instfiles

Section

	; all files are uncompressed into a temporary directory and run
	InitPluginsDir

	SetOutPath $pluginsdir
	File /r /x *nsh /x *nsi /x *qmlc /x previewqt-portable-*.exe ".\"

	; the directory of the executable is passed on as we store config/cache data there
	ExecWait '"$pluginsdir\previewqt.exe" "$exedir"'

	SetOutPath $temp

SectionEnd
