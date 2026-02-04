/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
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

import PreviewQt

Window {

    id: settings_top

    title: qsTr("Settings")
    visible: false
    modality: Qt.ApplicationModal
    width: 500
    height: 500

    color: palette.base

    property bool optionsLoaded: false

    onVisibilityChanged: (visibility) => {
        PQCConstants.windowAboutSettingsVisible = (visibility === Window.Hidden ? false : true)
    }

    // For this window, this item catches all key presses
    Item {
        id: catchKeyPress

        Keys.onPressed: (event) => {

            if(tabbar.currentIndex === 1 && set_external.shortcutChecked) {

                var txt = PQCScriptsOther.keycodeToString(event.modifiers, event.key)
                var reserved = ["Esc", "Space", "Left", "Right", "Up", "Down", "Home", "End"]

                if(txt === "Esc") {
                    set_external.shortcutText = set_external.shortcutBackup
                    set_external.shortcutChecked = false
                    return
                }

                set_external.shortcutText = txt

                if(event.modifiers === Qt.ControlModifier) {
                    set_external.setReservedError(2, true)
                    return
                }

                if(reserved.indexOf(txt) > -1) {
                    set_external.setReservedError(1, true)
                    return
                }

                set_external.setReservedError(1, false)
                set_external.setReservedError(2, false)

                if(!txt.endsWith("+")) {
                    PQCSettings.defaultAppShortcut = txt
                    set_external.shortcutBackup = txt
                    set_external.shortcutChecked = false
                }

            } else if(event.key === Qt.Key_Escape || event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                settings_top.close()

            else if(event.key === Qt.Key_Tab && event.modifiers&Qt.ControlModifier)
                tabbar.currentIndex = (tabbar.currentIndex+1)%2

        }

    }

    onClosing: {
        PQCNotify.resetFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
        tabbar.currentIndex = 0

        set_general.loadSettings()
        set_external.loadSettings()

        optionsLoaded = true

    }

    Item {

        id: tabbar

        width: parent.width
        height: 40
        property int currentIndex: 0

        Rectangle {
            x: 0
            width: parent.width/3
            height: parent.height
            color: tabbar.currentIndex==0 ? palette.base : palette.disabled.base
            Text {
                anchors.centerIn: parent
                text: qsTr("General")
                color: palette.text
                opacity: tabbar.currentIndex==0 ? 1 : 0.7
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: tabbar.currentIndex = 0
            }
            Rectangle {
                x: parent.width-width
                width: 1
                height: parent.height
                color: palette.text
            }
        }

        Rectangle {
            x: parent.width/3
            width: parent.width/3
            height: parent.height
            color: tabbar.currentIndex==1 ? palette.base : palette.disabled.base
            Text {
                anchors.centerIn: parent
                text: qsTr("External applications")
                color: palette.text
                opacity: tabbar.currentIndex==1 ? 1 : 0.7
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: tabbar.currentIndex = 1
            }
            Rectangle {
                x: parent.width-width
                width: 1
                height: parent.height
                color: palette.text
            }
        }

        Rectangle {
            x: 2*parent.width/3
            width: parent.width/3
            height: parent.height
            color: tabbar.currentIndex==2 ? palette.base : palette.disabled.base
            Text {
                anchors.centerIn: parent
                text: qsTr("Additional tools")
                color: palette.text
                opacity: tabbar.currentIndex==2 ? 1 : 0.7
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: tabbar.currentIndex = 2
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            y: parent.height-height
            color: palette.text
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

        /************************************/
        /************************************/

        // the general settings
        PQSettingsGeneral {
            id: set_general
            width: stack.width
            height: stack.height
            optionsLoaded: settings_top.optionsLoaded
        }

        /************************************/

        // tab for external applications
        PQSettingsExternalApplications {
            id: set_external
            width: stack.width
            height: stack.height
            optionsLoaded: settings_top.optionsLoaded
            onResetFocus: {
                catchKeyPress.forceActiveFocus()
            }
        }

        /************************************/

        // the general settings
        PQSettingsTools {
            id: set_tools
            width: stack.width
            height: stack.height
        }

        /************************************/
        /************************************/

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

    Component.onCompleted: {
        settings_top.show()
        if(PQCConstants.settingsTabNextTime > -1)
            settings_top.selectTab(PQCConstants.settingsTabNextTime)
        PQCConstants.settingsTabNextTime = -1
    }

    Connections {

        target: PQCNotify

        function onShowSubWindow(wdw : string) {

            if(wdw === "settings") {
                settings_top.show()
                if(PQCConstants.settingsTabNextTime > -1)
                    settings_top.selectTab(PQCConstants.settingsTabNextTime)
                PQCConstants.settingsTabNextTime = -1
            }

        }

    }

    /************************************/

    function selectTab(idx : int) {
        tabbar.currentIndex = idx
    }

}
