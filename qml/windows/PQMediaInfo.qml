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

ApplicationWindow {

    id: mediainfo_top

    //: window title
    title: qsTr("Media Info")

    // some stylings
    visible: false
    modality: Qt.ApplicationModal
    width: 400
    height: 300

    onVisibilityChanged: (visibility) => {
        PQCConstants.windowMediaInfoVisible = (visibility === Window.Hidden ? false : true)
    }

    // in this window, this item catches all key presses
    Item {
        id: catchKeyPress
        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape || event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
                mediainfo_top.close()
        }
    }

    Flickable {

        anchors.fill: parent
        anchors.margins: 5
        anchors.bottomMargin: parent.height-sep.y
        contentHeight: col.height
        clip: true

        Column {

            id: col

            width: parent.width
            spacing: 5


            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
                color: palette.text
                text: PQCConstants.mediainfoTitle
                font.pointSize: 12
                font.bold: true
            }

            /********************************************/
            Item { width: 1; height: 1; }
            /********************************************/

            Row {
                width: parent.width
                spacing: 5
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Artist:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoArtist==="" ? "-" : PQCConstants.mediainfoArtist
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoArtist)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Album:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoAlbum==="" ? "-" : PQCConstants.mediainfoAlbum
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoAlbum)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Date:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoDate==="" ? "-" : PQCConstants.mediainfoDate
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoDate)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Genre:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoGenre==="" ? "-" : PQCConstants.mediainfoGenre
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoGenre)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Copyright:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoCopyright==="" ? "-" : PQCConstants.mediainfoCopyright
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoCopyright)
                        }
                    }
                }
            }

            /********************************************/
            Item { width: 1; height: 1; }
            /********************************************/

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsAudio
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Track number:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoAudioTrack==="" ? "-" : PQCConstants.mediainfoAudioTrack
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoAudioTrack)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsAudio
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Bit Rate:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoAudioBitRate==="" ? "-" : PQCConstants.mediainfoAudioBitRate
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoAudioBitRate)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsAudio
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Audio Codec:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoAudioCodec==="" ? "-" : PQCConstants.mediainfoAudioCodec
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoAudioCodec)
                        }
                    }
                }
            }

            /********************************************/

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsVideo
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Frame Rate:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoVideoFrameRate==="" ? "-" : PQCConstants.mediainfoVideoFrameRate
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoVideoFrameRate)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsVideo
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Bit Rate:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoVideoBitRate==="" ? "-" : PQCConstants.mediainfoVideoBitRate
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoVideoBitRate)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsVideo
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: qsTr("Video Codec:")
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoVideoCodec==="" ? "-" : PQCConstants.mediainfoVideoCodec
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoVideoCodec)
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 5
                visible: PQCConstants.mediainfoIsVideo
                Text {
                    width: 2*parent.width/5-5
                    horizontalAlignment: Text.AlignRight
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: "HDR:"
                }
                Text {
                    width: 3*parent.width/5
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: palette.text
                    text: PQCConstants.mediainfoVideoHdr==="" ? "-" : PQCConstants.mediainfoVideoHdr
                    MouseArea {
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: qsTr("Click to copy value to clipboard")
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: {
                            PQCScriptsOther.copyTextToClipboard(PQCConstants.mediainfoVideoHdr)
                        }
                    }
                }
            }

        }

    }

    Rectangle {
        id: sep
        y: (closebut.y-5)
        width: parent.width
        height: 1
        color: palette.text
    }

    Button {
        id: closebut
        y: parent.height-height - 5
        x: (parent.width-width)/2
        text: qsTr("Close")
        onPressed:
            catchKeyPress.forceActiveFocus()
        onClicked:
            mediainfo_top.close()
    }

    // when closing we re-focus on main key catcher
    onClosing: {
        PQCNotify.resetFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
    }

    Component.onCompleted: {
        mediainfo_top.show()
    }

    Connections {

        target: PQCNotify

        function onShowSubWindow(wdw : string) {

            if(wdw === "mediainfo")
                mediainfo_top.show()

        }

    }

}
