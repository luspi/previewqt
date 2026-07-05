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
            PQCConstants.imageAvailableSizeDelay = PQCConstants.imageAvailableSize
        }
    }

    onWidthChanged: {
        PQCConstants.imageAvailableSize.width = width
        updateWindowSize.restart()
    }
    onHeightChanged: {
        PQCConstants.imageAvailableSize.height = height
        updateWindowSize.restart()
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
                if(imageloader.item != null && PQCConstants.currentType !== "aud") {
                    PQCNotify.updateWindowSize(PQCConstants.imagePaintedSize.width+10, PQCConstants.imagePaintedSize.height+10)
                }
                loading.hide()
            } else if(PQCConstants.imageStatus === Image.Error)
                loading.hide()
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
        z: 999
        onDropped: (drop) => {
            var src = PQCScriptsFilesPaths.cleanPath(drop.text)
            if(PQCScriptsFilesPaths.isFileSupported(src))
                image_top.loadImage(src)
            else
                PQCNotify.trayiconShowNotification(qsTr("Content not supported"), qsTr("The dropped file/content is currently not supported by PreviewQt"))
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
    function loadImage(path : string) {

        console.log("args: path =", path)

        loading.startTimer()

        PQCConstants.mainwindowManuallyResized = false
        imageloader.active = false
        setRotation = 0
        PQCConstants.mainwindowOverrideTitle = ""
        PQCConstants.mainwindowOverrideTitleSuffix = ""

        if(path === "") {
            console.log("empty path received")
            PQCConstants.currentSource = ""
            imageloader.sourceComponent = comp_empty
            PQCConstants.imageStatus = Image.Null
            loading.hide()
            return
        }

        PQCConstants.currentSource = PQCScriptsFilesPaths.cleanPath(path)
        PQCConstants.imageStatus = Image.Loading

        PQCSettings.filedialogLocation = PQCScriptsFilesPaths.getDir(PQCConstants.currentSource)

        if(PQCScriptsImages.isURL(PQCConstants.currentSource)) {
            PQCConstants.currentType = "url"
            imageloader.sourceComponent = comp_url
        } else if(PQCScriptsImages.isPDFDocument(PQCConstants.currentSource)) {
            PQCConstants.currentType = "doc"
            imageloader.sourceComponent = comp_doc
        } else if(PQCScriptsImages.isOfficeDocument(PQCConstants.currentSource)) {
            PQCConstants.currentType = "off"
            imageloader.sourceComponent = comp_off
        } else if(PQCScriptsImages.isEpub(PQCConstants.currentSource)) {
            PQCConstants.currentType = "bok"
            imageloader.sourceComponent = comp_bok
        } else if(PQCScriptsImages.isArchive(PQCConstants.currentSource)) {
            PQCConstants.currentType = "arc"
            imageloader.sourceComponent = comp_arc
        } else if(PQCScriptsImages.isMpvVideo(PQCConstants.currentSource)) {
            PQCConstants.currentType = "mpv"
            imageloader.sourceComponent = comp_mpv
        } else if(PQCScriptsImages.isQtVideo(PQCConstants.currentSource)) {
            PQCConstants.currentType = "vid"
            imageloader.sourceComponent = comp_vid
        } else if(PQCScriptsImages.isItAnimated(PQCConstants.currentSource)) {
            PQCConstants.currentType = "ani"
            imageloader.sourceComponent = comp_ani
        } else if(PQCScriptsImages.isPhotoSphere(PQCConstants.currentSource)) {
            PQCConstants.currentType = "sph"
            imageloader.sourceComponent = comp_sph
        } else if(PQCScriptsImages.isSVG(PQCConstants.currentSource)) {
            PQCConstants.currentType = "svg"
            imageloader.sourceComponent = comp_svg
        } else if(PQCScriptsImages.isTextDocument(PQCConstants.currentSource)) {
            PQCConstants.currentType = "txt"
            imageloader.sourceComponent = comp_txt
        } else if(PQCScriptsImages.isAudio(PQCConstants.currentSource)) {
            PQCConstants.currentType = "aud"
            imageloader.sourceComponent = comp_aud
        } else if(PQCScriptsImages.isPackage(PQCConstants.currentSource)) {
            PQCConstants.currentType = "pak"
            imageloader.sourceComponent = comp_pak
        } else {
            PQCConstants.currentType = "img"
            imageloader.sourceComponent = comp_img
        }
        console.log("detected type:", PQCConstants.currentType)

        // we wait to activate a sphere UNTIL the texture limit has been detected
        // if this functionality is not enabled, then the texture limit is set to 0 and this check is skipped
        if(PQCConstants.currentType === "sph" && PQCScriptsImages.getMaxTextureLimit() < 0)
            sphereWaitForTextureLimit.restart()
        else
            imageloader.active = true

    }

    Timer {
        id: sphereWaitForTextureLimit
        interval: 10
        onTriggered: {
            if(PQCScriptsImages.getMaxTextureLimit() < 0) {
                sphereWaitForTextureLimit.restart()
                return
            }
            imageloader.active = true
        }
    }

    Component {
        id: comp_empty
        Item{}
    }

    Component {
        id: comp_doc
        PQDocument {}
    }

    Component {
        id: comp_off
        PQOfficeDocument {}
    }

    Component {
        id: comp_bok
        PQEPUB {}
    }

    Component {
        id: comp_arc
        PQArchive {}
    }

    Component {
        id: comp_mpv
        PQVideoMpv {}
    }

    Component {
        id: comp_vid
        PQVideoQt {}
    }

    Component {
        id: comp_ani
        PQImageAnimated {}
    }

    Component {
        id: comp_sph
        PQPhotoSphere {}
    }

    Component {
        id: comp_svg
        PQSVG {}
    }

    Component {
        id: comp_txt
        PQTxt {}
    }

    Component {
        id: comp_img
        PQImageNormal {}
    }

    Component {
        id: comp_url
        PQURL {}
    }

    Component {
        id: comp_aud
        PQAudio {}
    }

    Component {
        id: comp_pak
        PQPackage {}
    }

    // LOADING indicator
    Rectangle {

        id: loading

        anchors.fill: parent
        color: "#88000000"
        visible: false

        Item {

            anchors.fill: parent

            Repeater {

                model: 3

                delegate: Canvas {
                    id: load
                    required property int modelData
                    x: (parent.width-width)/2
                    y: (parent.height-height)/2
                    width: (106 - modelData*20)
                    height: (106 - modelData*20)
                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.strokeStyle = "#ffffff";
                        ctx.lineWidth = 3
                        ctx.beginPath();
                        ctx.arc(width/2, height/2, width/2-3, 0, 3.14, false);
                        ctx.stroke();
                    }
                    RotationAnimator {
                        target: load
                        from: load.modelData%2 ? 360 : 0
                        to: load.modelData%2 ? 0 : 360
                        duration: 2000 - load.modelData*222
                        running: loading.visible
                        loops: Animation.Infinite
                    }
                }

            }

        }

        function startTimer() {
            showTimer.restart()
        }
        function hide() {
            showTimer.stop()
            visible = false
        }
        Timer {
            id: showTimer
            interval: 0
            onTriggered:
                loading.visible = true
        }
    }

}
