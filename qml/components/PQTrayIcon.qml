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
import Qt.labs.platform
import PQCSettings

SystemTrayIcon {

    // style tray icon
    visible: true
    icon.source: "image://svg/:/logo.svg"

    // show/hide application window
    onActivated: {
        if(toplevel.visible)
            toplevel.close()
        else {
            toplevel.show()
            toplevel.raise()
            toplevel.requestActivate()
        }
    }

    // the context menu
    menu: Menu {
        visible: false
        MenuItem {
            text: toplevel.visible ? qsTr("Hide window") : qsTr("Show window")
            onTriggered: {
                toplevel.visible = !toplevel.visible
            }
        }
        MenuItem {
            text: qsTr("Quit PreviewQt")
            onTriggered:
                Qt.quit()
        }
    }

    // check if a message is to be shown once set up
    Component.onCompleted: {
        if(toplevel.messageWhenReady[0] !== "")
            showMessage(toplevel.messageWhenReady[0], toplevel.messageWhenReady[1], SystemTrayIcon.Information, 5000)
    }

 }
