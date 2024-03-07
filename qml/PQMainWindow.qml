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
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import PQCScripts
import PQCImageFormats

import "./components"

ApplicationWindow {

    id: toplevel

    width: 640
    height: 480
    minimumWidth: 200
    minimumHeight: 200
    visible: true
    title: "PreviewQt"

    color: "#bb000000"

    PQImage { id: image }

    PQTopRow { id: toprow }

    Component.onCompleted: {
        if(Qt.application.arguments.length > 1 && PQCScripts.fileExists(Qt.application.arguments[1]))
            image.imageSource = "image://full/" + PQCScripts.cleanPath(Qt.application.arguments[1])
    }

    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        nameFilters: "Images (*.%1)".arg(PQCImageFormats.getAllFormats().join(" *."))
        onAccepted: image.imageSource = "image://full/" + PQCScripts.cleanPath(selectedFile)
    }

}
