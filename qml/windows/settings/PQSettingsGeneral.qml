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
import QtQuick.Controls

import PreviewQt

// the general settings
Flickable {

    id: generalsettings

    contentHeight: general_col.height

    ScrollBar.vertical: ScrollBar { id: scrollbargeneral }

    property bool optionsLoaded: false

    property int usableWidth: width-20 - (scrollbargeneral.visible ? scrollbargeneral.width : 0)

    clip: true

    function loadSettings() {}

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
            color: palette.text
        }

        Text {
            x: (generalsettings.usableWidth-width)/2
            text: qsTr("Note: Settings will be saved automatically.")
            color: palette.text
        }

        /************************************/

        Row {

            spacing: 10

            Text {
                y: (langcombo.height-height)/2
                text: qsTr("Language:")
                color: palette.text
            }

            ComboBox {
                id: langcombo
                property list<string> codes: ["en", "de_DE"]
                model: ["English",
                        "Deutsch"]
                currentIndex: codes.indexOf(PQCSettings.language)
                onCurrentIndexChanged: {
                    if(!generalsettings.optionsLoaded) return
                    catchKeyPress.forceActiveFocus()
                    PQCSettings.language = codes[currentIndex]
                    PQCScriptsConfig.updateTranslation(PQCSettings.language)
                }
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
            //: the top bar is the bar with the buttons
            text: qsTr("Keep top bar always visible")
            width: generalsettings.usableWidth
            checked: !PQCSettings.topBarAutoHide
            onCheckedChanged: {
                if(!generalsettings.optionsLoaded) return
                catchKeyPress.forceActiveFocus()
                if(PQCSettings.topBarAutoHide === checked)
                    PQCSettings.topBarAutoHide = !checked
            }
        }

        CheckBox {
            id: hideToTray
            text: qsTr("Hide to system tray when closing window")
            checked: PQCSettings.hideToSystemTray
            onCheckedChanged: {
                if(!generalsettings.optionsLoaded) return
                catchKeyPress.forceActiveFocus()
                if(PQCSettings.hideToSystemTray !== checked)
                    PQCSettings.hideToSystemTray = checked
            }
        }

        CheckBox {
            x: 20
            text: qsTr("Launch PreviewQt hidden to system tray")
            width: generalsettings.usableWidth-20
            checked: PQCSettings.launchHiddenToSystemTray
            enabled: hideToTray.checked
            onCheckedChanged: {
                if(!generalsettings.optionsLoaded) return
                catchKeyPress.forceActiveFocus()
                if(PQCSettings.launchHiddenToSystemTray !== checked) {
                    PQCSettings.launchHiddenToSystemTray = checked
                    PQCSettings.notifyNextLaunchHiddenToSystemTray = checked
                }
            }
        }

        CheckBox {
            text: qsTr("Hide PreviewQt when losing focus")
            width: generalsettings.usableWidth
            checked: PQCSettings.closeWhenLosingFocus
            onCheckedChanged: {
                if(!generalsettings.optionsLoaded) return
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
                if(!generalsettings.optionsLoaded) return
                catchKeyPress.forceActiveFocus()
                if(PQCSettings.defaultWindowMaximized !== checked)
                    PQCSettings.defaultWindowMaximized = checked
            }
        }

        Text {
            width: generalsettings.usableWidth
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: qsTr("Default window size:")
            color: palette.text
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
                    if(!generalsettings.optionsLoaded) return
                    if(value !== PQCSettings.defaultWindowWidth)
                        PQCSettings.defaultWindowWidth = value
                }
            }

            Text {
                height: defwin_w.height
                verticalAlignment: Text.AlignVCenter
                text: "x"
                color: palette.text
            }

            SpinBox {
                id: defwin_h
                width: 80
                from: 200
                to: 99999
                value: PQCSettings.defaultWindowHeight
                onValueChanged: {
                    if(!generalsettings.optionsLoaded) return
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
                if(!generalsettings.optionsLoaded) return
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
