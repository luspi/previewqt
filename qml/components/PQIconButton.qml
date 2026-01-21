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

Rectangle {

    id: control

    width: height
    height: parent.height

    color: active ?
               (control.down ? "#000000" : (control.hovered ? "#444444" : "#222222")) :
               "#88666666"
    Behavior on color { ColorAnimation { duration: 200 } }

    property string tooltip: ""

    property bool borderLeft: false
    property bool borderRight: true

    property bool down: false
    property bool hovered: false

    // we use active instead of enabled to keep the mouse area active
    property bool active: true
    property string source: ""

    signal pressed()
    signal released()
    signal clicked()

    Image {
        x: (parent.width-width)/2
        y: (parent.height-height)/2
        width: height
        height: parent.height*0.5
        source: control.source
        sourceSize: Qt.size(width, height)
        mipmap: false
        smooth: false
        opacity: control.active ? 1 : 0.5
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    Rectangle {
        visible: control.borderLeft
        x: 0
        y: 0
        width: 1
        height: parent.height
        color: "#ffffff"
    }

    Rectangle {
        visible: control.borderRight
        x: parent.width-1
        y: 0
        width: 1
        height: parent.height
        color: "#ffffff"
    }

    ToolTip {
        delay: 500
        text: parent.active ? control.tooltip : (image.imageSource==="" ? qsTr("No file loaded") : qsTr("Action not supported for this file type"))
        visible: text!=="" && control.hovered
    }

    Menu {
        id: menu
        MenuItem {
            enabled: false
            font.italic: true
            text: control.tooltip
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Trigger action")
            onTriggered: {
                control.clicked()
            }
        }
        onAboutToShow: {
            PQCConstants.menuIsOpen = true
        }
        onAboutToHide: {
            PQCConstants.menuIsOpen = false
        }
        Connections {
            target: PQCNotify
            function onCloseAllMenus() {
                menu.close()
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton|Qt.RightButton
        onEntered:
            control.hovered = true
        onExited:
            control.hovered = false
        onPressed: {
            control.down = true
            if(!control.active) return
            control.pressed()
        }
        onReleased: {
            control.down = false
            if(!control.active) return
            control.released()
        }
        onClicked: (mouse) => {
            if(!control.active) return
            if(mouse.button === Qt.RightButton)
                menu.popup()
            else
                control.clicked()
        }
    }

}
