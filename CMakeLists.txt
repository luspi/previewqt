###################################################
# CMakeLists for PreviewQt: http://previewqt.org/ #
###################################################

cmake_minimum_required(VERSION 3.26)
project(previewqt LANGUAGES CXX)

##################################################################
####  GLOBAL VERSION STRING FOR ALL FILES (INCLUDING CPP/QML) ####
##################################################################

SET(APPVERSION "dev")

###################################
####  ENSURE BUILD TYPE IS SET ####
###################################

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#############################################
#### OPTIONS THAT CAN BE SET BY THE USER ####
#############################################

option(WITH_IMAGEMAGICK           "Configure with support for the ImageMagick library" ON)
option(WITH_GRAPHICSMAGICK        "Configure with support for the GraphicsMagick library" OFF)
option(WITH_LIBRAW                "Configure with support for the libraw library" ON)
option(WITH_DEVIL                 "Configure with support for the DevIL library" ON)
option(WITH_FREEIMAGE             "Configure with support for the FreeImage library" OFF)
option(WITH_POPPLER               "Configure with support for the Poppler library" OFF)
option(WITH_QTPDF                 "Configure with support for the QtPDF module (instead of Poppler)" ON)
option(WITH_LIBARCHIVE            "Configure with support for the libarchive library" ON)
option(WITH_VIDEO_QT              "Configure with video support through Qt" ON)
option(WITH_VIDEO_MPV             "Configure with video support through MPV" ON)
option(WITH_LIBVIPS               "Configure with support for the libvips library" OFF)
option(WITH_RESVG                 "Configure with support for the resvg library (better SVG support)" OFF)

option(WITH_EXIV2                 "Configure with support for the Exiv2 library" ON)
option(WITH_EXIV2_ENABLE_BMFF     "Configure with BMFF format support in Exiv2 (if available) - only needed for Exiv2 0.27.x and before" ON)
option(WITH_LCMS2                 "Configure with support for the Little CMS library (advanced color management)" ON)
option(WITH_MOTIONPHOTO           "Configure with support for Google Motion Photos and Apple Live Photos" ON)
option(WITH_PHOTOSPHERE           "Configure with support for photo spheres and 360 degree panoramic views" ON)
option(WITH_EPUB                  "Configure with support for EPUB files" ON) #######
option(WITH_KF6SYNTAXHIGHLIGHT    "Configure with support for KSyntaxHighlighting for syntax highlighting" ON) #######

option(WITH_PORTABLETWEAKS        "Apply tweaks for a build of a portable version" OFF)
option(WITH_ADAPTSOURCE           "Adapt and change the source files for the current Qt version (if necessary)" OFF)
option(WITH_FLATPAKBUILD          "Enable this option if this is a build for Flatpak" OFF)

#####################################
#### RESOLVE CONFLICTING OPTIONS ####
#####################################

if(WITH_QTPDF)
    if(WITH_POPPLER)
        set(WITH_POPPLER OFF)
        message("** For displaying PDF documents you have to choose either Poppler OR QtPDF.")
        message("** Poppler has been automatically disabled in favour of QtPDF.")
    endif()
endif()

if(WITH_VIDEO_QT)
    if(WITH_VIDEO_MPV)
        set(WITH_VIDEO_MPV OFF)
        message("** Videos can be shown either using QtMultimedia or MPV.")
        message("** MPV support has been disabled in favour of QtMultimedia.")
    endif()
endif()

if(WITH_IMAGEMAGICK)
    if(WITH_GRAPHICSMAGICK)
        set(WITH_GRAPHICSMAGICK OFF)
        message("** ImageMagick and GraphicsMagick cannot be used at the same time.")
        message("** GraphicsMagick has been automatically disabled in favour of ImageMagick.")
    endif()
endif()

#############################
####  GET list of files  ####
#############################

include(CMake/ListFilesCPlusPlus.cmake)
include(CMake/ListFilesQML.cmake)

################################
#### FIND REQUIRED PACKAGES ####
################################

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Widgets Sql Core Svg Concurrent Multimedia PrintSupport DBus LinguistTools)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

#########

