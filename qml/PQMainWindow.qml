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

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import PQCFileFormats

import PreviewQt

import "./components"

ApplicationWindow {

    id: toplevel

    // default stylings
    width: 800
    height: 600
    minimumWidth: 300
    minimumHeight: 200

    property bool manualWindowSizeChange: false
    onWidthChanged: {
        if(!toplevelAni.running)
            manualWindowSizeChange = true
    }
    onHeightChanged: {
        if(!toplevelAni.running)
            manualWindowSizeChange = true
    }

    // it is hidden by default until we set the stylings from the settings below
    visibility: Window.Hidden
    property string overrideTitle: ""
    property string overrideTitleSuffix: ""
    title: (overrideTitle!= "" ?
                (overrideTitle+" | ") :
                (image.imageSource == "" ?
                     "" :
                     (PQCScriptsFilesPaths.getFilename(image.imageSource) + (overrideTitleSuffix!="" ?
                                                                       overrideTitleSuffix :
                                                                       "") + " | "))) + "PreviewQt"

    // convenience property to check whether window is in fullscreen or maximized
    property bool isFullscreen: toplevel.visibility === Window.FullScreen
    property bool isMaximized: toplevel.visibility === Window.Maximized

    // at startup the tray icon might not be ready when a message is supposed to be shown
    // a message stored here will be shown once the tray icon is ready
    property var messageWhenReady: ["",""]

    // whether the top row is supposed to be shown or not
    // we use a property here as the toprow is hidden behind an asynchronous loader
    property bool toprowMakeVisible: false

    // blakc background with slight transparency
    color: "#dd000000"

    // keypress cuaght
    signal keyPress(var modifiers, var keycode)

    // when hiding to tray, we do some cleanup
    onClosing: (close) => {
        if(PQCSettings.hideToSystemTray) {
            close.accepted = false
            toplevel.visibility = Window.Hidden
            image.loadImage("")
            extNotSet.hide()
            PQCScriptsFilesPaths.deleteTemporaryFiles()
        } else {
            PQCScriptsFilesPaths.deleteTemporaryFiles()
            Qt.quit()
        }
    }

    onVisibilityChanged: (visibility) => {
        if(toplevel.visibility == Window.Hidden) {
            closeAllMenus()
        } else {
            ignoreActiveChanges = true
            noLongerIgnoreActiveChanges.restart()
        }
    }

    property bool menuOpen: false
    signal closeAllMenus()

    // this ignores changes to active for hiding when focus is lost
    // this is necessary when 'focus follows mouse' is used and the window is, e.g., opened from the tray
    property bool ignoreActiveChanges: false
    Timer {
        id: noLongerIgnoreActiveChanges
        interval: 1000
        onTriggered: {
            ignoreActiveChanges = false
        }
    }

    onActiveChanged: {
        if(!active) closeAllMenus()
        if(!PQCSettings.closeWhenLosingFocus) return
        if(!active && !ignoreActiveChanges) {
            if((settings.status == Loader.Ready && settings.item.visible) ||
                    (about.status == Loader.Ready && about.item.visible) ||
                    (help.status == Loader.Ready && help.item.visible))
                return
            toplevel.close()
        }
    }

    function showMainContextMenu() {
        maincontextmenu.popup()
    }

    Menu {
        id: maincontextmenu
        MenuItem {
            icon.source: "image://svg/:/open.svg"
            text: qsTr("Open file")
            onTriggered: toplevel.openNewFile()
        }
        MenuItem {
            icon.source: "image://svg/:/settings.svg"
            text: qsTr("Settings")
            onTriggered: {
                settings.active = true
                settings.item.show()
            }
        }
        MenuSeparator {}
        MenuItem {
            icon.source: "image://svg/:/external.svg"
            enabled: image.imageSource!==""
            text: qsTr("Open externally")
            onTriggered: {
                if(image.imageSource === "") return
                if(PQCScriptsFilesPaths.openInDefault(image.imageSource)) {
                    if(PQCSettings.closeAfterDefaultApp)
                        toplevel.close()
                } else
                    extNotSet.open()
            }
        }
        MenuItem {
            icon.source: "image://svg/:/rotateleft.svg"
            enabled: ["sph", "vid", "mpv", "bok", "txt"].indexOf(image.currentType)==-1 && image.imageSource!==""
            text: qsTr("Rotate left")
            onTriggered: {
                image.setRotation -= 90
            }
        }
        MenuItem {
            icon.source: "image://svg/:/rotateright.svg"
            enabled: ["sph", "vid", "mpv", "bok", "txt"].indexOf(image.currentType)==-1 && image.imageSource!==""
            text: qsTr("Rotate right")
            onTriggered: {
                image.setRotation += 90
            }
        }
        MenuSeparator {}
        MenuItem {
            icon.source: "image://svg/:/about.svg"
            text: qsTr("About")
            onTriggered: {
                about.active = true
                about.item.show()
            }
        }
        MenuItem {
            icon.source: "image://svg/:/help.svg"
            text: qsTr("Help")
            onTriggered: {
                help.active = true
                help.item.show()
            }
        }
        MenuItem {
            icon.source: "image://svg/:/exit.svg"
            text: qsTr("Quit")
            onTriggered: {
                Qt.quit()
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
                maincontextmenu.close()
            }
        }
    }

    // we keep focus on this item in order to catch key presses
    Item {

        id: focusitem
        anchors.fill: parent

        Component.onCompleted:
            forceActiveFocus()

        Keys.onPressed: (event) => {
            toplevel.keyPress(event.modifiers, event.key)
        }

    }

    // at startup we ensure the focus is set to the focusitem
    Timer {
        interval: 200
        running: true
        onTriggered: focusitem.forceActiveFocus()
    }

    // Central message if no file is loaded
    Text {
        anchors.centerIn: parent
        color: "white"
        text: qsTr("Click to open file")
        font.pointSize: 12
        font.bold: true
        visible: image.imageSource===""
    }

    // The main image item
    PQImage { id: image }

    // The top row navigation
    Loader {
        id: toprow
        active: true
        asynchronous: true
        source: "components/PQTopRow.qml"
    }

    // The tray icon
    Loader {
        id: trayicon
        active: true
        asynchronous: true
        source: "components/PQTrayIcon.qml"
    }

    // The settings window
    Loader {
        id: settings
        active: false
        source: "windows/PQSettings.qml"
    }

    // The about window
    Loader {
        id: about
        active: false
        source: "windows/PQAbout.qml"
    }

    // The help window
    Loader {
        id: help
        active: false
        source: "windows/PQHelp.qml"
    }

    // The help window
    Loader {
        id: welcome
        active: false
        source: "windows/PQWelcome.qml"
    }

    ParallelAnimation {

        id: toplevelAni

        property int w_from
        property int w_to
        property int h_from
        property int h_to
        property int x_from
        property int x_to
        property int y_from
        property int y_to

        SmoothedAnimation {
            id: toplevelAniWidth
            target: toplevel
            duration: 200
            property: "width"
            from: toplevelAni.w_from
            to: toplevelAni.w_to
        }

        SmoothedAnimation {
            id: toplevelAniHeight
            target: toplevel
            duration: 200
            property: "height"
            from: toplevelAni.h_from
            to: toplevelAni.h_to
        }

        SmoothedAnimation {
            id: toplevelAniX
            target: toplevel
            duration: 200
            property: "x"
            from: toplevelAni.x_from
            to: toplevelAni.x_to
        }

        SmoothedAnimation {
            id: toplevelAniY
            target: toplevel
            duration: 200
            property: "y"
            from: toplevelAni.y_from
            to: toplevelAni.y_to
        }

    }

    // some things are done once window is set up
    Component.onCompleted: {

        PQCScriptsConfig.updateTranslation()

        // set the default window size
        toplevel.width = PQCSettings.defaultWindowWidth
        toplevel.height = PQCSettings.defaultWindowHeight

        // if PreviewQt is not supposed to be loaded hidden
        if(!PQCSettings.launchHiddenToSystemTray) {
            // show either maximized or normal
            if(PQCSettings.defaultWindowMaximized)
                showMaximized()
            else
                showNormal()
        }

        var msg = PQCScriptsFilesPaths.fromPercentEncoding(PQCScriptsOther.getStartupMessage())

        // if an image has been passed on, load that image
        if(msg !== "-") {

            if(!msg.includes(":/:/:"))
                image.loadImage(PQCScriptsFilesPaths.cleanPath(msg))
            else {
                var path = msg.split(":/:/:")[0]
                var inside = msg.split(":/:/:")[1]
                if(PQCScriptsImages.isArchive(path))
                    image.loadImage("%1::ARC::%2".arg(inside).arg(PQCScriptsFilesPaths.cleanPath(path)))
                else if(PQCScriptsImages.isPDFDocument(path))
                    image.loadImage("%1::PDF::%2".arg(inside).arg(PQCScriptsFilesPaths.cleanPath(path)))
                else
                    image.loadImage(PQCScriptsFilesPaths.cleanPath(msg))
            }

        // if no image has been passed on and PreviewQt is supposed to be loaded hidden
        } else if(PQCSettings.launchHiddenToSystemTray && PQCSettings.notifyNextLaunchHiddenToSystemTray) {
            PQCSettings.notifyNextLaunchHiddenToSystemTray = false
            // show launch message
            var title = qsTr("PreviewQt launched")
            var content = qsTr("PreviewQt has been launched and hidden to the system tray.")
            messageWhenReady = [title, content]
            if(trayicon.status == Loader.Ready)
                trayicon.item.showMessage(title, content, SystemTrayIcon.Information, 5000)
        }

        if(PQCSettings.getFirstStart()) {
            welcome.active = true
            welcome.item.show()
        }

    }

    // When a key combo has been pressed
    onKeyPress: (modifiers, keycode) => {
        processKeyEvent(modifiers, keycode)
    }

    function processKeyEvent(modifiers, keycode) {

        // convert to text
        var txt = PQCScriptsOther.keycodeToString(modifiers, keycode)

        if(extNotSet.visible) {
            if(txt === "Esc")
                extNotSet.hide()
            else if(txt === "Enter" || txt === "Return") {
                extNotSet.hide()
                settings.active = true
                settings.item.show()
                settings.item.selectTab(1)
            }
            return
        }

        // Escape either leaves fullscreen or closes the window
        if(txt === "Esc") {

            if(menuOpen) {
                closeAllMenus()
                return
            }

            if(isFullscreen)
                toplevel.showNormal()
            else
                toplevel.close()

        } else if(txt === "Ctrl+Q") {

            Qt.quit()

        } else if(txt === "Ctrl+O") {

            if(menuOpen) {
                closeAllMenus()
                return
            }

            openNewFile()

        } else if(txt === "Ctrl+P") {

            if(menuOpen) {
                closeAllMenus()
                return
            }

            settings.active = true
            settings.item.show()

        } else if(txt === "Ctrl+I") {

            if(menuOpen) {
                closeAllMenus()
                return
            }

            about.active = true
            about.item.show()

        } else if(txt === "F1") {

            if(menuOpen) {
                closeAllMenus()
                return
            }

            help.active = true
            help.item.show()

        } else if(txt === PQCSettings.defaultAppShortcut) {

            if(menuOpen)
                closeAllMenus()

            if(image.imageSource === "") return

            if(PQCScriptsFilesPaths.openInDefault(image.imageSource)) {
                if(PQCSettings.closeAfterDefaultApp)
                    toplevel.close()
            } else
                extNotSet.open()

        }
    }

    // listen to command line arguments
    Connections {

        target: PQCScriptsOther

        function onCommandLineArgumentReceived(msg) {

            closeAllMenus()

            msg = PQCScriptsFilesPaths.fromPercentEncoding(msg)

            // empty message -> show window
            if(msg === "-") {

                if(!toplevel.visible) {
                    if(PQCSettings.defaultWindowMaximized)
                        toplevel.showMaximized()
                    else
                        toplevel.showNormal()
                }
                toplevel.raise()
                toplevel.requestActivate()

            // file passed on
            } else {

                var fileInside = ""
                var path = msg
                if(msg.includes(":/:/:")) {
                    path = PQCScriptsFilesPaths.cleanPath(msg.split(":/:/:")[0])
                    fileInside = msg.split(":/:/:")[1]
                }

                // check if file exists
                if(!PQCScriptsFilesPaths.doesFileExist(path)) {
                    trayicon.item.showMessage(qsTr("File does not exist."), qsTr("The requested file does not exist:") + " %1".arg(path))
                    return
                }

                if(fileInside != "") {
                    if(PQCScriptsImages.isPDFDocument(path))
                        image.loadImage("%1::PDF::%2".arg(fileInside).arg(path))
                    else if(PQCScriptsImages.isArchive(path))
                        image.loadImage("%1::ARC::%2".arg(fileInside).arg(path))
                    else
                        image.loadImage(path)
                } else
                    image.loadImage(path)

                if(!toplevel.visible) {
                    if(PQCSettings.defaultWindowMaximized)
                        toplevel.showMaximized()
                    else
                        toplevel.showNormal()
                }
                toplevel.raise()
                toplevel.requestActivate()

            }

        }

    }

    Rectangle {

        id: extNotSet
        anchors.fill: parent
        color: "#ee222222"

        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        visible: opacity>0

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {}
        }

        Column {

            x: (parent.width-width)/2
            y: (parent.height-height)/2
            spacing: 10

            Text {
                x: (parent.width-width)/2
                width: Math.min(extNotSet.width, 500)
                horizontalAlignment: Text.AlignHCenter
                color: "white"
                font.pointSize: 14
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("Some of the external applications cannot be started or are not set up correctly.")
            }
            Text {
                x: (parent.width-width)/2
                width: Math.min(extNotSet.width, 500)
                horizontalAlignment: Text.AlignHCenter
                color: "white"
                font.pointSize: 14
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("Please correct this in the settings.")
            }

            Item {
                width: 1
                height: 1
            }

            Row {
                x: (parent.width-width)/2
                spacing: 5
                Button {
                    text: qsTr("Ok")
                    onClicked:  {
                        extNotSet.hide()
                    }
                }
                Button {
                    text: qsTr("Go to settings")
                    onClicked: {
                        extNotSet.hide()
                        settings.active = true
                        settings.item.show()
                        settings.item.selectTab(1)
                    }
                }
            }

        }

        function open() {
            opacity = 1
        }
        function hide() {
            opacity = 0
            focusitem.forceActiveFocus()
        }

    }

    function openNewFile() {
        var path = PQCScriptsFilesPaths.openNewFile()
        if(path !== "")
            image.loadImage(path)
    }

    // make sure the window is fit to the main image view
    function updateWindowSize(w, h) {

        if(!PQCSettings.maximizeImageSizeAndAdjustWindow || isMaximized || isFullscreen || manualWindowSizeChange)
            return

        var fitsize = PQCScriptsOther.fitSizeInsideSize(w, h, PQCSettings.defaultWindowWidth, PQCSettings.defaultWindowHeight)

        toplevelAni.stop()
        toplevelAni.w_from = toplevel.width
        toplevelAni.w_to = Math.max(fitsize.width, minimumWidth)
        toplevelAni.h_from = toplevel.height
        toplevelAni.h_to = Math.max(fitsize.height + (PQCSettings.topBarAutoHide ? 1 : toprow.height), minimumHeight)
        toplevelAni.x_from = toplevel.x
        toplevelAni.x_to = toplevel.x + (toplevel.width - toplevelAni.w_to)/2
        toplevelAni.y_from = toplevel.y
        toplevelAni.y_to = toplevel.y + (toplevel.height - toplevelAni.h_to)/2
        toplevelAni.start()

    }

}
