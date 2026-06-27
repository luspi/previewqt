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
option(WITH_POPPLER               "Configure with support for the Poppler library" OFF)
option(WITH_QTPDF                 "Configure with support for the QtPDF module (instead of Poppler)" ON)
option(WITH_LIBARCHIVE            "Configure with support for the libarchive library" ON)
option(WITH_LIBVIPS               "Configure with support for the libvips library" OFF)
option(WITH_RESVG                 "Configure with support for the resvg library (better SVG support)" OFF)
option(WITH_LIBSAI                "Configure with support for the libsai library (requires C++20 support)" OFF)
option(WITH_QTMULTIMEDIA          "Configure with support for QtMultimedia" ON)
option(WITH_LIBMPV                "Configure with support for libmpv" OFF)

option(WITH_EXIV2                 "Configure with support for the Exiv2 library" ON)
option(WITH_EXIV2_ENABLE_BMFF     "Configure with BMFF format support in Exiv2 (if available) - only needed for Exiv2 0.27.x and before" ON)
option(WITH_LCMS2                 "Configure with support for the Little CMS library (advanced color management)" ON)
option(WITH_MOTIONPHOTO           "Configure with support for Google Motion Photos and Apple Live Photos" ON)
option(WITH_PHOTOSPHERE           "Configure with support for photo spheres and 360 degree panoramic views" ON)
option(WITH_PHOTOSPHERE_QRHI      "Use private Qt6 Gui header QRhi to figure our maximum possible texture size" OFF)
option(WITH_EPUB                  "Configure with support for EPUB files" ON) #######
option(WITH_KF6SYNTAXHIGHLIGHT    "Configure with support for KSyntaxHighlighting for syntax highlighting" ON) #######
option(WITH_WEBENGINE             "Configure with support for Qt WebEngine" ON)
option(WITH_FFMPEGTHUMBNAILER     "Use ffmpegthumbnailer's C++ API instead of calling the executable" OFF)

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

if(WITH_QTMULTIMEDIA)
    if(WITH_LIBMPV)
        set(WITH_LIBMPV OFF)
        message("** Multimedia can be handler either using QtMultimedia or MPV.")
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

##################################
#### INTERNAL ADAPTED OPTIONS ####
##################################

# these will be set to ON when enabled AND when found
# and will be set to OFF otherwise
# this way the user facing options remain unchanged
# and there is clearer info about which ones FAILED to be enabled

set(_WITH_IMAGEMAGICK           ${WITH_IMAGEMAGICK})
set(_WITH_GRAPHICSMAGICK        ${WITH_GRAPHICSMAGICK})

set(_WITH_LIBRAW             ${WITH_LIBRAW})
set(_WITH_DEVIL              ${WITH_DEVIL})
set(_WITH_POPPLER            ${WITH_POPPLER})
set(_WITH_QTPDF              ${WITH_QTPDF})
set(_WITH_LIBARCHIVE         ${WITH_LIBARCHIVE})
set(_WITH_LIBVIPS            ${WITH_LIBVIPS})
set(_WITH_RESVG              ${WITH_RESVG})
set(_WITH_LIBSAI             ${WITH_LIBSAI})
set(_WITH_QTMULTIMEDIA       ${WITH_QTMULTIMEDIA})
set(_WITH_LIBMPV             ${WITH_LIBMPV})

set(_WITH_EXIV2              ${WITH_EXIV2})
set(_WITH_EXIV2_ENABLE_BMFF  ${WITH_EXIV2_ENABLE_BMFF})
set(_WITH_LCMS2              ${WITH_LCMS2})
set(_WITH_MOTIONPHOTO        ${WITH_MOTIONPHOTO})
set(_WITH_PHOTOSPHERE        ${WITH_PHOTOSPHERE})
set(_WITH_PHOTOSPHERE_QRHI   ${WITH_PHOTOSPHERE_QRHI})
set(_WITH_EPUB               ${WITH_EPUB})
set(_WITH_KF6SYNTAXHIGHLIGHT ${WITH_KF6SYNTAXHIGHLIGHT})
set(_WITH_WEBENGINE          ${WITH_WEBENGINE})
set(_WITH_FFMPEGTHUMBNAILER  ${WITH_FFMPEGTHUMBNAILER})

set(_WITH_PORTABLETWEAKS     ${WITH_PORTABLETWEAKS})
set(_WITH_ADAPTSOURCE        ${WITH_ADAPTSOURCE})
set(_WITH_FLATPAKBUILD       ${WITH_FLATPAKBUILD})

