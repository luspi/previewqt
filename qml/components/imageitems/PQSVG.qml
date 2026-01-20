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

Item {

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    width: imageitem.width
    height: imageitem.height

    property alias sourceSize: imageitem.sourceSize
    property alias asynchronous: imageitem.asynchronous
    property alias paintedWidth: imageitem.paintedWidth
    property alias paintedHeight: imageitem.paintedHeight

    Image {

        id: imageitem

        source: image_top.imageSource!=="" ? ("image://full/" + PQCScriptsFilesPaths.toPercentEncoding(image_top.imageSource)) : ""

        asynchronous: true

        fillMode: Image.PreserveAspectFit

        smooth: false
        mipmap: false

        rotation: image_top.setRotation

        width: rotation%180===0 ? image_top.width : image_top.height
        height: rotation%180===0 ? image_top.height : image_top.width
        sourceSize: rotation%180===0 ? Qt.size(image_top.windowWidth, image_top.windowHeight) : Qt.size(image_top.windowHeight, image_top.windowWidth)

        onStatusChanged: {
            image.status = status
            if(status == Image.Error)
                source = "image://svg/:/errorimage.svg"
        }

    }

}
