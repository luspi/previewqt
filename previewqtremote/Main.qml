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
import PQCScripts

Window {

    id: remote_top

    width: 100
    height: 100

    flags: Qt.FramelessWindowHint|Qt.Window|Qt.WindowStaysOnTopHint

    visible: true

    color: "transparent"

    property int bordersize: 8

    Rectangle {
        anchors.fill: parent
        color: "#88000000"
        radius: 10
        Keys.onEscapePressed: {
            remote_top.close()
        }
        Component.onCompleted:
            forceActiveFocus()
    }

    Text {
        anchors.fill: parent
        anchors.margins: 5
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        visible: PQCScripts.getShowText()
        text: "Drop files here to preview"
        font.pointSize: 8
        font.bold: true
        color: "white"
    }

    // drag and clicks
    MouseArea {
        anchors.fill: parent
        anchors.margins: bordersize
        hoverEnabled: true
        cursorShape: Qt.SizeAllCursor
        onPressed:
            remote_top.startSystemMove()
        onDoubleClicked: {
            // if(!toplevel.visible)
                // toplevel.show()
            // toplevel.raise()
            // toplevel.requestActivate()
            PQCScripts.passToPreviewQt("")
        }
    }

    // top edge
    MouseArea {
        x: bordersize
        y: 0
        width: parent.width-2*bordersize
        height: bordersize
        cursorShape: Qt.SizeVerCursor
        onPressed:
            remote_top.startSystemResize(Qt.TopEdge)
    }

    // left edge
    MouseArea {
        x: 0
        y: bordersize
        width: bordersize
        height: parent.height-2*bordersize
        cursorShape: Qt.SizeHorCursor
        onPressed:
            remote_top.startSystemResize(Qt.LeftEdge)
    }

    // right edge
    MouseArea {
        x: parent.width-width
        y: bordersize
        width: bordersize
        height: parent.height-2*bordersize
        cursorShape: Qt.SizeHorCursor
        onPressed:
            remote_top.startSystemResize(Qt.RightEdge)
    }

    // bottom edge
    MouseArea {
        x: bordersize
        y: parent.height-bordersize
        width: parent.width-2*bordersize
        height: bordersize
        cursorShape: Qt.SizeVerCursor
        onPressed:
            remote_top.startSystemResize(Qt.BottomEdge)
    }

    // top left
    MouseArea {
        x: 0
        y: 0
        width: bordersize
        height: bordersize
        cursorShape: Qt.SizeFDiagCursor
        onPressed:
            remote_top.startSystemResize(Qt.LeftEdge|Qt.TopEdge)
    }

    // top right
    MouseArea {
        x: parent.width-width
        y: 0
        width: bordersize
        height: bordersize
        cursorShape: Qt.SizeBDiagCursor
        onPressed:
            remote_top.startSystemResize(Qt.RightEdge|Qt.TopEdge)
    }

    // bottom right
    MouseArea {
        x: parent.width-width
        y: parent.height-height
        width: bordersize
        height: bordersize
        cursorShape: Qt.SizeFDiagCursor
        onPressed:
            remote_top.startSystemResize(Qt.RightEdge|Qt.BottomEdge)
    }

    // bottom left
    MouseArea {
        x: 0
        y: parent.height-height
        width: bordersize
        height: bordersize
        cursorShape: Qt.SizeFDiagCursor
        onPressed:
            remote_top.startSystemResize(Qt.LeftEdge|Qt.BottomEdge)
    }

    DropArea {
        anchors.fill: parent
        onDropped: (drag) =>{
            PQCScripts.passToPreviewQt(drag.text)
        }
    }

    Component.onCompleted: {
        if(PQCScripts.getPassedOnFilename() !== "")
            PQCScripts.passToPreviewQt(PQCScripts.getPassedOnFilename())
    }

}
