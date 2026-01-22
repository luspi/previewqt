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

        contentHeight: cont.height + (htmldisplay.visible ? 0 : hBar.height)
        contentWidth: (htmldisplay.visible||PQCSettings.textWordWrap) ? flickme.width : (cont.width + vBar.height)

        ScrollBar.horizontal: ScrollBar { id: hBar; opacity: (hBar.active ? 1 : 0.1); Behavior on opacity { NumberAnimation { duration: 200 } } }
        ScrollBar.vertical: ScrollBar { id: vBar; opacity: (vBar.active ? 1 : 0.1); Behavior on opacity { NumberAnimation { duration: 200 } } }

        /*1on_PQMKF6*/
        SyntaxHighlighter {
            id: myHighlighter
            textEdit: imageitem
            definition: "C++"
        }
        /*2on_PQMKF6*/

        Item {

            id: cont

            width: htmldisplay.visible ? htmldisplay.width : imageitem.width
            height: htmldisplay.visible ? htmldisplay.height : imageitem.height

            Rectangle {
                id: htmldisplay
                visible: htmldisplay_txt.text!==""
                width: txt_top.width
                height: htmldisplay_txt.height+10
                color: "white"
                property bool isMarkdown: false
                Text {
                    id: htmldisplay_txt
                    baseUrl: "file://" + PQCScriptsFilesPaths.getDir(PQCConstants.currentSource) + "/"
                    x: 5
                    y: 5
                    width: txt_top.width-10
                    height: contentHeight
                    textFormat: htmldisplay.isMarkdown ? Text.MarkdownText : Text.RichText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pointSize: Math.max(4, PQCSettings.textFontPointSize)
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {}
                }
            }

            TextEdit {

                id: imageitem
                x: 5
                y: 5

                width: PQCSettings.textWordWrap ? flickme.width-10-(vBar.visible ? vBar.width : 0) : undefined
                height: Math.max(flickme.height-(hBar.visible ? hBar.height : 0), contentHeight)

                color: colorPalette.text

                visible: !htmldisplay.visible

                font.pointSize: Math.max(4, PQCSettings.textFontPointSize)
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

                wrapMode: PQCSettings.textWordWrap ? Text.WrapAtWordBoundaryOrAnywhere : Text.NoWrap
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
                            PQCScriptsFilesPaths.copyTextToClipboard(imageitem.selectedText)
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
                            PQCScriptsFilesPaths.copyTextToClipboard(imageitem.text)
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
                        PQCNotify.mainwindowKeyPress(event.modifiers, event.key)
                }

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
                PQCConstants.menuIsOpen = true
            }
            onAboutToHide: {
                PQCConstants.menuIsOpen = false
            }
            Connections {
                target: PQCNotify
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
            PQCConstants.menuIsOpen = true
            searchinput.text = (imageitem.selectedText.length > 0 ? imageitem.selectedText : "")
            searchrect.opacity = 1
            searchinput.forceActiveFocus()
            searchinput.selectAll()
        }

        function hide() {
            findTimer.stop()
            PQCConstants.menuIsOpen = false
            searchrect.opacity = 0
            focusitem.forceActiveFocus()
        }

        Connections {
            target: PQCNotify
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

        opacity: formatmouse.containsMouse ? 1 : (formatText ? 0.5 : 0.1)
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

            ToolTip { text: qsTr("process file (Ctrl+R)") }

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
                text: qsTr("Process text")
                onTriggered:
                    formatbut.formatText = !formatbut.formatText
            }
            onAboutToShow: {
                PQCConstants.menuIsOpen = true
            }
            onAboutToHide: {
                PQCConstants.menuIsOpen = false
            }
            Connections {
                target: PQCNotify
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
                PQCConstants.menuIsOpen = true
            }
            onAboutToHide: {
                PQCConstants.menuIsOpen = false
            }
            Connections {
                target: PQCNotify
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
            PQCConstants.menuIsOpen = true
            height = setentrycol.height
            opacity = 1
        }
        function hide() {
            if(!visible) return
            PQCConstants.menuIsOpen = false
            height = 0
            opacity = 0
        }

        Connections {
            target: PQCNotify
            function onCloseAllMenus() {
                settingsrect.hide()
            }
        }
    }

    Component.onCompleted: {

        /*1on_PQMKF6*/
        // set current file type
        myHighlighter.definition = Repository.definitionForFileName(PQCConstants.currentSource)
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

        imageitem.text = ""
        PQCConstants.mainwindowOverrideTitleSuffix = (formatbut.formatText ? " (processed)" : "")

        var txt = PQCScriptsImages.getTextFileContents(PQCConstants.currentSource)
        var suf = PQCScriptsFilesPaths.getSuffix(PQCConstants.currentSource)

        var canReFormat = ["json","html","htm","xhtml","md"]
        formatbut.visible = (canReFormat.indexOf(suf)>-1)


        if(suf === "json") {
            if(formatbut.formatText)
                txt = PQCTextProcessing.prettifyJSON(txt)
        } else if(suf === "html" || suf === "htm" || suf === "xhtml" || suf === "md") {
            if(formatbut.formatText) {
                htmldisplay.isMarkdown = (suf === "md" ? true : false)
                htmldisplay_txt.text = txt
                txt = ""
            } else
                htmldisplay_txt.text = ""
        }

        imageitem.text = txt

    }

    Component.onDestruction: {
        PQCConstants.menuIsOpen = false
    }

    Connections {

        target: PQCNotify

        function onMainwindowKeyPress(modifiers : int, keycode : int) {

            if(modifiers === Qt.ControlModifier && keycode === Qt.Key_F) {

                searchrect.show()

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_R) {

                formatbut.formatText = !formatbut.formatText

            } else if(modifiers === Qt.ControlModifier && (keycode === Qt.Key_Plus || keycode === Qt.Key_Equal)) {

                PQCSettings.textFontPointSize += 1

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_Minus) {

                PQCSettings.textFontPointSize = Math.max(4, PQCSettings.textFontPointSize-1)

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_0) {

                PQCSettings.textFontPointSize = 12

            }

        }

    }

}
