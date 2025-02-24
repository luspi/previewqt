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

        onContentXChanged: {
            settingsrect.hide()
        }
        onContentYChanged: {
            settingsrect.hide()
        }

        /*1on_PQMKF6*/
        SyntaxHighlighter {
            id: myHighlighter
            textEdit: imageitem
            definition: "C++"
        }
        /*2on_PQMKF6*/

        TextEdit {

            id: imageitem
            y: 5

            width: PQCSettings.textWordWrap ? flickme.width : undefined
            height: Math.max(flickme.height-10, contentHeight)

            color: "white"
            font.family: "Monospace"
            font.pointSize: PQCSettings.textFontPointSize

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere

            readOnly: true

            Rectangle {
                anchors.fill: parent
                color: colorPalette.base
                z: -1
            }

            // With syntax highlighting the text content is set after the syntax highlighting is set at the end
            /*1on_PQMNOTKF6
            text: PQCScripts.getTextFileContents(image_top.imageSource)
            2off_PQMNOTKF6*/

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
        onClicked: {
            settingsrect.hide()
        }
    }

    Rectangle {

        id: searchbut

        x: 5
        y: parent.height-height-5
        width: 35
        height: 35
        color: searchmouse.containsPress ? "#21262b" : (searchmouse.containsMouse ? "#61666b" : "#31363b")
        Behavior on color { ColorAnimation { duration: 200 } }
        radius: 4

        opacity: 1-searchrect.opacity

        Image {
            anchors.fill: parent
            anchors.margins: 6
            sourceSize: Qt.size(width, height)
            source: "image://svg/:/magnifyingglass.svg"
        }

        MouseArea {
            id: searchmouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                searchrect.show()
            }
        }

    }

    Rectangle {

        id: searchrect

        color: notFound ? "#ff4444" : colorPalette.window
        Behavior on color { ColorAnimation { duration: 200 } }
        border.width: 1
        border.color: colorPalette.accent
        radius: 2

        property bool notFound: false

        x: 5
        y: parent.height-height-5
        height: 35
        width: visible ? 150 : 0
        Behavior on width { NumberAnimation { duration: 200 } }

        opacity: 0
        visible: opacity>0
        Behavior on opacity { NumberAnimation { duration: 200 } }

        TextInput {
            id: searchinput
            x: 5
            y: (parent.height-height)/2
            width: parent.width-10
            font.pointSize: 12
            color: searchrect.notFound ? "#440000" : colorPalette.text
            Behavior on color { ColorAnimation { duration: 200 } }
            enabled: searchrect.visible
            onEnabledChanged: {
                if(!enabled)
                    focusitem.forceActiveFocus()
                else
                    selectAll()
            }

            Keys.onPressed: (event) => {

                event.accepted = false

                if(event.modifiers === Qt.ControlModifier) {

                    if(event.key === Qt.Key_F)
                        selectAll()
                    else
                        toplevel.processKeyEvent(event.modifiers, event.key)

                } else if(event.key === Qt.Key_Escape) {

                    if(settingsrect.visible)
                        settingsrect.hide()
                    else
                        searchrect.hide()
                    event.accepted = true

                } else if(event.modifiers & Qt.ShiftModifier) {
                    findTimer.reverse = true
                }
            }

            Keys.onReleased: (event) => {
                if(!(event.modifiers & Qt.ShiftModifier)) {
                    findTimer.reverse = false
                }
            }

            onTextEdited: {
                if(text.length === 0 && searchrect.opacity === 1)
                    searchrect.notFound = false
                findTimer.restart()
            }

            onAccepted: {
                findTimer.stop()
                findTimer.triggered()
            }

        }

        NumberAnimation {
            id: animContY
            target: flickme
            property: "contentY"
            easing.type: Easing.OutCubic
            duration: 200
        }
        NumberAnimation {
            id: animContX
            target: flickme
            property: "contentX"
            easing.type: Easing.OutCubic
            duration: 200
        }

        Timer {
            id: findTimer
            interval: 1000
            property bool reverse: false
            onTriggered: {

                if(searchinput.text.length === 0) {
                    searchrect.notFound = false
                    imageitem.deselect()
                    return
                }

                var textContent = imageitem.text

                var index = imageitem.cursorPosition
                if(reverse)
                    index = textContent.lastIndexOf(searchinput.text, index-searchinput.text.length-1)
                else
                    index = textContent.indexOf(searchinput.text, index)
                if(index !== -1) {

                    searchrect.notFound = false

                    imageitem.select(index, index+searchinput.text.length)
                    var rect = imageitem.positionToRectangle(index);

                    animContX.stop()
                    animContY.stop()

                    animContX.from = flickme.contentX
                    animContX.to = Math.max(0, Math.min(imageitem.width-flickme.width, rect.x-flickme.width*0.8))

                    animContY.from = flickme.contentY
                    animContY.to = Math.max(0, Math.min(imageitem.height-flickme.height, rect.y-flickme.height*0.8))

                    animContX.restart()
                    animContY.restart()

                } else {
                    index = 0

                    if(reverse)
                        index = textContent.lastIndexOf(searchinput.text)
                    else
                        index = textContent.indexOf(searchinput.text, index)


                    if(index !== -1) {

                        searchrect.notFound = false

                        imageitem.select(index, index+searchinput.text.length)
                        var rect2 = imageitem.positionToRectangle(index);

                        animContX.stop()
                        animContY.stop()

                        animContX.from = flickme.contentX
                        animContX.to = Math.max(0, Math.min(imageitem.width-flickme.width, rect2.x-flickme.width*0.8))

                        animContY.from = flickme.contentY
                        animContY.to = Math.max(0, Math.min(imageitem.height-flickme.height, rect2.y-flickme.height*0.8))

                        animContX.restart()
                        animContY.restart()

                    } else {

                        searchrect.notFound = true
                        imageitem.deselect()

                    }
                }

            }
        }

        function show() {
            toplevel.menuOpen = true
            searchinput.text = (imageitem.selectedText.length > 0 ? imageitem.selectedText : "")
            searchrect.opacity = 1
            searchinput.forceActiveFocus()
            searchinput.selectAll()
        }

        function hide() {
            findTimer.stop()
            toplevel.menuOpen = false
            searchrect.opacity = 0
            focusitem.forceActiveFocus()
        }

        Connections {
            target: toplevel
            function onCloseAllMenus() {
                searchrect.hide()
            }
        }

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

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers === Qt.ControlModifier && keycode === Qt.Key_F) {

                searchrect.show()
            }

        }

    }

}
