import QtQuick

Item {

    id: image_top

    x: 5
    y: 5
    // y: (toprow.y > -5 ? toprow.height : 0)+5
    width: toplevel.width-2*5
    // height: toplevel.height-toprow.height-2*5
    height: toplevel.height-2*5

    clip: true

    property string imageSource: ""

    property bool isDocument: false//PQCScriptsImages.isPDFDocument(deleg.imageSource)
    property bool isArchive: false//!isDocument && PQCScriptsImages.isArchive(deleg.imageSource)
    property bool isMpv: false//!isDocument && !isArchive && PQCScriptsImages.isMpvVideo(deleg.imageSource)
    property bool isQtVideo: false//!isDocument && !isArchive && !isMpv && PQCScriptsImages.isQtVideo(deleg.imageSource)
    property bool isAnimated: false//!isDocument && !isArchive && !isMpv && !isQtVideo && PQCScriptsImages.isItAnimated(deleg.imageSource)

    // these are used for a delay in reloading the image
    property int windowWidth: 200
    property int windowHeight: 200

    Timer {
        id: updateWindowSize
        interval: 500
        onTriggered: {
            windowWidth = image_top.width
            windowHeight = image_top.height
        }
    }

    Connections {
        target: image_top
        function onWidthChanged(width) {
            updateWindowSize.restart()
        }
        function onHeightChanged(height) {
            updateWindowSize.restart()
        }
    }

    MouseArea {
        id: imagemouse
        anchors.fill: parent
        anchors.margins: -5
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: (mouse) => {
            fileDialog.open()
        }
        onPositionChanged: (mouse) => {
            if(mouse.y < 30)
                toprow.makeVisible = true
            else
                toprow.makeVisible = false
        }
    }

    // the actual image
    Loader {

        property string nameOfImage:
            isMpv ?
                "PQVideoMpv.qml" :
                (isQtVideo ?
                     "PQVideoQt.qml" :
                     (isAnimated ?
                          "PQImageAnimated.qml" :
                          (isDocument ?
                               "PQDocument.qml" :
                               (isArchive ?
                                    "PQArchive.qml" :
                                    "PQImageNormal.qml"))))

        source: "imageitems/" + nameOfImage

    }

    Component.onCompleted: {
        windowWidth = image_top.width
        windowHeight = image_top.height
    }

    // show the image
    function showImage() {
        opacity = 1
    }

}
