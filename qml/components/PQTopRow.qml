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
    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.InQuint } }

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
            tooltip: "Open settings"
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
            tooltip: "Open in external application"
            source: "/external.svg"
            onClicked: {
                if(image.imageSource === "") return
                if(PQCScripts.openInDefault(image.imageSource)) {
                    if(PQCSettings.closeAfterDefaultApp)
                        toplevel.close()
                } else
                    extNotSet.open()
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
            tooltip: "Quit PreviewQt"
            onClicked: {
                Qt.quit()
            }
        }

        // open about window
        PQIconButton {
            id: aboutbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/about.svg"
            tooltip: "About PreviewQt"
            onClicked: {
                about.active = true
                about.item.show()
            }
        }

    }

}
