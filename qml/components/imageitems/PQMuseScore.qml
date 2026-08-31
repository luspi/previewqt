/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
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

Item {

    id: msc_top

    x: (PQCConstants.imageAvailableSize.width-width)/2
    y: (PQCConstants.imageAvailableSize.height-height)/2

    width: imageitem.width
    height: imageitem.height

    Component.onCompleted: {
        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(imageitem.paintedWidth, imageitem.paintedHeight) })
    }

    // this will be set to wherever musescore extracted the score to
    property string useDirectory: ""

    Image {

        id: imageitem

        source: ""

        Component.onCompleted: {
            if(PQCConstants.currentSource === "") {
                source = ""
                return
            }
            if(PQCConstants.currentSource.includes("::MSC::"))
                PQCScriptsExternalTools.extractMuseScore(PQCConstants.currentSource.split("::MSC::")[1])
            else
                PQCScriptsExternalTools.extractMuseScore(PQCConstants.currentSource)
        }

        Connections {
            target: PQCScriptsExternalTools
            function onMusescoreTemporaryDirLoaded(dir) {
                console.warn(">>> dir =", dir)
                if(dir === "") {
                    source = ""
                    return
                }
                msc_top.useDirectory = dir
                msc_top.pageCount = PQCScriptsExternalTools.getMuseScoreCurrentPageCount()
                if(PQCConstants.currentSource.includes("::MSC::")) {
                    msc_top.currentPage = PQCConstants.currentSource.split("::MSC::")[0]*1
                    imageitem.source = "image://full/" + PQCScriptsFilesPaths.toPercentEncoding(dir + "/page-" + msc_top.currentPage + ".svg")
                } else {
                    msc_top.currentPage = 1
                    imageitem.source = "image://full/" + dir + "/" + PQCScriptsFilesPaths.toPercentEncoding("page-1.svg")
                }
            }
        }
        Timer {
            id: loadScore
            interval: 200
            onTriggered: {

            }
        }

        asynchronous: false

        fillMode: Image.PreserveAspectFit

        smooth: false
        mipmap: false

        rotation: PQCConstants.imageRotation

        width: rotation%180===0 ? PQCConstants.imageAvailableSize.width : PQCConstants.imageAvailableSize.height
        height: rotation%180===0 ? PQCConstants.imageAvailableSize.height : PQCConstants.imageAvailableSize.width
        sourceSize: rotation%180===0 ? Qt.size(PQCConstants.imageAvailableSizeDelay.width,
                                               PQCConstants.imageAvailableSizeDelay.height) : Qt.size(PQCConstants.imageAvailableSizeDelay.height,
                                                                                                 PQCConstants.imageAvailableSizeDelay.width)

        onStatusChanged: {
            PQCConstants.imageStatus = status
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
            else if(status == Image.Ready && source !== "")
                asynchronous = false
        }

    }

    property int currentPage: 0
    property int pageCount: 1

    onCurrentPageChanged: {
        if(PQCConstants.currentSource === "") {
            imageitem.source = ""
            return
        }
        imageitem.asynchronous = false
        imageitem.source = "image://full/" + useDirectory + "/page-" + msc_top.currentPage + ".svg"
        imageitem.asynchronous = false

        PQCCache.setEntry(PQCConstants.currentSource, currentPage)
    }

    Rectangle {

        id: controlitem

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        z: 1
        width: controlrow.width+20
        height: 40
        radius: 5
        color: "#88000000"

        // only show when needed
        opacity: msc_top.pageCount>1 ? (hovered ? 1 : 0.3) : 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        visible: opacity>0
        enabled: visible

        // the first property is set by PCNotify signals for everything else not caught with the elements below
        property bool emptyAreaHovered: false
        property bool hovered: emptyAreaHovered||mouseprev.containsMouse||mousenext.containsMouse||
                               mousefirst.containsMouse||mouselast.containsMouse||bgmouse.containsMouse||
                               pagemouse1.containsMouse||pagemouse2.containsMouse||pagenumberspin.hovered

        MouseArea {
            id: bgmouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {}
        }

        Row {

            id: controlrow

            x: 10
            height: parent.height

            Row {

                height: parent.height

                Rectangle {
                    color: mousefirst.containsPress ? "#88000000" : (mousefirst.containsMouse ? "#88444444" : "transparent")
                    Behavior on color { ColorAnimation { duration: 200 } }
                    width: parent.height/2.5 + 4
                    height: parent.height
                    radius: 4
                    Image {
                        x: 2
                        y: (parent.height-height)/2
                        width: height
                        height: controlitem.height/2.5
                        sourceSize: Qt.size(width, height)
                        source: "image://svg/:/first.svg"
                    }
                    MouseArea {
                        id: mousefirst
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: msc_top.currentPage = 1
                    }
                }

                Rectangle {
                    color: mouseprev.containsPress ? "#88000000" : (mouseprev.containsMouse ? "#88444444" : "transparent")
                    Behavior on color { ColorAnimation { duration: 200 } }
                    width: parent.height/1.5
                    height: parent.height
                    radius: 4
                    Image {
                        y: (parent.height-height)/2
                        width: height
                        height: controlitem.height/1.5
                        sourceSize: Qt.size(width, height)
                        source: "image://svg/:/backwards.svg"
                    }
                    MouseArea {
                        id: mouseprev
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: msc_top.currentPage = (msc_top.currentPage==1 ? 1 : msc_top.currentPage-1)
                    }
                }

                Rectangle {
                    color: mousenext.containsPress ? "#88000000" : (mousenext.containsMouse ? "#88444444" : "transparent")
                    Behavior on color { ColorAnimation { duration: 200 } }
                    width: parent.height/1.5
                    height: parent.height
                    radius: 4
                    Image {
                        y: (parent.height-height)/2
                        width: height
                        height: controlitem.height/1.5
                        sourceSize: Qt.size(width, height)
                        source: "image://svg/:/forwards.svg"
                    }
                    MouseArea {
                        id: mousenext
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: msc_top.currentPage = (msc_top.currentPage == msc_top.pageCount ? msc_top.pageCount : msc_top.currentPage+1)
                    }
                }

                Rectangle {
                    color: mouselast.containsPress ? "#88000000" : (mouselast.containsMouse ? "#88444444" : "transparent")
                    Behavior on color { ColorAnimation { duration: 200 } }
                    width: parent.height/2.5 + 4
                    height: parent.height
                    radius: 4
                    Image {
                        x: 2
                        y: (parent.height-height)/2
                        width: height
                        height: controlitem.height/2.5
                        sourceSize: Qt.size(width, height)
                        source: "image://svg/:/last.svg"
                    }
                    MouseArea {
                        id: mouselast
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: msc_top.currentPage = msc_top.pageCount
                    }
                }

            }

            Item {
                width: 10
                height: 1
            }

            Rectangle {
                y: (parent.height-height)/2
                height: controlitem.height*0.75
                width: 1
                color: "white"
            }

            Item {
                width: 10
                height: 1
            }

            Item {

                y: (parent.height-height)/2
                width: childrenRect.width
                height: Math.max(pagenumberrow.height, pagenumberspin.height)

                Row {

                    id: pagenumberrow
                    y: (parent.height-height)/2

                    spacing: 5

                    Text {

                        id: pagetitletxt

                        y: (parent.height-height)/2
                        text: qsTr("Page")
                        color: "white"

                        MouseArea {
                            id: pagemouse1
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: pagenumberrow.visible = false
                        }
                    }

                    Text {

                        id: pagenumbertxt

                        y: (parent.height-height)/2
                        text: "%1/%2".arg(msc_top.currentPage).arg(msc_top.pageCount)
                        color: "white"

                        MouseArea {
                            id: pagemouse2
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: pagenumberrow.visible = false
                        }
                    }
                }

                SpinBox {
                    id: pagenumberspin
                    visible: !pagenumberrow.visible
                    width: visible ? pagenumbertxt.width+pagetitletxt.width+5 : 0
                    height: controlitem.height-10

                    from: 1
                    to: pageCount

                    onVisibleChanged: {
                        if(visible) {
                            pagenumberspin.forceActiveFocus()
                            value = currentPage
                        } else
                            PQCNotify.resetFocus()
                    }

                    Keys.onPressed: (event) => {
                        if(event.key === Qt.Key_Escape)
                            pagenumberrow.visible = true
                        else if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                            currentPage = pagenumberspin.value
                            pagenumberrow.visible = true
                        }

                    }
                }

            }

        }

    }

    Connections {

        target: PQCNotify

        function onMainwindowKeyPress(modifiers : int, keycode : int) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Left) {

                msc_top.currentPage = (msc_top.currentPage==1 ? 1 : msc_top.currentPage-1)

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                msc_top.currentPage = (msc_top.currentPage == msc_top.pageCount ? msc_top.pageCount : msc_top.currentPage+1)

            } else if(keycode === Qt.Key_Home) {

                msc_top.currentPage = 1

            } else if(keycode === Qt.Key_End) {

                msc_top.currentPage = msc_top.pageCount

            }

        }


    }


}
