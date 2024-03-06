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

import QtQuick
import QtQuick.Controls
import PQCScripts

Window {

    id: toplevel

    width: 640
    height: 480
    visible: true
    title: "ViewQt"

    color: "#bb000000"

    Button {
        id: openbut
        anchors.centerIn: parent
        font.pointSize: 12
        text: "Open a file"
        visible: false
        onClicked: {
            var file = PQCScripts.openFile()
            if(file !== "")
                image.source = "image://full/" + file
        }
    }

    Image {
        id: image
        asynchronous: true
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size(width, height)
        smooth: false
        mipmap: false
        source: ""
        onSourceChanged: {
            if(source === "")
                toplevel.title = "ViewQt"
            else
                toplevel.title = PQCScripts.getFilename(source) + " | ViewQt"
        }
    }

    Component.onCompleted: {

        if(Qt.application.arguments.length > 1 && PQCScripts.fileExists(Qt.application.arguments[1]))
            image.source = "image://full/" + PQCScripts.cleanPath(Qt.application.arguments[1])
        else
            openbut.visible = true
    }

}
