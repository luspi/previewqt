import QtQuick
import QtQuick.Controls
import PQCScripts

Image {

    id: image

    source: ""

    Component.onCompleted: {
        if(image_top.imageSource.includes("::PDF::"))
            source = "image://full/" + PQCScripts.toPercentEncoding(image_top.imageSource)
        else
            source = "image://full/" + PQCScripts.toPercentEncoding("%1::PDF::%2".arg(currentPage).arg(image_top.imageSource))
    }

    asynchronous: true

    fillMode: Image.PreserveAspectFit

    smooth: false
    mipmap: false

    width: image_top.width
    height: image_top.height
    sourceSize: Qt.size(image_top.windowWidth, image_top.windowHeight)

    onStatusChanged: {
        if(status == Image.Error)
            source = "image://svg/:/other/errorimage.svg"
    }

    property int currentPage: 0
    property int pageCount: PQCScripts.getDocumentPageCount(image_top.imageSource)

    onCurrentPageChanged: {
        image.asynchronous = false
        if(image_top.imageSource.includes("::PDF::")) {
            image.source = "image://full/" + PQCScripts.toPercentEncoding("%1::PDF::%2".arg(image.currentPage).arg(image_top.imageSource.split("::PDF::")[1]))
        } else {
            image.source = "image://full/" + PQCScripts.toPercentEncoding("%1::PDF::%2".arg(image.currentPage).arg(image_top.imageSource))
        }
        image.asynchronous = true
    }

    Rectangle {

        parent: image_top

        id: controlitem

        x: (parent.width-width)/2
        y: Math.max(Math.min(0.9*parent.height, parent.height-height-10), parent.height-100)
        width: controlrow.width+20
        height: 40
        radius: 5
        color: "#88000000"

        // only show when needed
        opacity: image.pageCount>1 ? (hovered ? 1 : 0.3) : 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        visible: opacity>0
        enabled: visible

        // the first property is set by PCNotify signals for everything else not caught with the elements below
        property bool emptyAreaHovered: false
        property bool hovered: emptyAreaHovered||mouseprev.containsMouse||mousenext.containsMouse||
                               mousefirst.containsMouse||mouselast.containsMouse||bgmouse.containsMouse||
                               pagemouse1.containsMouse||pagemouse2.containsMouse||pagenumberspin.hovered

        MouseArea {
            id: bgmouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {}
        }

        Row {

            id: controlrow

            x: 10
            y: (parent.height-height)/2
            spacing: 10

            Row {
                y: (parent.height-height)/2

                Image {
                    y: (parent.height-height)/2
                    width: height
                    height: controlitem.height/2.5
                    sourceSize: Qt.size(width, height)
                    source: "/first.svg"
                    MouseArea {
                        id: mousefirst
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: currentPage = 0
                    }
                }

                Image {
                    y: (parent.height-height)/2
                    width: height
                    height: controlitem.height/1.5
                    sourceSize: Qt.size(width, height)
                    source: "/backwards.svg"
                    MouseArea {
                        id: mouseprev
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: currentPage = (currentPage+pageCount-1)%pageCount
                    }
                }

                Image {
                    y: (parent.height-height)/2
                    width: height
                    height: controlitem.height/1.5
                    sourceSize: Qt.size(width, height)
                    source: "/forwards.svg"
                    MouseArea {
                        id: mousenext
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: currentPage = (currentPage+1)%pageCount
                    }
                }

                Image {
                    y: (parent.height-height)/2
                    width: height
                    height: controlitem.height/2.5
                    sourceSize: Qt.size(width, height)
                    source: "/last.svg"
                    MouseArea {
                        id: mouselast
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: currentPage = pageCount-1
                    }
                }

            }

            Rectangle {
                y: (parent.height-height)/2
                height: controlitem.height*0.75
                width: 1
                color: "white"
            }

            Item {

                y: (parent.height-height)/2
                width: childrenRect.width
                height: Math.max(pagenumberrow.height, pagenumberspin.height)

                Row {

                    id: pagenumberrow
                    y: (parent.height-height)/2

                    spacing: 5

                    Text {

                        id: pagetitletxt

                        y: (parent.height-height)/2
                        text: qsTranslate("image", "Page")
                        color: "white"

                        MouseArea {
                            id: pagemouse1
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: pagenumberrow.visible = false
                        }
                    }

                    Text {

                        id: pagenumbertxt

                        y: (parent.height-height)/2
                        text: "%1/%2".arg(image.currentPage+1).arg(image.pageCount)
                        color: "white"

                        MouseArea {
                            id: pagemouse2
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: pagenumberrow.visible = false
                        }
                    }
                }

                SpinBox {
                    id: pagenumberspin
                    visible: !pagenumberrow.visible
                    width: visible ? pagenumbertxt.width+pagetitletxt.width+5 : 0
                    height: controlitem.height-10

                    from: 1
                    to: pageCount

                    onVisibleChanged: {
                        if(visible) {
                            pagenumberspin.forceActiveFocus()
                            value = currentPage+1
                        } else
                            focusitem.forceActiveFocus()
                    }

                    Keys.onPressed: (event) => {
                        if(event.key === Qt.Key_Escape)
                            pagenumberrow.visible = true
                        else if(event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                            currentPage = pagenumberspin.value-1
                            pagenumberrow.visible = true
                        }

                    }
                }

            }

        }

    }

    Connections {

        target: toplevel

        function onKeyPress(modifiers, keycode) {

            if(modifiers !== Qt.NoModifier)
                return

            if(keycode === Qt.Key_Left) {

                currentPage = (currentPage+pageCount-1)%pageCount

            } else if(keycode === Qt.Key_Right || keycode === Qt.Key_Space) {

                currentPage = (currentPage+1)%pageCount

            } else if(keycode === Qt.Key_Home) {

                currentPage = 0

            } else if(keycode === Qt.Key_End) {

                currentPage = pageCount-1

            }

        }

    }


}