#############################
####  GET list of files  ####
#############################

include(CMake/ListFilesCPlusPlus.cmake)
include(CMake/ListFilesQML.cmake)

################################
#### FIND REQUIRED PACKAGES ####
################################

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Widgets Sql Core Svg Concurrent PrintSupport DBus LinguistTools)

if(_WITH_PHOTOSPHERE_QRHI)
    # this module is needed for getting the maximum texture size possible using QRhi
    find_package(Qt6 COMPONENTS GuiPrivate)
endif()

if(_WITH_QTMULTIMEDIA)
    find_package(Qt6 6.4 REQUIRED COMPONENTS Multimedia)
endif()

SET(INeedWebEngineQuick 0)
if(_WITH_EPUB)
    if(_WITH_LIBARCHIVE)
        SET(INeedWebEngineQuick 1)
    endif()
endif()
if(_WITH_WEBENGINE)
    SET(INeedWebEngineQuick 1)
endif()

if(INeedWebEngineQuick)
    find_package(Qt6 6.4 REQUIRED COMPONENTS WebEngineQuick)
endif()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

#########

if(_WITH_IMAGEMAGICK)
    find_package(ImageMagick COMPONENTS Magick++)
    if(NOT ${ImageMagick_FOUND})
        message("** ImageMagick not found. Disabling.")
        SET(_WITH_IMAGEMAGICK OFF)
    endif()
endif()

if(_WITH_GRAPHICSMAGICK)
    find_package(GraphicsMagick)
    if(NOT ${MAGICK++_FOUND})
        message("** GraphicsMagick not found. Disabling.")
        SET(_WITH_GRAPHICSMAGICK OFF)
    endif()
endif()

if(_WITH_LIBRAW)
    find_package(LibRaw)
    if(NOT ${LibRaw_FOUND})
        message("** LibRaw not found. Disabling.")
        SET(_WITH_LIBRAW OFF)
    endif()
endif()

if(_WITH_DEVIL)
    find_package(DevIL)
    if(NOT ${DevIL_FOUND})
        message("** DevIL not found. Disabling.")
        SET(_WITH_DEVIL OFF)
    endif()
endif()

if(_WITH_POPPLER)
    find_package(Poppler COMPONENTS Qt6)
    if(NOT ${Poppler_FOUND})
        message("** Poppler not found. Disabling.")
        SET(_WITH_POPPLER OFF)
    endif()
endif()

if(_WITH_QTPDF)
    find_package(Qt6 COMPONENTS Pdf)
    if(NOT ${Qt6Pdf_FOUND})
        message("** Qt PDF component not found. Disabling.")
        SET(_WITH_QTPDF OFF)
    endif()
endif()

if(_WITH_LIBARCHIVE)
    find_package(LibArchive)
    if(NOT ${LibArchive_FOUND})
        message("** LibArchive not found. Disabling.")
        SET(_WITH_LIBARCHIVE OFF)
    endif()
endif()

if(_WITH_LIBMPV)
    find_package(Libmpv)
    if(NOT ${Libmpv_FOUND})
        message("** libmpv not found. Disabling.")
        SET(_WITH_LIBMPV OFF)
    endif()
endif()

if(_WITH_LIBVIPS)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB glib-2.0 gobject-2.0)
    if(NOT ${GLIB_FOUND})
        message("** glib not found. Disabling libvips.")
        SET(_WITH_LIBVIPS OFF)
    else()
        pkg_search_module(VIPS vips-cpp)
        if(NOT ${VIPS_FOUND})
            message("** libvips not found. Disabling.")
            SET(_WITH_LIBVIPS OFF)
        endif()
    endif()
endif()

if(_WITH_LIBSAI)
    find_package(Libsai)
    if(NOT ${Libsai_FOUND})
        message("** Libsai not found. Disabling.")
        SET(_WITH_LIBSAI OFF)
    else()
        set(CMAKE_CXX_STANDARD 20)
    endif()
endif()

if(_WITH_EXIV2)
    find_package(exiv2)
    if(NOT ${exiv2_FOUND})
        message("** Exiv2 not found. Disabling.")
        SET(_WITH_EXIV2 OFF)
    endif()
endif()

if(_WITH_LCMS2)
    find_package(LCMS2)
    if(NOT ${LCMS2_FOUND})
        message("** LCMS2 not found. Disabling.")
        SET(_WITH_LCMS2 OFF)
    endif()
