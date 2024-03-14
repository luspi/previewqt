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

import QtQuick
import QtQuick.Controls

Rectangle {

    id: control

    width: height
    height: parent.height

    color: control.down ? "#000000" : (control.hovered ? "#444444" : "#222222")
    Behavior on color { ColorAnimation { duration: 200 } }

    property string tooltip: ""

    property bool borderLeft: false
    property bool borderRight: true

    property bool down: false
    property bool hovered: false

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
    }

    Rectangle {
        visible: borderLeft
        x: 0
        y: 0
        width: 1
        height: parent.height
        color: "#ffffff"
    }

    Rectangle {
        visible: borderRight
        x: parent.width-1
        y: 0
        width: 1
        height: parent.height
        color: "#ffffff"
    }

    ToolTip {
        delay: 500
        text: control.tooltip
        visible: text!=="" && control.hovered
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onEntered:
            control.hovered = true
        onExited:
            control.hovered = false
        onPressed: {
            control.down = true
            control.pressed()
        }
        onReleased: {
            control.down = false
            control.released()
        }
        onClicked:
            control.clicked()
    }

}
