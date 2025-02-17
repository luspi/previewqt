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
import PQCScripts // qmllint disable import
import PQCSettings
/*1on_PQMKF6*/
import org.kde.syntaxhighlighting 1.0
/*2on_PQMKF6*/

Item {

    id: txt_top

    SystemPalette { id: colorPalette; colorGroup: SystemPalette.Active }

    x: 0
    y: (image_top.height-height)/2

    width: flickme.width
    height: flickme.height

    property size sourceSize: Qt.size(width, height)
    property bool asynchronous: false
    property alias paintedWidth: flickme.width
    property alias paintedHeight: flickme.height

    Flickable {

        id: flickme

        property int defw: Math.max(50, PQCSettings.defaultWindowWidth)
        property int defh: Math.max(50, PQCSettings.defaultWindowHeight)

        width: defw
        height: defh

        contentHeight: imageitem.height+10
        contentWidth: imageitem.width

        ScrollBar.horizontal: ScrollBar { }
        ScrollBar.vertical: ScrollBar { }

        onContentXChanged: settingsrect.hide()
        onContentYChanged: settingsrect.hide()

        /*1on_PQMKF6*/
        SyntaxHighlighter {
            id: myHighlighter
            textEdit: imageitem
            definition: "C++"
        }
        /*2on_PQMKF6*/

        TextArea {

            id: imageitem
            y: 5

            width: PQCSettings.textWordWrap ? flickme.width : undefined
            height: Math.max(flickme.height-10, contentHeight)

            color: "white"
            font.family: "Monospace"
            font.pointSize: PQCSettings.textFontPointSize

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere

            readOnly: true
            background: Rectangle {
                color: colorPalette.base
            }


            // With syntax highlighting the text content is set after the syntax highlighting is set at the end
            /*1off_PQMKF6NOT
            text: PQCScripts.getTextFileContents(image_top.imageSource)
            2off_PQMKF6NOT*/

            Component.onCompleted: {
                image.status = Image.Ready
                imageitem.forceActiveFocus()
            }

            Component.onDestruction: {
                focusitem.forceActiveFocus()
            }

            Timer {
                interval: 100
                running: true
                property bool firsttime: true
                onTriggered: {
                    if(firsttime) {
                        if(toplevel.width < flickme.defw || toplevel.height < flickme.defh)
                            image.statusChanged()
                        firsttime = false
                        restart()
                    } else {
                        flickme.width = Qt.binding(function() { return image_top.width})
                        flickme.height = Qt.binding(function() { return image_top.height})
                    }

                }
            }

            Keys.onPressed: (event) => {
                if(event.key === Qt.Key_Down)
                    flickme.flick(0, -500)
                else if(event.key === Qt.Key_Up)
                    flickme.flick(0, 500)
                else if(event.key === Qt.Key_PageDown)
                    flickme.flick(0, -1500)
                else if(event.key === Qt.Key_PageUp)
                    flickme.flick(0, 1500)
                else if(event.key === Qt.Key_End)
                    flickme.contentY = flickme.contentHeight-flickme.height
                else if(event.key === Qt.Key_Home)
                    flickme.contentY = 0
                else
                    toplevel.keyPress(event.modifiers, event.key)
            }

        }

    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        enabled: settingsrect.visible
        onClicked:
            settingsrect.hide()
        onWheel: {}
    }

    Row {

        id: settingsrow

        x: (parent.width-width)
        y: (parent.height-height)

        spacing: 5

        Rectangle  {
            id: settingsbut
            width: 35
            height: 35
            color: settingsmouse.containsPress ? "#21262b" : (settingsmouse.containsMouse ? "#61666b" : "#31363b")
            Behavior on color { ColorAnimation { duration: 200 } }
            radius: 4

            Image {
                anchors.fill: parent
                anchors.margins: 6
                sourceSize: Qt.size(width, height)
                source: "image://svg/:/settings.svg"
            }

            MouseArea {
                id: settingsmouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    settingsrect.show()
                }
            }

            Rectangle {
                id: settingsrect
                clip: true
                x: settingsrow.width-width
                y: settingsbut.y-height
                width: 200
                height: 0
                Behavior on height { NumberAnimation { duration: 200 } }
                opacity: 0
                visible: opacity>0
                Behavior on opacity { NumberAnimation { duration: 200 } }
                color: colorPalette.base
                border.color: colorPalette.accent
                border.width: 1

                Column {

                    Row {
                        height: 40
                        Rectangle {
                            color: setzoomplus.containsMouse ? colorPalette.highlight : colorPalette.base
                            Behavior on color { ColorAnimation { duration: 200 } }
                            opacity: 0.6
                            width: 60
                            height: 40
                            border.color: colorPalette.dark
                            border.width: 1
                            Text {
                                color: colorPalette.text
                                font.pointSize: 14
                                font.bold: true
                                anchors.centerIn: parent
                                text: "+"
                            }
                            MouseArea {
                                id: setzoomplus
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: PQCSettings.textFontPointSize += 1
                            }
                        }
                        Rectangle {
                            color: setzoomminus.containsMouse ? colorPalette.highlight : colorPalette.base
                            Behavior on color { ColorAnimation { duration: 200 } }
                            opacity: 0.6
                            width: 60
                            height: 40
                            border.color: colorPalette.dark
                            border.width: 1
                            Text {
                                color: colorPalette.text
                                font.pointSize: 14
                                font.bold: true
                                anchors.centerIn: parent
                                text: "-"
                            }
                            MouseArea {
                                id: setzoomminus
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: PQCSettings.textFontPointSize -= 1
                            }
                        }
                        Rectangle {
                            color: setzoomzero.containsMouse ? colorPalette.highlight : colorPalette.base
                            Behavior on color { ColorAnimation { duration: 200 } }
                            opacity: 0.6
                            width: 80
                            height: 40
                            border.color: colorPalette.dark
                            border.width: 1
                            Text {
                                color: colorPalette.text
                                font.pointSize: 14
                                font.bold: true
                                anchors.centerIn: parent
                                text: "0"
                            }
                            MouseArea {
                                id: setzoomzero
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: PQCSettings.textFontPointSize = 12
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: colorPalette.accent
                    }

                    Item {
                        height: 39
                        width: 200
                        Rectangle {
                            color: setwrapmouse.containsMouse ? colorPalette.highlight : colorPalette.base
                            Behavior on color { ColorAnimation { duration: 200 } }
                            anchors.fill: parent
                            opacity: 0.6
                        }
                        CheckBox {
                            x: 5
                            width: 190
                            y: (parent.height-height)/2
                            font.bold: true
                            checked: PQCSettings.textWordWrap
                            text: "Wrap lines"
                        }
                        MouseArea {
                            id: setwrapmouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                PQCSettings.textWordWrap = !PQCSettings.textWordWrap
                                settingsrect.hide()
                            }
                        }
                    }

                }

                function show() {
                    toplevel.menuOpen = true
                    height = 80
                    opacity = 1
                }
                function hide() {
                    if(!visible) return
                    toplevel.menuOpen = false
                    height = 0
                    opacity = 0
                }

                Connections {
                    target: toplevel
                    function onCloseAllMenus() {
                        settingsrect.hide()
                    }
                }
            }

        }

        /*1on_PQMKF6*/

        ComboBox {
            id: control
            y: (parent.height-height)/2
            width: 150
            model : Repository.definitions
            displayText: currentValue.translatedName
            textRole: "translatedName"
            property var sortme
            popup: Popup {
                id: thepopup
                x: -50
                y: control.height
                width: 200
                implicitHeight: Math.min(contentItem.implicitHeight, 300)
                padding: 0
                contentItem:
                    ListView {
                        clip: true
                        implicitHeight: contentHeight
                        implicitWidth: 200
                        model: control.sortme
                        delegate: Rectangle {
                            color: delegmouse.containsMouse ? "white" : "black"
                            implicitWidth: 200-thebar.width
                            implicitHeight: 30
                            Text {
                                x: 20
                                anchors.verticalCenter: parent.verticalCenter
                                width: parent.width-20
                                elide: Text.ElideMiddle
                                text: control.sortme[index].name
                                color: delegmouse.containsMouse ? "black" : "white"
                            }
                            MouseArea {
                                id: delegmouse
                                anchors.fill: parent
                                hoverEnabled: true
                                ToolTip.visible: delegmouse.containsMouse
                                ToolTip.text: control.sortme[index].name
                                ToolTip.delay: 500
                                onClicked: {
                                    control.currentIndex = index
                                    thepopup.close()
                                }
                            }
                        }

                        currentIndex: control.highlightedIndex
                        section.property: "section"
                        section.criteria: ViewSection.FullString
                        section.delegate: Rectangle {
                            x: 10
                            height: 25
                            color: "#333333"
                            Text {
                                text: section
                                anchors.verticalCenter: parent.verticalCenter
                                color: "white"
                                font.bold: true
                            }
                        }

                        ScrollBar.vertical: ScrollBar { id: thebar }
                }
            }

            onCurrentIndexChanged: {
                myHighlighter.definition = Repository.definitions[currentIndex]
                focusitem.forceActiveFocus()
            }

            Component.onCompleted: {
                var m = []
                var mtxt = []
                for(var r in Repository.definitions) {
                    var val = Repository.definitions[r]
                    mtxt.push(val.translatedName)
                    var c = {
                        "name" : val.translatedName,
                        "section" : val.section
                    }
                    m.push(c)
                }
                control.sortme = m
            }
        }

        Component.onCompleted: {
            // set current file type
            myHighlighter.definition = Repository.definitionForName(PQCScripts.getSuffix(image_top.imageSource))
            control.currentIndex = Repository.definitions.indexOf(myHighlighter.definition)

            // This HAS to be set after setting the styling!
            // Otherwise for slightly larger files the interface is blocked for quite a while.
            imageitem.text = PQCScripts.getTextFileContents(image_top.imageSource)
        }
        /*2on_PQMKF6*/

    }

    Component.onDestruction: {
        toplevel.menuOpen = false
    }

}
