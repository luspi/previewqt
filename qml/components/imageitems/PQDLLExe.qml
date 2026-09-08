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

                TextEdit {
                    id: textedit
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pointSize: 11
                    readOnly: true
                    textFormat: Text.RichText
                    color: palette.text
                    width: col.width
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

        const mainorder = [
            "FileName",
            "FileSize",
            "TimeCreated",
            "TimeModified",
            "SHA256"
        ] // VersionInfo is automatically added at the end

        const versionorder = [
            "Comments",
            "CompanyName",
            "FileDescription",
            "LegalCopyright",
            "---",
            "ProductName",
            "OriginalFilename",
            "Architecture",
            "Timestamp",
            "Sections",
            "---",
            "FileVersion",
            "ProductVersion",
            "FileFlagsMask",
            "FileFlags",
            "FileOS",
            "FileType",
            "FileSubtype",
            "---",
            "StructureVersion",
            "VersionLanguage",
            "VersionCodePage",
            "FileDate"
        ]

        const LOCALIZED = {
            "VersionInfo": qsTr("Version information") + " (VERSIONINFO)",

            "FileName": qsTr("Filename"),
            "FileSize": qsTr("Filesize"),
            "TimeCreated": qsTr("Time created"),
            "TimeModified": qsTr("Time modified"),
            "SHA256": "SHA256",

            "Comments": qsTr("Comments"),
            "CompanyName": qsTr("Company name"),
            "FileDescription": qsTr("Description"),
            "LegalCopyright": qsTr("Copyright"),

            "ProductName": qsTr("Product name"),
            "OriginalFilename": qsTr("Original filename"),
            "Architecture": qsTr("Architecture"),
            "Timestamp": qsTr("Timestamp"),
            "Sections": qsTr("Sections"),

            "FileVersion": qsTr("File version"),
            "ProductVersion": qsTr("Product version"),
            "FileFlagsMask": qsTr("File flags (mask)"),
            "FileFlags": qsTr("File flags"),
            "FileOS": qsTr("File OS"),
            "FileType": qsTr("Filetype"),
            "FileSubtype": qsTr("Filesubtype"),

            "StructureVersion": qsTr("Structure version"),
            "VersionLanguage": qsTr("Language"),
            "VersionCodePage": qsTr("Code page"),
            "FileDate": qsTr("File date")
        }

        var main = "<br>"
        var vinfo = "<h3>" + LOCALIZED["VersionInfo"] + ":</h3><ul>"

        for(let i in mainorder) {
            const m = mainorder[i]
            if(m in dllexe_top.currentData) {
                if(main != "<br>") main += "<br>"
                main += "<b>" + LOCALIZED[m] + "</b>: " + dllexe_top.currentData[m];
            }
        }

        for(let j in versionorder) {
            const v = versionorder[j]
            if(v in dllexe_top.currentData["VersionInfo"]) {
                vinfo += "<li><b>" + LOCALIZED[v] + "</b>: " + dllexe_top.currentData["VersionInfo"][v] + "</li>"
            }
        }

        textedit.text = main + vinfo + "</ul><br><br><br><br><br>"

        PQCNotify.updateWindowSizeDefault()

        PQCConstants.imageStatus = Image.Ready

    }

}
