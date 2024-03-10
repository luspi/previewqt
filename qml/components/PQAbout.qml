import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window
import PQCSettings
import PQCScripts

Window {

    id: about_top

    // some stylings
    title: "About"
    visible: false

    // in this window, this item catches all key presses
    Item {
        id: catchKeyPress
        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape || event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
                about_top.close()
        }
    }

    Flickable {

        id: flickable

        anchors.fill: parent
        anchors.margins: 5
        anchors.bottomMargin: 40
        clip: true

        contentHeight: contcol.height

        Column {

            id: contcol

            spacing: 10

            width: parent.width

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: "About PreviewQt"
                font.pointSize: 20
                font.bold: true
            }

            Image {

                x: (parent.width-width)/2
                width: height
                height: Math.min(flickable.width/4, 200)
                sourceSize: Qt.size(width, height)

                source: "/logo.svg"

            }

            Row {

                spacing: 5

                Column {

                    spacing: 5

                    width: (12*flickable.width)/25

                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignRight
                        text: "License:"
                    }
                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignRight
                        text: "Website:"
                    }
                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignRight
                        text: "Developed by:"
                    }
                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignRight
                        text: "Contact:"
                    }

                }

                Column {

                    spacing: 5

                    width: (13*flickable.width)/25

                    Text {
                        width: parent.width
                        text: "GPL v2+"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://opensource.org/license/gpl-2-0")
                        }
                    }
                    Text {
                        width: parent.width
                        text: "https://photoqt.org"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://photoqt.org")
                        }
                    }
                    Text {
                        width: parent.width
                        text: "Lukas Spies"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://luspi.de")
                        }
                    }
                    Text {
                        width: parent.width
                        text: "Lukas@photoqt.org"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("mailto:Lukas@photoqt.org")
                        }
                    }

                }

            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: "PreviewQt is a lightweight application allowing for quickly previewing a wide range of files, from images and videos to documents and archives. It supports the same file types as supported by PhotoQt and uses some of the same code behind the scenes."

            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: "If you like PreviewQt but would like to have a more featured viewer along the same lines, check out the full-featured viewer application PhotoQt."

            }

        }
    }

    // when closing we re-focus on main key catcher
    onClosing: {
        focusitem.forceActiveFocus()
    }

    function show() {
        catchKeyPress.forceActiveFocus()
        visible = true
    }


    Rectangle {
        x: 0
        y: parent.height-40
        width: parent.width
        height: 1
        color: "black"
    }

    Button {
        x: (parent.width-width)/2
        y: parent.height-40 + (40-height)/2
        text: "Close"
        onClicked:
            about_top.close()

    }

}
