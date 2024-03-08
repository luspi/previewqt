import QtQuick
import Qt.labs.platform
import PQCSettings

SystemTrayIcon {

    visible: PQCSettings.hideToSystemTray
    icon.source: "qrc:/logo.svg"

    onActivated: {
        toplevel.show()
        toplevel.raise()
        toplevel.requestActivate()
    }

    menu: Menu {
        visible: false
        MenuItem {
            text: toplevel.visible ? "Hide window" : "Show window"
            onTriggered: {
                toplevel.visible = !toplevel.visible
            }
        }
        MenuItem {
            text: "Quit PreviewQt"
            onTriggered:
                Qt.quit()
        }
    }

 }
