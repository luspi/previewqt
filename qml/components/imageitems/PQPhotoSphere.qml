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

    id: sphere_top

    parent: image_top
    anchors.fill: parent
    anchors.margins: -5

    // dummy item
    property bool asynchronous

    // these ensure that we resize the window to its initial default size
    property size sourceSize: Qt.size(paintedWidth, paintedHeight)
    property int paintedWidth: PQCSettings.defaultWindowWidth - 10
    property int paintedHeight: PQCSettings.defaultWindowHeight - (PQCSettings.topBarAutoHide ? 1 : toprow.height) - 10

    PQCPhotoSphere {

        id: thesphere
        anchors.fill: parent

        source: PQCConstants.currentSource!=="" ? PQCScriptsFilesPaths.toPercentEncoding(PQCConstants.currentSource) : ""
        onSourceChanged: {
            azimuth = 180
            elevation = 0
            fieldOfView = 90
        }

        // these need to have a small duration as otherwise touchpad handling is awkward
        // key events are handled with their own animations below
        Behavior on fieldOfView { NumberAnimation { id: behavior_fov; duration: 50 } }
        Behavior on azimuth { NumberAnimation { id: behavior_az; duration: 50 } }
        Behavior on elevation { NumberAnimation { id: behavior_ele; duration: 50 } }

        PinchArea {

            id: pincharea

            anchors.fill: parent

            property real storeFieldOfView

            onPinchStarted: {
                leftrightani.stop()
                storeFieldOfView = thesphere.fieldOfView
            }

            onPinchUpdated: (pinch) => {
                // compute the rate of change initiated by this pinch
                var startLength = Math.sqrt(Math.pow(pinch.startPoint1.x-pinch.startPoint2.x, 2) + Math.pow(pinch.startPoint1.y-pinch.startPoint2.y, 2))
                var curLength = Math.sqrt(Math.pow(pinch.point1.x-pinch.point2.x, 2) + Math.pow(pinch.point1.y-pinch.point2.y, 2))
                thesphere.fieldOfView = storeFieldOfView * (startLength / curLength)
            }

            MouseArea {

                id: mousearea

                anchors.fill: parent

                property var clickedPos
                property var clickedAzimuth
                property var clickedElevation

                onPressed: (mouse) => {
                    leftrightani.stop()
                    behavior_fov.duration = 0
                    behavior_az.duration = 0
                    behavior_ele.duration = 0
                    clickedPos = Qt.point(mouse.x, mouse.y)
                    clickedAzimuth = thesphere.azimuth
                    clickedElevation = thesphere.elevation
                }
                onPositionChanged: (mouse) => {
                    var posDiff = Qt.point(mouse.x-mousearea.clickedPos.x , mouse.y-mousearea.clickedPos.y)
                    var curTan = Math.tan(thesphere.fieldOfView * ((0.5*Math.PI)/180));
                    thesphere.azimuth = clickedAzimuth - (((3*256)/image.height) * posDiff.x/6) * curTan
                    thesphere.elevation = clickedElevation + (((3*256)/image.height) * posDiff.y/6) * curTan
                }
                onReleased: {
                    behavior_fov.duration = 50
                    behavior_az.duration = 50
                    behavior_ele.duration = 50
                }

                onWheel: (wheel) => {
                    if(wheel.modifiers & Qt.ControlModifier) {
                        thesphere.azimuth +=  wheel.angleDelta.x*0.1
                        thesphere.elevation -=  wheel.angleDelta.y*0.05
                    } else
                        thesphere.fieldOfView -=  wheel.angleDelta.y*0.05
                }

                onDoubleClicked: {
                    if(PQCConstants.mainwindowIsFullscreen)
                        toplevel.showNormal()
                    else
                        toplevel.showFullScreen()
                }
            }
        }

    }

    // these are not handled with the behavior above because key events are handled smoother than mouse events
    NumberAnimation {
        id: animatedAzimuth
        target: thesphere
        property: "azimuth"
        duration: 200
    }
    NumberAnimation {
        id: animatedElevation
        target: thesphere
        property: "elevation"
        duration: 200
    }
    NumberAnimation {
        id: animatedFieldOfView
        target: thesphere
        property: "fieldOfView"
        duration: 200
    }

    // we set the status after a short timeout
    // if we set it immediately, then the loader item might get reported as being null still
    Timer {
        interval: 50
        running: true
        onTriggered: {
            image.status = Image.Ready
            startPanAfterWindowResize.restart()
        }
    }

    // we perform a pan after a short delay to allow for enough time to adjust the size of the window
    Timer {
        id: startPanAfterWindowResize
        interval: 200
        onTriggered:
            leftrightani.restart()
    }

    // This is a short animation to the right and back
    // This is used when a photo sphere has been entered to inform the user that there is more to the image than what they can see
    SequentialAnimation {

        id: leftrightani

        loops: 1

        NumberAnimation {
            target: thesphere
            property: "azimuth"
            from: 180
            to: 190
            duration: 500
            easing.type: Easing.OutCirc
        }

        NumberAnimation {
            target: thesphere
            property: "azimuth"
            from: 190
            to: 180
            duration: 500
            easing.type: Easing.OutBack
        }

    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(keycode === Qt.Key_Left)
                moveView("left")

            else if(keycode === Qt.Key_Right)
                moveView("right")

            else if(keycode === Qt.Key_Up)
                moveView("up")

            else if(keycode === Qt.Key_Down)
                moveView("down")

            else if(keycode === Qt.Key_Plus)
                zoom("in")

            else if(keycode === Qt.Key_Minus)
                zoom("out")

            else if(keycode === Qt.Key_0) {

                moveView("reset")
                zoom("reset")

            }

        }

    }

    // these are not handled with the behavior above because key events are handled smoother than mouse events
    function zoom(dir) {

        leftrightani.stop()

        animatedFieldOfView.stop()

        if(dir === "in") {
            animatedFieldOfView.from = thesphere.fieldOfView
            animatedFieldOfView.to = thesphere.fieldOfView-10
        } else if(dir === "out") {
            animatedFieldOfView.from = thesphere.fieldOfView
            animatedFieldOfView.to = thesphere.fieldOfView+10
        } else if(dir === "reset") {
            animatedFieldOfView.from = thesphere.fieldOfView
            animatedFieldOfView.to = 90
        }

        animatedFieldOfView.restart()

    }

    // these are not handled with the behavior above because key events are handled smoother than mouse events
    function moveView(dir) {

        leftrightani.stop()

        if(dir === "up" || dir === "down" || dir === "reset")
            animatedElevation.stop()
        if(dir === "left" || dir === "right" || dir === "reset")
            animatedAzimuth.stop()

        if(dir === "up") {
            animatedElevation.from = thesphere.elevation
            animatedElevation.to = thesphere.elevation + thesphere.fieldOfView/5
        } else if(dir === "down") {
            animatedElevation.from = thesphere.elevation
            animatedElevation.to = thesphere.elevation - thesphere.fieldOfView/5
        } else if(dir === "left") {
            animatedAzimuth.from = thesphere.azimuth
            animatedAzimuth.to = thesphere.azimuth - thesphere.fieldOfView/3
        } else if(dir === "right") {
            animatedAzimuth.from = thesphere.azimuth
            animatedAzimuth.to = thesphere.azimuth + thesphere.fieldOfView/3
        } else if(dir === "reset") {
            animatedElevation.from = thesphere.elevation
            animatedElevation.to = 0
            animatedAzimuth.from = thesphere.azimuth
            animatedAzimuth.to = 180
        }

        if(dir === "up" || dir === "down" || dir === "reset")
            animatedElevation.restart()
        if(dir === "left" || dir === "right" || dir === "reset")
            animatedAzimuth.restart()

    }


}