endif()

if(_WITH_EPUB)
    if(NOT _WITH_LIBARCHIVE)
        message("** EPUB support requires support for LibArchive")
        message("** Disabling EPUB support")
        set(_WITH_EPUB OFF)
    endif()
endif()

if(_WITH_FLATPAKBUILD)
    find_package(PkgConfig REQUIRED)
    if(NOT ${PkgConfig_FOUND})
        message("** PkgConfig not found. Disabling flatpak workaround.")
        SET(_WITH_FLATPAKBUILD OFF)
    else()
        pkg_search_module(GLIB glib-2.0 gobject-2.0 gio-2.0)
        if(NOT ${GLIB_FOUND})
            message("** GLIB not found. Disabling flatpak workaround.")
            SET(_WITH_FLATPAKBUILD OFF)
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
if(_WITH_QTMULTIMEDIA)
    target_link_libraries(previewqt PRIVATE Qt6::Multimedia)
endif()
if(_WITH_QTPDF)
    target_link_libraries(previewqt PRIVATE Qt6::Pdf)
endif()
if(INeedWebEngineQuick)
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
set(STATUS_FAILED "")

if(_WITH_IMAGEMAGICK)
    list(APPEND STATUS_FORMATS_ENABLED "ImageMagick ${ImageMagick_VERSION_STRING}")
    # These checks are necessary to "fix" compiling PreviewQt with both ImageMagick 6 and 7 available
    if(ImageMagick_VERSION_STRING MATCHES "^7")
        string(REPLACE "libMagick++-6." "libMagick++-7." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
        string(REPLACE "ImageMagick-6" "ImageMagick-7" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
    endif()
    target_compile_definitions(previewqt PRIVATE PQMIMAGEMAGICK)
    target_link_libraries(previewqt PRIVATE ImageMagick::Magick++)
else()
    if(WITH_IMAGEMAGICK)
        list(APPEND STATUS_FAILED "ImageMagick")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "ImageMagick")
    endif()
endif()

if(_WITH_GRAPHICSMAGICK)
    list(APPEND STATUS_FORMATS_ENABLED "Graphicsmagick")
    target_include_directories(previewqt PRIVATE "${MAGICK++_INCLUDE_DIR}")
    target_compile_definitions(previewqt PRIVATE PQMGRAPHICSMAGICK)
    target_link_libraries(previewqt PRIVATE "GraphicsMagick++")
else()
    if(WITH_GRAPHICSMAGICK)
        list(APPEND STATUS_FAILED "GraphicsMagick")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "GraphicsMagick")
    endif()
endif()

if(_WITH_LIBRAW)
    list(APPEND STATUS_FORMATS_ENABLED "LibRaw")
    target_compile_definitions(previewqt PRIVATE PQMRAW)
    target_include_directories(previewqt PRIVATE "${LibRaw_INCLUDE_DIR}")
    target_link_libraries(previewqt PRIVATE ${LibRaw_LIBRARIES})
else()
    if(WITH_LIBRAW)
        list(APPEND STATUS_FAILED "LibRaw")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "LibRaw")
    endif()
endif()

if(_WITH_DEVIL)
    list(APPEND STATUS_FORMATS_ENABLED "DevIL")
    target_compile_definitions(previewqt PRIVATE PQMDEVIL)
    target_link_libraries(previewqt PRIVATE DevIL::IL)
else()
    if(WITH_DEVIL)
        list(APPEND STATUS_FAILED "DevIL")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "DevIL")
    endif()
endif()

if(_WITH_POPPLER)
    list(APPEND STATUS_FORMATS_ENABLED "Poppler ${Poppler_VERSION}")
    target_include_directories(previewqt PRIVATE "${Poppler_INCLUDE_DIRS}")
    target_compile_definitions(previewqt PRIVATE PQMPOPPLER)
    target_link_libraries(previewqt PRIVATE ${Poppler_LIBRARIES})
else()
    if(WITH_POPPLER)
        list(APPEND STATUS_FAILED "Poppler")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "Poppler")
    endif()
endif()

if(_WITH_QTPDF)
    list(APPEND STATUS_FORMATS_ENABLED "QtPDF module")
    target_compile_definitions(previewqt PRIVATE PQMQTPDF)
else()
    if(WITH_QTPDF)
        list(APPEND STATUS_FAILED "QtPDF module")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "QtPDF module")
    endif()
