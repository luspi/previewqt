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

import QtQuick
import PQCScripts
import PQCSettings

Item {

    id: image_top

    // some stylings
    x: 5
    y: (PQCSettings.topBarAutoHide ? 0 : 40)+5
    width: toplevel.width-2*5
    height: toplevel.height-(PQCSettings.topBarAutoHide ? 0 : 40)-2*5

    clip: true

    // the source of the current image
    property string imageSource: ""

    // these are used for a delay in reloading the image
    property int windowWidth: 200
    property int windowHeight: 200

    // react to window size changes with a delau
    Timer {
        id: updateWindowSize
        interval: 500
        onTriggered: {
            windowWidth = image_top.width
            windowHeight = image_top.height
        }
    }

    Connections {
        target: image_top
        function onWidthChanged(width) {
            updateWindowSize.restart()
        }
        function onHeightChanged(height) {
            updateWindowSize.restart()
        }
    }

    Component.onCompleted: {
        windowWidth = image_top.width
        windowHeight = image_top.height
    }

    // react to clicks, double clicks, and movements
    MouseArea {
        id: imagemouse
        anchors.fill: parent
        anchors.margins: -5
        hoverEnabled: true
        cursorShape: image.imageSource === "" ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: (mouse) => {
            if(image.imageSource === "")
                fileDialog.open()
        }
        onPositionChanged: (mouse) => {
            if(mouse.y < 30)
                toplevel.toprowMakeVisible = true
            else
                toplevel.toprowMakeVisible = false
        }

        onDoubleClicked: {
            if(toplevel.isFullscreen)
                toplevel.showNormal()
            else
                toplevel.showFullScreen()
        }
    }

    DropArea {
        anchors.fill: parent
        onDropped: (drop) => {
            var src = PQCScripts.cleanPath(drop.text)
            if(PQCScripts.isFileSupported(src))
                loadImage(src)
        }
    }

    // the actual image
    Loader {
        id: imageloader
    }

    // show the image
    function showImage() {
        opacity = 1
    }

    // load a new image
    function loadImage(path) {

        imageloader.source = ""

        if(path === "") {
            imageSource = ""
            return
        }

        imageSource = PQCScripts.cleanPath(path)

        PQCSettings.filedialogLocation = PQCScripts.getDir(imageSource)

        if(PQCScripts.isPDFDocument(imageSource))
            imageloader.source = "imageitems/PQDocument.qml"
        else if(PQCScripts.isArchive(imageSource))
            imageloader.source = "imageitems/PQArchive.qml"
        else if(PQCScripts.isMpvVideo(imageSource))
            imageloader.source = "imageitems/PQVideoMpv.qml"
        else if(PQCScripts.isQtVideo(imageSource))
            imageloader.source = "imageitems/PQVideoQt.qml"
        else if(PQCScripts.isItAnimated(imageSource))
            imageloader.source = "imageitems/PQImageAnimated.qml"
        else if(PQCScripts.isPhotoSphere(imageSource))
            imageloader.source = "imageitems/PQPhotoSphere.qml"
        else
            imageloader.source = "imageitems/PQImageNormal.qml"

    }

}
