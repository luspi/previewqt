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

    width: 800
    height: 600
    minimumWidth: 200
    minimumHeight: 200
    visible: false
    title: "PreviewQt"

    property bool isFullscreen: toplevel.visibility === Window.FullScreen

    color: "#dd000000"

    signal keyPress(var modifiers, var keycode)

    onClosing: {
        if(PQCSettings.hideToSystemTray)
            clearSetImage.restart()
        else
            PQCScripts.deleteTemporaryFiles()
    }

    Timer {
        id: clearSetImage
        interval: 200
        onTriggered: {
            image.imageSource = ""
            PQCScripts.deleteTemporaryFiles()
        }
    }

    Item {

        id: focusitem

        // this is for catching key presses
        Component.onCompleted:
            forceActiveFocus()

        Keys.onPressed: (event) => {
            toplevel.keyPress(event.modifiers, event.key)
        }

    }

    Timer {
        interval: 200
        running: true
        onTriggered: focusitem.forceActiveFocus()
    }

    Text {
        anchors.centerIn: parent
        color: "white"
        text: "Click to open file"
        font.pointSize: 12
        font.bold: true
        visible: image.imageSource===""
    }

    PQImage { id: image }

    PQTopRow { id: toprow }

    PQTrayIcon { id: trayicon }

    PQSettings { id: settings }

    PQAbout { id: about }

    Component.onCompleted: {

        toplevel.width = PQCSettings.defaultWindowWidth
        toplevel.height = PQCSettings.defaultWindowHeight
        if(PQCSettings.defaultWindowMaximized)
            showMaximized()
        else
            showNormal()

        if(PQCSettings.launchHiddenToSystemTray)
            PQCSettings.hideToSystemTray = true

        if(Qt.application.arguments.length > 1 && PQCScripts.fileExists(Qt.application.arguments[1]))
            image.imageSource = PQCScripts.toPercentEncoding(PQCScripts.cleanPath(Qt.application.arguments[1]))
        else if(PQCSettings.launchHiddenToSystemTray) {
            toplevel.close()
            trayicon.showMessage("PreviewQt launched", "PreviewQt has been launched hidden to the system tray.", SystemTrayIcon.Information, 5000)
        }
    }

    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        nameFilters: "Images (*.%1)".arg(PQCImageFormats.getAllFormats().join(" *."))
        onAccepted: image.loadImage(selectedFile)
    }

    onKeyPress: (modifiers, keycode) => {

        var txt = PQCScripts.keycodeToString(modifiers, keycode)

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
            settings.show()
        } else if(txt === "Ctrl+I") {
            about.show()
        } else if(txt === PQCSettings.defaultAppShortcut) {
            PQCScripts.openInDefault(image.imageSource)
            if(PQCSettings.closeAfterDefaultApp)
                toplevel.close()
        }
    }

    Connections {

        target: image

        function onDoubleClick() {

            if(isFullscreen)
                toplevel.showNormal()
            else
                toplevel.showFullScreen()

        }

    }

    Connections {

        target: PQCScripts

        function onCommandLineArgumentReceived(msg) {

            if(msg === ":/:/:") {
                if(!toplevel.visible) {
                    if(PQCSettings.defaultWindowMaximized)
                        toplevel.showMaximized()
                    else
                        toplevel.showNormal()
                }
                toplevel.raise()
                toplevel.requestActivate()
            } else {

                if(!PQCScripts.doesFileExist(PQCScripts.cleanPath(msg))) {
                    trayicon.showMessage("File does not exist.", "The requested file does not exist...")
                    return
                }

                image.imageSource = PQCScripts.toPercentEncoding(PQCScripts.cleanPath(msg))

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
