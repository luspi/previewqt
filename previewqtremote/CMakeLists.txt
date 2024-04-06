################################################################
# CMakeLists for PreviewQtRemote: http://photoqt.org/previewqt #
################################################################

cmake_minimum_required(VERSION 3.16)
project(previewqtremote VERSION 0.2 LANGUAGES CXX)

########################################
####  SOME SETTINGS/FLAGS FOR CMAKE ####
########################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

################################
#### FIND REQUIRED PACKAGES ####
################################

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick Widgets)

#############################
#### Add the executeable ####
#############################

if(WIN32)
    qt_add_executable(previewqtremote main.cpp scripts.cpp windowsicons.rc)
else()
    qt_add_executable(previewqtremote main.cpp scripts.cpp)
endif()

# add qml files
qt_add_qml_module(previewqtremote URI src VERSION 1.0 QML_FILES Main.qml RESOURCE_PREFIX)

# set some properties for executable
set_target_properties(previewqtremote PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# link executable
target_link_libraries(previewqtremote PRIVATE Qt6::Quick Qt6::Widgets)

##################################
#### COMPOSE THE DESKTOP FILE ####
##################################

include("CMake/ComposeDesktopFile.cmake")
composeDesktopFile()

#######################
#### INSTALL FILES ####
#######################

if(UNIX)

    install(
        TARGETS previewqtremote
        DESTINATION bin/
    )
    install(
        FILES org.previewqt.PreviewQtRemote.desktop
        DESTINATION share/applications
    )
    install(
        FILES icons/32x32/org.previewqt.PreviewQtRemote.png
        DESTINATION share/icons/hicolor/32x32/apps/
    )
    install(
        FILES icons/48x48/org.previewqt.PreviewQtRemote.png
        DESTINATION share/icons/hicolor/48x48/apps/
    )
    install(
        FILES icons/64x64/org.previewqt.PreviewQtRemote.png
        DESTINATION share/icons/hicolor/64x64/apps/
    )
    install(
        FILES icons/128x128/org.previewqt.PreviewQtRemote.png
        DESTINATION share/icons/hicolor/128x128/apps/
    )
    install(
        FILES icons/256x256/org.previewqt.PreviewQtRemote.png
        DESTINATION share/icons/hicolor/256x256/apps/
    )
    install(
        FILES icons/512x512/org.previewqt.PreviewQtRemote.png
        DESTINATION share/icons/hicolor/512x512/apps/
    )
    if(NOT FLATPAKBUILD)
        install(
            FILES icons/1024x1024/org.previewqt.PreviewQtRemote.png
            DESTINATION share/icons/hicolor/1024x1024/apps/
        )
    endif()
    install(
        FILES org.previewqt.PreviewQtRemote.metainfo.xml
        DESTINATION share/metainfo/
    )
endif()
