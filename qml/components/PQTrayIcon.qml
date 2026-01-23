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
import PreviewQt

SystemTrayIcon {

    id: traytop

    // style tray icon
    visible: true
    icon.source: "image://svg/:/logo.svg"

    // show/hide application window
    onActivated: {
        PQCNotify.mainwindowToggleVisibility()
    }

    // the context menu
    menu: Menu {
        visible: false
        MenuItem {
            text: PQCConstants.windowMainVisible ? qsTr("Hide window") : qsTr("Show window")
            onTriggered: {
                PQCNotify.mainwindowToggleVisibility()
            }
        }
        MenuItem {
            text: qsTr("Quit PreviewQt")
            onTriggered:
                Qt.quit()
        }

        // This cannot be part of the SystemTrayIcon element, that results in the error:
        // 'Cannot assign to non-existent default property'
        Connections {
            target: PQCNotify
            function onTrayiconShowNotification(title : string, txt : string) {
                traytop.showMessage(title, txt, SystemTrayIcon.Information, 5000)
            }
        }

    }

    // check if a message is to be shown once set up
    Component.onCompleted: {
        if(PQCConstants.trayiconShowNotificationWhenReady[0] !== "")
            showMessage(PQCConstants.trayiconShowNotificationWhenReady[0], PQCConstants.trayiconShowNotificationWhenReady[1], SystemTrayIcon.Information, 5000)
    }

 }
