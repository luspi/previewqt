/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
 ** Contact: https://previewqt.org                                       **
 **                                                                      **
 ** This file is part of PreviewQt.                                      **
 **                                                                      **
 ** PreviewQt is free software: you can redistribute it and/or modify    **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PreviewQt is distributed in the hope that it will be useful,         **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.    **
 **                                                                      **
 **************************************************************************/

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import PreviewQt

// the general settings
Flickable {

    id: tools_top

    clip: true

    contentHeight: toolscol.height

    property int usableWidth: width-20-(scrollbar.visible ? scrollbar.width : 0)
    ScrollBar.vertical: ScrollBar { id: scrollbar }

    function loadSettings() {

    }

    Column {

        id: toolscol

        x: 10
        spacing: 10

        Item {
            width: 1
            height: 1
        }

        Text {
            x: (tools_top.usableWidth-width)/2
            font.pointSize: 18
            font.bold: true
            text: qsTr("Settings")
            color: palette.text
        }

        Text {
            x: (tools_top.usableWidth-width)/2
            text: qsTr("Note: Settings will be saved automatically.")
            color: palette.text
        }

        /************************************/

        CheckBox {
            id: processYtdlp
            text: qsTr("Process URLs with yt-dlp.")
            checked: PQCSettings.processUrlWithYtdlp
            onCheckedChanged: {
                catchKeyPress.forceActiveFocus()
                if(PQCSettings.processUrlWithYtdlp !== checked)
                    PQCSettings.processUrlWithYtdlp = checked
            }
        }

        Row {
            spacing: 5
            enabled: processYtdlp.checked
            TextField {
                id: ytdlpedit
                y: (ytdlpbut.height-height)/2
                width: tools_top.usableWidth-ytdlpbut.width-5
                text: PQCSettings.executableYtDlp
                onTextChanged: {
                    if(text !== PQCSettings.executableYtDlp)
                        PQCSettings.executableYtDlp = text
                }
            }
            Button {
                id: ytdlpbut
                text: "..."
                onClicked: {
                    selectExe.category = "ytdlp"
                    selectExe.prevexe = ytdlpedit.text
                    selectExe.open()
                }
            }
        }

    }

    // file dialog to choose an executable
    FileDialog {
        id: selectExe
        currentFolder: "file://" + (prevexe.slice(0,1)==="/" ?
                                       PQCScriptsFilesPaths.getDir(prevexe) :
                                       (PQCScriptsConfig.amIOnWindows() ?
                                            StandardPaths.standardLocations(StandardPaths.HomeLocation)[0] :
                                            "/usr/bin"))
        nameFilters: "All Files (*.*)"
        property string category: ""
        property string prevexe: ""
        onAccepted: {

            var file = selectedFile
            var fname = PQCScriptsFilesPaths.getFilename(file)

            if(StandardPaths.findExecutable(fname) === file)
                file = fname

            if(category == "ytdlp")
                ytdlpedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else
                console.warn("Unknown category:", category)

        }
    }

}
