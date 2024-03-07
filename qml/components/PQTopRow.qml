import QtQuick
import QtQuick.Controls

import PQCScripts
import "../elements"

Rectangle {
    id: toprow
    x: -1
    y: makeVisible ? -1 : -height-2
    Behavior on y { NumberAnimation { duration: 200 } }
    // z: makeVisible ? 1 : 0

    width: parent.width+2
    height: 40
    color: "#aa000000"
    border.width: 1
    border.color: "white"

    property bool makeVisible: false

    Row {
        y: (parent.height-height)/2
        // spacing: 10

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
            }
        }

        PQIconButton {
            id: openwithdefaultbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/external.svg"
            onClicked: {
            }
        }

        PQIconButton {
            id: exitbut
            y: (parent.height-height)/2
            height: toprow.height-1
            source: "/exit.svg"
            onClicked: {
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

        // PQButton {
        //     id: openwithdefaultbut
        //     visible: x+width < toprow.width-50
        //     y: (parent.height-height)/2
        //     height: toprow.height-1
        //     text: "Open in default application"
        //     onClicked: {
        //     }
        // }

        // PQButton {
        //     id: openwithbut
        //     visible: x+width < toprow.width-50
        //     y: (parent.height-height)/2
        //     height: toprow.height-1
        //     text: "Open with..."
        //     onClicked: {
        //     }
        // }

        // Rectangle {
        //     visible: aboutbut.visible
        //     y: (parent.height-height)/2
        //     width: 1
        //     height: parent.height
        //     color: "white"
        // }

        // PQButton {
        //     id: aboutbut
        //     visible: x+width < toprow.width-50
        //     y: (parent.height-height)/2
        //     height: toprow.height-1
        //     text: "About"
        //     onClicked: {
        //     }
        // }


    }

    // PQButton {
    //     id: hiddenmenu
    //     x: (parent.width-width)
    //     y: (parent.height-height)/2
    //     visible: !aboutbut.visible
    //     width: height
    //     height: toprow.height-1
    //     borderLeft: true
    //     borderRight: false
    //     text: ">"
    //     checkable: true
    //     onClicked: overflowmenu.popup(x, y+height)
    // }

    // Timer {
    //     id: resetChecked
    //     interval: 50
    //     onTriggered: {
    //         hiddenmenu.checked = false
    //     }
    // }

    // Menu {
    //     id: overflowmenu

    //     onAboutToShow:
    //         resetChecked.stop()
    //     onAboutToHide:
    //         resetChecked.restart()

    //     MenuItem {
    //         height: visible ? 30 : 0
    //         visible: !openbut.visible
    //         text: "Open file"
    //     }
    //     MenuItem {
    //         height: visible ? 30 : 0
    //         visible: !settingsbut.visible
    //         text: "Settings"
    //     }
    //     MenuItem {
    //         height: visible ? 30 : 0
    //         visible: !openwithdefaultbut.visible
    //         text: "Open in default application"
    //     }
    //     MenuItem {
    //         height: visible ? 30 : 0
    //         visible: !openwithbut.visible
    //         text: "Open with..."
    //     }
    //     MenuItem {
    //         height: visible ? 30 : 0
    //         visible: !aboutbut.visible
    //         text: "About"
    //     }
    // }

}
