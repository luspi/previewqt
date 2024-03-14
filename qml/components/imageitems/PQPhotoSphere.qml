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
import PQCPhotoSphere
import PQCScripts

Item {

    id: sphere_top

    parent: image_top
    anchors.fill: parent
    anchors.margins: -5

    PQCPhotoSphere {

        id: thesphere
        anchors.fill: parent

        source: image_top.imageSource!=="" ? PQCScripts.toPercentEncoding(image_top.imageSource) : ""
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

            onPinchStarted:
                storeFieldOfView = thesphere.fieldOfView

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
            }
        }

    }

}
