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
import PQCScripts   // qmllint disable import
import PQCSettings  // qmllint disable import

Item {

    id: arc_top

    x: (image_top.width-width)/2 // qmllint disable unqualified
    y: (image_top.height-height)/2 // qmllint disable unqualified

    width: imageitem.width
    height: imageitem.height

    property string source: image_top.imageSource // qmllint disable unqualified

    property bool thisIsAComicBook: PQCScripts.isComicBook(source) // qmllint disable unqualified
    property var fileList: []
    property int currentFile: 0
    property int fileCount: fileList.length

    property alias sourceSize: imageitem.sourceSize
    property alias asynchronous: imageitem.asynchronous
    property alias paintedWidth: imageitem.paintedWidth
    property alias paintedHeight: imageitem.paintedHeight

    Image {

        id: imageitem

        source: ""
        Component.onCompleted: {
            if(arc_top.source === "") {
                source = ""
                return
            }
            if(arc_top.source.includes("::ARC::") || arc_top.fileCount == 0)
                source = "image://full/" + PQCScripts.toPercentEncoding(arc_top.source) // qmllint disable unqualified
            else
                source = "image://full/" + PQCScripts.toPercentEncoding("%1::ARC::%2".arg(arc_top.fileList[arc_top.currentFile]).arg(arc_top.source))
        }

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: false
        mipmap: false

        rotation: image_top.setRotation // qmllint disable unqualified

        property int defw: Math.max(50, PQCSettings.defaultWindowWidth) // qmllint disable unqualified
        property int defh: Math.max(50, PQCSettings.defaultWindowHeight) // qmllint disable unqualified

        width: rotation%180===0 ? image_top.width : image_top.height // qmllint disable unqualified
        height: rotation%180===0 ? image_top.height : image_top.width // qmllint disable unqualified
        sourceSize: (PQCSettings.maximizeImageSizeAndAdjustWindow && !toplevel.isMaximized && !toplevel.isFullscreen && !toplevel.manualWindowSizeChange) ? // qmllint disable unqualified
                        (rotation%180===0 ? Qt.size(defw, defh) : Qt.size(defh, defw)) :
                        (rotation%180===0 ? Qt.size(image_top.windowWidth, image_top.windowHeight) : Qt.size(image_top.windowHeight, image_top.windowWidth))

        onStatusChanged: {
            image.status = status // qmllint disable unqualified
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
            else if(status == Image.Ready)
                asynchronous = false
        }

    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            arc_top.fileList = PQCScripts.getArchiveContent(arc_top.source) // qmllint disable unqualified
            if(arc_top.source.includes("::ARC::"))
                arc_top.currentFile = arc_top.fileList.indexOf(arc_top.source.split("::ARC::")[0])
        }
    }

    onCurrentFileChanged: {
        updateSource()
    }

    Connections {
        target: image_top // qmllint disable unqualified
        function onImageSourceChanged(imageSource) {
            arc_top.currentFile = 0
            arc_top.fileList = PQCScripts.getArchiveContent(arc_top.source) // qmllint disable unqualified
            updateSource()
        }
    }

    function updateSource() {
        imageitem.asynchronous = false
        if(currentFile == -1 || currentFile >= fileList.length) return
        if(arc_top.source.includes("::ARC::")) {
            imageitem.source = "image://full/" + PQCScripts.toPercentEncoding("%1::ARC::%2".arg(fileList[currentFile]).arg(arc_top.source.split("::ARC::")[1]))
        } else {
            imageitem.source = "image://full/" + PQCScripts.toPercentEncoding("%1::ARC::%2".arg(fileList[currentFile]).arg(arc_top.source))
        }
        imageitem.asynchronous = true
    }

    Rectangle {

        parent: image_top // qmllint disable unqualified

        id: archivelisting

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: listing_combo.width+20
        height: 40
        radius: 5
        color: "#88000000"

        // only show when needed
        opacity: !arc_top.thisIsAComicBook && arc_top.fileCount>1 ? (hovered ? 1 : 0.3) : 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        visible: opacity>0
        enabled: visible

        // the first property is set by PCNotify signals for everything else not caught with the elements below
        property bool emptyAreaHovered: false
        property bool hovered: bgmouse_listing.containsMouse||listing_combo.hovered

        MouseArea {
            id: bgmouse_listing
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {}
        }

        ComboBox {
            id: listing_combo
            x: 10
            y: 5
            width: 200
            height: parent.height-10
            model: arc_top.fileList
            currentIndex: arc_top.currentFile
            onCurrentIndexChanged: {
                focusitem.forceActiveFocus() // qmllint disable unqualified
                if(currentIndex !== arc_top.currentFile)
                    arc_top.currentFile = currentIndex
            }
        }

    }

    Rectangle {

        parent: image_top // qmllint disable unqualified

        id: controlitem

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: controlrow.width+20
        height: 40
        radius: 5
        color: "#88000000"

        // only show when needed
        opacity: arc_top.thisIsAComicBook && arc_top.fileCount>1 ? (hovered ? 1 : 0.3) : 0
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
                        onClicked: arc_top.currentFile = 0
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
                        onClicked: arc_top.currentFile = (arc_top.currentFile+arc_top.fileCount-1)%arc_top.fileCount
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
                        onClicked: arc_top.currentFile = (arc_top.currentFile+1)%arc_top.fileCount
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
                        onClicked: arc_top.currentFile = arc_top.fileCount-1
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
                        text: "%1/%2".arg(arc_top.currentFile+1).arg(arc_top.fileCount)
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
                    to: arc_top.fileCount

                    onVisibleChanged: {
                        if(visible) {
                            pagenumberspin.forceActiveFocus()
                            value = arc_top.currentFile+1
                        } else
                            focusitem.forceActiveFocus() // qmllint disable unqualified
                    }

                    Keys.onPressed: (event) => {
                        if(event.key === Qt.Key_Escape)
                            pagenumberrow.visible = true
                        else if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                            arc_top.currentFile = pagenumberspin.value-1
                            pagenumberrow.visible = true
                        }

                    }
                }

            }

        }

    }

    Connections {

        target: toplevel // qmllint disable unqualified

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Left) {

                arc_top.currentFile = (arc_top.currentFile+arc_top.fileCount-1)%arc_top.fileCount

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                arc_top.currentFile = (arc_top.currentFile+1)%arc_top.fileCount

            } else if(keycode === Qt.Key_Home) {

                arc_top.currentFile = 0

            } else if(keycode === Qt.Key_End) {

                arc_top.currentFile = arc_top.fileCount-1

            }

        }

    }

}
