import QtQuick
import QtQuick.Controls

import PQCSettings
import PQCScripts
import "../elements"

Rectangle {
    id: toprow
    x: -1
    y: makeVisible||!PQCSettings.topBarAutoHide ? -1 : -height-2
    Behavior on y { NumberAnimation { duration: 200 } }

    width: parent.width+2
    height: 40
    color: "#aa000000"
    border.width: 1
    border.color: "white"

    property bool makeVisible: false

    Row {
        y: (parent.height-height)/2

        PQIconButton {
            id: openbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/open.svg"
            tooltip: "Open a file"
            onClicked: {
                fileDialog.open()
            }
        }

        PQIconButton {
            id: settingsbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/settings.svg"
            onClicked: {
                settings.show()
            }
        }

        PQIconButton {
            id: openwithdefaultbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/external.svg"
            onClicked: {
                PQCScripts.openInDefault(image.imageSource)
                if(PQCSettings.closeAfterDefaultApp)
                    toplevel.close()
            }
        }

    }

    Row {

        x: (parent.width-width)

        PQIconButton {
            id: exitbut
            y: (parent.height-height)/2
            borderLeft: true
            height: toprow.height-1
            source: "/exit.svg"
            onClicked: {
                toplevel.close()
            }
        }

        PQIconButton {
            id: aboutbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/about.svg"
            onClicked: {
            }
        }

    }

}
