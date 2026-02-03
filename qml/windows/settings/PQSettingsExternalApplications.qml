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

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import PreviewQt

// the general settings
Flickable {

    id: defaultappsettings

    property list<string> imgoptions_key: ["[system default]", "PhotoQt", "Eye of Gnome", "Eye of Mate", "Geeqie", "GThumb", "GwenView", "LXImage-Qt", "Nomacs", "Ristretto", "Showfoto", "(custom)"]
    property list<string> imgoptions_val: ["_default_", "photoqt", "eog",          "eom",         "geeqie", "gthumb", "gwenview", "lximage-qt", "nomacs", "ristretto", "showfoto", ""]

    property list<string> docoptions_key: ["[system default]", "PhotoQt", "Atril", "Evince", "Okular", "(custom)"]
    property list<string> docoptions_val: ["_default_", "photoqt", "atril", "evince", "okular", ""]

    property list<string> arcoptions_key: ["[system default]", "PhotoQt", "Ark", "Engrampa", "File Roller", "(custom)"]
    property list<string> arcoptions_val: ["_default_", "photoqt", "ark", "engrampa", "file-roller", ""]

    property list<string> comoptions_key: ["[system default]", "PhotoQt", "Calibre", "Evince", "MComix", "Okular", "(custom)"]
    property list<string> comoptions_val: ["_default_", "photoqt", "calibre", "evince", "mcomix", "okular", ""]

    property list<string> bokoptions_key: ["[system default]", "E-Book viewer", "Calibre", "Evince", "Okular", "(custom)"]
    property list<string> bokoptions_val: ["_default_", "ebook-viewer",  "calibre", "evince", "okular", ""]

    property list<string> vidoptions_key: ["[system default]", "PhotoQt", "Dragon Player", "Parole", "SMPlayer", "Totem", "VLC", "(custom)"]
    property list<string> vidoptions_val: ["_default_", "photoqt", "dragon",        "parole", "smplayer", "totem", "vlc", ""]

    property list<string> txtoptions_key: ["[system default]", "Kate", "KWrite", "Gedit", "Sublime", "(custom)"]
    property list<string> txtoptions_val: ["_default_", "kate", "kwrite", "gedit", "sublime", ""]

    property list<string> urloptions_key: ["[system default]", "Firefox", "chrome", "chromium", "(custom)"]
    property list<string> urloptions_val: ["_default_", "firefox", "chrome", "chromium", ""]

    clip: true

    contentHeight: appcol.height

    property alias shortcutText: shortcutbut.text
    property alias shortcutChecked: shortcutbut.checked
    property alias shortcutBackup: shortcutbut.backupshortcut

    property bool optionsLoaded: false

    property int usableWidth: width-20-(scrollbar.visible ? scrollbar.width : 0)
    ScrollBar.vertical: ScrollBar { id: scrollbar }

    signal resetFocus()

    function setReservedError(which : int, vis : bool) {
        if(which === 1) reservederror.visible = vis
        if(which === 2) reservederror2.visible = vis
    }

    function loadSettings() {

        var imgindex = Object.values(imgoptions_val).indexOf(PQCSettings.defaultAppImages)
        custom_img.checked = (imgindex !== 0)
        imgcombo.currentIndex = (imgindex===-1 ? imgcombo.currentIndex=imgcombo.model.length-1 : imgindex)

        var docindex = Object.values(docoptions_val).indexOf(PQCSettings.defaultAppDocuments)
        custom_doc.checked = (docindex !== 0)
        doccombo.currentIndex = (docindex===-1 ? doccombo.currentIndex=doccombo.model.length-1 : docindex)

        var vidindex = Object.values(vidoptions_val).indexOf(PQCSettings.defaultAppVideos)
        custom_vid.checked = (vidindex !== 0)
        vidcombo.currentIndex = (vidindex===-1 ? vidcombo.currentIndex=vidcombo.model.length-1 : vidindex)

        var arcindex = Object.values(arcoptions_val).indexOf(PQCSettings.defaultAppArchives)
        custom_arc.checked = (arcindex !== 0)
        arccombo.currentIndex = (arcindex===-1 ? arccombo.currentIndex=arccombo.model.length-1 : arcindex)

        var comindex = Object.values(comoptions_val).indexOf(PQCSettings.defaultAppComicBooks)
        custom_com.checked = (comindex !== 0)
        comcombo.currentIndex = (comindex===-1 ? comcombo.currentIndex=comcombo.model.length-1 : comindex)

        var bokindex = Object.values(bokoptions_val).indexOf(PQCSettings.defaultAppEBooks)
        custom_bok.checked = (bokindex !== 0)
        bokcombo.currentIndex = (bokindex===-1 ? bokcombo.currentIndex=bokcombo.model.length-1 : bokindex)

        var txtindex = Object.values(txtoptions_val).indexOf(PQCSettings.defaultAppText)
        custom_txt.checked = (txtindex !== 0)
        txtcombo.currentIndex = (txtindex===-1 ? txtcombo.currentIndex=txtcombo.model.length-1 : txtindex)

        var urlindex = Object.values(urloptions_val).indexOf(PQCSettings.defaultAppUrl)
        custom_url.checked = (urlindex !== 0)
        urlcombo.currentIndex = (urlindex===-1 ? urlcombo.currentIndex=urlcombo.model.length-1 : urlindex)

    }

    Column {

        id: appcol

        x: 10
        spacing: 10

        Item {
            width: 1
            height: 1
        }

        Text {
            x: (defaultappsettings.usableWidth-width)/2
            font.pointSize: 18
            font.bold: true
            text: qsTr("Settings")
            color: palette.text
        }

        Text {
            x: (defaultappsettings.usableWidth-width)/2
            text: qsTr("Note: Settings will be saved automatically.")
            color: palette.text
        }

        /************************************/

        Text {
            y: (shortcutbut.height-height)/2
            text: qsTr("Shortcut to load in external application:")
            color: palette.text
        }

        Button {
            id: shortcutbut
            x: (defaultappsettings.usableWidth-width)/2
            width: Math.min(300, parent.width*0.8)
            checkable: true
            text: PQCSettings.defaultAppShortcut
            property string backupshortcut: PQCSettings.defaultAppShortcut
            onCheckedChanged: defaultappsettings.resetFocus()
        }

        Text {
            id: reservederror
            width: defaultappsettings.usableWidth
            visible: false
            font.pointSize: 8
            font.bold: true
            color: "red"
            text: qsTr("This is a reserved shortcut for PreviewQt.")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        Text {
            id: reservederror2
            width: defaultappsettings.usableWidth
            visible: false
            font.pointSize: 8
            font.bold: true
            color: "red"
            //: The alone here refers to the fact that if the Ctrl modifier key is used without any other modifier keys (Shift, ...), then this is reserved.
            text: qsTr("Shortcuts with the Ctrl modifier alone are reserved for PreviewQt.")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        CheckBox {
            width: defaultappsettings.usableWidth
            text: qsTr("Hide window after launching external application")
            checked: PQCSettings.closeAfterDefaultApp
            onCheckedChanged: {
                defaultappsettings.resetFocus()
                if(checked !== PQCSettings.closeAfterDefaultApp)
                    PQCSettings.closeAfterDefaultApp = checked
            }
        }

        /************************************/
        Rectangle {
            width: defaultappsettings.usableWidth
            height: 1
            color: "black"
        }
        /************************************/

        Text {
            width: defaultappsettings.usableWidth
            color: palette.text
            text: qsTr("If no custom application is defined, then PreviewQt will use whatever application is set as default on your system.")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        CheckBox {
            id: custom_img
            text: "Use custom application for images"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppImages = "_default_"
                else PQCSettings.defaultAppImages = defaultappsettings.imgoptions_val[imgcombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_img.checked ? imgcol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: imgcol

                width: parent.width

                ComboBox {
                    id: imgcombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.imgoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < imgcombo.model.length-1) {
                            PQCSettings.defaultAppImages = defaultappsettings.imgoptions_val[currentIndex]
                        } else {
                            imgedit.text = PQCSettings.defaultAppImages
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: imgcombo.currentIndex === imgcombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: imgedit
                        y: (imgbut.height-height)/2
                        width: defaultappsettings.usableWidth-imgbut.width-5
                        text: PQCSettings.defaultAppImages
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppImages)
                                PQCSettings.defaultAppImages = text
                        }
                    }
                    Button {
                        id: imgbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "images"
                            selectExe.prevexe = imgedit.text
                            selectExe.open()
                        }
                    }
                }

            }

        }

        CheckBox {
            id: custom_doc
            text: "Use custom application for documents"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppDocuments = "_default_"
                else PQCSettings.defaultAppDocuments = defaultappsettings.docoptions_val[doccombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_doc.checked ? doccol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: doccol

                ComboBox {
                    id: doccombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.docoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < doccombo.model.length-1) {
                            PQCSettings.defaultAppDocuments = defaultappsettings.docoptions_val[currentIndex]
                        } else {
                            docedit.text = PQCSettings.defaultAppDocuments
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: doccombo.currentIndex === doccombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: docedit
                        y: (docbut.height-height)/2
                        width: defaultappsettings.usableWidth-docbut.width-5
                        text: PQCSettings.defaultAppDocuments
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppDocuments)
                                PQCSettings.defaultAppDocuments = text
                        }
                    }
                    Button {
                        id: docbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "documents"
                            selectExe.prevexe = docedit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        CheckBox {
            id: custom_vid
            text: "Use custom application for videos"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppVideos = "_default_"
                else PQCSettings.defaultAppVideos = defaultappsettings.vidoptions_val[vidcombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_vid.checked ? vidcol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: vidcol

                ComboBox {
                    id: vidcombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.vidoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < vidcombo.model.length-1) {
                            PQCSettings.defaultAppVideos = defaultappsettings.vidoptions_val[currentIndex]
                        } else {
                            videdit.text = PQCSettings.defaultAppVideos
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: vidcombo.currentIndex === vidcombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: videdit
                        y: (vidbut.height-height)/2
                        width: defaultappsettings.usableWidth-vidbut.width-5
                        text: PQCSettings.defaultAppVideos
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppVideos)
                                PQCSettings.defaultAppVideos = text
                        }
                    }
                    Button {
                        id: vidbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "videos"
                            selectExe.prevexe = videdit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        CheckBox {
            id: custom_arc
            text: "Use custom application for archives"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppArchives = "_default_"
                else PQCSettings.defaultAppArchives = defaultappsettings.arcoptions_val[arccombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_arc.checked ? arccol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: arccol

                ComboBox {
                    id: arccombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.arcoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < arccombo.model.length-1) {
                            PQCSettings.defaultAppArchives = defaultappsettings.arcoptions_val[currentIndex]
                        } else {
                            arcedit.text = PQCSettings.defaultAppArchives
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: arccombo.currentIndex === arccombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: arcedit
                        y: (arcbut.height-height)/2
                        width: defaultappsettings.usableWidth-arcbut.width-5
                        text: PQCSettings.defaultAppArchives
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppArchives)
                                PQCSettings.defaultAppArchives = text
                        }
                    }
                    Button {
                        id: arcbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "archives"
                            selectExe.prevexe = arcedit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        CheckBox {
            id: custom_com
            text: "Use custom application for comic books"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppComicBooks = "_default_"
                else PQCSettings.defaultAppComicBooks = defaultappsettings.comoptions_val[comcombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_com.checked ? comcol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: comcol

                ComboBox {
                    id: comcombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.comoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < comcombo.model.length-1) {
                            PQCSettings.defaultAppComicBooks = defaultappsettings.comoptions_val[currentIndex]
                        } else {
                            comedit.text = PQCSettings.defaultAppComicBooks
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: comcombo.currentIndex === comcombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: comedit
                        y: (combut.height-height)/2
                        width: defaultappsettings.usableWidth-combut.width-5
                        text: PQCSettings.defaultAppComicBooks
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppComicBooks)
                                PQCSettings.defaultAppComicBooks = text
                        }
                    }
                    Button {
                        id: combut
                        text: "..."
                        onClicked: {
                            selectExe.category = "comicbooks"
                            selectExe.prevexe = comedit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        CheckBox {
            id: custom_bok
            text: "Use custom application for E-books"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppEBooks = "_default_"
                else PQCSettings.defaultAppEBooks = defaultappsettings.bokoptions_val[bokcombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_bok.checked ? bokcol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: bokcol

                ComboBox {
                    id: bokcombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.bokoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < bokcombo.model.length-1) {
                            PQCSettings.defaultAppEBooks = defaultappsettings.bokoptions_val[currentIndex]
                        } else {
                            bokedit.text = PQCSettings.defaultAppEBooks
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: bokcombo.currentIndex === bokcombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: bokedit
                        y: (bokbut.height-height)/2
                        width: defaultappsettings.usableWidth-bokbut.width-5
                        text: PQCSettings.defaultAppEBooks
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppEBooks)
                                PQCSettings.defaultAppEBooks = text
                        }
                    }
                    Button {
                        id: bokbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "ebooks"
                            selectExe.prevexe = bokedit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        CheckBox {
            id: custom_txt
            text: "Use custom application for text documents"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppText = "_default_"
                else PQCSettings.defaultAppText = defaultappsettings.txtoptions_val[txtcombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_txt.checked ? txtcol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: txtcol

                ComboBox {
                    id: txtcombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.txtoptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < txtcombo.model.length-1) {
                            PQCSettings.defaultAppText = defaultappsettings.txtoptions_val[currentIndex]
                        } else {
                            txtedit.text = PQCSettings.defaultAppText
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: txtcombo.currentIndex === txtcombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: txtedit
                        y: (txtbut.height-height)/2
                        width: defaultappsettings.usableWidth-txtbut.width-5
                        text: PQCSettings.defaultAppText
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppText)
                                PQCSettings.defaultAppText = text
                        }
                    }
                    Button {
                        id: txtbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "text"
                            selectExe.prevexe = txtedit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        CheckBox {
            id: custom_url
            text: "Use custom application for URLs"
            onCheckedChanged: {
                if(!defaultappsettings.optionsLoaded) return
                if(!checked) PQCSettings.defaultAppUrl = "_default_"
                else PQCSettings.defaultAppUrl = defaultappsettings.urloptions_val[urlcombo.currentIndex]
            }
        }

        Item {

            width: parent.width
            height: custom_url.checked ? urlcol.height : 0
            Behavior on height { NumberAnimation { duration: 200 } }
            clip: true

            Column {

                id: urlcol

                ComboBox {
                    id: urlcombo
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, defaultappsettings.usableWidth*0.8)
                    model: defaultappsettings.urloptions_key
                    visible: !PQCScriptsConfig.amIOnWindows()
                    onCurrentIndexChanged: {
                        if(!defaultappsettings.optionsLoaded) return
                        defaultappsettings.resetFocus()
                        if(currentIndex < urlcombo.model.length-1) {
                            PQCSettings.defaultAppUrl = defaultappsettings.urloptions_val[currentIndex]
                        } else {
                            urledit.text = PQCSettings.defaultAppUrl
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: urlcombo.currentIndex === urlcombo.model.length-1 || PQCScriptsConfig.amIOnWindows()
                    TextField {
                        id: urledit
                        y: (urlbut.height-height)/2
                        width: defaultappsettings.usableWidth-urlbut.width-5
                        text: PQCSettings.defaultAppUrl
                        onTextChanged: {
                            if(text !== PQCSettings.defaultAppUrl)
                                PQCSettings.defaultAppUrl = text
                        }
                    }
                    Button {
                        id: urlbut
                        text: "..."
                        onClicked: {
                            selectExe.category = "url"
                            selectExe.prevexe = urledit.text
                            selectExe.open()
                        }
                    }
                }
            }

        }

        /************************************/
        Item {
            width: 1
            height: 1
        }
        /************************************/

    }

    // file dialog to choose an executable
    FileDialog {
        id: selectExe
        currentFolder: "file://" + (prevexe.slice(0,1)==="/" ?
                                       PQCScriptsFilesPaths.getDir(prevexe) :
                                       (PQCScriptsConfig.amIOnWindows() ?
                                            StandardPaths.standardLocations(StandardPaths.HomeLocation)[0] :
                                            "/usr/bin"))
        nameFilters: "All Files (*.*)"
        property string category: ""
        property string prevexe: ""
        onAccepted: {

            var file = selectedFile
            var fname = PQCScriptsFilesPaths.getFilename(file)

            if(StandardPaths.findExecutable(fname) === file)
                file = fname

            if(category == "images")
                imgedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "documents")
                imgedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "videos")
                imgedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "archives")
                imgedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "comicbooks")
                comedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "ebooks")
                bokedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "text")
                txtedit.text = PQCScriptsFilesPaths.cleanPath(file)
            else if(category == "url")
                urledit.text = PQCScriptsFilesPaths.cleanPath(file)
            else
                console.warn("Unknown category:", category)

        }
    }

}
