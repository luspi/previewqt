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

import PQCSettings
import PQCScripts

Rectangle {

    id: toprow

    // location of top bar
    x: -1
    y: toplevel.toprowMakeVisible||!PQCSettings.topBarAutoHide ? -1 : -height-2
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.InQuint } }

    // some stylings
    width: toplevel.width+2
    height: 40
    color: "#aa000000"
    border.width: 1
    border.color: "white"

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton|Qt.LeftButton
        onDoubleClicked: {
            if(toplevel.visibility === Window.Maximized)
                toplevel.showNormal()
            else
                toplevel.showMaximized()
        }

        onPressed: (mouse) => {
            if(mouse.button === Qt.LeftButton) {
                cursorShape = Qt.ClosedHandCursor
                toplevel.startSystemMove()
            }
        }
        onReleased:
            cursorShape = Qt.PointingHandCursor
        onClicked: (mouse) => {
            if(mouse.button === Qt.RightButton)
                toplevel.showMainContextMenu()
        }
    }

    // left part of the bar
    Row {

        y: (parent.height-height)/2

        // open a file
        PQIconButton {
            id: openbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "image://svg/:/open.svg"
            tooltip: qsTr("Open a file (Ctrl+O)")
            onClicked: {
                toplevel.openNewFile()
            }
        }

        // open settings
        PQIconButton {
            id: settingsbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "image://svg/:/settings.svg"
            tooltip: qsTr("Open settings (Ctrl+P)")
            onClicked: {
                settings.active = true
                settings.item.show()
            }
        }

        // open in external/default app
        PQIconButton {
            id: openwithdefaultbut
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Open in external application") + " (" + PQCSettings.defaultAppShortcut + ")"
            active: image.imageSource!==""
            source: "image://svg/:/external.svg"
            onClicked: {
                if(image.imageSource === "") return
                if(PQCScripts.openInDefault(image.imageSource)) {
                    if(PQCSettings.closeAfterDefaultApp)
                        toplevel.close()
                } else
                    extNotSet.open()
            }
            MouseArea {
                enabled: !parent.active
                anchors.fill: parent
                acceptedButtons: Qt.RightButton|Qt.LeftButton
                onClicked: {}
            }
        }

        // rotate left
        PQIconButton {
            id: rotleftbut
            active: ["sph", "vid", "mpv", "bok", "txt"].indexOf(image.currentType)==-1 && image.imageSource!==""
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Rotate image to the left")
            source: "image://svg/:/rotateleft.svg"
            onClicked: {
                image.setRotation -= 90
            }
            MouseArea {
                enabled: !parent.active
                anchors.fill: parent
                acceptedButtons: Qt.RightButton|Qt.LeftButton
                onClicked: {}
            }
        }

        // rotate right
        PQIconButton {
            id: rotrightbut
            active: ["sph", "vid", "mpv", "bok", "txt"].indexOf(image.currentType)==-1 && image.imageSource!==""
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Rotate image to the right")
            source: "image://svg/:/rotateright.svg"
            onClicked: {
                image.setRotation += 90
            }
            MouseArea {
                enabled: !parent.active
                anchors.fill: parent
                acceptedButtons: Qt.RightButton|Qt.LeftButton
                onClicked: {}
            }
        }

    }

    Row {

        x: (parent.width-width)

        // open about window
        PQIconButton {
            id: aboutbut
            y: (parent.height-height)/2
            borderLeft: true
            height: toprow.height-2
            source: "image://svg/:/about.svg"
            tooltip: qsTr("About PreviewQt (Ctrl+I)")
            onClicked: {
                about.active = true
                about.item.show()
            }
        }

        // open help window
        PQIconButton {
            id: helpbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "image://svg/:/help.svg"
            tooltip: qsTr("Help (F1)")
            onClicked: {
                help.active = true
                help.item.show()
            }
        }

        // close window
        PQIconButton {
            id: exitbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "image://svg/:/exit.svg"
            tooltip: qsTr("Quit PreviewQt (Ctrl+Q)")
            onClicked: {
                Qt.quit()
            }
        }

    }

    MouseArea {
        anchors.fill: parent
        enabled: toplevel.menuOpen
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton
        onClicked: {
            toplevel.closeAllMenus()
        }
    }

}