endif()

if(_WITH_LIBARCHIVE)
    list(APPEND STATUS_FORMATS_ENABLED "LibArchive ${LibArchive_VERSION}")
    target_compile_definitions(previewqt PRIVATE PQMLIBARCHIVE)
    target_link_libraries(previewqt PRIVATE LibArchive::LibArchive)
else()
    if(WITH_LIBARCHIVE)
        list(APPEND STATUS_FAILED "LibArchive")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "LibArchive")
    endif()
endif()

if(_WITH_QTMULTIMEDIA)
    list(APPEND STATUS_FORMATS_ENABLED "Qt Multimedia module")
    target_compile_definitions(previewqt PRIVATE PQMQTMULTIMEDIA)
else()
    if(WITH_QTMULTIMEDIA)
        list(APPEND STATUS_FAILED "Qt Multimedia module")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "Qt Multimedia module")
    endif()
endif()

if(_WITH_LIBMPV)
    list(APPEND STATUS_FORMATS_ENABLED "libmpv ${Libmpv_VERSION}")
    target_compile_definitions(previewqt PRIVATE PQMLIBMPV)
    target_link_libraries(previewqt PRIVATE Libmpv::Libmpv)
else()
    if(WITH_VIDEO_MPV)
        list(APPEND STATUS_FAILED "libmpv")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "libmpv")
    endif()
endif()

if(_WITH_LIBVIPS)
    list(APPEND STATUS_FORMATS_ENABLED "libvips")
    target_include_directories(previewqt PRIVATE "${GLIB_INCLUDE_DIRS}")
    target_compile_definitions(previewqt PRIVATE PQMLIBVIPS)
    target_link_libraries(previewqt PRIVATE "${GLIB_LIBRARIES}" "vips" "gobject-2.0" "vips-cpp")
else()
    if(WITH_LIBVIPS)
        list(APPEND STATUS_FAILED "libvips")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "libvips")
    endif()
endif()

if(_WITH_RESVG)
    list(APPEND STATUS_FORMATS_ENABLED "resvg")
    target_link_libraries(previewqt PRIVATE "resvg")
    target_compile_definitions(previewqt PRIVATE PQMRESVG)
    find_file(RESVGQTINC "ResvgQt.h" PATHS ${INCLUDE_DIRECTORIES})
    if(EXISTS ${RESVGQTINC})
        target_compile_definitions(photoqt PRIVATE PQMRESVG_QT)
    endif()
else()
    if(WITH_RESVG)
        list(APPEND STATUS_FAILED "resvg")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "resvg")
    endif()
endif()

if(_WITH_LIBSAI)
    list(APPEND STATUS_FORMATS_ENABLED "libsai")
    target_include_directories(previewqt PRIVATE "${Libsai_INCLUDE_DIR}")
    target_compile_definitions(previewqt PRIVATE PQMLIBSAI)
    target_link_libraries(previewqt PRIVATE "sai")
else()
    if(WITH_LIBSAI)
        list(APPEND STATUS_FAILED "libsai")
    else()
        list(APPEND STATUS_FORMATS_DISABLED "libsai")
    endif()
endif()

if(_WITH_EXIV2)
    list(APPEND STATUS_ENABLED "Exiv2 ${exiv2_VERSION}")
    target_compile_definitions(previewqt PRIVATE PQMEXIV2)
    target_link_libraries(previewqt PRIVATE "exiv2lib")
    if(_WITH_EXIV2_ENABLE_BMFF)
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
    if(WITH_EXIV2)
        list(APPEND STATUS_FAILED "Exiv2")
    else()
        list(APPEND STATUS_DISABLED "Exiv2")
    endif()
endif()

if(_WITH_FFMPEGTHUMBNAILER)
    list(APPEND STATUS_ENABLED "ffmpegthumbnailer API")
    target_compile_definitions(previewqt PRIVATE PQMFFMPEGTHUMBNAILER)
    target_link_libraries(previewqt PRIVATE ffmpegthumbnailer)
else()
    if(WITH_FFMPEGTHUMBNAILER)
        list(APPEND STATUS_FAILED "ffmpegthumbnailer API")
    else()
        list(APPEND STATUS_DISABLED "ffmpegthumbnailer API")
    endif()
endif()

