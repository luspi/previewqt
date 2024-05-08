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

    property var imgoptions: {
        "PhotoQt" : "photoqt",
        "GwenView" : "gwenview",
        "Showfoto" : "showfoto",
        "Eye of Mate" : "eom",
        "Eye of Gnome" : "eog",
        "GThumb" : "gthumb",
        "Geeqie" : "geeqie",
        "Nomacs" : "nomacs",
        "LXImage-Qt" : "lximage-qt",
        "Ristretto" : "ristretto",
        "(custom)" : ""
    }

    property var docoptions: {
        "PhotoQt" : "photoqt",
        "Okular" : "okular",
        "Evince" : "evince",
        "Atril" : "atril",
        "(custom)" : ""
    }

    property var arcoptions: {
        "PhotoQt" : "photoqt",
        "Ark" : "ark",
        "File Roller" : "file-roller",
        "Engrampa" : "engrampa",
        "(custom)" : ""
    }

    property var comoptions: {
        "PhotoQt" : "photoqt",
        "Okular" : "okular",
        "Evince" : "evince",
        "Calibre" : "calibre",
        "MComix" : "mcomix",
        "(custom)" : ""
    }

    property var vidoptions: {
        "PhotoQt" : "photoqt",
        "VLC" : "vlc",
        "SMPlayer" : "smplayer",
        "Dragon Player" : "dragon",
        "Totem" : "totem",
        "Parole" : "parole",
        "(custom)" : ""
    }

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

        var imgindex = Object.values(imgoptions).indexOf(PQCSettings.defaultAppImages)
        imgcombo.currentIndex = (imgindex===-1 ? imgcombo.currentIndex=imgcombo.model.length-1 : imgindex)

        var docindex = Object.values(docoptions).indexOf(PQCSettings.defaultAppDocuments)
        doccombo.currentIndex = (docindex===-1 ? doccombo.currentIndex=doccombo.model.length-1 : docindex)

        var vidindex = Object.values(vidoptions).indexOf(PQCSettings.defaultAppVideos)
        vidcombo.currentIndex = (vidindex===-1 ? vidcombo.currentIndex=vidcombo.model.length-1 : vidindex)

        var arcindex = Object.values(arcoptions).indexOf(PQCSettings.defaultAppArchives)
        arccombo.currentIndex = (arcindex===-1 ? arccombo.currentIndex=arccombo.model.length-1 : arcindex)

        var comindex = Object.values(comoptions).indexOf(PQCSettings.defaultAppComicBooks)
        comcombo.currentIndex = (comindex===-1 ? comcombo.currentIndex=comcombo.model.length-1 : comindex)

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
                    id: radio_defminmax
                    width: generalsettings.usableWidth
                    text: qsTr("Maximize image inside of this maximum size and adjust window")
                    checked: PQCSettings.maximizeImageSizeAndAdjustWindow
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.maximizeImageSizeAndAdjustWindow !== checked)
                            PQCSettings.maximizeImageSizeAndAdjustWindow = checked
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
                    text: qsTr("Shortcut to launch in external application:")
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
                        model: Object.keys(imgoptions)
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < imgcombo.model.length-1) {
                                PQCSettings.defaultAppImages = imgoptions[model[currentIndex]]
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
                        model: Object.keys(docoptions)
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < doccombo.model.length-1) {
                                PQCSettings.defaultAppDocuments = docoptions[model[currentIndex]]
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
                        model: Object.keys(vidoptions)
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < vidcombo.model.length-1) {
                                PQCSettings.defaultAppVideos = vidoptions[model[currentIndex]]
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
                        model: Object.keys(arcoptions)
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < arccombo.model.length-1) {
                                PQCSettings.defaultAppArchives = arcoptions[model[currentIndex]]
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
                        model: Object.keys(comoptions)
                        visible: !PQCScripts.amIOnWindows()
                        onCurrentIndexChanged: {
                            if(!optionsLoaded) return
                            catchKeyPress.forceActiveFocus()
                            if(currentIndex < comcombo.model.length-1) {
                                PQCSettings.defaultAppComicBooks = comoptions[model[currentIndex]]
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
            else
                console.warn("Unknown category:", category)

        }
    }

    /************************************/

    function selectTab(idx) {
        tabbar.currentIndex = idx
    }

}
