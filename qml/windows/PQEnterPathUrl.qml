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

import QtQuick
import QtQuick.Controls
import PreviewQt

Window {

    id: enter_top

    title: qsTr("Enter a path or URL")

    // some stylings
    visible: false
    modality: Qt.ApplicationModal
    width: 250
    height: contcol.height+15

    color: palette.base

    onVisibilityChanged: (visibility) => {
        PQCConstants.windowEnterPathVisible = (visibility === Window.Hidden ? false : true)
    }

    // in this window, this item catches all key presses
    Item {
        id: catchKeyPress
        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape)
                enter_top.rejectPath()
            else if(event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                enter_top.acceptPath()
            }
        }
    }

    Column {

        id: contcol

        y: 10
        width: parent.width

        spacing: 10

        Text {
            font.pointSize: 12
            text: "Enter a path or URL"
            font.bold: true
            color: palette.text
            x: 10
            width: parent.width-20
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Rectangle {
            x: 2
            width: parent.width-4
            height: textinput.height+10
            color: "transparent"
            border.color: "#88000000"
            radius: 5
            TextInput {
                id: textinput
                font.pointSize: 12
                x: 10
                y: 5
                width: parent.width-20
                Keys.onEscapePressed: {
                    enter_top.rejectPath()
                }

                onAccepted: {
                    enter_top.acceptPath()
                }
            }
        }

        Text {
            id: errormsg
            font.pointSize: 10
            text: "The file does not exist."
            font.bold: true
            color: "red"
            x: 10
            width: parent.width-20
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Row {

            x: (parent.width-width)/2

            Button {
                font.bold: true
                text: "Load"
                onClicked: enter_top.acceptPath()
            }

            Button {
                font.bold: true
                text: "Cancel"
                onClicked: enter_top.rejectPath()
            }

        }

    }

    function acceptPath() {
        if(PQCScriptsFilesPaths.isFileSupported(textinput.text)) {
            PQCNotify.loadNewFile(textinput.text)
            enter_top.close()
        } else
            errormsg.visible = true
    }

    function rejectPath() {
        enter_top.close()
    }

    Component.onCompleted: {
        errormsg.visible = false
        enter_top.show()
        textinput.forceActiveFocus()
        textinput.selectAll()
    }

    Connections {

        target: PQCNotify

        function onShowSubWindow(wdw : string) {

            if(wdw === "enterpath") {
                errormsg.visible = false
                enter_top.show()
                textinput.forceActiveFocus()
                textinput.selectAll()
            }

        }

    }

}
