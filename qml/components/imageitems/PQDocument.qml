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
import PQCScripts

Item {

    id: image

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    width: imageitem.width
    height: imageitem.height

    Image {

        id: imageitem

        source: ""

        Component.onCompleted: {
            if(image_top.imageSource === "") {
                source = ""
                return
            }
            if(image_top.imageSource.includes("::PDF::")) {
                currentPage = image_top.imageSource.split("::PDF::")[0]*1
                source = "image://full/" + PQCScripts.toPercentEncoding(image_top.imageSource)
            } else
                source = "image://full/" + PQCScripts.toPercentEncoding("%1::PDF::%2".arg(currentPage).arg(image_top.imageSource))
        }

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: false
        mipmap: false

        rotation: image_top.setRotation

        width: rotation%180===0 ? image_top.width : image_top.height
        height: rotation%180===0 ? image_top.height : image_top.width
        sourceSize: rotation%180===0 ? Qt.size(image_top.windowWidth, image_top.windowHeight) : Qt.size(image_top.windowHeight, image_top.windowWidth)

        onStatusChanged: {
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
        }

    }

    property int currentPage: 0
    property int pageCount: PQCScripts.getDocumentPageCount(image_top.imageSource)

    onCurrentPageChanged: {
        if(image_top.imageSource === "") {
            source = ""
            return
        }
        imageitem.asynchronous = false
        if(image_top.imageSource.includes("::PDF::")) {
            imageitem.source = "image://full/" + PQCScripts.toPercentEncoding("%1::PDF::%2".arg(currentPage).arg(image_top.imageSource.split("::PDF::")[1]))
        } else {
            imageitem.source = "image://full/" + PQCScripts.toPercentEncoding("%1::PDF::%2".arg(currentPage).arg(image_top.imageSource))
        }
        imageitem.asynchronous = true
    }

    Rectangle {

        parent: image_top

        id: controlitem

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: controlrow.width+20
        height: 40
        radius: 5
        color: "#88000000"

        // only show when needed
        opacity: pageCount>1 ? (hovered ? 1 : 0.3) : 0
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
                        onClicked: currentPage = 0
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
                        onClicked: currentPage = (currentPage+pageCount-1)%pageCount
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
                        onClicked: currentPage = (currentPage+1)%pageCount
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
                        onClicked: currentPage = pageCount-1
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
                        text: "%1/%2".arg(currentPage+1).arg(pageCount)
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
                            value = currentPage+1
                        } else
                            focusitem.forceActiveFocus()
                    }

                    Keys.onPressed: (event) => {
                        if(event.key === Qt.Key_Escape)
                            pagenumberrow.visible = true
                        else if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                            currentPage = pagenumberspin.value-1
                            pagenumberrow.visible = true
                        }

                    }
                }

            }

        }

    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Left) {

                currentPage = (currentPage+pageCount-1)%pageCount

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                currentPage = (currentPage+1)%pageCount

            } else if(keycode === Qt.Key_Home) {

                currentPage = 0

            } else if(keycode === Qt.Key_End) {

                currentPage = pageCount-1

            }

        }

    }


}
