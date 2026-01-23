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
import QtMultimedia
import PreviewQt

Item {

    x: (PQCConstants.imageAvailableSize.width-width)/2
    y: (PQCConstants.imageAvailableSize.height-height)/2

    width: imageitem.width
    height: imageitem.height

    property Item imageParent

    Component.onCompleted: {
        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(imageitem.paintedWidth, imageitem.paintedHeight) })
        PQCConstants.imageAsynchronous = Qt.binding(function() { return imageitem.asynchronous })
    }

    Image {

        id: imageitem

        source: PQCConstants.currentSource!=="" ? ("image://full/" + PQCScriptsFilesPaths.toPercentEncoding(PQCConstants.currentSource)) : ""

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: false
        mipmap: false

        rotation: PQCConstants.imageRotation

        width: rotation%180===0 ? PQCConstants.imageAvailableSize.width : PQCConstants.imageAvailableSize.height
        height: rotation%180===0 ? PQCConstants.imageAvailableSize.height : PQCConstants.imageAvailableSize.width
        sourceSize: rotation%180===0 ? Qt.size(PQCConstants.imageAvailableSizeDelay.width,
                                               PQCConstants.imageAvailableSizeDelay.height) : Qt.size(PQCConstants.imageAvailableSizeDelay.height,
                                                                                                 PQCConstants.imageAvailableSizeDelay.width)

        onStatusChanged: {
            PQCConstants.imageStatus = status
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
        }

    }

    Connections {

        target: PQCNotify

        function onSetImageAsync(async : bool) {
            imageitem.asynchronous = async
        }

    }

}
