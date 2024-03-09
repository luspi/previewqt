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

    title: "Settings"
    visible: false

    // For this window, this item catches all key presses
    Item {
        id: catchKeyPress

        Keys.onPressed: (event) => {

            if(tabbar.currentIndex == 1 && shortcutbut.checked) {

                var txt = PQCScripts.keycodeToString(event.modifiers, event.key)
                var reserved = ["Esc", "Space", "Left", "Right", "M", "Home", "End", "Ctrl+Q", "Ctrl+O", "Ctrl+P", "Ctrl+I"]

                if(txt === "Esc") {
                    shortcutbut.text = shortcutbut.backupshortcut
                    shortcutbut.checked = false
                    return
                }

                if(reserved.indexOf(txt) > -1) {
                    reservederror.visible = true
                    return
                }

                reservederror.visible = false

                shortcutbut.text = txt
                PQCSettings.defaultAppShortcut = txt

                if(!txt.endsWith("+")) {
                    shortcutbut.backupshortcut = txt
                    shortcutbut.checked = false
                }

            } else if(event.key === Qt.Key_Escape)
                settings_top.close()

        }

    }

    onClosing: {
        focusitem.forceActiveFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
    }

    // the top bars
    TabBar {
        id: tabbar
        width: parent.width
        TabButton {
            text: "General"
            width: settings_top.width/2
        }
        TabButton {
            text: "Default applications"
            width: settings_top.width/2
        }
    }

    StackLayout {

        id: stack

        x: 0
        y: tabbar.height
        width: parent.width
        height: parent.height - 40 - tabbar.height

        currentIndex: tabbar.currentIndex
        onCurrentIndexChanged:
            catchKeyPress.forceActiveFocus()

        // the general settings
        Item {

            id: generalsettings
            width: settings_top.width-20
            height: settings_top.height

            Column {

                x: 10
                spacing: 10

                Item {
                    width: 1
                    height: 1
                }

                Text {
                    x: (settings_top.width-width)/2
                    font.pointSize: 18
                    font.bold: true
                    text: "Settings"
                }

                Text {
                    x: (settings_top.width-width)/2
                    text: "Note: Settings will be saved automatically."
                }

                /************************************/

                CheckBox {
                    text: "Keep top bar always visible"
                    checked: !PQCSettings.topBarAutoHide
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.topBarAutoHide === checked)
                            PQCSettings.topBarAutoHide = !checked
                    }
                }

                CheckBox {
                    text: "Launch PreviewQt hidden to system tray"
                    checked: PQCSettings.launchHiddenToSystemTray
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.launchHiddenToSystemTray !== checked)
                            PQCSettings.launchHiddenToSystemTray = checked
                    }
                }

                /************************************/
                Rectangle {
                    width: settings_top.width-20
                    height: 1
                    color: "black"
                }
                /************************************/

                Text {
                    y: (defwin_w.height-height)/2
                    text: "Default window size at launch:"
                }

                Row {

                    spacing: 5

                    SpinBox {
                        id: defwin_w
                        width: 75
                        from: 200
                        to: 99999
                        value: PQCSettings.defaultWindowWidth
                        onValueChanged: {
                            if(value !== PQCSettings.defaultWindowWidth)
                                PQCSettings.defaultWindowWidth = value
                        }
                    }

                    Text {
                        y: (defwin_w.height-height)/2
                        text: "x"
                    }

                    SpinBox {
                        id: defwin_h
                        width: 75
                        from: 200
                        to: 99999
                        value: PQCSettings.defaultWindowHeight
                        onValueChanged: {
                            if(value !== PQCSettings.defaultWindowHeight)
                                PQCSettings.defaultWindowHeight = value
                        }
                    }
                }
                CheckBox {
                    text: "Launch PreviewQt maximized"
                    checked: PQCSettings.defaultWindowMaximized
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(PQCSettings.defaultWindowMaximized !== checked)
                            PQCSettings.defaultWindowMaximized = checked
                    }
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
                    x: (settings_top.width-width)/2
                    font.pointSize: 18
                    font.bold: true
                    text: "Settings"
                }

                Text {
                    x: (settings_top.width-width)/2
                    text: "Note: Settings will be saved automatically."
                }

                /************************************/

                Text {
                    y: (shortcutbut.height-height)/2
                    text: "Shortcut to launch in default application:"
                }

                Button {
                    id: shortcutbut
                    width: 200
                    checkable: true
                    text: PQCSettings.defaultAppShortcut
                    property string backupshortcut: PQCSettings.defaultAppShortcut
                    onCheckedChanged: catchKeyPress.forceActiveFocus()
                }

                Text {
                    id: reservederror
                    width: settings_top.width
                    visible: false
                    font.pointSize: 8
                    font.bold: true
                    color: "red"
                    text: "This is a reserved shortcut for PreviewQt."
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
                    text: "Close after opening external application"
                    checked: PQCSettings.closeAfterDefaultApp
                    onCheckedChanged: {
                        catchKeyPress.forceActiveFocus()
                        if(checked !== PQCSettings.closeAfterDefaultApp)
                            PQCSettings.closeAfterDefaultApp = checked
                    }
                }

                Column {

                    Text {
                        text: "Default application for images:"
                    }

                    Row {
                        spacing: 5
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
                        text: "Default application for documents:"
                    }

                    Row {
                        spacing: 5
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
                        text: "Default application for videos:"
                    }

                    Row {
                        spacing: 5
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
                        text: "Default application for archives:"
                    }

                    Row {
                        spacing: 5
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
                        text: "Default application for comic books:"
                    }

                    Row {
                        spacing: 5
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
        y: parent.height-40
        width: parent.width
        height: 1
        color: "black"
    }
    /************************************/

    Button {

        x: (parent.width-width)/2
        y: parent.height-40 + (40-height)/2
        text: "Close"
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

}