if(_WITH_LCMS2)
    list(APPEND STATUS_ENABLED "LCMS2 ${LCMS2_VERSION}")
    target_include_directories(previewqt PRIVATE "${LCMS2_INCLUDE_DIR}")
    target_compile_definitions(previewqt PRIVATE PQMLCMS2)
    target_link_libraries(previewqt PRIVATE ${LCMS2_LIBRARIES})
else()
    if(WITH_LCMS2)
        list(APPEND STATUS_FAILED "LCMS2")
    else()
        list(APPEND STATUS_DISABLED "LCMS2")
    endif()
endif()

if(_WITH_MOTIONPHOTO)
    list(APPEND STATUS_ENABLED "Motion photos and Apple live photos")
    target_compile_definitions(previewqt PRIVATE PQMMOTIONPHOTO)
else()
    if(WITH_MOTIONPHOTO)
        list(APPEND STATUS_FAILED "Motion photos and Apple live photos")
    else()
        list(APPEND STATUS_DISABLED "Motion photos and Apple live photos")
    endif()
endif()

if(_WITH_PHOTOSPHERE)
    list(APPEND STATUS_ENABLED "Photo spheres")
    target_compile_definitions(previewqt PRIVATE PQMPHOTOSPHERE)
    if(_WITH_PHOTOSPHERE_QRHI)
        target_link_libraries(previewqt PRIVATE Qt6::GuiPrivate)
        target_compile_definitions(previewqt PRIVATE PQMPHOTOSPHEREQRHI)
    endif()
else()
    if(WITH_PHOTOSPHERE)
        list(APPEND STATUS_FAILED "Photo spheres")
    else()
        list(APPEND STATUS_DISABLED "Photo spheres")
    endif()
endif()

if(_WITH_EPUB)
    list(APPEND STATUS_ENABLED "EPUB")
    target_compile_definitions(previewqt PRIVATE PQMEPUB)
else()
    if(WITH_EPUB)
        list(APPEND STATUS_FAILED "EPUB")
    else()
        list(APPEND STATUS_DISABLED "EPUB")
    endif()
endif()

if(_WITH_WEBENGINE)
    list(APPEND STATUS_ENABLED "Web Engine")
    target_compile_definitions(previewqt PRIVATE PQMWEBENGINE)
else()
    if(WITH_WEBENGINE)
        list(APPEND STATUS_FAILED "Web Engine")
    else()
        list(APPEND STATUS_DISABLED "Web Engine")
    endif()
endif()

if(_WITH_KF6SYNTAXHIGHLIGHT)

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
            if(_WITH_ADAPTSOURCE)
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
            if(_WITH_ADAPTSOURCE)
                file(WRITE ${QMLFILE} "${FILE_CONTENTS}")
            else()
                message(FATAL_ERROR "Error: Source files are adapted for use with KSyntaxHighlight. You need to enable the WITH_ADAPTSOURCE option to automatically convert them!")
            endif()
        endif()

    endforeach()

endif()

if(_WITH_FLATPAKBUILD)
    list(APPEND STATUS_ENABLED "Flatpak workarounds")
    target_compile_definitions(previewqt PRIVATE PQMFLATPAKBUILD)
    target_include_directories(previewqt PRIVATE "${GLIB_INCLUDE_DIRS}")
    target_link_libraries(previewqt PRIVATE "${GLIB_LIBRARIES}" "gobject-2.0" "gio-2.0")
else()
    if(WITH_FLATPAKBUILD)
        list(APPEND STATUS_FAILED "Flatpak workarounds")
    else()
        list(APPEND STATUS_DISABLED "Flatpak workarounds")
    endif()
endif()

if(_WITH_PORTABLETWEAKS)
    list(APPEND STATUS_ENABLED "Portable tweaks (do not use unless you know what you are doing!)")
    target_compile_definitions(previewqt PRIVATE PQMPORTABLETWEAKS)
else()
    if(WITH_PORTABLETWEAKS)
        list(APPEND STATUS_FAILED "Portable tweaks (leave disabled unless you know what you are doing!)")
    else()
        list(APPEND STATUS_DISABLED "Portable tweaks (leave disabled unless you know what you are doing!)")
    endif()
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

list(LENGTH STATUS_FAILED failedlength)
if(failedlength GREATER 0)
    message("")
    message("")
    message("** The following options FAILED to be ENABLED for this build:")
    message("")
    foreach(opt IN LISTS STATUS_FAILED)
        message("    - ${opt}")
    endforeach()
    message("")
endif()

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
    if(NOT _WITH_FLATPAKBUILD)
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
