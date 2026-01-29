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

    id: toprow

    // location of top bar
    x: -1
    y: PQCConstants.toprowForceVisible||!PQCSettings.topBarAutoHide ? -1 : -height-2
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.InQuint } }

    // some stylings
    width: PQCConstants.mainwindowWidth+2
    height: 40
    color: "#aa000000"
    border.width: 1
    border.color: "white"

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton|Qt.LeftButton
        onDoubleClicked: {
            PQCNotify.mainwindowToggleMaximized()
        }

        onPressed: (mouse) => {
            if(mouse.button === Qt.LeftButton) {
                cursorShape = Qt.ClosedHandCursor
                PQCNotify.mainwindowStartSystemMove()
            }
        }
        onReleased:
            cursorShape = Qt.PointingHandCursor
        onClicked: (mouse) => {
            if(mouse.button === Qt.RightButton)
                PQCNotify.showMainContextMenu()
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
                PQCNotify.requestNewFile()
            }
        }

        // type in url
        PQIconButton {
            id: enterbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "image://svg/:/textedit.svg"
            tooltip: qsTr("Type in a path or URL (Ctrl+U)")
            onClicked: {
                PQCNotify.showSubWindow("enterpath")
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
                PQCNotify.showSubWindow("settings")
            }
        }

        // open in external/default app
        PQIconButton {
            id: openwithdefaultbut
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Open in external application") + " (" + PQCSettings.defaultAppShortcut + ")"
            active: PQCConstants.currentSource!==""
            source: "image://svg/:/external.svg"
            onClicked: {
                if(PQCConstants.currentSource === "") return
                if(PQCScriptsFilesPaths.openInDefault(PQCConstants.currentSource)) {
                    if(PQCSettings.closeAfterDefaultApp)
                        PQCNotify.mainwindowClose()
                } else
                    PQCNotify.showExtNotSet()
            }
            MouseArea {
                enabled: !openwithdefaultbut.active
                anchors.fill: parent
                acceptedButtons: Qt.RightButton|Qt.LeftButton
                onClicked: {}
            }
        }

        // rotate left
        PQIconButton {
            id: rotleftbut
            visible: ["doc", "arc", "ani", "svg", "img"].indexOf(PQCConstants.currentType)>-1 && PQCConstants.currentSource!==""
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Rotate image to the left")
            source: "image://svg/:/rotateleft.svg"
            onClicked: {
                PQCNotify.addRotation(-90)
            }
        }

        // rotate right
        PQIconButton {
            id: rotrightbut
            visible: ["doc", "arc", "ani", "svg", "img"].indexOf(PQCConstants.currentType)>-1 && PQCConstants.currentSource!==""
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Rotate image to the right")
            source: "image://svg/:/rotateright.svg"
            onClicked: {
                PQCNotify.addRotation(90)
            }
        }

        PQTopRowDownloadButton {}

        // reload website/online video
        PQIconButton {
            id: reloadbutton
            visible: PQCConstants.currentType==="url" && PQCConstants.currentSource!==""
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Reload website or online video")
            source: "image://svg/:/reload.svg"
            onClicked: {
                PQCNotify.loadNewFile(PQCConstants.currentSource)
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
                PQCNotify.showSubWindow("about")
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
                PQCNotify.showSubWindow("help")
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
        enabled: PQCConstants.menuIsOpen
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton
        onClicked: {
            PQCNotify.closeAllMenus()
        }
    }

}
