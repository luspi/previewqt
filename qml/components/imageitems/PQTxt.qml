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

        ScrollBar.vertical: ScrollBar { id: scrollbar }

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

            width: flickme.width
            height: Math.max(flickme.height-10, contentHeight)

            color: "white"
            font.family: "Monospace"

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere

            font.pointSize: 12

            readOnly: true
            background: Rectangle {
                color: colorPalette.base
            }

            text: PQCScripts.getTextFileContents(image_top.imageSource)

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

    /*1on_PQMKF6*/
    Item {
        x: (parent.width-width)
        y: (parent.height-height)
        width: 200
        height: 35

        ComboBox {
            id: control
            x: 50
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

    }

    Component.onCompleted: {
        // set current file type
        myHighlighter.definition = Repository.definitionForFileName(image_top.imageSource)
        control.currentIndex = Repository.definitions.indexOf(myHighlighter.definition)
    }
    /*2on_PQMKF6*/

}
