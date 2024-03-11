import QtQuick
import QtQuick.Controls

import PQCSettings
import PQCScripts

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
            height: toprow.height-2
            source: "/open.svg"
            tooltip: qsTr("Open a file")
            onClicked: {
                fileDialog.open()
            }
        }

        // open settings
        PQIconButton {
            id: settingsbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "/settings.svg"
            tooltip: qsTr("Open settings")
            onClicked: {
                settings.active = true
                settings.item.show()
            }
        }

        // open in external/default app
        PQIconButton {
            id: openwithdefaultbut
            y: (parent.height-height)/2
            height: toprow.height-2
            tooltip: qsTr("Open in external application")
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

        // close window
        PQIconButton {
            id: exitbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "/exit.svg"
            tooltip: qsTr("Quit PreviewQt")
            onClicked: {
                Qt.quit()
            }
        }

    }

    Row {

        x: (parent.width-width)

        // open about window
        PQIconButton {
            id: aboutbut
            y: (parent.height-height)/2
            borderLeft: true
            height: toprow.height-2
            source: "/about.svg"
            tooltip: qsTr("About PreviewQt")
            onClicked: {
                about.active = true
                about.item.show()
            }
        }

        // open help window
        PQIconButton {
            id: helpbut
            y: (parent.height-height)/2
            height: toprow.height-2
            source: "/help.svg"
            tooltip: qsTr("Help")
            onClicked: {
                help.active = true
                help.item.show()
            }
        }

    }

}