if(WITH_IMAGEMAGICK)
    find_package(ImageMagick COMPONENTS Magick++)
    if(NOT ${ImageMagick_FOUND})
        message("** ImageMagick not found. Disabling.")
        SET(WITH_IMAGEMAGICK OFF)
    endif()
endif()

if(WITH_GRAPHICSMAGICK)
    find_package(GraphicsMagick)
    if(NOT ${MAGICK++_FOUND})
        message("** GraphicsMagick not found. Disabling.")
        SET(WITH_GRAPHICSMAGICK OFF)
    endif()
endif()

if(WITH_LIBRAW)
    find_package(LibRaw)
    if(NOT ${LibRaw_FOUND})
        message("** LibRaw not found. Disabling.")
        SET(WITH_LIBRAW OFF)
    endif()
endif()

if(WITH_DEVIL)
    find_package(DevIL)
    if(NOT ${DevIL_FOUND})
        message("** DevIL not found. Disabling.")
        SET(WITH_DEVIL OFF)
    endif()
endif()

if(WITH_FREEIMAGE)
    find_package(FreeImage)
    if(NOT ${FREEIMAGE_FOUND})
        message("** FreeImage not found. Disabling.")
        SET(WITH_FREEIMAGE OFF)
    endif()
endif()

if(WITH_POPPLER)
    find_package(Poppler COMPONENTS Qt6)
    if(NOT ${Poppler_FOUND})
        message("** Poppler not found. Disabling.")
        SET(WITH_POPPLER OFF)
    endif()
endif()

if(WITH_QTPDF)
    find_package(Qt6 COMPONENTS Pdf)
    if(NOT ${Qt6Pdf_FOUND})
        message("** Qt PDF component not found. Disabling.")
        SET(WITH_QTPDF OFF)
    endif()
endif()

if(WITH_LIBARCHIVE)
    find_package(LibArchive)
    if(NOT ${LibArchive_FOUND})
        message("** LibArchive not found. Disabling.")
        SET(WITH_LIBARCHIVE OFF)
    endif()
endif()

if(WITH_VIDEO_MPV)
    find_package(Libmpv)
    if(NOT ${Libmpv_FOUND})
        message("** libmpv not found. Disabling.")
        SET(WITH_VIDEO_MPV OFF)
    endif()
endif()

if(WITH_LIBVIPS)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB glib-2.0 gobject-2.0)
    if(NOT ${GLIB_FOUND})
        message("** glib not found. Disabling libvips.")
        SET(WITH_LIBVIPS OFF)
    else()
        pkg_search_module(VIPS vips-cpp)
        if(NOT ${VIPS_FOUND})
            message("** libvips not found. Disabling.")
            SET(WITH_LIBVIPS OFF)
        endif()
    endif()
endif()

if(WITH_EXIV2)
    find_package(exiv2)
    if(NOT ${exiv2_FOUND})
        message("** Exiv2 not found. Disabling.")
        SET(WITH_EXIV2 OFF)
    endif()
endif()

if(WITH_LCMS2)
    find_package(LCMS2)
    if(NOT ${LCMS2_FOUND})
        message("** LCMS2 not found. Disabling.")
        SET(WITH_LCMS2 OFF)
    endif()
endif()

if(WITH_EPUB)
    if(WITH_LIBARCHIVE)
        find_package(Qt6 6.4 REQUIRED COMPONENTS WebEngineQuick)
    else()
        message("** EPUB support requires support for LibArchive")
        message("** Disabling EPUB support")
        set(WITH_EPUB OFF)
    endif()
endif()

if(WITH_FLATPAKBUILD)
    find_package(PkgConfig REQUIRED)
    if(NOT ${PkgConfig_FOUND})
        message("** PkgConfig not found. Disabling flatpak workaround.")
        SET(WITH_FLATPAKBUILD OFF)
    else()
        pkg_search_module(GLIB glib-2.0 gobject-2.0 gio-2.0)
        if(NOT ${GLIB_FOUND})
            message("** GLIB not found. Disabling flatpak workaround.")
            SET(WITH_FLATPAKBUILD OFF)
        endif()
    endif()
endif()


########################
#### QT SETUP STUFF ####
########################

qt_standard_project_setup(REQUIRES 6.4)

qt_add_resources(previewqt_SRC img/img.qrc misc/misc.qrc)

