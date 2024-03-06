import QtQuick
import QtQuick.Controls
import PQCScripts

Window {

    id: toplevel

    width: 640
    height: 480
    visible: true
    title: "ViewQt"

    color: "#bb000000"

    Button {
        id: openbut
        anchors.centerIn: parent
        font.pointSize: 12
        text: "Open a file"
        visible: false
        onClicked: {
            var file = PQCScripts.openFile()
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
                toplevel.title = PQCScripts.getFilename(source) + " | ViewQt"
        }
    }

    Component.onCompleted: {

        if(Qt.application.arguments.length > 1 && PQCScripts.fileExists(Qt.application.arguments[1]))
            image.source = "image://full/" + PQCScripts.cleanPath(Qt.application.arguments[1])
        else
            openbut.visible = true
    }

}
