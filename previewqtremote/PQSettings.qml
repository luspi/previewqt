/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2024 Lukas Spies                                       **
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
import PQCScripts

Window {

    id: settings_top

    title: qsTr("Settings")
    visible: false
    modality: Qt.ApplicationModal
    width: 450
    height: 350

    // For this window, this item catches all key presses
    Item {
        id: catchKeyPress

        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape || event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                settings_top.close()
        }

    }

    onClosing: {
        focusitem.forceActiveFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true

    }

    Column {

        x: 10
        width: parent.width-20
        spacing: 10

        Item {
            width: 1
            height: 1
        }

        Text {
            x: (settings_top.width-width)/2
            font.pointSize: 18
            font.bold: true
            //: Same as tab name but used as title
            text: qsTr("Settings")
        }

        Text {
            x: (settings_top.width-width)/2
            text: qsTr("Note: Any changes will be saved automatically.")
        }

        /************************************/

        Text {
            text: "PreviewQt executable:"
        }

        Row {
            spacing: 5
            TextField {
                id: exectxt
                width: parent.parent.width-execbut.width - 10
                placeholderText: "executable"
                text: PQCScripts.previewQtExec
                onTextChanged: {
                    PQCScripts.previewQtExec = text
                    verifytxt.visible = !PQCScripts.verifyExecutable()
                }
                Keys.onPressed: (event) => {
                    if(event.key === Qt.Key_Escape || event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                        settings_top.close()
                }
            }
            Button {
                id: execbut
                text: "..."
                onClicked: {
                    selectExe.prevexe = exectxt.text
                    selectExe.open()
                }
            }
        }

        Text {
            id: verifytxt
            width: parent.width
            visible: !PQCScripts.verifyExecutable()
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: "red"
            text: "The executable for PreviewQt is invalid."
        }

        /************************************/

        Rectangle {
            width: parent.width
            height: 1
            color: "black"
        }

        /************************************/

        CheckBox {
            text: "Show integrated help text"
            checked: PQCScripts.showText
            onCheckedChanged: {
                PQCScripts.showText = checked
            }
        }

    }

    /************************************/
    Rectangle {
        x: 0
        y: parent.height-45
        width: parent.width
        height: 1
        color: "black"
    }
    /************************************/

    Button {

        x: (parent.width-width)/2
        y: parent.height-45 + (45-height)/2
        width: Math.min(200, parent.width*0.5)
        //: written on button
        text: qsTr("Close")
        onClicked:
            settings_top.close()

    }

    /************************************/

    // file dialog to choose an executable
    FileDialog {
        id: selectExe
        currentFolder: "file://" + (prevexe.slice(0,1)==="/" ?
                                       PQCScripts.getDir(prevexe) :
                                       (PQCScripts.amIOnWindows() ?
                                            StandardPaths.standardLocations(StandardPaths.HomeLocation)[0] :
                                            "/usr/bin"))
        nameFilters: "All Files (*.*)"
        property string prevexe: ""
        onAccepted: {

            var file = selectedFile
            var fname = PQCScripts.getFilename(file)

            if(StandardPaths.findExecutable(fname) === file)
                file = fname

            exectxt.text = PQCScripts.cleanPath(file)

        }
    }

}