#############################
#### Add the executeable ####
#############################

if(WIN32)
    qt_add_executable(previewqt ${previewqt_SRC} windowsicons.rc)
else()
    qt_add_executable(previewqt ${previewqt_SRC})
endif()

# ensure that the QTP0001 policy is set to NEW otherwise the QML files will not be found on import (on Qt 6.8+)
if(QT_KNOWN_POLICY_QTP0001)
    qt_policy(SET QTP0001 "NEW")
endif()
# disable warning with this policy not being set (on Qt 6.8+)
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 "OLD")
endif()

# add qml files
qt_add_qml_module(previewqt URI PreviewQt VERSION 1.0 QML_FILES ${previewqt_QML})

###########################

# set the version number
target_compile_definitions(previewqt PRIVATE PQMVERSION="${APPVERSION}")

# set some properties for executable
set_target_properties(previewqt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER previewqt.PreviewQt.org
    MACOSX_BUNDLE_BUNDLE_VERSION "${APPVERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${APPVERSION}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# set header files as include files
target_include_directories(previewqt PRIVATE "cplusplus/header")

# link executable
target_link_libraries(previewqt PRIVATE Qt6::Quick Qt6::Widgets Qt6::Svg Qt6::Sql Qt6::DBus)
if(WITH_QTPDF)
    target_link_libraries(previewqt PRIVATE Qt6::Pdf)
endif()
if(WITH_EPUB)
    target_link_libraries(previewqt PRIVATE Qt6::WebEngineQuick)
endif()


###############################
#### ADDITIONAL QT OPTIONS ####
###############################

# Since Python might be imported we have to avoid using Qt keywords (like 'slots') to avoid naming conflicts
target_compile_definitions(previewqt PRIVATE QT_NO_KEYWORDS)

# we always want to capture debug/log context information
target_compile_definitions(previewqt PRIVATE QT_MESSAGELOGCONTEXT)

##################################
#### COMPOSE THE DESKTOP FILE ####
##################################

include("CMake/ComposeDesktopFile.cmake")
composeDesktopFile()

######################
#### TRANSLATIONS ####
######################

# the compiled translations are automatically embedded as resource in executable
file(GLOB files "lang/*.ts")
qt_add_translations(previewqt TS_FILES ${files} RESOURCE_PREFIX "/lang")

########################
#### CUSTOM OPTIONS ####
########################

SET(STATUS_FORMATS_ENABLED "")
SET(STATUS_FORMATS_DISABLED "")
SET(STATUS_ENABLED "")
SET(STATUS_DISABLED "")

if(WITH_IMAGEMAGICK)
    list(APPEND STATUS_FORMATS_ENABLED "ImageMagick ${ImageMagick_VERSION_STRING}")
    # These checks are necessary to "fix" compiling PreviewQt with both ImageMagick 6 and 7 available
    if(ImageMagick_VERSION_STRING MATCHES "^7")
        string(REPLACE "libMagick++-6." "libMagick++-7." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
        string(REPLACE "ImageMagick-6" "ImageMagick-7" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
    endif()
    target_compile_definitions(previewqt PRIVATE PQMIMAGEMAGICK)
    target_link_libraries(previewqt PRIVATE ImageMagick::Magick++)
else()
    list(APPEND STATUS_FORMATS_DISABLED "ImageMagick")
endif()

if(WITH_GRAPHICSMAGICK)
    list(APPEND STATUS_FORMATS_ENABLED "Graphicsmagick")
    target_include_directories(previewqt PRIVATE "${MAGICK++_INCLUDE_DIR}")
    target_compile_definitions(previewqt PRIVATE PQMGRAPHICSMAGICK)
    target_link_libraries(previewqt PRIVATE "GraphicsMagick++")
else()
    list(APPEND STATUS_FORMATS_DISABLED "GraphicsMagick")
endif()

if(WITH_LIBRAW)
    list(APPEND STATUS_FORMATS_ENABLED "LibRaw")
    find_package(OpenMP)
    if(OpenMP_C_FOUND)
        message("OPENMP_C FOUND")
        target_link_libraries(previewqt PUBLIC OpenMP::OpenMP_C)
    else()
        message("OPENMP_C NOT FOUND")
    endif()
    target_compile_definitions(previewqt PRIVATE PQMRAW)
    target_include_directories(previewqt PRIVATE "${LibRaw_INCLUDE_DIR}")
    target_link_libraries(previewqt PRIVATE ${LibRaw_LIBRARIES})
else()
    list(APPEND STATUS_FORMATS_DISABLED "LibRaw")
endif()

if(WITH_DEVIL)
    list(APPEND STATUS_FORMATS_ENABLED "DevIL")
    target_compile_definitions(previewqt PRIVATE PQMDEVIL)
    target_link_libraries(previewqt PRIVATE DevIL::IL)
else()
    list(APPEND STATUS_FORMATS_DISABLED "DevIL")
endif()

if(WITH_FREEIMAGE)
    list(APPEND STATUS_FORMATS_ENABLED "FreeImage")
    target_include_directories(previewqt PRIVATE "${FREEIMAGE_INCLUDE_DIRS}")
    target_compile_definitions(previewqt PRIVATE PQMFREEIMAGE)
    target_link_libraries(previewqt PRIVATE ${FREEIMAGE_C_LIBRARY})
else()
    list(APPEND STATUS_FORMATS_DISABLED "FreeImage")
endif()

if(WITH_POPPLER)
    list(APPEND STATUS_FORMATS_ENABLED "Poppler ${Poppler_VERSION}")
    target_include_directories(previewqt PRIVATE "${Poppler_INCLUDE_DIRS}")
    target_compile_definitions(previewqt PRIVATE PQMPOPPLER)
    target_link_libraries(previewqt PRIVATE ${Poppler_LIBRARIES})
else()
    list(APPEND STATUS_FORMATS_DISABLED "Poppler")
endif()

if(WITH_QTPDF)
    list(APPEND STATUS_FORMATS_ENABLED "QtPDF module")
    target_compile_definitions(previewqt PRIVATE PQMQTPDF)
else()
    list(APPEND STATUS_FORMATS_DISABLED "QtPDF module")
endif()

if(WITH_LIBARCHIVE)
    list(APPEND STATUS_FORMATS_ENABLED "LibArchive ${LibArchive_VERSION}")
    target_compile_definitions(previewqt PRIVATE PQMLIBARCHIVE)
    target_link_libraries(previewqt PRIVATE LibArchive::LibArchive)
else()
    list(APPEND STATUS_FORMATS_DISABLED "LibArchive")
endif()

if(WITH_VIDEO_QT)
    list(APPEND STATUS_FORMATS_ENABLED "Qt video module")
    target_compile_definitions(previewqt PRIVATE PQMVIDEOQT)
else()
    list(APPEND STATUS_FORMATS_DISABLED "Qt video module")
endif()

if(WITH_VIDEO_MPV)
    list(APPEND STATUS_FORMATS_ENABLED "libmpv ${Libmpv_VERSION}")
    target_compile_definitions(previewqt PRIVATE PQMVIDEOMPV)
    target_link_libraries(previewqt PRIVATE Libmpv::Libmpv)
else()
    list(APPEND STATUS_FORMATS_DISABLED "libmpv")
endif()

if(WITH_LIBVIPS)
    list(APPEND STATUS_FORMATS_ENABLED "libvips")
    target_include_directories(previewqt PRIVATE "${GLIB_INCLUDE_DIRS}")
    target_compile_definitions(previewqt PRIVATE PQMLIBVIPS)
    target_link_libraries(previewqt PRIVATE "${GLIB_LIBRARIES}" "vips" "gobject-2.0" "vips-cpp")
else()
    list(APPEND STATUS_FORMATS_DISABLED "libvips")
endif()

if(WITH_RESVG)
    list(APPEND STATUS_FORMATS_ENABLED "resvg")
    target_link_libraries(previewqt PRIVATE "resvg")
    target_compile_definitions(previewqt PRIVATE PQMRESVG)
else()
    list(APPEND STATUS_FORMATS_DISABLED "resvg")
endif()

if(WITH_EXIV2)
    list(APPEND STATUS_ENABLED "Exiv2 ${exiv2_VERSION}")
    target_compile_definitions(previewqt PRIVATE PQMEXIV2)
    target_link_libraries(previewqt PRIVATE "exiv2lib")
    if(WITH_EXIV2_ENABLE_BMFF)
        target_compile_definitions(previewqt PRIVATE PQMEXIV2_ENABLE_BMFF)
    endif()
    if(WIN32)
        target_compile_definitions(previewqt PRIVATE NOMINMAX)
    endif()
    # if exiv2 0.27.x is used some c++ features removed in c++17 need to be re-enabled
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        if(${exiv2_VERSION} MATCHES "^0\.27\.")
            message("** Enabling C++ features removed in C++17 for Exiv2 0.27.x")
            message("** Please update Exiv2 to at least 0.28.x!")
            target_compile_definitions(previewqt PRIVATE _HAS_AUTO_PTR_ETC=1)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(${exiv2_VERSION} MATCHES "^0\.27\.")
            message("** Enabling C++ features removed in C++17 for Exiv2 0.27.x")
            message("** Please update Exiv2 to at least 0.28.x!")
            add_definitions(-fpermissive)
        endif()
    endif()
else()
    list(APPEND STATUS_DISABLED "Exiv2")
endif()

if(WITH_LCMS2)
    list(APPEND STATUS_ENABLED "LCMS2 ${LCMS2_VERSION}")
    target_include_directories(previewqt PRIVATE "${LCMS2_INCLUDE_DIR}")
    target_compile_definitions(previewqt PRIVATE PQMLCMS2)
    target_link_libraries(previewqt PRIVATE ${LCMS2_LIBRARIES})
else()
    list(APPEND STATUS_DISABLED "LCMS2")
endif()

if(WITH_MOTIONPHOTO)
    list(APPEND STATUS_ENABLED "Motion photos and Apple live photos")
    target_compile_definitions(previewqt PRIVATE PQMMOTIONPHOTO)
else()
    list(APPEND STATUS_DISABLED "Motion photos and Apple live photos")
endif()

if(WITH_PHOTOSPHERE)
    list(APPEND STATUS_ENABLED "Photo spheres")
    target_compile_definitions(previewqt PRIVATE PQMPHOTOSPHERE)
else()
    list(APPEND STATUS_DISABLED "Photo spheres")
endif()

if(WITH_EPUB)
    list(APPEND STATUS_ENABLED "EPUB")
    target_compile_definitions(previewqt PRIVATE PQMEPUB)
else()
    list(APPEND STATUS_DISABLED "EPUB")
endif()

if(WITH_KF6SYNTAXHIGHLIGHT)

    list(APPEND STATUS_ENABLED "KSyntaxHighlight")
    target_compile_definitions(previewqt PRIVATE PQMKSYNTAXHIGHLIGHT)

    foreach(QMLFILE "qml/components/imageitems/PQTxt.qml")

        file(READ ${QMLFILE} FILE_CONTENTS)

        set(anything_changed FALSE)

        if(FILE_CONTENTS MATCHES "/\\*1off_PQMKF6")
            set(anything_changed TRUE)
            string(REPLACE "/*1off_PQMKF6" "/*1on_PQMKF6*/" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()
        if(FILE_CONTENTS MATCHES "2off_PQMKF6\\*/")
            set(anything_changed TRUE)
            string(REPLACE "2off_PQMKF6*/" "/*2on_PQMKF6*/" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()

        if(FILE_CONTENTS MATCHES "/\\*1on_PQMNOTKF6\\*/")
            set(anything_changed TRUE)
            string(REPLACE "/*1on_PQMNOTKF6*/" "/*1off_PQMNOTKF6" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()
        if(FILE_CONTENTS MATCHES "/\\*2on_PQMNOTKF6\\*/")
            set(anything_changed TRUE)
            string(REPLACE "/*2on_PQMNOTKF6*/" "2off_PQMNOTKF6*/" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()

        if(anything_changed)
            message(${QMLFILE})
            if(WITH_ADAPTSOURCE)
                file(WRITE ${QMLFILE} "${FILE_CONTENTS}")
            else()
                message(FATAL_ERROR "Error: Source files are not adapted for use with KSyntaxHighlight. You need to enable the WITH_ADAPTSOURCE option to automatically convert them!")
            endif()
        endif()

    endforeach()

else()

    list(APPEND STATUS_DISABLED "KSyntaxHighlight")

    foreach(QMLFILE "qml/components/imageitems/PQTxt.qml")

        file(READ ${QMLFILE} FILE_CONTENTS)

        if(FILE_CONTENTS MATCHES "/\\*1on_PQMKF6\\*/")
            set(anything_changed TRUE)
            string(REPLACE "/*1on_PQMKF6*/" "/*1off_PQMKF6" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()
        if(FILE_CONTENTS MATCHES "/\\*2on_PQMKF6\\*/")
            set(anything_changed TRUE)
            string(REPLACE "/*2on_PQMKF6*/" "2off_PQMKF6*/" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()

        if(FILE_CONTENTS MATCHES "/\\*1off_PQMNOTKF6")
            set(anything_changed TRUE)
            string(REPLACE "/*1off_PQMNOTKF6" "/*1on_PQMNOTKF6*/" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()
        if(FILE_CONTENTS MATCHES "2off_PQMKF6\\*/")
            set(anything_changed TRUE)
            string(REPLACE "2off_PQMNOTKF6*/" "/*2on_PQMNOTKF6*/" FILE_CONTENTS "${FILE_CONTENTS}")
        endif()

        if(anything_changed)
            if(WITH_ADAPTSOURCE)
                file(WRITE ${QMLFILE} "${FILE_CONTENTS}")
            else()
                message(FATAL_ERROR "Error: Source files are adapted for use with KSyntaxHighlight. You need to enable the WITH_ADAPTSOURCE option to automatically convert them!")
            endif()
        endif()

    endforeach()

endif()

if(WITH_FLATPAKBUILD)
    list(APPEND STATUS_ENABLED "Flatpak workarounds")
    target_compile_definitions(previewqt PRIVATE PQMFLATPAKBUILD)
    target_include_directories(previewqt PRIVATE "${GLIB_INCLUDE_DIRS}")
    target_link_libraries(previewqt PRIVATE "${GLIB_LIBRARIES}" "gobject-2.0" "gio-2.0")
else()
    list(APPEND STATUS_DISABLED "Flatpak workarounds")
endif()

if(WITH_PORTABLETWEAKS)
    list(APPEND STATUS_ENABLED "Portable tweaks (do not use unless you know what you are doing!)")
    target_compile_definitions(previewqt PRIVATE PQMPORTABLETWEAKS)
else()
    list(APPEND STATUS_DISABLED "Portable tweaks (leave disabled unless you know what you are doing!)")
endif()

message("")
message("** The following options are enabled for this build:")
message("")
foreach(opt IN LISTS STATUS_FORMATS_ENABLED)
    message("    - ${opt}")
endforeach()
message("")
foreach(opt IN LISTS STATUS_ENABLED)
    message("    - ${opt}")
endforeach()
message("")


message("")
message("")
message("** The following options are DISABLED for this build:")
message("")
foreach(opt IN LISTS STATUS_FORMATS_DISABLED)
    message("    - ${opt}")
endforeach()
message("")
foreach(opt IN LISTS STATUS_DISABLED)
    message("    - ${opt}")
endforeach()
message("")

#######################
#### INSTALL FILES ####
#######################

if(UNIX)

    include(GNUInstallDirs)

    # Install executeable
    install(
        TARGETS previewqt
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    # Install desktop file
    install(
        FILES org.previewqt.PreviewQt.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )

    # And install all the icons
    install(
        FILES icons/16x16/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps/
    )
    install(
        FILES icons/32x32/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps/
    )
    install(
        FILES icons/48x48/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps/
    )
    install(
        FILES icons/64x64/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps/
    )
    install(
        FILES icons/128x128/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps/
    )
    install(
        FILES icons/256x256/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps/
    )
    install(
        FILES icons/512x512/org.previewqt.PreviewQt.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps/
    )
    if(NOT WITH_FLATPAKBUILD)
        install(
            FILES icons/1024x1024/org.previewqt.PreviewQt.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/1024x1024/apps/
        )
    endif()
    install(
        FILES org.previewqt.PreviewQt.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo/
    )

endif()


##########################
#### UNINSTALL TARGET ####
##########################

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
