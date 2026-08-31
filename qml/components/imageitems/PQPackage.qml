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

    id: package_top

    width: PQCConstants.imageAvailableSize.width
    height: PQCConstants.imageAvailableSize.height

    color: palette.base

    property bool isDEB: false
    property bool isRPM: false
    property bool isAPPIMAGE: false

    property var currentData: ({})

    property list<string> keysToDisplay: isDEB ? keysToDisplay_DEB : (isRPM||isAPPIMAGE ? keysToDisplay_METAINFO : [])

    property list<string> keysToDisplay_DEB: [
        "name",
        "description",
        "version",
        "section",
        "architecture",
        "maintainer",
        "homepage",
        "depends"
    ]

    property list<string> keysToDisplay_METAINFO: [
        "name",
        "id",
        "summary",
        "version",
        "releaseDate",
        "author",
        "homepage",
        "keywords",
        "description"
    ]

    property var keysToString: {
        "name" : "Name",
        "id" : "ID",
        "summary" : "Summary",
        "description" : "Description",
        "version" : "Version",
        "section" : "Section",
        "architecture" : "Architecture",
        "maintainer" : "Maintainer",
        "homepage" : "Homepage",
        "depends" : "Dependencies",
        "author" : "Author",
        "releaseDate" : "Release date",
        "keywords" : "Keywords"
    }

    Flickable {

        id: content

        y: 0
        width: parent.width
        height: Math.min(parent.height, contentHeight)

        contentHeight: contrect.height

        ScrollBar.vertical: ScrollBar { id: vscroll }

        Item {

            id: contrect

            width: package_top.width-(vscroll.visible ? vscroll.width : 0)
            height: col.height

            Column {

                id: col

                x: (contrect.width-10-width)/2+5
                width: Math.min(800, contrect.width-10)
                y: (parent.height-10-height)/2+5

                spacing: 10

                Item {
                    width: 1
                    height: 1
                }

                Text {
                    x: (parent.width-width)/2
                    width: parent.width
                    horizontalAlignment: Qt.AlignHCenter
                    font.pointSize: 16
                    font.bold: true
                    text: package_top.isDEB
                            ? "Debian installer package (*.deb)"
                            : isRPM
                                ? "RPM package (*.rpm)"
                                : isAPPIMAGE
                                    ? "AppImage"
                                    : "unknown"
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
                    font.pointSize: 14
                    font.bold: true
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: "ERROR: abc"
                }

                property int maxTitleWidth: 150

                Repeater {

                    model: package_top.keysToDisplay.length

                    Row {

                        id: deleg

                        required property int index
                        property string key: package_top.keysToDisplay[index]
                        property string val: package_top.currentData[key]

                        spacing: 5

                        Text {
                            id: title
                            font.pointSize: deleg.key==="name" ? 13 : 11
                            font.bold: true
                            text: package_top.keysToString[deleg.key] + ":"
                            color: palette.text
                            horizontalAlignment: Text.AlignRight
                            onWidthChanged: {
                                if(width > 0 && width > col.maxTitleWidth)
                                    col.maxTitleWidth = width
                                width = Qt.binding(function() { return col.maxTitleWidth })
                            }
                        }

                        TextEdit {
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pointSize: deleg.key==="name" ? 13 : 11
                            readOnly: true
                            textFormat: Text.RichText
                            font.bold: deleg.key==="name"
                            text: deleg.val
                            color: palette.text
                            width: col.width - title.width-deleg.spacing
                        }

                    }

                }

                Item {
                    width: 1
                    height: 1
                }

            }

        }

    }

    Component.onCompleted: {
        const dat = PQCFileHandler.getDataWithPlugin("package", PQCConstants.currentSource)
        if(dat.length === 0) {
            const msg = "ERROR: No data received"
            console.warn(msg)
            error.text = msg
            error.visible = true
            PQCConstants.imageStatus = Image.Ready
            return;
        }

        if(!dat[0]) {
            const msg = "ERROR: " + dat[1]
            console.warn(msg)
            error.text = msg
            error.visible = true
            PQCConstants.imageStatus = Image.Ready
            return
        }

        error.visible = false
        package_top.currentData = dat[1]

        const suf = PQCScriptsFilesPaths.getSuffix(PQCConstants.currentSource).toLowerCase()
        const mim = PQCScriptsFilesPaths.getMimetype(PQCConstants.currentSource)
        if(suf === "deb" || mim === "application/vnd.debian.binary-package") {
            isDEB = true
        } else if(suf === "rpm") {
            isRPM  = true
        } else if(suf === "appimage") {
            isAPPIMAGE  = true
        } else {
            const msg = "ERROR: Unsupported file? Filename: " + PQCConstants.currentSource
            console.warn(msg)
            error.text = msg
            error.visible = true
            PQCConstants.imageStatus = Image.Ready
            return
        }

        var wToSet = PQCConstants.imagePaintedSize.width
        var hToSet = PQCConstants.imagePaintedSize.height
        var toUpd = false
        if(wToSet < 1200) {
            wToSet = 1200
            toUpd = true
        }
        if(hToSet < 800) {
            hToSet = 800
            toUpd = true
        }
        if(toUpd) PQCNotify.updateWindowSize(wToSet, hToSet)

        PQCConstants.imageStatus = Image.Ready

    }

}
