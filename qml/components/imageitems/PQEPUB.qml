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

import QtQuick
import QtQuick.Controls
import PQCScripts
import QtWebEngine
import PQCSettings

Item {

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    width: view.width
    height: view.height

    property bool asynchronous: false
    property int paintedWidth: Math.max(view.contentsSize.width, (coverimage.source!=="" ? coverimage.paintedWidth : width))
    property int paintedHeight: Math.max(view.contentsSize.height, (coverimage.source!=="" ? coverimage.paintedHeight : height))

    property int documentCount: 1
    property int currentDocument: 0

    onCurrentDocumentChanged:
        loadBook()

    // the book contents
    // 0: title of book
    // 1: cover image (can be empty)
    // 2-: filepath for documents composing book
    property var book: []

    Component.onCompleted: {

        // load book
        book = PQCScripts.loadEPUB(image_top.imageSource)

        // set book title
        toplevel.overrideTitle = book[0]

        // store document count
        documentCount = book.length-2

        // if we have a cover image, show that one (currentDocument := -1)
        if(book[1] !== "") {

            coverimage.source = "data:image/jpeg;base64," + book[1]
            currentDocument = -1

        // if we don't have a cover image (currentDocument := 0)
        } else {

            coverimage.source = ""
            currentDocument = 0

            // load the first page
            loadBook()

        }
    }


    // the view for the actual book content
    WebEngineView {

        id: view

        // hidden when cover image is shown
        visible: currentDocument>-1

        width: image_top.width
        height: image_top.height

        // monitor loading progress
        onLoadProgressChanged: {

            // everything loaded
            if(view.loadProgress == 100)
                image.status = Image.Ready

            // this disables user selection of content
            // we do this to help us force the active focus to the focusitem to catch key presses
            runJavaScript("var head = document.head;" +
                              "var node= document.createElement('style');" +
                              "var textnode = document.createTextNode('body {-webkit-user-select: none !important;}');" +
                              "node.appendChild(textnode);head.appendChild(node);"
                             ,function(){/*Empty function for callback*/})


        }

    }

    // This makes sure that whenever the webengineview stole the focus we return it to the focusitem
    Timer {
        interval: 250
        running: true
        repeat: true
        onTriggered:
            focusitem.forceActiveFocus()
    }

    // The cover image
    Image {

        id: coverimage

        visible: currentDocument==-1
        fillMode: Image.PreserveAspectFit

        property int defw: Math.max(50, PQCSettings.defaultWindowWidth)
        property int defh: Math.max(50, PQCSettings.defaultWindowHeight)

        width: image_top.width
        height: image_top.height
        sourceSize: (PQCSettings.maximizeImageSizeAndAdjustWindow && !toplevel.isMaximized && !toplevel.isFullscreen && !toplevel.manualWindowSizeChange) ?
                        Qt.size(defw, defh) :
                        Qt.size(image_top.windowWidth, image_top.windowHeight)

        onStatusChanged: {
            if(status == Image.Ready)
                image.status = Image.Ready
        }

    }

    // The progress in the current file
    Rectangle {

        x: (parent.width-width) - 15
        y: (parent.height-height)

        width: progressTxt.width+10
        height: progressTxt.height+10

        radius: 3
        visible: view.contentsSize.height>view.height
        color: "#88000000"

        opacity: progressInfo.containsMouse||textHasChanged ? 0.8 : 0.3
        Behavior on opacity { NumberAnimation { duration: 200 } }

        // when the view is scrolled we show this information shortly
        property bool textHasChanged: false
        Timer {
            running: parent.textHasChanged
            interval: 2000
            onTriggered:
                parent.textHasChanged = false
        }

        // what progress is done
        Text {
            id: progressTxt
            x: 5
            y: 5
            text: Math.min(100, Math.round(100* (view.scrollPosition.y / (view.contentsSize.height-view.height)))) + "%"
            color: "white"
            onTextChanged:
                parent.textHasChanged = true
        }

        // catche mouse events
        MouseArea {
            id: progressInfo
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    // Navigate through book
    Rectangle {

        id: navcont

        x: (parent.width-width)/2
        y: (parent.height-height-10)

        width: navrow.width+10
        height: 30

        radius: 5
        color: "#88000000"

        opacity: nextmouse.containsMouse||backmouse.containsMouse||bgmouse.containsMouse ? 0.8 : 0.1
        Behavior on opacity { NumberAnimation { duration: 200 } }

        MouseArea {
            id: bgmouse
            anchors.fill: parent
            hoverEnabled: true
        }

        Row {

            id: navrow
            x: 5
            height: parent.height

            spacing: 5

            // go to the previous document in the book
            Rectangle {

                y: (parent.height-height)/2
                width: height
                height: navcont.height*0.8
                color: backmouse.containsPress ? "#88000000" : (backmouse.containsMouse ? "#88444444" : "transparent")
                Behavior on color { ColorAnimation { duration: 200 } }
                radius: 5

                Image {
                    anchors.fill: parent
                    sourceSize: Qt.size(width, height)
                    source: "image://svg/:/backwards.svg"
                    MouseArea {
                        id: backmouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                            currentDocument = Math.max((book[1] !== "" ? -1 : 0), currentDocument-1)
                    }
                }
            }

            // go to the next document in the book
            Rectangle {

                y: (parent.height-height)/2
                width: height
                height: navcont.height*0.8
                color: nextmouse.containsPress ? "#88000000" : (nextmouse.containsMouse ? "#88444444" : "transparent")
                Behavior on color { ColorAnimation { duration: 200 } }
                radius: 5

                Image {
                    anchors.fill: parent
                    sourceSize: Qt.size(width, height)
                    source: "image://svg/:/forwards.svg"
                    MouseArea {
                        id: nextmouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                            currentDocument = Math.min(documentCount-1, currentDocument+1)
                    }
                }

            }

            // show what number of document currently is shown
            Text {
                y: (parent.height-height)/2
                color: "white"
                text: "%1 / %2".arg(currentDocument+(book[1]==="" ? 1 : 2)).arg(documentCount+(book[1]==="" ? 0 : 1))
            }

        }

    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Left) {

                currentDocument = Math.max((book[1] !== "" ? -1 : 0), currentDocument-1)

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                currentDocument = Math.min(documentCount-1, currentDocument+1)

            } else if(keycode === Qt.Key_Home) {

                currentDocument = 0

            } else if(keycode === Qt.Key_End) {

                currentDocument = documentCount-3

            } else if(keycode === Qt.Key_PageDown) {

                view.runJavaScript("window.scrollBy(0,%1);".arg(view.height*0.9));

            } else if(keycode === Qt.Key_PageUp) {

                view.runJavaScript("window.scrollBy(0,%1);".arg(-view.height*0.9));

            } else if(keycode === Qt.Key_Down) {

                view.runJavaScript("window.scrollBy(0, 20);");

            } else if(keycode === Qt.Key_Up) {

                view.runJavaScript("window.scrollBy(0, -20);");

            }

        }

    }

    // load the current document file
    function loadBook() {
        if(currentDocument == -1) return
        view.url = "file://" + book[currentDocument+2]
    }

}
