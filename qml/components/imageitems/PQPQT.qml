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

    id: pqt_top

    width: PQCConstants.imageAvailableSize.width
    height: PQCConstants.imageAvailableSize.height

    color: palette.base

    property var currentData: ({})

    property bool isPQE: false
    property bool isPQT: false

    property list<string> keysToDisplay: (isPQE ? keysToDisplay_PQE : (isPQT ? keysToDisplay_PQT : []))

    property list<string> keysToDisplay_PQT: [
        "version",
        "filelist"
    ]

    property list<string> keysToDisplay_PQE: [
        "name",
        "version",
        "description",
        "author",
        "contact",
        "website",
        "targetAPI"
    ]

    property var keysToString: {
        "version" : "Version",
        "name" : "Name",
        "description" : "Description",
        "author" : "Author",
        "contact" : "Contact",
        "website" : "Website",
        "targetAPI" : "Target API",
        "filelist" : "Backed up files"
    }

    property var backupToDB: {
        "CFG_CONTEXTMENU_DB" : "contextmenu.db",
        "CFG_IMAGEPLUGINS" : "imageplugins.db",
        "CFG_SHORTCUTS_DB" : "shortcuts.db",
        "CFG_USERSETTINGS_DB" : "usersettings.db",
        "CFG_SETTINGS_DB" : "settings.db"
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

            width: pqt_top.width-(vscroll.visible ? vscroll.width : 0)
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
                    font.pointSize: 18
                    font.bold: true
                    text: pqt_top.isPQE ? "PhotoQt Extension" : (pqt_top.isPQT ? "PhotoQt config backup" : "unknown?")
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

                Text {
                    width: parent.width
                    font.pointSize: 13
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: qsTr("This is a backup of the PhotoQt Image Viewer. You can see the version of PhotoQt that generated this backup file below, including a list of all files that are included in this backup. This file can be imported by PhotoQt.")
                    visible: pqt_top.isPQT
                }

                Text {
                    width: parent.width
                    font.pointSize: 13
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: qsTr("This is an extension for the PhotoQt Image Viewer. It provides additional functionality and features. You can install this file via PhotoQt's settings manager.")
                    visible: pqt_top.isPQE
                }


                property int maxTitleWidth: 250

                Repeater {

                    model: pqt_top.keysToDisplay.length

                    Row {

                        id: deleg

                        required property int index
                        property string key: pqt_top.keysToDisplay[index]
                        property string val: (key!=="filelist"||pqt_top.isPQE) ? pqt_top.currentData[key] : convertPQTFileList(pqt_top.currentData[key])

                        spacing: 5

                        Text {
                            id: title
                            font.pointSize: deleg.key==="name" ? 15 : 13
                            font.bold: true
                            text: pqt_top.keysToString[deleg.key] + ":"
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
                            font.pointSize: deleg.key==="name" ? 15 : 13
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

        const dat = PQCFileHandler.getDataWithPlugin("pqt", PQCConstants.currentSource)

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
        pqt_top.currentData = dat[1]

        const suf = PQCScriptsFilesPaths.getSuffix(PQCConstants.currentSource).toLowerCase()
        if(suf === "pqe")
            pqt_top.isPQE = true
        else if(suf === "pqt")
            pqt_top.isPQT = true

        var wToSet = PQCConstants.imagePaintedSize.width
        var hToSet = PQCConstants.imagePaintedSize.height
        var toUpd = false
        if(wToSet < 1000) {
            wToSet = 1000
            toUpd = true
        }
        if(hToSet < 700) {
            hToSet = 700
            toUpd = true
        }
        if(toUpd) PQCNotify.updateWindowSize(wToSet, hToSet)

        PQCConstants.imageStatus = Image.Ready

    }

    function convertPQTFileList(lst) {
        var ret = "";
        for(let i = 0; i < lst.length; ++i) {
            const cur = lst[i];
            if(cur in backupToDB)
                ret += backupToDB[cur] + "<br>\n"
            else
                ret += cur + "<br>\n"
        }
        return ret;
    }

}
