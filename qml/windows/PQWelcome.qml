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
import PreviewQt

Window {

    id: welcome_top

    //: window title
    title: qsTr("Welcome to PreviewQt")

    // some stylings
    visible: false
    modality: Qt.ApplicationModal

    color: palette.base

    // in this window, this item catches all key presses
    Item {
        id: catchKeyPress
        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape || event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
                welcome_top.close()
        }
    }

    width: 500
    height: 350

    onVisibilityChanged: (visibility) => {
        PQCConstants.windowWelcomeVisible = (visibility === Window.Hidden ? false : true)
    }

    Flickable {

        id: flickable

        anchors.fill: parent
        anchors.margins: 5
        anchors.bottomMargin: 45
        clip: true

        contentHeight: contcol.height

        Column {

            id: contcol

            spacing: 10

            width: parent.width

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Welcome to PreviewQt")
                font.pointSize: 20
                font.bold: true
                color: palette.text
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.bold: true
                text: qsTr("PreviewQt is a simple application for previewing all kinds of files.")
                color: palette.text
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("It is optimized for speed and lightweight and thus does not have all the features one might expect from an image/document viewer. You can always pass on a file to an external application through a button and shortcut. The selection of external applications can be adjusted freely.")
                color: palette.text
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("When not in use, PreviewQt sits idle in the system tray, ready to be shown as instanteneously as possible.")
                color: palette.text
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.bold: true
                text: qsTr("Questions? Feedback? Send me an email:") + " Lukas@previewqt.org"
                color: palette.text
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("Also check out the website:") + " https://previewqt.org/"
                color: palette.text
            }

        }
    }

    // when closing we re-focus on main key catcher
    onClosing: {
        PQCNotify.resetFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
    }


    Rectangle {
        x: 0
        y: parent.height-45
        width: parent.width
        height: 1
        color: "black"
    }

    Button {
        x: (parent.width-width)/2
        y: parent.height-45 + (45-height)/2
        text: qsTr("Get started")
        onClicked:
            welcome_top.close()

    }

}
