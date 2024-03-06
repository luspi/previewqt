import QtQuick
import QtQuick.Controls
import VQCScripts

Window {

    id: toplevel

    width: 640
    height: 480
    visible: true
    title: "ViewQt"

    color: "#bb000000"

    Button {
        anchors.centerIn: parent
        font.pointSize: 12
        text: "Open a file"
        onClicked: {
            var file = VQCScripts.openFile()
            if(file !== "")
                image.source = "image://full/" + file
        }
    }

    Image {
        id: image
        asynchronous: true
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size(width, height)
        smooth: false
        mipmap: false
        source: ""
        onSourceChanged: {
            if(source === "")
                toplevel.title = "ViewQt"
            else
                toplevel.title = VQCScripts.getFilename(source) + " | ViewQt"
        }
    }

    Component.onCompleted: {

        if(Qt.application.arguments.length > 1 && VQCScripts.fileExists(Qt.application.arguments[1]))
            image.source = "image://full/" + VQCScripts.cleanPath(Qt.application.arguments[1])
    }

}
