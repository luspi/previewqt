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
import PQCScripts

ApplicationWindow {

    id: about_top

    SystemPalette { id: colorPalette; colorGroup: SystemPalette.Active }

    //: window title
    title: qsTr("About")

    // some stylings
    visible: false
    modality: Qt.ApplicationModal
    width: 500
    height: 500

    // in this window, this item catches all key presses
    Item {
        id: catchKeyPress
        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape || event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                if(configcontainer.visible)
                    configcontainer.opacity = 0
                else
                    about_top.close()
            }
        }
    }

    Flickable {

        id: flickable

        anchors.fill: parent
        anchors.margins: 5
        anchors.bottomMargin: 45
        clip: true

        opacity: 1 - configcontainer.opacity
        visible: opacity>0

        contentHeight: contcol.height

        Column {

            id: contcol

            spacing: 0

            width: parent.width

            Image {

                x: (parent.width-width)/2
                width: Math.min(flickable.width/2, 300)
                height: width
                sourceSize: Qt.size(width, height)
                smooth: true
                mipmap: false

                source: "image://svg/:/logo_full.svg"

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    ToolTip {
                        visible: parent.containsMouse
                        text: qsTr("Click here to show configuration of this build")
                    }
                    onClicked: {
                        configcontainer.opacity = 1
                    }
                }

            }

            Item {
                width: 1
                height: 5
            }

            Row {

                x: (parent.width-width)/2
                spacing: 5

                Column {

                    spacing: 5

                    Text {
                        font.bold: true
                        text: qsTr("Version:")
                        color: colorPalette.text
                    }
                    Text {
                        font.bold: true
                        text: qsTr("License:")
                        color: colorPalette.text
                    }
                    Text {
                        font.bold: true
                        text: qsTr("Website:")
                        color: colorPalette.text
                    }
                    Text {
                        font.bold: true
                        text: qsTr("Developer:")
                        color: colorPalette.text
                    }
                    Text {
                        font.bold: true
                        text: qsTr("Contact:")
                        color: colorPalette.text
                    }

                }

                Column {

                    spacing: 5

                    Text {
                        text: "PreviewQt v" + PQCScripts.getVersion()
                        color: colorPalette.text
                    }

                    Text {
                        text: "GPL v2+"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://opensource.org/license/gpl-2-0")
                        }
                        color: colorPalette.text
                    }
                    Text {
                        text: "https://previewqt.org/"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://previewqt.org/")
                        }
                        color: colorPalette.text
                    }
                    Text {
                        text: "Lukas Spies"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://luspi.de")
                        }
                        color: colorPalette.text
                    }
                    Text {
                        text: "Lukas@previewqt.org"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("mailto:Lukas@previewqt.org")
                        }
                        color: colorPalette.text
                    }

                }

            }

            Item {
                width: 1
                height: 10
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: qsTr("PreviewQt is a lightweight application allowing for quickly previewing a wide range of files, from images and videos to documents and archives. A list of supported file formats can be found on its website, though the number of actually supported formats might be even higher. If you like PreviewQt and would like to have a fully featured image viewer along the same lines check out PhotoQt: https://photoqt.org")

                color: colorPalette.text

            }

            Item {
                width: 1
                height: 10
            }

        }
    }

    Rectangle {
        id: configcontainer
        anchors.fill: parent
        anchors.bottomMargin: 45
        color: colorPalette.base
        clip: true
        opacity: 0
        visible: opacity>0
        Behavior on opacity { NumberAnimation { duration: 200 } }

        property bool configLoaded: false
        onVisibleChanged: {
            if(visible && !configLoaded) {
                configtxt.text = PQCScripts.getConfigInfo()
                configLoaded = true
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
        }

        Flickable {
            id: configflick
            anchors.fill: parent

            contentHeight: configcol.height

            Column {

                id: configcol

                width: configflick.width
                spacing: 10

                Text {
                    x: (parent.width-width)/2
                    font.pointSize: 18
                    font.bold: true
                    text: qsTr("Configuration")
                    color: colorPalette.text
                }

                Button {
                    x: (parent.width-width)/2
                    text: qsTr("Copy to clipboard")
                    onClicked: {
                        PQCScripts.copyTextToClipboard(PQCScripts.getConfigInfo(false))
                        catchKeyPress.forceActiveFocus()
                    }
                }

                Text {
                    id: configtxt
                    x: (parent.width-width)/2
                    text: ""
                    color: colorPalette.text
                }

                Item {
                    width: 1
                    height: 20
                }

            }

        }

    }

    // when closing we re-focus on main key catcher
    onClosing: {
        configcontainer.opacity = 0
        focusitem.forceActiveFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
    }


    Rectangle {
        x: 0
        y: parent.height-45
        width: parent.width
        height: 1
        color: "black"
    }

    Button {
        x: (parent.width-width)/2
        y: parent.height-45 + (45-height)/2
        text: qsTr("Close")
        onClicked: {
            if(configcontainer.visible) {
                configcontainer.opacity = 0
                focusitem.forceActiveFocus()
            } else
                about_top.close()
        }

    }

}
