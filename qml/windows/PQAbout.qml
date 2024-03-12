import QtCore
import QtQuick
import QtQuick.Controls
import PQCScripts

Window {

    id: about_top

    //: window title
    title: qsTr("About")

    // some stylings
    visible: false
    modality: Qt.ApplicationModal
    width: 500
    height: 500

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
                text: qsTr("About")
                font.pointSize: 25
                font.bold: true
            }

            Image {

                x: (parent.width-width)/2
                width: height
                height: Math.min(flickable.width/4, 200)
                sourceSize: Qt.size(width, height)

                source: "image://svg/:/logo.svg"

            }

            Row {

                x: (parent.width-width)/2
                spacing: 5

                Column {

                    spacing: 5

                    Text {
                        font.bold: true
                        font.pointSize: 11
                        text: qsTr("Version:")
                    }
                    Text {
                        font.bold: true
                        font.pointSize: 11
                        text: qsTr("License:")
                    }
                    Text {
                        font.bold: true
                        font.pointSize: 11
                        text: qsTr("Website:")
                    }
                    Text {
                        font.bold: true
                        font.pointSize: 11
                        text: qsTr("Developer:")
                    }
                    Text {
                        font.bold: true
                        font.pointSize: 11
                        text: qsTr("Contact:")
                    }

                }

                Column {

                    spacing: 5

                    Text {
                        font.pointSize: 12
                        text: "PreviewQt v" + PQCScripts.getVersion()
                    }

                    Text {
                        font.pointSize: 12
                        text: "GPL v2+"
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://opensource.org/license/gpl-2-0")
                        }
                    }
                    Text {
                        text: "https://photoqt.org/PreviewQt"
                        font.pointSize: 12
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://photoqt.org/PreviewQt")
                        }
                    }
                    Text {
                        text: "Lukas Spies"
                        font.pointSize: 12
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: Qt.openUrlExternally("https://luspi.de")
                        }
                    }
                    Text {
                        text: "Lukas@photoqt.org"
                        font.pointSize: 12
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

                text: qsTr("PreviewQt is a lightweight application allowing for quickly previewing a wide range of files, from images and videos to documents and archives. It supports the same file types as supported by PhotoQt and uses some of the same code behind the scenes.")

            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: qsTr("If you like PreviewQt but would like to have a more featured viewer along the same lines, check out the full-featured viewer application PhotoQt.")

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
        y: parent.height-45
        width: parent.width
        height: 1
        color: "black"
    }

    Button {
        x: (parent.width-width)/2
        y: parent.height-45 + (45-height)/2
        text: qsTr("Close")
        onClicked:
            about_top.close()

    }

}
