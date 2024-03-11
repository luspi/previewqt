import QtQuick
import QtQuick.Controls
import PQCSettings

Window {

    id: help_top

    // some stylings
    //: window title
    title: qsTr("Help")
    visible: false
    modality: Qt.ApplicationModal

    // in this window, this item catches all key presses
    Item {
        id: catchKeyPress
        Keys.onPressed: (event) => {
            if(event.key === Qt.Key_Escape || event.key === Qt.Key_Return || event.key === Qt.Key_Enter)
                help_top.close()
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
                text: qsTr("Help")
                font.pointSize: 20
                font.bold: true
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                //: Please keep the html tags for bold text in place as they help with parsing this block of text
                text: qsTr("<b>In the main application window</b>, combining the Control modifier with either of O, P, or I launches the action to open a file, the settings, or the About information, respectively. F1 opens this help message, Escape hides the application, and Ctrl+Q quits it altogether. Double clicking on a loaded image toggles the fullscreen mode. Then there exists a customizable shortcut for opening a loaded file in an external application.")
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                //: Please keep the html tags for bold text in place as they help with parsing this block of text
                text: qsTr("<b>In the settings window</b>, Escape, Enter, and Return all close the window, and Ctrl+Tab switches between the two tabs.")
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                //: Please keep the html tags for bold text in place as they help with parsing this block of text
                text: qsTr("<b>In both the Help and About window</b>, either one of Escape, Enter, and Return close the window.")
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                //: Please keep the html tags for bold text in place as they help with parsing this block of text
                text: qsTr("When <b>archives and documents</b> are loaded, the arrow keys allow switching between pages/files, and Home/End go to the beginning/end of the loaded document. Similarly, when <b>video files</b> are loaded, the space bar toggles the play status, the arrow keys jump left/right in the video file, and Home/End go to the start and end of the video.")
            }

            Column {

                width: parent.width

                Text {
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    //: Please keep the html tags for bold text in place as they help with parsing this block of text
                    text: qsTr("<b>Need more help?</b> Then check out the website for PreviewQt:")
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                    text: "https://photoqt.org/previewqt"
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                            Qt.openUrlExternally("https://photoqt.org/previewqt")
                    }
                }

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
            help_top.close()

    }

}
