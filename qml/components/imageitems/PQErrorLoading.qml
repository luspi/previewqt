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
import QtQuick.Controls
import PreviewQt

Rectangle {

    color: colorPalette.window

    id: txt_top

    SystemPalette { id: colorPalette; colorGroup: SystemPalette.Active }

    x: 0
    y: (PQCConstants.imageAvailableSize.height-height)/2

    width: flickme.width
    height: flickme.height

    property int vBarWidth: 0
    property real vBarOpacity: 1

    Rectangle {
        x: parent.width-width
        width: txt_top.vBarWidth
        height: parent.height
        color: colorPalette.shadow
        opacity: 0.1*txt_top.vBarOpacity
    }

    Flickable {

        id: flickme

        width: PQCConstants.imageAvailableSize.width
        height: PQCConstants.imageAvailableSize.height

        clip: true
        contentItem.clip: true

        contentHeight: cont.height
        contentWidth: flickme.width

        ScrollBar.vertical: ScrollBar {
            id: vBar
            onWidthChanged: txt_top.vBarWidth = width
            onOpacityChanged: txt_top.vBarOpacity = opacity
            opacity: (vBar.active ? 1 : 0.1)
            Behavior on opacity { NumberAnimation { duration: 200 } }
        }

        Item {

            id: cont

            width: imageitem.width
            height: imageitem.height

            TextEdit {

                id: imageitem
                x: 5
                y: 5

                width: flickme.width-10-(vBar.visible ? vBar.width : 0)
                height: Math.max(flickme.height, contentHeight)

                color: colorPalette.text

                font.pointSize: 14
                textFormat: TextEdit.RichText
                horizontalAlignment: Text.AlignHCenter

                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                readOnly: true

                text: (function() {
                    var parts = PQCScriptsImages.getFileLoadError().split("\n")
                    var uniq = [...new Set(parts)];
                    return "<br>\n<h2>" + qsTr("Error loading preview") + "</h2>\n<br>\n" + uniq.join("<br><br>")
                })()

                Component.onCompleted: {
                    PQCConstants.imageStatus = Image.Ready
                    PQCNotify.updateWindowSize(PQCSettings.defaultWindowWidth, PQCSettings.defaultWindowHeight)
                    imageitem.forceActiveFocus()
                }

                Component.onDestruction: {
                    PQCNotify.resetFocus()
                }

            }

        }

    }

}
