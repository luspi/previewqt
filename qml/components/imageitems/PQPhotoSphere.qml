import QtQuick
import QtQuick3D
import PreviewQt

Item {

    id: sphere_top

    /*******************************************/

    property real azimuth: 180
    property real elevation: 0
    property real fieldOfView: 90

    width: PQCConstants.imageAvailableSize.width
    height: PQCConstants.imageAvailableSize.height

    // these need to have a small duration as otherwise touchpad handling is awkward
    // key events are handled with their own animations below
    Behavior on fieldOfView {
        NumberAnimation {
            id: behavior_fov
            duration: sphere_top.aniDuration
        }
    }
    Behavior on azimuth {
        NumberAnimation {
            id: behavior_az
            duration: sphere_top.aniDuration
        }
    }
    Behavior on elevation {
        NumberAnimation {
            id: behavior_ele
            duration: sphere_top.aniDuration
        }
    }

    property int aniDuration: 0
    property int aniSpeed: 30
    property bool animationRunning: false
    property int aniDirection: -1

    View3D {

        anchors.fill: parent

        environment: SceneEnvironment {
            backgroundMode: SceneEnvironment.Color
            clearColor: "black"
        }

        Node {
            id: cameraRig

            eulerRotation.x: sphere_top.elevation
            eulerRotation.y: sphere_top.azimuth

            PerspectiveCamera {
                id: camera
                position: Qt.vector3d(0, 0, 0)
                fieldOfView: sphere_top.fieldOfView
            }
        }

        Model {

            source: "#Sphere"

            scale: Qt.vector3d(-100, 100, 100)

            materials: PrincipledMaterial {
                lighting: PrincipledMaterial.NoLighting
                cullMode: Material.NoCulling

                baseColorMap: Texture {
                    source: "file:/" + PQCScriptsImages.prepareSphereFile(PQCConstants.currentSource)
                }
            }
        }

    }

    PinchArea {

        anchors.fill: parent

        property real startFov

        onPinchStarted: startFov = sphere_top.fieldOfView

        onPinchUpdated: pinch => {
            sphere_top.fieldOfView = Math.max(3, Math.min(150, startFov / pinch.scale))
        }

        MouseArea {

            id: mousearea

            anchors.fill: parent

            property point startPos
            property real startAz
            property real startEl

            onPressed: mouse => {
                startPos = Qt.point(mouse.x, mouse.y)
                startAz = sphere_top.azimuth
                startEl = sphere_top.elevation
            }

            onDoubleClicked: (mouse) => {
                if(mouse.button === Qt.RightButton) return
                if(PQCConstants.mainwindowIsFullscreen)
                    PQCNotify.mainwindowShowNormal()
                else
                    PQCNotify.mainwindowShowFullscreen()
            }

            onPositionChanged: mouse => {
                sphere_top.azimuth = startAz + (mouse.x - startPos.x) * 0.1
                sphere_top.elevation = Math.max(-90, Math.min(90, startEl + (mouse.y - startPos.y) * 0.1))
            }

            onWheel: (wheel) => {
                if(wheel.angleDelta.y < 0)
                    sphere_top.zoom("out")
                else
                    sphere_top.zoom("in")
            }

        }

    }

    onVisibleChanged: {

        if(!panOnCompleted.running)
            panOnCompleted.start()

    }


    Component.onCompleted: {

        sphere_top.aniDuration = 50

        if(!panOnCompleted.running)
            panOnCompleted.start()

        PQCConstants.imagePaintedSize = Qt.binding(function() { return Qt.size(sphere_top.width, sphere_top.height) })
        PQCConstants.imageStatus = Image.Ready
    }

    // these are not handled with the behavior above because key events are handled smoother than mouse events
    NumberAnimation {
        id: animatedAzimuth
        target: sphere_top
        property: "azimuth"
        duration: 200
        onRunningChanged: {
            if(!running) {
                animatedAzimuth.easing.type = Easing.Linear
                animatedAzimuth.duration = 200
            }
        }
    }
    NumberAnimation {
        id: animatedElevation
        target: sphere_top
        property: "elevation"
        duration: 200
        onRunningChanged: {
            if(!running) {
                animatedElevation.easing.type = Easing.Linear
                animatedElevation.duration = 200
            }
        }
    }
    NumberAnimation {
        id: animatedFieldOfView
        target: sphere_top
        property: "fieldOfView"
        duration: 200
    }

    Connections {

        target: PQCNotify

        function onMainwindowKeyPress(modifiers : int, keycode : int) {

            if((modifiers === Qt.ControlModifier && keycode === Qt.Key_0) ||
                    keycode === Qt.Key_0) {

                sphere_top.zoom("reset")
                sphere_top.moveView("reset")

            } else if(modifiers === Qt.NoModifier) {

                if(keycode === Qt.Key_Left)
                    sphere_top.moveView("left")
                else if(keycode === Qt.Key_Right)
                    sphere_top.moveView("right")
                else if(keycode === Qt.Key_Up)
                    sphere_top.moveView("up")
                else if(keycode === Qt.Key_Down)
                    sphere_top.moveView("down")

            }

        }

    }

    // these are not handled with the behavior above because key events are handled smoother than mouse events
    function zoom(dir : string) {

        leftrightani.stop()

        animatedFieldOfView.stop()

        if(dir === "in") {
            animatedFieldOfView.from = sphere_top.fieldOfView
            animatedFieldOfView.to = Math.max(3, Math.min(150, sphere_top.fieldOfView-20))
        } else if(dir === "out") {
            animatedFieldOfView.from = sphere_top.fieldOfView
            animatedFieldOfView.to = Math.max(3, Math.min(150, sphere_top.fieldOfView+20))
        } else if(dir === "reset") {
            animatedFieldOfView.from = sphere_top.fieldOfView
            animatedFieldOfView.to = 90
        }

        animatedFieldOfView.restart()

    }

    // these are not handled with the behavior above because key events are handled smoother than mouse events
    function moveView(dir : string) {

        leftrightani.stop()

        if(dir === "up" || dir === "down" || dir === "reset")
            animatedElevation.stop()
        if(dir === "left" || dir === "right" || dir === "reset")
            animatedAzimuth.stop()

        if(dir === "up") {
            animatedElevation.from = sphere_top.elevation
            animatedElevation.to = Math.max(-90, Math.min(90, sphere_top.elevation + sphere_top.fieldOfView/5))
        } else if(dir === "down") {
            animatedElevation.from = sphere_top.elevation
            animatedElevation.to = Math.max(-90, Math.min(90, sphere_top.elevation - sphere_top.fieldOfView/5))
        } else if(dir === "left") {
            animatedAzimuth.from = sphere_top.azimuth
            animatedAzimuth.to = sphere_top.azimuth + sphere_top.fieldOfView/3
        } else if(dir === "right") {
            animatedAzimuth.from = sphere_top.azimuth
            animatedAzimuth.to = sphere_top.azimuth - sphere_top.fieldOfView/3
        } else if(dir === "reset") {
            animatedElevation.from = sphere_top.elevation
            animatedElevation.to = 0
            animatedElevation.easing.type = Easing.OutBack
            animatedElevation.duration = 500
            animatedAzimuth.from = sphere_top.azimuth
            animatedAzimuth.to = 180
            animatedAzimuth.easing.type = Easing.OutBack
            animatedAzimuth.duration = 500
        }

        if(dir === "up" || dir === "down" || dir === "reset")
            animatedElevation.restart()
        if(dir === "left" || dir === "right" || dir === "reset")
            animatedAzimuth.restart()

    }

    // Animation: to left
    SequentialAnimation {

        id: kb_left

        loops: Animation.Infinite
        running: sphere_top.animationRunning

        // animate from middle to the left
        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 180
            to: 0
            duration: Math.abs(from-to)*sphere_top.aniSpeed
        }

        // animate to the right
        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 0
            to: 360
            duration: Math.abs(from-to)*sphere_top.aniSpeed
        }

        // animate to the middle
        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 360
            to: 180
            duration: Math.abs(from-to)*sphere_top.aniSpeed
        }

    }

    // Animation: to right
    SequentialAnimation {

        id: kb_right

        loops: Animation.Infinite
        running: false

        // animate from middle to the right
        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 180
            to: 360
            duration: Math.abs(from-to)*sphere_top.aniSpeed
        }

        // animate to the left
        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 360
            to: 0
            duration: Math.abs(from-to)*sphere_top.aniSpeed
        }

        // animate to the middle
        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 0
            to: 180
            duration: Math.abs(from-to)*sphere_top.aniSpeed
        }

    }

    // This is a short animation to the right and back
    // This is used when a photo sphere has been entered to inform the user that there is more to the image than what they can see
    // The timer below is called from Component.onCompleted above

    SequentialAnimation {

        id: leftrightani

        loops: 1

        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 180
            to: 190
            duration: 500
            easing.type: Easing.OutCirc
        }

        NumberAnimation {
            target: sphere_top
            property: "azimuth"
            from: 190
            to: 180
            duration: 500
            easing.type: Easing.OutBack
        }

    }

    Timer {
        id: panOnCompleted
        interval: 0
        onTriggered: {
            if(!mousearea.pressed)
                leftrightani.start()
        }
    }

}
