/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PreviewQt.                                      **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/

import QtQuick
import PreviewQt

Item {

    id: tags_top

    property list<var> dat: ({})
    property int entries: 0

    opacity: 0
    visible: opacity>0
    Behavior on opacity { NumberAnimation { duration: 200 } }

    rotation: PQCConstants.imageRotation

    Repeater {
        model: tags_top.entries

        Rectangle {

            id: deleg

            required property int index

            property real dat_x: tags_top.dat[5*index+0]
            property real dat_y: tags_top.dat[5*index+1]
            property real dat_w: tags_top.dat[5*index+2]
            property real dat_h: tags_top.dat[5*index+3]
            property string dat_name: tags_top.dat[5*index+4]

            color: "transparent"

            border.width: 2
            border.color: "red"
            radius: 5

            x: tags_top.width*dat_x
            y: tags_top.height*dat_y
            width: tags_top.width*dat_w
            height: tags_top.height*dat_h

            Rectangle {
                anchors.fill: parent
                color: "white"
                radius: 5
                opacity: 0.1
            }

            Rectangle {
                x: (parent.width-width)/2
                y: parent.height
                width: Math.max(100, parent.width)
                height: nametxt.height+4
                color: "black"
                radius: 5
                Text {
                    id: nametxt
                    x: 3
                    y: 2
                    width: parent.width-6
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 9
                    font.bold: true
                    color: "white"
                    text: deleg.dat_name
                }
            }

        }

    }

    Timer {
        id: hideAfterX
        interval: 2500
        onTriggered: tags_top.opacity = 0
    }

    Connections {
        target: PQCNotify
        function onMouseMoved(x, y) {
            hideAfterX.stop()
            tags_top.opacity = 1
            hideAfterX.restart()
        }
    }

    function setFaceTags(tags) {
        dat = tags
        entries = tags.length/5
    }

}
