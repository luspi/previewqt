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
import PQCScripts // qmllint disable import
import PQCSettings
import PQCTextProcessing
/*1on_PQMKF6*/
import org.kde.syntaxhighlighting 1.0
/*2on_PQMKF6*/

Rectangle {

    color: colorPalette.window

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

    Rectangle {
        x: parent.width-width
        width: vBar.width
        height: parent.height
        color: colorPalette.shadow
        opacity: 0.1*vBar.opacity
    }

    Rectangle {
        y: parent.height-height
        width: parent.width
        height: hBar.height
        color: colorPalette.shadow
        opacity: 0.1*hBar.opacity
    }

    Flickable {

        id: flickme

        property int defw: Math.max(50, PQCSettings.defaultWindowWidth)
        property int defh: Math.max(50, PQCSettings.defaultWindowHeight)

        width: defw
        height: defh

        clip: true
        contentItem.clip: true

        contentHeight: imageitem.height + hBar.height
        contentWidth: imageitem.width + vBar.height

        ScrollBar.horizontal: ScrollBar { id: hBar; opacity: (hBar.active ? 1 : 0.1); Behavior on opacity { NumberAnimation { duration: 200 } } }
        ScrollBar.vertical: ScrollBar { id: vBar; opacity: (vBar.active ? 1 : 0.1); Behavior on opacity { NumberAnimation { duration: 200 } } }

        /*1on_PQMKF6*/
        SyntaxHighlighter {
            id: myHighlighter
            textEdit: imageitem
            definition: "C++"
        }
        /*2on_PQMKF6*/

        TextEdit {

            id: imageitem
            x: 5
            y: 5

            width: PQCSettings.textWordWrap ? flickme.width-10-(vBar.visible ? vBar.width : 0) : undefined
            height: Math.max(flickme.height-(hBar.visible ? hBar.height : 0), contentHeight)

            color: colorPalette.text

            font.pointSize: PQCSettings.textFontPointSize
            font.family: "Monospace"
            textFormat: TextEdit.PlainText

            /*1on_PQMKF6*/
            onFontChanged: {
                var txt = imageitem.text
                if(txt === "") return
                var vpos = vBar.position
                var hpos = hBar.position
                imageitem.text = ""
                imageitem.text = txt
                vBar.position = vpos
                hBar.position = hpos
            }
            /*2on_PQMKF6*/

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            readOnly: true

            Component.onCompleted: {
                image.status = Image.Ready
                imageitem.forceActiveFocus()
            }

            Component.onDestruction: {
                focusitem.forceActiveFocus()
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    contextmenu.popup()
                }
            }

            Menu {

                id: contextmenu

                MenuItem {
                    enabled: imageitem.selectedText.length>0
                    text: qsTr("Copy selection")
                    onTriggered: {
                        PQCScripts.copyTextToClipboard(imageitem.selectedText)
                    }
                }
                MenuItem {
                    enabled: imageitem.selectedText.length>0
                    text: qsTr("Search for selected text")
                    onTriggered: {
                        searchrect.show()
                    }
                }

                MenuSeparator {}

                MenuItem {
                    text: qsTr("Select all")
                    onTriggered: {
                        imageitem.selectAll()
                    }
                }
                MenuItem {
                    text: qsTr("Copy all content")
                    onTriggered: {
                        PQCScripts.copyTextToClipboard(imageitem.text)
                    }
                }

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
                if(event.key === Qt.Key_Down) {
                    if(flickme.contentHeight <= flickme.height) return
                    if(event.modifiers === Qt.ControlModifier)
                        vBar.position = Math.min(1-(flickme.height/flickme.contentHeight), vBar.position + (1/(imageitem.lineCount-1)))
                    else
                        flickme.flick(0, -500)
                } else if(event.key === Qt.Key_Up) {
                    if(flickme.contentHeight <= flickme.height) return
                    if(event.modifiers === Qt.ControlModifier)
                        vBar.position = Math.max(0, vBar.position - (1/(imageitem.lineCount-1)))
                    else
                        flickme.flick(0, 500)
                } else if(event.key === Qt.Key_Right) {
                    if(flickme.contentWidth <= flickme.width) return
                    if(event.modifiers === Qt.ControlModifier)
                        hBar.position = Math.min(1-(flickme.width/flickme.contentWidth), hBar.position + (10/flickme.contentWidth))
                    else
                        flickme.flick(-500, 0)
                } else if(event.key === Qt.Key_Left) {
                    if(flickme.contentWidth <= flickme.width) return
                    if(event.modifiers === Qt.ControlModifier)
                        hBar.position = Math.max(0, hBar.position - (10/flickme.contentWidth))
                    else
                        flickme.flick(500, 0)
                } else if(event.key === Qt.Key_PageDown)
                    flickme.flick(0, -1500)
                else if(event.key === Qt.Key_PageUp)
                    flickme.flick(0, 1500)
                else if(event.key === Qt.Key_End) {
                    flickme.contentX = 0
                    flickme.contentY = flickme.contentHeight-flickme.height
                } else if(event.key === Qt.Key_Home) {
                    flickme.contentX = 0
                    flickme.contentY = 0
                } else
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

        opacity: (searchmouse.containsMouse ? 1 : 0.1) * (1-searchrect.opacity)
        visible: opacity>0
        Behavior on opacity { NumberAnimation { duration: 200 } }

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
            acceptedButtons: Qt.LeftButton|Qt.RightButton

            ToolTip { text: qsTr("search in file (Ctrl+F)") }

            onClicked: (mouse) => {
                if(mouse.button === Qt.LeftButton)
                    searchrect.show()
                else
                    searchmenu.popup()
            }
        }

        Menu {
            id: searchmenu
            MenuItem {
                text: qsTr("Search in file")
                onTriggered: {
                    searchrect.show()
                }
            }
            onAboutToShow: {
                toplevel.menuOpen = true
            }
            onAboutToHide: {
                toplevel.menuOpen = false
            }
            Connections {
                target: toplevel
                function onCloseAllMenus() {
                    searchmenu.close()
                }
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
                if(text.length === 0)
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
                var searchText = searchinput.text

                if(!PQCSettings.textSearchCaseSensitive) {
                    textContent = textContent.toLowerCase()
                    searchText = searchText.toLowerCase()
                }

                var index = imageitem.cursorPosition
                if(reverse)
                    index = textContent.lastIndexOf(searchText, index-searchText.length-1)
                else
                    index = textContent.indexOf(searchText, index)
                if(index !== -1) {

                    searchrect.notFound = false

                    imageitem.select(index, index+searchText.length)
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
                        index = textContent.lastIndexOf(searchText)
                    else
                        index = textContent.indexOf(searchText, index)

                    if(index !== -1) {

                        searchrect.notFound = false

                        imageitem.select(index, index+searchText.length)
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

    Rectangle {
        id: formatbut
        x: (parent.width-settingsbut.width-width-5)
        y: (parent.height-height)

        width: 35
        height: 35
        color: formatmouse.containsPress ? "#21262b" : (formatmouse.containsMouse ? "#61666b" : "#31363b")
        Behavior on color { ColorAnimation { duration: 200 } }
        radius: 4

        property bool formatText: false
        onFormatTextChanged: {
            if(visible)
                txt_top.loadText()
        }

        opacity: formatmouse.containsMouse||formatText ? 1 : 0.1
        Behavior on opacity { NumberAnimation { duration: 200 } }

        Image {
            anchors.fill: parent
            anchors.margins: 6
            sourceSize: Qt.size(width, height)
            source: "image://svg/:/magicwand.svg"
        }

        MouseArea {
            id: formatmouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            acceptedButtons: Qt.LeftButton|Qt.RightButton

            ToolTip { text: qsTr("automatically reformat file (Ctrl+R)") }

            onClicked: (mouse) => {
                if(mouse.button === Qt.LeftButton) {
                    formatbut.formatText = !formatbut.formatText
                } else
                    formatmenu.popup()
            }
        }

        Menu {
            id: formatmenu
            MenuItem {
                text: qsTr("Auto-format text")
                onTriggered:
                    formatbut.formatText = !formatbut.formatText
            }
            onAboutToShow: {
                toplevel.menuOpen = true
            }
            onAboutToHide: {
                toplevel.menuOpen = false
            }
            Connections {
                target: toplevel
                function onCloseAllMenus() {
                    formatmenu.close()
                }
            }
        }

    }

    Rectangle {
        id: settingsbut
        x: (parent.width-width)
        y: (parent.height-height)

        width: 35
        height: 35
        color: settingsmouse.containsPress ? "#21262b" : (settingsmouse.containsMouse ? "#61666b" : "#31363b")
        Behavior on color { ColorAnimation { duration: 200 } }
        radius: 4

        opacity: settingsmouse.containsMouse||settingsrect.visible ? 1 : 0.1
        Behavior on opacity { NumberAnimation { duration: 200 } }

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
            acceptedButtons: Qt.LeftButton|Qt.RightButton

            ToolTip { text: qsTr("settings specific for text files") }

            onClicked: (mouse) => {
                if(mouse.button === Qt.LeftButton) {
                    if(settingsrect.visible)
                        settingsrect.hide()
                    else
                        settingsrect.show()
                } else
                    settingsmenu.popup()
            }
        }

        Menu {
            id: settingsmenu
            MenuItem {
                text: qsTr("Show settings")
                onTriggered: {
                    if(settingsrect.visible)
                        settingsrect.hide()
                    else
                        settingsrect.show()
                }
            }
            onAboutToShow: {
                toplevel.menuOpen = true
            }
            onAboutToHide: {
                toplevel.menuOpen = false
            }
            Connections {
                target: toplevel
                function onCloseAllMenus() {
                    settingsmenu.close()
                }
            }
        }

    }

    Rectangle {
        id: settingsrect
        x: txt_top.width-width
        y: txt_top.height - settingsbut.height - height
        width: 200
        height: 0
        Behavior on height { NumberAnimation { duration: 200 } }
        opacity: 0
        visible: opacity>0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        clip: true
        color: colorPalette.base
        border.color: colorPalette.accent
        border.width: 1

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton|Qt.RightButton
            onClicked: {}
        }

        Column {

            id: setentrycol

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
                    color: setcasemouse.containsMouse ? colorPalette.highlight : colorPalette.base
                    Behavior on color { ColorAnimation { duration: 200 } }
                    anchors.fill: parent
                    opacity: 0.6
                }
                CheckBox {
                    x: 5
                    width: 190
                    y: (parent.height-height)/2
                    font.bold: true
                    checked: PQCSettings.textSearchCaseSensitive
                    text: qsTr("Case-sensitive search")
                }
                MouseArea {
                    id: setcasemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        PQCSettings.textSearchCaseSensitive = !PQCSettings.textSearchCaseSensitive
                    }
                }
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
                    text: qsTr("Wrap lines")
                }
                MouseArea {
                    id: setwrapmouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        PQCSettings.textWordWrap = !PQCSettings.textWordWrap
                    }
                }
            }

        }

        function show() {
            toplevel.menuOpen = true
            height = setentrycol.height
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

    Component.onCompleted: {

        /*1on_PQMKF6*/
        // set current file type
        myHighlighter.definition = Repository.definitionForName(PQCScripts.getSuffix(image_top.imageSource))
        myHighlighter.definition = Repository.definitionForFileName(image_top.imageSource)
        /*2on_PQMKF6*/

        // This HAS to be set after setting the styling!
        // Otherwise for slightly larger files the interface is blocked for quite a while.
        loadText()

    }

    function loadText() {

        /*1on_PQMKF6*/
        var sec = myHighlighter.definition.section
        /*2on_PQMKF6*/
        /*1off_PQMNOTKF6
        var sec = ""
        2off_PQMNOTKF6*/

        toplevel.overrideTitleSuffix = ""

        var txt = PQCScripts.getTextFileContents(image_top.imageSource)
        var suf = PQCScripts.getSuffix(image_top.imageSource)

        var canReFormat = ["json"]
        formatbut.visible = canReFormat.indexOf(suf)>-1

        if(suf === "json") {
            sec = ""; // by default this is markdown but we need "normal" formatting
            if(formatbut.formatText) {
                toplevel.overrideTitleSuffix = " (reformatted)"
                txt = PQCTextProcessing.prettifyJSON(txt)
            }
        }

        if(sec === "Sources" || sec === "Assembler" || sec === "Configuration" || sec === "Database"|| sec === "Hardware" || sec === "Scientific" || sec === "Scripts") {
            imageitem.textFormat = TextEdit.MarkdownText
            imageitem.text = '```\n'+txt+'\n```'
        } else if(sec === "Markup") {
            imageitem.textFormat = TextEdit.MarkdownText
            imageitem.text = (suf === "md" ? txt : ('```\n'+txt+'\n```'))
        } else {
            imageitem.textFormat = TextEdit.PlainText
            imageitem.text = txt
        }

    }

    Component.onDestruction: {
        toplevel.menuOpen = false
    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers === Qt.ControlModifier && keycode === Qt.Key_F) {

                searchrect.show()

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_R) {

                formatbut.formatText = !formatbut.formatText

            }

        }

    }

}
