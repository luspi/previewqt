import QtCore
import QtQuick
import QtQuick.Controls

Window {

    id: welcome_top

    //: window title
    title: qsTr("Welcome to PreviewQt")

    // some stylings
    visible: false
    modality: Qt.ApplicationModal

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
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.bold: true
                text: qsTr("PreviewQt is a simple application for previewing all kinds of files.")
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("It is optimized for speed and lightweight and thus does not have all the features one might expect from an image/document viewer. You can always pass on a file to an external application through a button and shortcut. The selection of external applications can be adjusted freely.")
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("When not in use, PreviewQt sits idle in the system tray, ready to be shown as instanteneously as possible.")
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.bold: true
                text: qsTr("Questions? Feedback? Let me know:") + " Lukas@photoqt.org"
            }

            Text {
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("Also check out the website:") + " https://photoqt.org/PreviewQt"
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
        text: qsTr("Get started")
        onClicked:
            welcome_top.close()

    }

}
