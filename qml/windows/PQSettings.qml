/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2024 Lukas Spies                                       **
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
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window
import PQCSettings
import PQCScripts

Window {

    id: settings_top

    title: qsTr("Settings")
    visible: false
    modality: Qt.ApplicationModal
    width: 500
    height: 500

    property bool optionsLoaded: false

    property var imgoptions_key: ["PhotoQt", "Eye of Gnome", "Eye of Mate", "Geeqie", "GThumb", "GwenView", "LXImage-Qt", "Nomacs", "Ristretto", "Showfoto", "(custom)"]
    property var imgoptions_val: ["photoqt", "eog",          "eom",         "geeqie", "gthumb", "gwenview", "lximage-qt", "nomacs", "ristretto", "showfoto", ""]

    property var docoptions_key: ["PhotoQt", "Atril", "Evince", "Okular", "(custom)"]
    property var docoptions_val: ["photoqt", "atril", "evince", "okular", ""]

    property var arcoptions_key: ["PhotoQt", "Ark", "Engrampa", "File Roller", "(custom)"]
    property var arcoptions_val: ["photoqt", "ark", "engrampa", "file-roller", ""]

    property var comoptions_key: ["PhotoQt", "Calibre", "Evince", "MComix", "Okular", "(custom)"]
    property var comoptions_val: ["photoqt", "calibre", "evince", "mcomix", "okular", ""]

    property var bokoptions_key: ["E-Book viewer", "Calibre", "Evince", "Okular", "(custom)"]
    property var bokoptions_val: ["ebook-viewer",  "calibre", "evince", "okular", ""]

    property var vidoptions_key: ["PhotoQt", "Dragon Player", "Parole", "SMPlayer", "Totem", "VLC", "(custom)"]
    property var vidoptions_val: ["photoqt", "dragon",        "parole", "smplayer", "totem", "vlc", ""]

    // For this window, this item catches all key presses
    Item {
        id: catchKeyPress

        Keys.onPressed: (event) => {

            if(tabbar.currentIndex === 1 && shortcutbut.checked) {

                var txt = PQCScripts.keycodeToString(event.modifiers, event.key)
                var reserved = ["Esc", "Space", "Left", "Right", "M", "Home", "End", "Ctrl+Q", "Ctrl+O", "Ctrl+P", "Ctrl+I", "F1", "Ctrl+Tab"]

                if(txt === "Esc") {
                    shortcutbut.text = shortcutbut.backupshortcut
                    shortcutbut.checked = false
                    return
                }

                if(reserved.indexOf(txt) > -1) {
                    reservederror.visible = true
                    shortcutbut.text = shortcutbut.backupshortcut
                    shortcutbut.checked = false
                    return
                }

                reservederror.visible = false

                shortcutbut.text = txt

                if(!txt.endsWith("+")) {
                    PQCSettings.defaultAppShortcut = txt
                    shortcutbut.backupshortcut = txt
                    shortcutbut.checked = false
                }

            } else if(event.key === Qt.Key_Escape || event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                settings_top.close()

            else if(event.key === Qt.Key_Tab && event.modifiers&Qt.ControlModifier)
                tabbar.currentIndex = (tabbar.currentIndex+1)%2

        }

    }

    onClosing: {
        focusitem.forceActiveFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
        tabbar.currentIndex = 0

        var imgindex = Object.values(imgoptions_val).indexOf(PQCSettings.defaultAppImages)
        imgcombo.currentIndex = (imgindex===-1 ? imgcombo.currentIndex=imgcombo.model.length-1 : imgindex)

        var docindex = Object.values(docoptions_val).indexOf(PQCSettings.defaultAppDocuments)
        doccombo.currentIndex = (docindex===-1 ? doccombo.currentIndex=doccombo.model.length-1 : docindex)

        var vidindex = Object.values(vidoptions_val).indexOf(PQCSettings.defaultAppVideos)
        vidcombo.currentIndex = (vidindex===-1 ? vidcombo.currentIndex=vidcombo.model.length-1 : vidindex)

        var arcindex = Object.values(arcoptions_val).indexOf(PQCSettings.defaultAppArchives)
        arccombo.currentIndex = (arcindex===-1 ? arccombo.currentIndex=arccombo.model.length-1 : arcindex)

        var comindex = Object.values(comoptions_val).indexOf(PQCSettings.defaultAppComicBooks)
        comcombo.currentIndex = (comindex===-1 ? comcombo.currentIndex=comcombo.model.length-1 : comindex)

        var bokindex = Object.values(bokoptions_val).indexOf(PQCSettings.defaultAppEBooks)
        bokcombo.currentIndex = (bokindex===-1 ? bokcombo.currentIndex=bokcombo.model.length-1 : bokindex)

        optionsLoaded = true

    }

    // the top bars
    TabBar {
        id: tabbar
        width: parent.width
        TabButton {
            //: Tab name: general settings
            text: qsTr("General")
            font.bold: tabbar.currentIndex===0
            width: settings_top.width/2
        }
        TabButton {
            //: Tab name
            text: qsTr("External applications")
            font.bold: tabbar.currentIndex===1
            width: settings_top.width/2
        }
    }

    StackLayout {

        id: stack

        x: 0
        y: tabbar.height
        width: parent.width
        height: parent.height - 45 - tabbar.height

        currentIndex: tabbar.currentIndex
        onCurrentIndexChanged:
            catchKeyPress.forceActiveFocus()

        // the general settings
        Flickable {

            id: generalsettings
            width: stack.width
            height: stack.height

            contentHeight: general_col.height

            ScrollBar.vertical: ScrollBar { id: scrollbargeneral }

            property int usableWidth: width-20 - (scrollbargeneral.visible ? scrollbargeneral.width : 0)

            clip: true

            Column {

                id: general_col

                x: 10
                spacing: 10

                Item {
                    width: 1
                    height: 1
                }

                Text {
                    x: (generalsettings.usableWidth-width)/2
                    font.pointSize: 18
                    font.bold: true
                    //: Same as tab name but used as title
                    text: qsTr("Settings")
                }

                Text {
                    x: (generalsettings.usableWidth-width)/2
                    text: qsTr("Note: Settings will be saved automatically.")
                }

                /************************************/

                CheckBox {
                    //: the top bar is the bar with the buttons
                    text: qsTr("Keep top bar always visible")
                    width: generalsettings.usableWidth
                    checked: !PQCSettings.topBarAutoHide
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.topBarAutoHide === checked)
                            PQCSettings.topBarAutoHide = !checked
                    }
                }

                CheckBox {
                    text: qsTr("Launch PreviewQt hidden to system tray")
                    width: generalsettings.usableWidth
                    checked: PQCSettings.launchHiddenToSystemTray
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.launchHiddenToSystemTray !== checked)
                            PQCSettings.launchHiddenToSystemTray = checked
                    }
                }

                CheckBox {
                    text: qsTr("Hide PreviewQt when losing focus")
                    width: generalsettings.usableWidth
                    checked: PQCSettings.closeWhenLosingFocus
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.closeWhenLosingFocus !== checked)
                            PQCSettings.closeWhenLosingFocus = checked
                    }
                }

                /************************************/
                Rectangle {
                    width: generalsettings.usableWidth
                    height: 1
                    color: "black"
                }
                /************************************/

                CheckBox {
                    text: qsTr("Launch PreviewQt maximized")
                    width: generalsettings.usableWidth
                    checked: PQCSettings.defaultWindowMaximized
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.defaultWindowMaximized !== checked)
                            PQCSettings.defaultWindowMaximized = checked
                    }
                }

                Text {
                    width: generalsettings.usableWidth
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: qsTr("Default window size:")
                }

                Flow {

                    spacing: 5
                    width: generalsettings.usableWidth

                    Item {
                        width: 20
                        height: 1
                    }

                    SpinBox {
                        id: defwin_w
                        width: 80
                        from: 200
                        to: 99999
                        value: PQCSettings.defaultWindowWidth
                        onValueChanged: {
                            if(value !== PQCSettings.defaultWindowWidth)
                                PQCSettings.defaultWindowWidth = value
                        }
                    }

                    Text {
                        height: defwin_w.height
                        verticalAlignment: Text.AlignVCenter
                        text: "x"
                    }

                    SpinBox {
                        id: defwin_h
                        width: 80
                        from: 200
                        to: 99999
                        value: PQCSettings.defaultWindowHeight
                        onValueChanged: {
                            if(value !== PQCSettings.defaultWindowHeight)
                                PQCSettings.defaultWindowHeight = value
                        }
                    }
                }

                RadioButton {
                    id: radio_defsize
                    width: generalsettings.usableWidth
                    text: qsTr("Only set window size at launch")
                    checked: !PQCSettings.maximizeImageSizeAndAdjustWindow
                    onCheckedChanged:
                        catchKeyPress.forceActiveFocus()
                }

                RadioButton {
                    id: radio_defminmax
                    width: generalsettings.usableWidth
                    text: qsTr("Resize window to content")
                    checked: PQCSettings.maximizeImageSizeAndAdjustWindow
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.maximizeImageSizeAndAdjustWindow !== checked)
                            PQCSettings.maximizeImageSizeAndAdjustWindow = checked
                    }
                }

                /************************************/
                Item {
                    width: 1
                    height: 1
                }
                /************************************/

            }

        }

        /************************************/
        /************************************/

        // tab for external applications
        Flickable {

            id: defaultappsettings
            width: stack.width
            height: stack.height

            clip: true

            contentHeight: appcol.height

            property int usableWidth: width-20-(scrollbar.visible ? scrollbar.width : 0)
            ScrollBar.vertical: ScrollBar { id: scrollbar }

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
                }

                Text {
                    x: (defaultappsettings.usableWidth-width)/2
                    text: qsTr("Note: Settings will be saved automatically.")
                }

                /************************************/

                Text {
                    y: (shortcutbut.height-height)/2
                    text: qsTr("Shortcut to load in external application:")
                }

                Button {
                    id: shortcutbut
                    x: (defaultappsettings.usableWidth-width)/2
                    width: Math.min(300, parent.width*0.8)
                    checkable: true
                    text: PQCSettings.defaultAppShortcut
                    property string backupshortcut: PQCSettings.defaultAppShortcut
                    onCheckedChanged: catchKeyPress.forceActiveFocus()
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

                /************************************/
                Rectangle {
                    width: defaultappsettings.usableWidth
                    height: 1
                    color: "black"
                }
                /************************************/

                CheckBox {
                    width: defaultappsettings.usableWidth
                    text: qsTr("Hide window after launching external application")
                    checked: PQCSettings.closeAfterDefaultApp
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(checked !== PQCSettings.closeAfterDefaultApp)
                            PQCSettings.closeAfterDefaultApp = checked
                    }
                }

                Column {

                    Text {
                        text: qsTr("External application for images:")
                    }

                    ComboBox {
                        id: imgcombo
                        x: (defaultappsettings.usableWidth-width)/2
                        width: Math.min(300, defaultappsettings.usableWidth*0.8)
                        model: imgoptions_key
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < imgcombo.model.length-1) {
                                PQCSettings.defaultAppImages = imgoptions_val[currentIndex]
                            } else {
                                imgedit.text = PQCSettings.defaultAppImages
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        visible: imgcombo.currentIndex === imgcombo.model.length-1 || PQCScripts.amIOnWindows()
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

                Column {

                    Text {
                        text: qsTr("External application for documents:")
                    }

                    ComboBox {
                        id: doccombo
                        x: (defaultappsettings.usableWidth-width)/2
                        width: Math.min(300, defaultappsettings.usableWidth*0.8)
                        model: docoptions_key
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < doccombo.model.length-1) {
                                PQCSettings.defaultAppDocuments = docoptions_val[currentIndex]
                            } else {
                                docedit.text = PQCSettings.defaultAppDocuments
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        visible: doccombo.currentIndex === doccombo.model.length-1 || PQCScripts.amIOnWindows()
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

                Column {

                    Text {
                        text: qsTr("External application for videos:")
                    }

                    ComboBox {
                        id: vidcombo
                        x: (defaultappsettings.usableWidth-width)/2
                        width: Math.min(300, defaultappsettings.usableWidth*0.8)
                        model: vidoptions_key
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < vidcombo.model.length-1) {
                                PQCSettings.defaultAppVideos = vidoptions_val[currentIndex]
                            } else {
                                videdit.text = PQCSettings.defaultAppVideos
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        visible: vidcombo.currentIndex === vidcombo.model.length-1 || PQCScripts.amIOnWindows()
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

                Column {

                    Text {
                        text: qsTr("External application for archives:")
                    }

                    ComboBox {
                        id: arccombo
                        x: (defaultappsettings.usableWidth-width)/2
                        width: Math.min(300, defaultappsettings.usableWidth*0.8)
                        model: arcoptions_key
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < arccombo.model.length-1) {
                                PQCSettings.defaultAppArchives = arcoptions_val[currentIndex]
                            } else {
                                arcedit.text = PQCSettings.defaultAppArchives
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        visible: arccombo.currentIndex === arccombo.model.length-1 || PQCScripts.amIOnWindows()
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

                Column {

                    Text {
                        text: qsTr("External application for comic books:")
                    }

                    ComboBox {
                        id: comcombo
                        x: (defaultappsettings.usableWidth-width)/2
                        width: Math.min(300, defaultappsettings.usableWidth*0.8)
                        model: comoptions_key
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < comcombo.model.length-1) {
                                PQCSettings.defaultAppComicBooks = comoptions_val[currentIndex]
                            } else {
                                comedit.text = PQCSettings.defaultAppComicBooks
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        visible: comcombo.currentIndex === comcombo.model.length-1 || PQCScripts.amIOnWindows()
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

                Column {

                    Text {
                        text: qsTr("External application for E-books:")
                    }

                    ComboBox {
                        id: bokcombo
                        x: (defaultappsettings.usableWidth-width)/2
                        width: Math.min(300, defaultappsettings.usableWidth*0.8)
                        model: bokoptions_key
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < bokcombo.model.length-1) {
                                PQCSettings.defaultAppEBooks = bokoptions_val[currentIndex]
                            } else {
                                bokedit.text = PQCSettings.defaultAppEBooks
                            }
                        }
                    }

                    Row {
                        spacing: 5
                        visible: bokcombo.currentIndex === bokcombo.model.length-1 || PQCScripts.amIOnWindows()
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

                /************************************/
                Item {
                    width: 1
                    height: 1
                }
                /************************************/

            }

        }

    }

    /************************************/
    Rectangle {
        x: 0
        y: parent.height-45
        width: parent.width
        height: 1
        color: "black"
    }
    /************************************/

    Button {

        x: (parent.width-width)/2
        y: parent.height-45 + (45-height)/2
        width: Math.min(200, parent.width*0.5)
        //: written on button
        text: qsTr("Close")
        onClicked:
            settings_top.close()

    }

    /************************************/

    // file dialog to choose an executable
    FileDialog {
        id: selectExe
        currentFolder: "file://" + (prevexe.slice(0,1)==="/" ?
                                       PQCScripts.getDir(prevexe) :
                                       (PQCScripts.amIOnWindows() ?
                                            StandardPaths.standardLocations(StandardPaths.HomeLocation)[0] :
                                            "/usr/bin"))
        nameFilters: "All Files (*.*)"
        property string category: ""
        property string prevexe: ""
        onAccepted: {

            var file = selectedFile
            var fname = PQCScripts.getFilename(file)

            if(StandardPaths.findExecutable(fname) === file)
                file = fname

            if(category == "images")
                imgedit.text = PQCScripts.cleanPath(file)
            else if(category == "documents")
                imgedit.text = PQCScripts.cleanPath(file)
            else if(category == "videos")
                imgedit.text = PQCScripts.cleanPath(file)
            else if(category == "archives")
                imgedit.text = PQCScripts.cleanPath(file)
            else if(category == "comicbooks")
                comedit.text = PQCScripts.cleanPath(file)
            else if(category == "ebooks")
                bokedit.text = PQCScripts.cleanPath(file)
            else
                console.warn("Unknown category:", category)

        }
    }

    /************************************/

    function selectTab(idx) {
        tabbar.currentIndex = idx
    }

}
