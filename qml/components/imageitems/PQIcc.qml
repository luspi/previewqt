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

    id: icc_top

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

            width: icc_top.width-(vscroll.visible ? vscroll.width : 0)
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
                    text: "ICC color profile"
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

                Row {

                    TextEdit {
                        id: textedit
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        font.pointSize: 13
                        readOnly: true
                        textFormat: Text.RichText
                        color: palette.text
                        width: col.width
                    }

                }

                Item {
                    width: 1
                    height: 1
                }

            }

        }

    }

    property list<string> dataInOrder: [
        "description",
        "signature",
        "class",
        "version",
        "renderingIntent",
        "profileSize",
        "whitepoint",
        "copyright"
    ]
    property var dataTrans: {
        "description" : qsTr("Name"),
        "signature" : qsTr("Color space"),
        "class" : qsTr("Profile class"),
        "version" : qsTr("Version"),
        "renderingIntent" : qsTr("Rendering Intent"),
        "profileSize" : qsTr("Profile Size"),
        "whitepoint" : qsTr("White Point"),
        "copyright" : qsTr("Copyright")
    }

    Component.onCompleted: {

        const dat = PQCFileHandler.getDataWithPlugin("icc", PQCConstants.currentSource)
        if(dat.length !== 2 || !dat[0]) {
            PQCConstants.imageStatus = Image.Error
            return;
        }
        icc_top.currentData = dat[1]

        var txt = "<table>"

        for(let i in dataInOrder) {
            const cur = dataInOrder[i]
            if(cur in icc_top.currentData) {
                txt += "<tr><td align=right width=30%><b>" + dataTrans[cur] + "</b>:</td><td width=70%> "
                if(cur === "profileSize")
                    txt += icc_top.currentData[cur] + " Bytes"
                else if(cur === "whitepoint") {
                    const val = icc_top.currentData[cur]
                    txt += "X = " + val[0].toFixed(4) + " / Y = " + val[1].toFixed(4) + " / Z = " + val[2].toFixed(4)
                } else
                    txt += icc_top.currentData[cur]
                txt += "</td></tr>"
            }
        }
        if("gammut" in icc_top.currentData) {
            txt += "<tr><td></td><td><img align=center src='data:image/png;base64," + icc_top.currentData["gammut"] + "'></td></tr>"
            PQCConstants.mainwindowExtraH = 100
        }
        txt += "</table>"

        textedit.text = txt

        PQCNotify.updateWindowSizeDefault()

        PQCConstants.imageStatus = Image.Ready

    }

}
