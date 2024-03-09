/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
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
    title: "PreviewQt"

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

    // when hiding to tray, we do some cleanup with a short delay
    onClosing: {
        clearSetImage.restart()
    }

    Timer {
        id: clearSetImage
        interval: 200
        onTriggered: {
            image.loadImage("")
            PQCScripts.deleteTemporaryFiles()
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
        text: "Click to open file"
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
        source: "components/PQSettings.qml"
    }

    // The about window
    Loader {
        id: about
        active: false
        source: "components/PQAbout.qml"
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

        // if an image has been passed on, load that image
        if(Qt.application.arguments.length > 1 && PQCScripts.fileExists(Qt.application.arguments[1]))

            image.loadImage(PQCScripts.toPercentEncoding(PQCScripts.cleanPath(Qt.application.arguments[1])))

        // if no image has been passed on and PreviewQt is supposed to be loaded hidden
        else if(PQCSettings.launchHiddenToSystemTray) {
            // show launch message
            var title = "PreviewQt launched"
            var content = "PreviewQt has been launched hidden to the system tray."
            messageWhenReady = [title, content]
            if(trayicon.status == Loader.Ready)
                trayicon.item.showMessage(title, content, SystemTrayIcon.Information, 5000)
        }
    }

    // dialog for opening an image file
    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        nameFilters: "Images (*.%1)".arg(PQCImageFormats.getAllFormats().join(" *."))
        onAccepted: image.loadImage(selectedFile)
    }

    // When a key combo has been pressed
    onKeyPress: (modifiers, keycode) => {

        // convert to text
        var txt = PQCScripts.keycodeToString(modifiers, keycode)

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

        } else if(txt === PQCSettings.defaultAppShortcut) {

            PQCScripts.openInDefault(image.imageSource)
            if(PQCSettings.closeAfterDefaultApp)
                toplevel.close()

        }
    }

    // listen to command line arguments
    Connections {

        target: PQCScripts

        function onCommandLineArgumentReceived(msg) {

            // empty message -> show window
            if(msg === ":/:/:") {

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

                // check if file exists
                if(!PQCScripts.doesFileExist(PQCScripts.cleanPath(msg))) {
                    trayicon.item.showMessage("File does not exist.", "The requested file does not exist...")
                    return
                }

                image.loadImage(PQCScripts.toPercentEncoding(PQCScripts.cleanPath(msg)))

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

}
