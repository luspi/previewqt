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

import QtCore
import Qt.labs.platform
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import PQCScripts
import PQCImageFormats
import PQCSettings

import "./components"

ApplicationWindow {

    id: toplevel

    // default stylings
    width: 800
    height: 600
    minimumWidth: 200
    minimumHeight: 200

    // it is hidden by default until we set the stylings from the settings below
    visible: false
    title: (image.imageSource == "" ? "" : (PQCScripts.getFilename(image.imageSource) + " | ")) + "PreviewQt"

    // convenience property to check whether window is in fullscreen
    property bool isFullscreen: toplevel.visibility === Window.FullScreen

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
        close.accepted = false
        toplevel.visibility = Window.Hidden
        image.loadImage("")
        extNotSet.hide()
        PQCScripts.deleteTemporaryFiles()
    }

    onActiveChanged: {
        if(!PQCSettings.closeWhenLosingFocus) return
        if(!active) {
            if((settings.status == Loader.Ready && settings.item.visible) ||
                    (about.status == Loader.Ready && about.item.visible) ||
                    (help.status == Loader.Ready && help.item.visible))
                return
            toplevel.close()
        }
    }


    // we keep focus on this item in order to catch key presses
    Item {

        id: focusitem

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

    // some things are done once window is set up
    Component.onCompleted: {

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

        var msg = PQCScripts.fromPercentEncoding(PQCScripts.getStartupMessage())

        // if an image has been passed on, load that image
        if(msg !== "-") {

            if(!msg.includes(":/:/:"))
                image.loadImage(PQCScripts.cleanPath(msg))
            else {
                var path = msg.split(":/:/:")[0]
                var inside = msg.split(":/:/:")[1]
                if(PQCScripts.isArchive(path))
                    image.loadImage("%1::ARC::%2".arg(inside).arg(PQCScripts.cleanPath(path)))
                else if(PQCScripts.isPDFDocument(path))
                    image.loadImage("%1::PDF::%2".arg(inside).arg(PQCScripts.cleanPath(path)))
                else
                    image.loadImage(PQCScripts.cleanPath(msg))
            }

        // if no image has been passed on and PreviewQt is supposed to be loaded hidden
        } else if(PQCSettings.launchHiddenToSystemTray) {
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

    // dialog for opening an image file
    FileDialog {
        id: fileDialog
        currentFolder: (PQCScripts.amIOnWindows() ? "file:/" : "file://") + PQCSettings.filedialogLocation
        nameFilters: "Images (*.%1)".arg(PQCImageFormats.getAllFormats().join(" *."))
        onAccepted: image.loadImage(selectedFile)
    }

    // When a key combo has been pressed
    onKeyPress: (modifiers, keycode) => {

        // convert to text
        var txt = PQCScripts.keycodeToString(modifiers, keycode)

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

            if(isFullscreen)
                toplevel.showNormal()
            else
                toplevel.close()

        } else if(txt === "Ctrl+Q") {

            Qt.quit()

        } else if(txt === "Ctrl+O") {

            fileDialog.open()

        } else if(txt === "Ctrl+P") {

            settings.active = true
            settings.item.show()

        } else if(txt === "Ctrl+I") {

            about.active = true
            about.item.show()

        } else if(txt === "F1") {

            help.active = true
            help.item.show()

        } else if(txt === PQCSettings.defaultAppShortcut) {

            if(image.imageSource === "") return

            if(PQCScripts.openInDefault(image.imageSource)) {
                if(PQCSettings.closeAfterDefaultApp)
                    toplevel.close()
            } else
                extNotSet.open()

        }
    }

    // listen to command line arguments
    Connections {

        target: PQCScripts

        function onCommandLineArgumentReceived(msg) {

            msg = PQCScripts.fromPercentEncoding(msg)

            console.warn("** msg =", msg)

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
                    path = PQCScripts.cleanPath(msg.split(":/:/:")[0])
                    fileInside = msg.split(":/:/:")[1]
                }

                // check if file exists
                if(!PQCScripts.doesFileExist(path)) {
                    trayicon.item.showMessage(qsTr("File does not exist."), qsTr("The requested file does not exist:") + " %1".arg(path))
                    return
                }

                console.warn("**", path, "/", fileInside)

                if(fileInside != "") {
                    if(PQCScripts.isPDFDocument(path))
                        image.loadImage("%1::PDF::%2".arg(fileInside).arg(path))
                    else if(PQCScripts.isArchive(path))
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

}
