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
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia
import PreviewQt

Rectangle {

    id: dllexe_top

    width: PQCConstants.imageAvailableSize.width
    height: PQCConstants.imageAvailableSize.height

    color: palette.base

    property var currentData: ({})

    Flickable {

        id: content

        y: 0
        width: parent.width
        height: Math.min(parent.height, contentHeight)

        contentHeight: contrect.height

        ScrollBar.vertical: ScrollBar { id: vscroll }

        Item {

            id: contrect

            width: dllexe_top.width-(vscroll.visible ? vscroll.width : 0)
            height: col.height

            Column {

                id: col

                x: (contrect.width-10-width)/2+5
                width: Math.min(1000, contrect.width-10)
                y: (parent.height-10-height)/2+5

                spacing: 5

                Item {
                    width: 1
                    height: 1
                }

                Text {
                    x: (parent.width-width)/2
                    width: parent.width
                    horizontalAlignment: Qt.AlignHCenter
                    font.pointSize: 18
                    font.bold: true
                    text: "Windows DLL"
                    color: palette.text
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: palette.text
                }

                Text {
                    id: error
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    color: "red"
                    font.pointSize: 15
                    font.bold: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: "ERROR"
                    visible: false
                }


                Item {
                    width: 1
                    height: 1
                }

            }

        }

    }

    Component.onCompleted: {

        const dat = PQCFileHandler.getDataWithPlugin("dllexe", PQCConstants.currentSource)
        if(dat.length !== 2 || !dat[0]) {
            PQCConstants.imageStatus = Image.Error
            return;
        }
        dllexe_top.currentData = dat[1]
        for(let i in dllexe_top.currentData) {
            if(i === "VersionInfo") {
                console.warn(">>> VersionInfo")
                for(let j in dllexe_top.currentData[i])
                    console.warn("      **", j, dllexe_top.currentData[i][j])

            } else
                console.warn(">>>", i, dllexe_top.currentData[i])
        }

        PQCNotify.updateWindowSizeDefault()

        PQCConstants.imageStatus = Image.Ready

    }

}
