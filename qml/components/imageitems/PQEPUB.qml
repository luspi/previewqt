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
import PQCCache

Item {

    id: epub_top

    x: (image_top.width-width)/2
    y: (image_top.height-height)/2

    width: view.width
    height: view.height

    // during a short startup period we ignore any changes to the document/window
    property bool startup: true
    Timer {
        interval: 1000
        running: true
        onTriggered:
            startup = false
    }

    // store the currently manually changed window size
    onWidthChanged:
        storePagePos.restart()
    onHeightChanged:
        storePagePos.restart()

    property bool asynchronous: false
    property int paintedWidth: Math.max(view.contentsSize.width, (coverimage.source!=="" ? coverimage.paintedWidth : width))
    property int paintedHeight: Math.max(view.contentsSize.height, (coverimage.source!=="" ? coverimage.paintedHeight : height))

    // where we are at in the e book
    property int documentCount: 1
    property int currentDocument: 0

    // react to changes
    onCurrentDocumentChanged: {
        loadBook()
        storePagePos.restart()
    }

    // the book contents
    // 0: title of book
    // 1: cover image (can be empty)
    // 2-: filepath for documents composing book
    property var book: []

    // some specific cached properties to be loaded after setup
    property real scrollToWhenSetup: -1

    Component.onCompleted: {

        // load book
        book = PQCScripts.loadEPUB(image_top.imageSource)

        // set book title
        toplevel.overrideTitle = book[0]

        // store document count
        documentCount = book.length-2

        // get cached values (if any)
        var cached = PQCCache.getEntry(image_top.imageSource)
        var haveCache = (cached!=="")   // note: *after* split() the length of an array is reported as 1
        cached = cached.split("::")

        // if this book was loaded before at a specific page
        var overrideCurrentDocument = -1
        if(haveCache) {
            overrideCurrentDocument = cached[0]*1
            if(cached.length > 2)
                view.zoomFactor = cached[2]
        }

        // if we have a cover image, show that one (currentDocument := -1)
        if(book[1] !== "") {

            coverimage.source = "data:image/jpeg;base64," + book[1]

            // if overrideCurrentDocument is not set we want -1 which is the value of overrideCurrentDocument if not set
            currentDocument = overrideCurrentDocument

        // if we don't have a cover image (currentDocument := 0)
        } else {

            coverimage.source = ""
            currentDocument = (overrideCurrentDocument===-1 ? 0 : overrideCurrentDocument)

        }

        // load the selected book page
        if(currentDocument !== -1) {
            loadBook()
            if(haveCache && cached.length > 1)
                scrollToWhenSetup = cached[1]
        }
    }

    // store current page and scroll position at exit
    Component.onDestruction: {
        storePagePos.stop()
        storePagePos.triggered()
    }

    // with timeout store updates to page and scroll position
    Timer {
        id: storePagePos
        interval: 250
        onTriggered: {

            // we do not do anything if we're still setting it up (will fire some signals)
            if(startup)
                return

            // store data
            var val = "%1::%2::%3".arg(currentDocument).arg(view.scrollPosition.y / (view.contentsSize.height-view.height)).arg(view.zoomFactor)

            PQCCache.setEntry(image_top.imageSource, val)
        }
    }

    // the view for the actual book content
    WebEngineView {

        id: view

        // hidden when cover image is shown
        visible: currentDocument>-1

        width: image_top.width
        height: image_top.height

        onLoadingChanged: (loadingInfo) => {
            if (loadingInfo.status === WebEngineView.LoadSucceededStatus && url !== "") {

                image.status = Image.Ready

                // this disables user selection of content
                // we do this to help us force the active focus to the focusitem to catch key presses
                runJavaScript("var head = document.head;" +
                              "var node= document.createElement('style');" +
                              "var textnode = document.createTextNode('body {-webkit-user-select: none !important;}');" +
                              "node.appendChild(textnode);head.appendChild(node);")

            } else if(loadingInfo.status === WebEngineView.LoadFailedStatus)

                image.status = Image.Error

        }

        // react to url changes
        onUrlChanged:
            checkUrlChange()

        // load scroll position once content is properly set up
        onContentsSizeChanged: {
            if(view.loadProgress == 100 && contentsSize.height > 0 && !loadInitiScrollWithDelay.running) {
                if(scrollToWhenSetup != -1) {
                    loadInitiScrollWithDelay.restart()
                }
                storePagePos.restart()
            }
        }

        // we add a very short delay after the page has been fully loaded to ensure the contentSize is fully populated
        Timer {
            id: loadInitiScrollWithDelay
            interval: 50
            onTriggered: {
                view.runJavaScript("window.scrollBy(0,%1);".arg(scrollToWhenSetup * (view.contentsSize.height-view.height) / view.zoomFactor));
                scrollToWhenSetup = -1
            }

        }

        // disable context menu
        onContextMenuRequested: (request) => {
            request.accepted = true
        }

        onScrollPositionChanged:
            storePagePos.restart()

        Behavior on zoomFactor { NumberAnimation { duration: 50 } }
        onZoomFactorChanged:
            storePagePos.restart()

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

        id: progressCont

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
            id: changedTimer
            interval: 2000
            onTriggered:
                parent.textHasChanged = false
        }

        // what progress is done
        Text {
            id: progressTxt
            x: 5
            y: 5
            text: Math.round(Math.min(100, 100* (view.scrollPosition.y / (view.contentsSize.height-view.height)))) + "%"
            color: "white"
        }

        Connections {
            target: view
            function onScrollPositionChanged() {
                progressCont.textHasChanged = true
                changedTimer.restart()
                storePagePos.restart()
            }
        }

        // catche mouse events
        MouseArea {
            id: progressInfo
            anchors.fill: parent
            hoverEnabled: true
            ToolTip {
                delay: 500
                text: qsTr("Progress in current chapter")
                visible: parent.containsMouse
            }
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
        color: "#000000"

        opacity: nextmouse.containsMouse||backmouse.containsMouse||plusmouse.containsMouse||minusmouse.containsMouse||currentMouse.containsMouse||bgmouse.containsMouse||docChanged ? 0.8 : 0.1
        Behavior on opacity { NumberAnimation { duration: 200 } }

        property bool docChanged: false
        Timer {
            id: resetDocChanged
            interval: 1000
            onTriggered:
                navcont.docChanged = false
        }
        Connections {
            target: epub_top
            function onCurrentDocumentChanged() {
                navcont.docChanged = true
                resetDocChanged.restart()
            }
        }

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
                        ToolTip {
                            delay: 500
                            text: qsTr("Go to previous chapter")
                            visible: parent.containsMouse
                        }
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
                        ToolTip {
                            delay: 500
                            text: qsTr("Go to next section/chapter")
                            visible: parent.containsMouse
                        }
                    }
                }

            }

            // show what number of document currently is shown
            Text {
                y: (parent.height-height)/2
                color: "white"
                font.bold: true
                text: "%1 / %2".arg(currentDocument+(book[1]==="" ? 1 : 2)).arg(documentCount+(book[1]==="" ? 0 : 1))
                MouseArea {
                    id: currentMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    ToolTip {
                        delay: 500
                        text: qsTr("Current section/chapter")
                        visible: parent.containsMouse
                    }
                }
            }

            // zoom into document
            Rectangle {

                y: (parent.height-height)/2
                width: height
                height: navcont.height*0.8
                color: plusmouse.containsPress ? "#88000000" : (plusmouse.containsMouse ? "#88444444" : "transparent")
                Behavior on color { ColorAnimation { duration: 200 } }
                radius: 5

                Image {
                    x: 3
                    y: 3
                    width: parent.width-6
                    height: parent.height-6
                    sourceSize: Qt.size(width, height)
                    smooth: false
                    source: "image://svg/:/font-plus.svg"
                    MouseArea {
                        id: plusmouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                            view.zoomFactor += 0.1
                        ToolTip {
                            delay: 500
                            text: qsTr("Zoom in")
                            visible: parent.containsMouse
                        }
                    }
                }

            }

            // zoom out of document
            Rectangle {

                y: (parent.height-height)/2
                width: height
                height: navcont.height*0.8
                color: minusmouse.containsPress ? "#88000000" : (minusmouse.containsMouse ? "#88444444" : "transparent")
                Behavior on color { ColorAnimation { duration: 200 } }
                radius: 5

                Image {
                    x: 5
                    y: 5
                    width: parent.width-10
                    height: parent.height-10
                    sourceSize: Qt.size(width, height)
                    smooth: false
                    source: "image://svg/:/font-minus.svg"
                    MouseArea {
                        id: minusmouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                            view.zoomFactor -= 0.1
                        ToolTip {
                            delay: 500
                            text: qsTr("Zoom out")
                            visible: parent.containsMouse
                        }
                    }
                }

            }

        }

    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(keycode === Qt.Key_Left) {

                currentDocument = Math.max((book[1] !== "" ? -1 : 0), currentDocument-1)

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                currentDocument = Math.min(documentCount-1, currentDocument+1)

            } else if(keycode === Qt.Key_Home) {

                view.runJavaScript("window.scrollBy(0,%1);".arg(-view.scrollPosition.y));

            } else if(keycode === Qt.Key_End) {

                view.runJavaScript("window.scrollBy(0,%1);".arg(view.contentsSize.height));

            } else if(keycode === Qt.Key_PageDown) {

                view.runJavaScript("window.scrollBy(0,%1);".arg(view.height*0.9));

            } else if(keycode === Qt.Key_PageUp) {

                view.runJavaScript("window.scrollBy(0,%1);".arg(-view.height*0.9));

            } else if(keycode === Qt.Key_Down) {

                view.runJavaScript("window.scrollBy(0, 20);");

            } else if(keycode === Qt.Key_Up) {

                view.runJavaScript("window.scrollBy(0, -20);");

            } else if(keycode === Qt.Key_Plus || keycode === Qt.Key_Equal) {

                view.zoomFactor += 0.1

            } else if(keycode === Qt.Key_Minus) {

                view.zoomFactor -= 0.1

            } else if(modifiers === Qt.ControlModifier && keycode === Qt.Key_0) {

                view.zoomFactor = 1.0

            }

        }

    }

    // when the url changed we make sure the counter reflects the right file
    // that counter can differ it a document was loaded through a link in a document
    function checkUrlChange() {

        // right url recorded
        // three slashes are needed for this to work both in Linux and Windows
        if(view.url === "file:///" + book[currentDocument+2])
            return

        // the url might contain an anchor -> remove for checking
        var cleanedurl = view.url + ""
        if(cleanedurl.includes("#"))
            cleanedurl = cleanedurl.split("#")[0]

        // figure out correct index
        var ind = currentDocument
        for(var i = 2; i < book.length; ++i) {
            // three slashes are needed for this to work both in Linux and Windows
            if(cleanedurl === "file:///" + book[i]) {
                ind = i-2
                break
            }
        }

        currentDocument = ind

    }

    // load the current document file
    function loadBook() {
        if(currentDocument == -1) return
        // three slashes are needed for this to work both in Linux and Windows
        view.url = "file:///" + book[currentDocument+2]
    }

}
