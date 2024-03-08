import QtQuick
import PQCScripts
import PQCSettings

Item {

    id: image_top

    x: 5
    y: (PQCSettings.topBarAutoHide ? 0 : toprow.height)+5
    width: toplevel.width-2*5
    height: toplevel.height-(PQCSettings.topBarAutoHide ? 0 : toprow.height)-2*5
    // height: toplevel.height-2*5

    clip: true

    property string imageSource: ""

    property bool isDocument: PQCScripts.isPDFDocument(imageSource)
    property bool isArchive: !isDocument && PQCScripts.isArchive(imageSource)
    property bool isMpv: !isDocument && !isArchive && PQCScripts.isMpvVideo(imageSource)
    property bool isQtVideo: !isDocument && !isArchive && !isMpv && PQCScripts.isQtVideo(imageSource)
    property bool isAnimated: !isDocument && !isArchive && !isMpv && !isQtVideo && PQCScripts.isItAnimated(imageSource)
    property bool isPhotoSphere: !isDocument && !isArchive && !isMpv && !isQtVideo && !isAnimated && PQCScripts.isPhotoSphere(imageSource)

    // these are used for a delay in reloading the image
    property int windowWidth: 200
    property int windowHeight: 200

    signal keyPress(var modifiers, var keycode)
    signal doubleClick()

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
        cursorShape: image.imageSource == "" ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: (mouse) => {
            if(image.imageSource == "")
                fileDialog.open()
        }
        onPositionChanged: (mouse) => {
            if(mouse.y < 30)
                toprow.makeVisible = true
            else
                toprow.makeVisible = false
        }

        onDoubleClicked: {
            image_top.doubleClick()
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
                                    (isPhotoSphere ?
                                         "PQPhotoSphere.qml" :
                                         "PQImageNormal.qml")))))

        source: "imageitems/" + nameOfImage

    }

    Component.onCompleted: {
        windowWidth = image_top.width
        windowHeight = image_top.height
    }

    Connections {
        target: toplevel
        function onKeyPress(modifiers, keycode) {
            image_top.keyPress(modifiers, keycode)
        }
    }

    // show the image
    function showImage() {
        opacity = 1
    }

    function loadImage(path) {

        isPhotoSphere = PQCScripts.isPhotoSphere(path)
        imageSource = PQCScripts.cleanPath(path)
    }

}
