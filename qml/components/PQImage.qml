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
import PreviewQt

Item {

    id: image_top

    x: 5
    y: (PQCSettings.topBarAutoHide ? 0 : 40)+5
    width: PQCConstants.mainwindowWidth-2*5
    height: PQCConstants.mainwindowHeight-(PQCSettings.topBarAutoHide ? 0 : 40)-2*5

    property int setRotation: 0
    onSetRotationChanged:
        PQCConstants.imageRotation = setRotation

    clip: true

    // react to window size changes with a delay
    Timer {
        id: updateWindowSize
        interval: 500
        onTriggered: {
            if(imageloader.item != null)
                PQCNotify.setImageAsync(false)
            PQCConstants.imageAvailableSizeDelay = PQCConstants.imageAvailableSize
        }
    }

    Connections {
        target: image_top
        function onWidthChanged(width : int) {
            PQCConstants.imageAvailableSize.width = image_top.width
            updateWindowSize.restart()
        }
        function onHeightChanged(height : int) {
            PQCConstants.imageAvailableSize.height = image_top.height
            updateWindowSize.restart()
        }
    }

    Connections {

        target: PQCNotify

        function onAddRotation(rot : int) {
            image_top.setRotation += rot
        }

        function onLoadNewFile(path : string) {
            image_top.loadImage(path)
        }

    }

    Connections {

        target: PQCConstants

        function onImageStatusChanged() {
            if(PQCConstants.imageStatus === Image.Ready) {
                if(imageloader.item != null) {
                    PQCNotify.updateWindowSize(PQCConstants.imagePaintedSize.width+10, PQCConstants.imagePaintedSize.height+10)
                }
            }
        }

    }

    Component.onCompleted: {
        PQCConstants.imageAvailableSize = Qt.size(image_top.width, image_top.height)
        PQCConstants.imageAvailableSizeDelay = Qt.size(image_top.width, image_top.height)
    }

    // react to clicks, double clicks, and movements
    MouseArea {
        id: imagemouse
        anchors.fill: parent
        enabled: !PQCConstants.menuIsOpen
        anchors.margins: -5
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton|Qt.RightButton
        cursorShape: PQCConstants.currentSource === "" ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: (mouse) => {
            if(mouse.button === Qt.RightButton)
                PQCNotify.showMainContextMenu()
            else if(PQCConstants.menuIsOpen)
                PQCNotify.closeAllMenus()
            else if(PQCConstants.currentSource === "")
                PQCNotify.requestNewFile()
        }
        onPositionChanged: (mouse) => {
            if(mouse.y < 30)
                PQCConstants.toprowForceVisible = true
            else
                PQCConstants.toprowForceVisible = false
        }

        onDoubleClicked: (mouse) => {
            if(mouse.button === Qt.RightButton) return
            if(PQCConstants.mainwindowIsFullscreen)
                PQCNotify.mainwindowShowNormal()
            else
                PQCNotify.mainwindowShowFullscreen()
        }
    }

    DropArea {
        anchors.fill: parent
        onDropped: (drop) => {
            var src = PQCScriptsFilesPaths.cleanPath(drop.text)
            if(PQCScriptsFilesPaths.isFileSupported(src))
                image_top.loadImage(src)
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

        PQCConstants.mainwindowManuallyResized = false
        imageloader.source = ""
        setRotation = 0
        PQCConstants.mainwindowOverrideTitle = ""
        PQCConstants.mainwindowOverrideTitleSuffix = ""

        if(path === "") {
            PQCConstants.currentSource = ""
            imageloader.source = ""
            PQCConstants.imageStatus = Image.Null
            return
        }

        PQCConstants.currentSource = PQCScriptsFilesPaths.cleanPath(path)
        PQCConstants.imageStatus = Image.Loading

        PQCSettings.filedialogLocation = PQCScriptsFilesPaths.getDir(PQCConstants.currentSource)

        if(PQCScriptsImages.isPDFDocument(PQCConstants.currentSource)) {
            PQCConstants.currentType = "doc"
            imageloader.source = "imageitems/PQDocument.qml"
        } else if(PQCScriptsImages.isEpub(PQCConstants.currentSource)) {
            PQCConstants.currentType = "bok"
            imageloader.source = "imageitems/PQEPUB.qml"
        } else if(PQCScriptsImages.isArchive(PQCConstants.currentSource)) {
            PQCConstants.currentType = "arc"
            imageloader.source = "imageitems/PQArchive.qml"
        } else if(PQCScriptsImages.isMpvVideo(PQCConstants.currentSource)) {
            PQCConstants.currentType = "mpv"
            imageloader.source = "imageitems/PQVideoMpv.qml"
        } else if(PQCScriptsImages.isQtVideo(PQCConstants.currentSource)) {
            PQCConstants.currentType = "vid"
            imageloader.source = "imageitems/PQVideoQt.qml"
        } else if(PQCScriptsImages.isItAnimated(PQCConstants.currentSource)) {
            PQCConstants.currentType = "ani"
            imageloader.source = "imageitems/PQImageAnimated.qml"
        } else if(PQCScriptsImages.isPhotoSphere(PQCConstants.currentSource)) {
            PQCConstants.currentType = "sph"
            imageloader.source = "imageitems/PQPhotoSphere.qml"
        } else if(PQCScriptsImages.isSVG(PQCConstants.currentSource)) {
            PQCConstants.currentType = "svg"
            imageloader.source = "imageitems/PQSVG.qml"
        } else if(PQCScriptsImages.isTextDocument(PQCConstants.currentSource)) {
            PQCConstants.currentType = "txt"
            imageloader.source = "imageitems/PQTxt.qml"
        } else {
            PQCConstants.currentType = "img"
            imageloader.source = "imageitems/PQImageNormal.qml"
        }

    }

}
