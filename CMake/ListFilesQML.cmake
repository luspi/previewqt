set(previewqt_QML "qml/PQMainWindow.qml"

                  "qml/components/PQIconButton.qml"
                  "qml/components/PQImage.qml"
                  "qml/components/PQTopRow.qml"
                  "qml/components/PQTrayIcon.qml"

                  "qml/components/imageitems/PQArchive.qml"
                  "qml/components/imageitems/PQDocument.qml"
                  "qml/components/imageitems/PQImageAnimated.qml"
                  "qml/components/imageitems/PQImageNormal.qml"
                  "qml/components/imageitems/PQSVG.qml"
                  "qml/components/imageitems/PQTxt.qml"

                  "qml/windows/PQAbout.qml"
                  "qml/windows/PQHelp.qml"
                  "qml/windows/PQSettings.qml"
                  "qml/windows/PQWelcome.qml"
)

if(WITH_EPUB)
    set(previewqt_QML ${previewqt_QML} "qml/components/imageitems/PQEPUB.qml")
endif()

if(WITH_PHOTOSPHERE)
    set(previewqt_QML ${previewqt_QML} "qml/components/imageitems/PQPhotoSphere.qml")
endif()

if(WITH_VIDEO_QT)
    set(previewqt_QML ${previewqt_QML} "qml/components/imageitems/PQVideoQt.qml")
endif()

if(WITH_VIDEO_MPV)
    set(previewqt_QML ${previewqt_QML} "qml/components/imageitems/PQVideoMpv.qml")
endif()
