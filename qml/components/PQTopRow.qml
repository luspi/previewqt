import QtQuick
import QtQuick.Controls

import PQCSettings
import PQCScripts
import "../elements"

Rectangle {

    id: toprow

    // location of top bar
    x: -1
    y: toplevel.toprowMakeVisible||!PQCSettings.topBarAutoHide ? -1 : -height-2
    Behavior on y { NumberAnimation { duration: 200 } }

    // some stylings
    width: toplevel.width+2
    height: 40
    color: "#aa000000"
    border.width: 1
    border.color: "white"

    // left part of the bar
    Row {

        y: (parent.height-height)/2

        // open a file
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

        // open settings
        PQIconButton {
            id: settingsbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/settings.svg"
            onClicked: {
                settings.active = true
                settings.item.show()
            }
        }

        // open in external/default app
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

        // close window
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

        // open about window
        PQIconButton {
            id: aboutbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/about.svg"
            onClicked: {
                about.active = true
                about.item.show()
            }
        }

    }

}
