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

    id: sqlite_top

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

            width: sqlite_top.width-(vscroll.visible ? vscroll.width : 0)
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
                    text: "SQLite database"
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

                /**********************************************************/
                /**********************************************************/
                // SQLITE DATABASE
                //
                // - Database: sqliteVersion, encoding, databaseSize
                // - Journal Mode: xxxx
                // - Pages: x, page size xxxx
                // - x tables: xxxx (x entries
                // [- Triggers: xxxx -> xxxx]
                //
                // ------
                //
                // TABLE1
                // Entries: x
                // Columns xx XXXXXX [XX]
                // [Foreign keys: xxxxx -> xxx.xxx]
                //

                Column {

                    id: dbCol

                    spacing: 4
                    width: parent.width

                    Row {

                        spacing: 5

                        Text {
                            text: "Database:"
                            font.pointSize: 12
                            color: palette.text
                            font.bold: true
                        }

                        TextEdit {
                            text: "SQLite " + sqlite_top.currentData["sqliteVersion"] + ", " +
                                  sqlite_top.currentData["encoding"] + ", " +
                                  formatBytes(sqlite_top.currentData["databaseSize"])
                            font.pointSize: 12
                            color: palette.text
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            readOnly: true
                            textFormat: Text.RichText
                        }

                    }

                    Row {

                        spacing: 5

                        Text {
                            text: "Journal mode:"
                            font.pointSize: 12
                            color: palette.text
                            font.bold: true
                        }

                        TextEdit {
                            text: sqlite_top.currentData["journalMode"]
                            font.pointSize: 12
                            color: palette.text
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            readOnly: true
                            textFormat: Text.RichText
                        }

                    }

                    Row {

                        spacing: 5

                        Text {
                            text: "Pages:"
                            font.pointSize: 12
                            color: palette.text
                            font.bold: true
                        }

                        TextEdit {
                            text: sqlite_top.currentData["pageCount"] + ", page size " + sqlite_top.currentData["pageSize"]
                            font.pointSize: 12
                            color: palette.text
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            readOnly: true
                            textFormat: Text.RichText
                        }

                    }

                    Row {
                        spacing: 5
                        Text {
                            text: sqlite_top.currentData["tableNames"].length + " tables:"
                            font.pointSize: 12
                            color: palette.text
                            font.bold: true
                        }
                        TextEdit {
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            readOnly: true
                            textFormat: Text.RichText
                            text: (function() {
                                var txt = []
                                for(let i in sqlite_top.currentData["tableNames"]) {
                                    txt.push(sqlite_top.currentData["tableNames"][i] + " <i>(" + sqlite_top.currentData["tables"][i]["rowCount"] + " entries)</i>")
                                }
                                return txt.join("<br>")
                            })()
                            font.pointSize: 12
                            color: palette.text
                        }
                    }

                    Row {
                        spacing: 5
                        visible: sqlite_top.currentData["triggerCount"]
                        Text {
                            text: "Triggers:"
                            font.pointSize: 12
                            color: palette.text
                            font.bold: true
                        }
                        TextEdit {
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            readOnly: true
                            textFormat: Text.RichText
                            text: (function() {
                                var txt = []
                                for(let i in sqlite_top.currentData["triggers"]) {
                                    const cur = sqlite_top.currentData["triggers"][i]
                                    txt.push(cur["name"] + " → " + cur["table"])
                                }
                                return txt.join("<br>")
                            })()
                            font.pointSize: 12
                            color: palette.text
                        }
                    }

                }

                /***************************************/
                /***************************************/

                Item {
                    width: 1
                    height: 1
                }

                /***************************************/
                /***************************************/
                // TABLES

                Rectangle {
                    x: (parent.width-width)/2
                    width: parent.width
                    height: 1
                    color: palette.text
                }

                /***************************************/
                /***************************************/

                Repeater {

                    id: tableRptr

                    property list<string> keys: [
                        "name",
                        "rowCount",
                        "columnCount",
                        "indexCount",
                        "foreignKeysCount"
                    ]

                    model: sqlite_top.currentData["tables"].length

                    delegate:
                    Column {

                        id: delegTables

                        width: col.width

                        required property int index
                        property var tableData: sqlite_top.currentData["tables"][index]

                        spacing: 10

                        Column {

                            width: parent.width

                            Row {
                                Text {
                                    id: tableNameText
                                    font.bold: true
                                    font.pixelSize: 25
                                    text: delegTables.tableData["name"]
                                }
                                Text {
                                    visible: delegTables.tableData["withoutRowId"]
                                    text: "WITHOUT ROWID"
                                    color: "orange"
                                }
                                Text {
                                    visible: delegTables.tableData["strict"]
                                    text: "STRICT"
                                    color: "orange"
                                }
                            }

                        }

                        /***************************************/
                        /***************************************/

                        Column {

                            Row {

                                spacing: 5

                                Text {
                                    text: "Entries:"
                                    font.pointSize: 12
                                    color: palette.text
                                    font.bold: true
                                }

                                TextEdit {
                                    text: delegTables.tableData["rowCount"]
                                    font.pointSize: 12
                                    color: palette.text
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    readOnly: true
                                    textFormat: Text.RichText
                                }

                            }

                            /***************************************/

                            Row {

                                spacing: 5

                                visible: delegTables.tableData["columnCount"]

                                Text {
                                    id: titleCol
                                    text: "Columns:"
                                    font.pointSize: 12
                                    color: palette.text
                                    font.bold: true
                                }

                                TextEdit {
                                    text: (function() {
                                        var txt = []
                                        for(let i in delegTables.tableData["columns"]) {
                                            const cur = delegTables.tableData["columns"][i]
                                            txt.push(cur["name"] + "&nbsp;<i>" + cur["type"] + "</i>" +
                                                     (cur["primaryKey"] ? "&nbsp;[PK]" : "") +
                                                     (cur["notNull"] ? "&nbsp;[NOT&nbsp;NULL]" : "") +
                                                     (cur["generated"] ? "&nbsp;[GENERATED]" : "") +
                                                    (cur["defaultValue"]!=="" ? ("&nbsp;DEFAULT&nbsp;"+cur["defaultValue"]) : ""))
                                        }
                                        return txt.join("<br>")
                                    })()
                                    font.pointSize: 12
                                    color: palette.text
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    readOnly: true
                                    textFormat: Text.RichText
                                    width: col.width-titleCol.width-15
                                }

                            }

                            /***************************************/

                            Row {

                                spacing: 5

                                visible: delegTables.tableData["indexCount"]

                                Text {
                                    id: titleInd
                                    text: "Indexes:"
                                    font.pointSize: 12
                                    color: palette.text
                                    font.bold: true
                                }

                                TextEdit {
                                    text: (function() {
                                        var txt = []
                                        for(let i in delegTables.tableData["indexes"]) {
                                            const cur = delegTables.tableData["indexes"][i]
                                            txt.push(cur["name"] + " " +
                                                     "<i>" + cur["columns"].join(", ") + "</i>" +
                                                     (cur["unique"] ? " [UNIQUE]" : ""))
                                        }
                                        return txt.join(", ")
                                    })()
                                    font.pointSize: 12
                                    color: palette.text
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    readOnly: true
                                    textFormat: Text.RichText
                                    width: col.width-titleInd.width-15
                                }

                            }

                            /***************************************/

                            Row {

                                spacing: 5

                                visible: delegTables.tableData["foreignKeyCount"]

                                Text {
                                    id: titleFor
                                    text: "Foreign keys:"
                                    font.pointSize: 12
                                    color: palette.text
                                    font.bold: true
                                }

                                TextEdit {
                                    text: (function() {
                                        var txt = []
                                        for(let i in delegTables.tableData["foreignKeys"]) {
                                            const cur = delegTables.tableData["foreignKeys"][i]
                                            txt.push(cur["from"] + " → " + cur["to"] + " [" + cur["onUpdate"] + "&nbsp;/&nbsp;" + cur["onDelete"] + "]")
                                        }
                                        return txt.join(", ")
                                    })()
                                    font.pointSize: 12
                                    color: palette.text
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    readOnly: true
                                    textFormat: Text.RichText
                                    width: col.width-titleFor.width-15
                                }

                            }

                        }

                        Item {
                            width: 1
                            height: 1
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

        const dat = PQCFileHandler.getDataWithPlugin("sqlite", PQCConstants.currentSource)
        sqlite_top.currentData = dat[1]

        PQCNotify.updateWindowSizeDefault()

        PQCConstants.imageStatus = Image.Ready

    }

    function formatBytes(bytes) {

        if(bytes < 1024)
            return bytes + " B"
        if(bytes < 1024*1024)
            return (bytes/1024).toFixed(1) + " KB"
        if(bytes < 1024*1024*1024)
            return (bytes/1024/1024).toFixed(1) + " MB"

        return (bytes/1024/1024/1024).toFixed(2) + " GB"
    }

}
