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

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import PreviewQt

ApplicationWindow {

    id: toplevel

    // default stylings
    width: 800
    height: 600
    minimumWidth: 300
    minimumHeight: 200

    Item {

        anchors.fill: parent

        onWidthChanged: {
            PQCConstants.mainwindowWidth = width
            if(!toplevelAni.running)
                PQCConstants.mainwindowManuallyResized = true
        }
        onHeightChanged: {
            PQCConstants.mainwindowHeight = height
            if(!toplevelAni.running)
                PQCConstants.mainwindowManuallyResized = true
        }

    }

    // it is hidden by default until we set the stylings from the settings below
    visibility: Window.Hidden
    title: (PQCConstants.mainwindowOverrideTitle!== "" ?
                (PQCConstants.mainwindowOverrideTitle+" | ") :
                (PQCConstants.currentSource=== "" ?
                     "" :
                     (PQCScriptsFilesPaths.getFilename(PQCConstants.currentSource) + PQCConstants.mainwindowOverrideTitleSuffix + " | "))) + "PreviewQt"

    // black background with slight transparency
    color: "#dd000000"

    // when hiding to tray, we do some cleanup
    onClosing: (close) => {
        if(PQCSettings.hideToSystemTray) {
            close.accepted = false
            toplevel.visibility = Window.Hidden
            PQCNotify.loadNewFile("")
            extNotSet.hide()
            PQCScriptsFilesPaths.deleteTemporaryFiles()
        } else {
            PQCScriptsFilesPaths.deleteTemporaryFiles()
            Qt.quit()
        }
    }

    onVisibilityChanged: (visibility) => {

        PQCConstants.mainwindowIsFullscreen = (visibility === Window.FullScreen)
        PQCConstants.mainwindowIsMaximized = (visibility === Window.Maximized)

        if(visibility === Window.Hidden) {
            PQCConstants.windowMainVisible = false
            PQCNotify.closeAllMenus()
        } else {
            PQCConstants.windowMainVisible = true
            ignoreActiveChanges = true
            noLongerIgnoreActiveChanges.restart()
        }
    }

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
        if(!active) PQCNotify.closeAllMenus()
        if(!PQCSettings.closeWhenLosingFocus) return
        if(!active && !ignoreActiveChanges) {
            if(PQCConstants.windowSettingsVisible || PQCConstants.windowAboutVisible ||
                    PQCConstants.windowHelpVisible || PQCConstants.windowEnterPathVisible ||
                    PQCConstants.windowMediaInfoVisible)
                return
            toplevel.close()
        }
    }

    Menu {
        id: maincontextmenu
        MenuItem {
            text: qsTr("Open file")
            onTriggered: PQCNotify.requestNewFile()
        }
        MenuItem {
            text: qsTr("Type in a path or URL")
            onTriggered: PQCNotify.showSubWindow("enterpath")
        }
        MenuItem {
            text: qsTr("Settings")
            onTriggered: {
                PQCNotify.showSubWindow("settings")
            }
        }
        MenuSeparator {}
        MenuItem {
            enabled: PQCConstants.currentSource!==""
            text: qsTr("Open externally")
            onTriggered: {
                if(PQCConstants.currentSource === "") return
                if(PQCScriptsFilesPaths.openInDefault(PQCConstants.currentSource)) {
                    if(PQCSettings.closeAfterDefaultApp)
                        toplevel.close()
                } else
                    extNotSet.open()
            }
        }
        MenuItem {
            enabled: ["doc", "arc", "ani", "svg", "img"].indexOf(PQCConstants.currentType)>-1 && PQCConstants.currentSource!==""
            text: qsTr("Rotate left")
            onTriggered: {
                PQCNotify.addRotation(-90)
            }
        }
        MenuItem {
            enabled: ["doc", "arc", "ani", "svg", "img"].indexOf(PQCConstants.currentType)>-1 && PQCConstants.currentSource!==""
            text: qsTr("Rotate right")
            onTriggered: {
                PQCNotify.addRotation(90)
            }
        }
        MenuItem {
            enabled: PQCConstants.currentType==="url" && PQCConstants.currentSource!==""
            text: qsTr("Reload")
            onTriggered: {
                PQCNotify.loadNewFile(PQCConstants.currentSource)
            }
        }
        MenuItem {
            enabled: PQCConstants.currentType==="aud" || (PQCConstants.currentType === "vid" && PQCScriptsConfig.isQtMultimediaEnabled())
            text: qsTr("Show media info")
            onTriggered: {
                PQCNotify.showSubWindow("mediainfo")
            }
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("About")
            onTriggered: {
                PQCNotify.showSubWindow("about")
            }
        }
        MenuItem {
            text: qsTr("Help")
            onTriggered: {
                PQCNotify.showSubWindow("help")
            }
        }
        MenuItem {
            text: qsTr("Quit")
            onTriggered: {
                Qt.quit()
            }
        }
        onAboutToShow: {
            PQCConstants.menuIsOpen = true
        }
        onAboutToHide: {
            PQCConstants.menuIsOpen = false
            focusitem.forceActiveFocus()
        }
        Connections {
            target: PQCNotify
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
            PQCNotify.mainwindowKeyPress(event.modifiers, event.key)
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
        visible: PQCConstants.currentSource===""
    }

    // The main image item
    PQImage { id: image }

    // The top row navigation
    Loader {
        id: toprow
        active: true
        asynchronous: true
        sourceComponent: PQTopRow {}
    }

    // The tray icon
    Loader {
        id: trayicon
        active: true
        asynchronous: true
        sourceComponent: PQTrayIcon {}
    }

    // The settings window
    Loader {
        id: settings
        active: false
        sourceComponent: PQSettings {}
    }

    // The about window
    Loader {
        id: about
        active: false
        sourceComponent: PQAbout {}
    }

    // The help window
    Loader {
        id: help
        active: false
        sourceComponent: PQHelp {}
    }

    // The help window
    Loader {
        id: welcome
        active: false
        sourceComponent: PQWelcome {}
    }

    // The text input for entering path/url
    Loader {
        id: enterpath
        active: false
        sourceComponent:
            PQEnterPathUrl {}
    }

    // The media info window
    Loader {
        id: mediainfo
        active: false
        sourceComponent: PQMediaInfo {}
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

        PQCScriptsConfig.updateTranslation(PQCSettings.language)

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
                PQCNotify.loadNewFile(PQCScriptsFilesPaths.cleanPath(msg))
            else {
                var path = msg.split(":/:/:")[0]
                var inside = msg.split(":/:/:")[1]
                if(PQCScriptsImages.isArchive(path))
                    PQCNotify.loadNewFile("%1::ARC::%2".arg(inside).arg(PQCScriptsFilesPaths.cleanPath(path)))
                else if(PQCScriptsImages.isPDFDocument(path))
                    PQCNotify.loadNewFile("%1::PDF::%2".arg(inside).arg(PQCScriptsFilesPaths.cleanPath(path)))
                else
                    PQCNotify.loadNewFile(PQCScriptsFilesPaths.cleanPath(msg))
            }

        // if no image has been passed on and PreviewQt is supposed to be loaded hidden
        } else if(PQCSettings.launchHiddenToSystemTray && PQCSettings.notifyNextLaunchHiddenToSystemTray) {
            PQCSettings.notifyNextLaunchHiddenToSystemTray = false
            // show launch message
            var title = qsTr("PreviewQt launched")
            var content = qsTr("PreviewQt has been launched and hidden to the system tray.")
            if(trayicon.status == Loader.Ready)
                PQCNotify.trayiconShowNotification(title, content)
            else
                PQCConstants.trayiconShowNotificationWhenReady = [title, content]
        }

        if(PQCSettings.getFirstStart()) {
            welcome.active = true
            welcome.item.show()
        }

    }

    function processKeyEvent(modifiers : int, keycode : int) {

        // convert to text
        var txt = PQCScriptsOther.keycodeToString(modifiers, keycode)

        if(extNotSet.visible) {
            if(txt === "Esc")
                extNotSet.hide()
            else if(txt === "Enter" || txt === "Return") {
                extNotSet.hide()
                PQCConstants.settingsTabNextTime = 1
                PQCNotify.showSubWindow("settings")
            }
            return
        }

        // Escape either leaves fullscreen or closes the window
        if(txt === "Esc") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            if(PQCConstants.mainwindowIsFullscreen)
                toplevel.showNormal()
            else
                toplevel.close()

        } else if(txt === "Ctrl+Q") {

            Qt.quit()

        } else if(txt === "Ctrl+O") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            PQCNotify.requestNewFile()

        } else if(txt === "Ctrl+P") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            PQCNotify.showSubWindow("settings")

        } else if(txt === "Ctrl+I") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            PQCNotify.showSubWindow("about")

        } else if(txt === "F1") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            PQCNotify.showSubWindow("help")

        } else if(txt === "Ctrl+U") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            PQCNotify.showSubWindow("enterpath")

        } else if(txt === "Ctrl+V") {

            if(PQCConstants.menuIsOpen) {
                PQCNotify.closeAllMenus()
                return
            }

            var clp = PQCScriptsOther.getClipboardContents()
            if(PQCScriptsFilesPaths.isFileSupported(clp))
                PQCNotify.loadNewFile(clp)
            else
                PQCNotify.trayiconShowNotification(qsTr("Unsupported"), qsTr("The pasted content appears to be unsupported by PreviewQt."))

        } else if(txt === PQCSettings.defaultAppShortcut) {

            if(PQCConstants.menuIsOpen)
                PQCNotify.closeAllMenus()

            if(PQCConstants.currentSource === "") return

            if(PQCScriptsFilesPaths.openInDefault(PQCConstants.currentSource)) {
                if(PQCSettings.closeAfterDefaultApp)
                    toplevel.close()
            } else
                extNotSet.open()

        }
    }

    // listen to command line arguments
    Connections {

        target: PQCScriptsOther

        function onCommandLineArgumentReceived(msg : string) : void {

            PQCNotify.closeAllMenus()

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

                // check if file is supported
                if(!PQCScriptsFilesPaths.isFileSupported(path)) {
                    trayicon.item.showMessage(qsTr("File does not exist."), qsTr("The requested file does not exist:") + " %1".arg(path))
                    return
                }

                if(fileInside != "") {
                    if(PQCScriptsImages.isPDFDocument(path))
                        PQCNotify.loadNewFile("%1::PDF::%2".arg(fileInside).arg(path))
                    else if(PQCScriptsImages.isArchive(path))
                        PQCNotify.loadNewFile("%1::ARC::%2".arg(fileInside).arg(path))
                    else
                        PQCNotify.loadNewFile(path)
                } else
                    PQCNotify.loadNewFile(path)

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
                        PQCConstants.settingsTabNextTime = 1
                        PQCNotify.showSubWindow("settings")
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

    Connections {

        target: PQCNotify

        function onUpdateWindowSize(w : int, h : int) {

            if(!PQCSettings.maximizeImageSizeAndAdjustWindow || PQCConstants.mainwindowIsMaximized ||
                    PQCConstants.mainwindowIsFullscreen || PQCConstants.mainwindowManuallyResized)
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

        function onMainwindowKeyPress(modifiers : int, keycode : int) {
            toplevel.processKeyEvent(modifiers, keycode)
        }

        function onMainwindowToggleVisibility() {
            if(toplevel.visible) {
                PQCSettings.hideToSystemTray = true
                toplevel.close()
            } else {
                toplevel.show()
                toplevel.raise()
                toplevel.requestActivate()
            }
        }

        function onMainwindowToggleMaximized() {
            if(toplevel.visibility === Window.Maximized)
                toplevel.showNormal()
            else
                toplevel.showMaximized()
        }

        function onMainwindowStartSystemMove() {
            toplevel.startSystemMove()
        }

        function onShowMainContextMenu() {
            maincontextmenu.popup()
        }

        function onRequestNewFile() {
            var path = PQCScriptsFilesPaths.openNewFile()
            if(path !== "")
                PQCNotify.loadNewFile(path)
        }

        function onMainwindowClose() {
            toplevel.close()
        }

        function onShowSubWindow(wdw : string) {
            if(wdw === "help") {
                if(!help.active)
                    help.active = true
            } else if(wdw === "about") {
                if(!about.active)
                    about.active = true
            } else if(wdw === "settings") {
                if(!settings.active)
                    settings.active = true
            } else if(wdw === "enterpath") {
                if(!enterpath.active)
                    enterpath.active = true
            } else if(wdw === "mediainfo") {
                if(!mediainfo.active)
                    mediainfo.active = true
            }
        }

        function onShowExtNotSet() {
            extNotSet.open()
        }

        function onMainwindowShowNormal() {
            toplevel.showNormal()
        }

        function onMainwindowShowFullscreen() {
            toplevel.showFullScreen()
        }

        function onResetFocus() {
            focusitem.forceActiveFocus()
        }

    }

}
