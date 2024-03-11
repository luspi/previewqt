import QtQuick
import Qt.labs.platform
import PQCSettings

SystemTrayIcon {

    // style tray icon
    visible: true
    icon.source: "qrc:/logo.svg"

    // show/hide application window
    onActivated: {
        if(toplevel.visible)
            toplevel.close()
        else {
            toplevel.show()
            toplevel.raise()
            toplevel.requestActivate()
        }
    }

    // the context menu
    menu: Menu {
        visible: false
        MenuItem {
            text: toplevel.visible ? qsTr("Hide window") : qsTr("Show window")
            onTriggered: {
                toplevel.visible = !toplevel.visible
            }
        }
        MenuItem {
            text: qsTr("Quit PreviewQt")
            onTriggered:
                Qt.quit()
        }
    }

    // check if a message is to be shown once set up
    Component.onCompleted: {
        if(toplevel.messageWhenReady[0] !== "")
            showMessage(toplevel.messageWhenReady[0], toplevel.messageWhenReady[1], SystemTrayIcon.Information, 5000)
    }

 }
