###################################################
# CMakeLists for PreviewQt: http://previewqt.org/ #
###################################################

cmake_minimum_required(VERSION 3.16)
project(previewqt LANGUAGES CXX)

##################################################################
####  GLOBAL VERSION STRING FOR ALL FILES (INCLUDING CPP/QML) ####
##################################################################

SET(APPVERSION "dev")

########################################
####  SOME SETTINGS/FLAGS FOR CMAKE ####
########################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
option(WITH_FREEIMAGE             "Configure with support for the FreeImage library" ON)
option(WITH_POPPLER               "Configure with support for the Poppler library" ON)
option(WITH_QTPDF                 "Configure with support for the QtPDF module (instead of Poppler)" OFF)
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

##################################################################################################################
# The options above were renamed for 4.0
# For the time being we still check for the old names and pass any value on to the new options
if(DEFINED IMAGEMAGICK)
    message("** The IMAGEMAGICK option is deprecated, use WITH_IMAGEMAGICK instead: ${IMAGEMAGICK}")
    set(WITH_IMAGEMAGICK ${IMAGEMAGICK} CACHE BOOL "Configure with support for the ImageMagick library" FORCE)
    unset(IMAGEMAGICK CACHE)
endif()
if(DEFINED GRAPHICSMAGICK)
    message("** The GRAPHICSMAGICK option is deprecated, use WITH_GRAPHICSMAGICK instead")
    set(WITH_GRAPHICSMAGICK ${GRAPHICSMAGICK} CACHE BOOL "Configure with support the for GraphicsMagick library" FORCE)
    unset(GRAPHICSMAGICK CACHE)
endif()
if(DEFINED RAW)
    message("** The RAW option is deprecated, use WITH_LIBRAW instead")
    set(WITH_LIBRAW ${RAW} CACHE BOOL "Configure with support for the libraw library" FORCE)
    unset(RAW CACHE)
endif()
if(DEFINED DEVIL)
    message("** The DEVIL option is deprecated, use WITH_DEVIL instead")
    set(WITH_DEVIL ${DEVIL} CACHE BOOL "Configure with support for the DevIL library" FORCE)
    unset(DEVIL CACHE)
endif()
if(DEFINED FREEIMAGE)
    message("** The FREEIMAGE option is deprecated, use WITH_FREEIMAGE instead")
    set(WITH_FREEIMAGE ${FREEIMAGE} CACHE BOOL "Configure with support for the FreeImage library" FORCE)
    unset(FREEIMAGE CACHE)
endif()
if(DEFINED POPPLER)
    message("** The POPPLER option is deprecated, use WITH_POPPLER instead")
    set(WITH_POPPLER ${POPPLER} CACHE BOOL "Configure with support for the Poppler library" FORCE)
    unset(POPPLER CACHE)
endif()
if(DEFINED QTPDF)
    message("** The QTPDF option is deprecated, use WITH_QTPDF instead")
    set(WITH_QTPDF ${QTPDF} CACHE BOOL "Configure with support for the QtPDF module (instead of Poppler)" FORCE)
    unset(QTPDF CACHE)
endif()
if(DEFINED LIBARCHIVE)
    message("** The LIBARCHIVE option is deprecated, use WITH_LIBARCHIVE instead")
    set(WITH_LIBARCHIVE ${LIBARCHIVE} CACHE BOOL "Configure with support for the libarchive library" FORCE)
    unset(LIBARCHIVE CACHE)
endif()
if(DEFINED VIDEO_QT)
    message("** The VIDEO_QT option is deprecated, use WITH_VIDEO_QT instead")
    set(WITH_VIDEO_QT ${VIDEO_QT} CACHE BOOL "Configure with video support through Qt" FORCE)
    unset(VIDEO_QT CACHE)
endif()
if(DEFINED VIDEO_MPV)
    message("** The VIDEO_MPV option is deprecated, use WITH_VIDEO_MPV instead")
    set(WITH_VIDEO_MPV ${VIDEO_MPV} CACHE BOOL "Configure with video support through MPV" FORCE)
    unset(VIDEO_MPV CACHE)
endif()
if(DEFINED LIBVIPS)
    message("** The LIBVIPS option is deprecated, use WITH_LIBVIPS instead")
    set(WITH_LIBVIPS ${LIBVIPS} CACHE BOOL "Configure with support for the libvips library" FORCE)
    unset(LIBVIPS CACHE)
endif()
if(DEFINED RESVG)
    message("** The RESVG option is deprecated, use WITH_RESVG instead")
    set(WITH_RESVG ${RESVG} CACHE BOOL "Configure with support for the resvg library (better SVG support)" FORCE)
    unset(RESVG CACHE)
endif()
if(DEFINED EXIV2)
    message("** The EXIV2 option is deprecated, use WITH_EXIV2 instead")
    set(WITH_EXIV2 ${EXIV2} CACHE BOOL "Configure with support for the Exiv2 library" FORCE)
    unset(EXIV2 CACHE)
endif()
if(DEFINED EXIV2_ENABLE_BMFF)
    message("** The EXIV2_ENABLE_BMFF option is deprecated, use WITH_EXIV2_ENABLE_BMFF instead")
    set(WITH_EXIV2_ENABLE_BMFF ${EXIV2_ENABLE_BMFF} CACHE BOOL "Configure with BMFF format support in Exiv2 (if available) - only needed for Exiv2 0.27.x and before" FORCE)
    unset(EXIV2_ENABLE_BMFF CACHE)
endif()
if(DEFINED LCMS2)
    message("** The LCMS2 option is deprecated, use WITH_LCMS2 instead")
    set(WITH_LCMS2 ${LCMS2} CACHE BOOL "Configure with support for the Little CMS library (advanced color management)" FORCE)
    unset(LCMS2 CACHE)
endif()
if(DEFINED MOTIONPHOTO)
    message("** The MOTIONPHOTO option is deprecated, use WITH_MOTIONPHOTO instead")
    set(WITH_MOTIONPHOTO ${MOTIONPHOTO} CACHE BOOL "Configure with support for Google Motion Photos and Apple Live Photos" FORCE)
    unset(MOTIONPHOTO CACHE)
endif()
if(DEFINED PHOTOSPHERE)
    message("** The PHOTOSPHERE option is deprecated, use WITH_PHOTOSPHERE instead")
    set(WITH_PHOTOSPHERE ${PHOTOSPHERE} CACHE BOOL "Configure with support for photo spheres and 360 degree panoramic views" FORCE)
    unset(PHOTOSPHERE CACHE)
endif()
if(DEFINED EPUB)
    message("** The EPUB option is deprecated, use WITH_EPUB instead")
    set(WITH_EPUB ${EPUB} CACHE BOOL "Configure with support for EPUB files" FORCE)
    unset(EPUB CACHE)
endif()
if(DEFINED KF6SYNTAXHIGHLIGHT)
    message("** The KF6SYNTAXHIGHLIGHT option is deprecated, use WITH_KF6SYNTAXHIGHLIGHT instead")
    set(WITH_KF6SYNTAXHIGHLIGHT ${KF6SYNTAXHIGHLIGHT} CACHE BOOL "Configure with support for KSyntaxHighlighting for syntax highlighting" FORCE)
    unset(KF6SYNTAXHIGHLIGHT CACHE)
endif()
if(DEFINED FLATPAKBUILD)
    message("** The FLATPAKBUILD option is deprecated, use WITH_FLATPAKBUILD instead")
    set(WITH_FLATPAKBUILD ${FLATPAKBUILD} CACHE BOOL "Enable this option if this is a build for Flatpak" FORCE)
    unset(FLATPAKBUILD CACHE)
endif()
if(DEFINED ADAPT_SOURCE)
    message("** The ADAPT_SOURCE option is deprecated, use WITH_ADAPTSOURCE instead")
    set(WITH_ADAPTSOURCE ${ADAPT_SOURCE} CACHE BOOL "Adapt and change the source files for the current Qt version (if necessary)" FORCE)
    unset(ADAPT_SOURCE CACHE)
endif()
##################################################################################################################
##################################################################################################################

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

if(WITH_IMAGEMAGICK)
    if(WITH_GRAPHICSMAGICK)
        set(WITH_GRAPHICSMAGICK OFF)
        message("** ImageMagick and GraphicsMagick cannot be used at the same time.")
        message("** GraphicsMagick has been automatically disabled in favour of ImageMagick.")
    endif()
endif()

#############################
####  SET list of files  ####
#############################

set(d "cplusplus")
set(previewqt_SRC "${d}/main.cpp")
set(d "cplusplus/singletons")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_scripts.cpp" "${d}/pqc_fileformats.cpp" "${d}/pqc_settings.cpp" "${d}/pqc_cache.cpp" "${d}/pqc_textprocessing.cpp")
set(d "cplusplus/files")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadfile.cpp" "${d}/pqc_providerfull.cpp" "${d}/pqc_providersvg.cpp")
set(d "cplusplus/files/plugins")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadfile_archive.cpp" "${d}/pqc_loadfile_devil.cpp" "${d}/pqc_loadfile_freeimage.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadfile_libvips.cpp" "${d}/pqc_loadfile_magick.cpp" "${d}/pqc_loadfile_poppler.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadfile_qt.cpp" "${d}/pqc_loadfile_qtpdf.cpp" "${d}/pqc_loadfile_raw.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadfile_resvg.cpp" "${d}/pqc_loadfile_video.cpp" "${d}/pqc_loadfile_xcf.cpp")
set(d "cplusplus/other")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_photosphere.cpp" "${d}/pqc_photosphereitem.cpp" "${d}/pqc_photosphererenderer.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_singleinstance.cpp" "${d}/pqc_specialactions.cpp")


set(d "cplusplus/header")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_fileformats.h" "${d}/pqc_scripts.h" "${d}/pqc_messagehandler.h" "${d}/pqc_textprocessing.h")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_photosphere.h" "${d}/pqc_photosphereitem.h" "${d}/pqc_photosphererenderer.h")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_settings.h" "${d}/pqc_singleinstance.h" "${d}/pqc_cache.h" "${d}/pqc_specialactions.h")

set(d "qml")
set(previewqt_QML "${d}/PQMainWindow.qml")
set(d "qml/components")
set(previewqt_QML ${previewqt_QML} "${d}/PQTopRow.qml" "${d}/PQImage.qml" "${d}/PQIconButton.qml" "${d}/PQTrayIcon.qml")
set(d "qml/components/imageitems")
set(previewqt_QML ${previewqt_QML} "${d}/PQImageNormal.qml" "${d}/PQImageAnimated.qml")
set(previewqt_QML ${previewqt_QML} "${d}/PQDocument.qml" "${d}/PQArchive.qml" "${d}/PQSVG.qml" "${d}/PQTxt.qml")
set(d "qml/windows")
set(previewqt_QML ${previewqt_QML} "${d}/PQSettings.qml" "${d}/PQAbout.qml" "${d}/PQHelp.qml" "${d}/PQWelcome.qml")

if(WITH_EPUB)
    set(d "qml/components/imageitems")
    set(previewqt_QML ${previewqt_QML} "${d}/PQEPUB.qml")
endif()

if(WITH_PHOTOSPHERE)
    set(previewqt_QML ${previewqt_QML} "qml/components/imageitems/PQPhotoSphere.qml")
endif()

if(WITH_VIDEO_QT)
    set(d "qml/components/imageitems")
    set(previewqt_QML ${previewqt_QML} "${d}/PQVideoQt.qml")
endif()

if(WITH_VIDEO_MPV)
    set(d "cplusplus/header")
    set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_mpvobject.h")
    set(d "cplusplus/other")
    set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_mpvobject.cpp" )
    set(d "qml/components/imageitems")
    set(previewqt_QML ${previewqt_QML} "${d}/PQVideoMpv.qml")
endif()

if(WITH_QTPDF)
    if(WITH_POPPLER)
        set(WITH_POPPLER OFF)
        message("** For displaying PDF documents you have to choose either Poppler OR QtPDF.")
        message("** Poppler has been automatically disabled in favour of QtPDF.")
    endif()
endif()

################################
#### FIND REQUIRED PACKAGES ####
################################

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Widgets Sql Core Svg Concurrent Multimedia PrintSupport DBus LinguistTools)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

#########

if(WITH_IMAGEMAGICK)
    find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
endif()

if(WITH_GRAPHICSMAGICK)
    find_package(GraphicsMagick REQUIRED)
endif()

if(WITH_LIBRAW)
    find_package(LibRaw REQUIRED)
endif()

if(WITH_DEVIL)
    find_package(DevIL REQUIRED)
endif()

if(WITH_FREEIMAGE)
    find_package(FreeImage REQUIRED)
endif()

if(WITH_POPPLER)
    find_package(Poppler COMPONENTS Qt6 REQUIRED)
endif()

if(WITH_QTPDF)
    find_package(Qt6 COMPONENTS Pdf REQUIRED)
endif()

if(WITH_LIBARCHIVE)
    find_package(LibArchive REQUIRED)
endif()

if(WITH_VIDEO_MPV)
    find_package(Libmpv REQUIRED)
endif()

if(WITH_LIBVIPS)
    pkg_search_module(GLIB REQUIRED glib-2.0 gobject-2.0)
    pkg_search_module(VIPS REQUIRED vips-cpp)
endif()

if(WITH_EXIV2)
    find_package(exiv2 REQUIRED)
endif()

if(WITH_LCMS2)
    find_package(LCMS2 REQUIRED)
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
    pkg_search_module(GLIB REQUIRED glib-2.0 gobject-2.0 gio-2.0)
endif()


###############################
#### ADDITIONAL QT OPTIONS ####
###############################

add_definitions(-DQT_NO_KEYWORDS)

# we always want to capture debug/log context information
add_definitions(-DQT_MESSAGELOGCONTEXT)

#############################
#### Add the executeable ####
#############################

if(WIN32)
    qt_add_executable(previewqt ${previewqt_SRC} img/img.qrc misc/misc.qrc windowsicons.rc)
else()
    qt_add_executable(previewqt ${previewqt_SRC} img/img.qrc misc/misc.qrc)
endif()

# set the version number
target_compile_definitions(previewqt PRIVATE PQMVERSION="${APPVERSION}")

# add qml files
qt_add_qml_module(previewqt URI src VERSION 1.0 QML_FILES ${previewqt_QML} RESOURCE_PREFIX)

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

if(WITH_IMAGEMAGICK)
    if(NOT ${ImageMagick_FOUND})
        message(FATAL_ERROR "** Unable to locate ImageMagick... is it installed?")
    else()
        message("** Using ImageMagick ${ImageMagick_VERSION_STRING}")
        # These checks are necessary to "fix" compiling PreviewQt with both ImageMagick 6 and 7 available
        if(ImageMagick_VERSION_STRING MATCHES "^7")
            string(REPLACE "libMagick++-6." "libMagick++-7." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
            string(REPLACE "ImageMagick-6" "ImageMagick-7" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
        endif()
        target_compile_definitions(previewqt PRIVATE PQMIMAGEMAGICK)
        target_compile_definitions(previewqt PRIVATE MAGICKCORE_QUANTUM_DEPTH=16)
        target_compile_definitions(previewqt PRIVATE MAGICKCORE_HDRI_ENABLE=1)
        target_link_libraries(previewqt PRIVATE ImageMagick::Magick++)
    endif()
else()
    message("** ImageMagick DISABLED")
endif()

if(WITH_GRAPHICSMAGICK)
    if(NOT ${MAGICK++_FOUND})
        message(FATAL_ERROR "** Unable to locate GraphicsMagick... is it installed?")
    else()
        message("** Using Graphicsmagick")
        target_include_directories(previewqt PRIVATE ${MAGICK++_INCLUDE_DIR})
        target_compile_definitions(previewqt PRIVATE PQMGRAPHICSMAGICK)
        target_link_libraries(previewqt PRIVATE "GraphicsMagick++")
    endif()
else()
    message("** Graphicsmagick DISABLED")
endif()

if(WITH_LIBRAW)
    if(NOT ${LibRaw_FOUND})
        message(FATAL_ERROR "** Unable to locate LibRaw... is it installed?")
    else()
        message("** Using LibRaw")
        target_compile_definitions(previewqt PRIVATE PQMRAW)
        target_include_directories(previewqt PRIVATE ${LibRaw_INCLUDE_DIR})
        target_link_libraries(previewqt PRIVATE ${LibRaw_LIBRARIES})
    endif()
else()
    message("** LibRaw DISABLED")
endif()

if(WITH_DEVIL)
    if(NOT ${DevIL_FOUND})
        message(FATAL_ERROR "** Unable to locate DevIL... is it installed?")
    else()
        message("** Using DevIL")
        target_compile_definitions(previewqt PRIVATE PQMDEVIL)
        target_link_libraries(previewqt PRIVATE DevIL::IL)
    endif()
else()
    message("** DevIL DISABLED")
endif()

if(WITH_FREEIMAGE)
    if(NOT ${FREEIMAGE_FOUND})
        message(FATAL_ERROR "** Unable to locate FreeImage... is it installed?")
    else()
        message("** Using FreeImage")
        target_include_directories(previewqt PRIVATE ${FREEIMAGE_INCLUDE_DIRS})
        target_compile_definitions(previewqt PRIVATE PQMFREEIMAGE)
        target_link_libraries(previewqt PRIVATE ${FREEIMAGE_C_LIBRARY})
    endif()
else()
    message("** FreeImage DISABLED")
endif()

if(WITH_POPPLER)
    if(NOT ${Poppler_FOUND})
        message(FATAL_ERROR "** Unable to locate Poppler... is it installed?")
    else()
        message("** Using Poppler ${Poppler_VERSION}")
        target_include_directories(previewqt PRIVATE ${Poppler_INCLUDE_DIRS})
        target_compile_definitions(previewqt PRIVATE PQMPOPPLER)
        target_link_libraries(previewqt PRIVATE ${Poppler_LIBRARIES})
    endif()
else()
    message("** Poppler DISABLED")
endif()

if(WITH_QTPDF)
    message("** Using QtPDF module")
    target_compile_definitions(previewqt PRIVATE PQMQTPDF)
endif()

if(WITH_LIBARCHIVE)
    if(NOT ${LibArchive_FOUND})
        message(FATAL_ERROR "** Unable to locate LibArchive... is it installed?")
    else()
        message("** Using LibArchive " ${LibArchive_VERSION})
        target_compile_definitions(previewqt PRIVATE PQMLIBARCHIVE)
        target_link_libraries(previewqt PRIVATE LibArchive::LibArchive)
    endif()
else()
    message("** LibArchive disabled")
endif()

if(WITH_VIDEO_QT)
    target_compile_definitions(previewqt PRIVATE PQMVIDEOQT)
endif()

if(WITH_VIDEO_MPV)
    if(NOT ${Libmpv_FOUND})
        message(FATAL_ERROR "** Unable to locate libmpv... is it installed?")
    else()
        message("** Using libmpv " ${Libmpv_VERSION})
        target_compile_definitions(previewqt PRIVATE PQMVIDEOMPV)
        target_link_libraries(previewqt PRIVATE Libmpv::Libmpv)
    endif()
else()
    message("** libmpv DISABLED")
endif()

if(WITH_LIBVIPS)
    message("** Using libvips")
    target_include_directories(previewqt PRIVATE ${GLIB_INCLUDE_DIRS})
    target_compile_definitions(previewqt PRIVATE PQMLIBVIPS)
    target_link_libraries(previewqt PRIVATE ${GLIB_LIBRARIES})
    target_link_libraries(previewqt PRIVATE "vips")
    target_link_libraries(previewqt PRIVATE "gobject-2.0")
    target_link_libraries(previewqt PRIVATE "vips-cpp")
else()
    message("** libvips DISABLED")
endif()

if(WITH_RESVG)
    target_link_libraries(previewqt PRIVATE "resvg")
    message("** Using resvg")
    target_compile_definitions(previewqt PRIVATE PQMRESVG)
else()
    message("** resvg DISABLED")
endif()

if(WITH_EXIV2)
    if(NOT ${exiv2_FOUND})
        message(FATAL_ERROR "** Unable to locate Exiv2... is it installed?")
    else()
        message("** Using Exiv2 ${exiv2_VERSION}")
        target_compile_definitions(previewqt PRIVATE PQMEXIV2)
        target_link_libraries(previewqt PRIVATE exiv2lib)
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
    endif()
else()
    message("** Exiv2 DISABLED")
endif()

if(WITH_LCMS2)
    if(NOT ${LCMS2_FOUND})
        message(FATAL_ERROR "** Unable to locate LCMS2... is it installed?")
    else()
        message("** Using LCMS2 " ${LCMS2_VERSION})
        target_include_directories(previewqt PRIVATE ${LCMS2_INCLUDE_DIR})
        target_compile_definitions(previewqt PRIVATE PQMLCMS2)
        target_link_libraries(previewqt PRIVATE ${LCMS2_LIBRARIES})
    endif()
else()
    message("** LCMS2 DISABLED")
endif()

if(WITH_MOTIONPHOTO)
    message("** Support for Motion Photos and Apple Live Photos enabled")
    target_compile_definitions(previewqt PRIVATE PQMMOTIONPHOTO)
else()
    message("** Support for Motion Photos and Apple Live Photos DISABLED")
endif()

if(WITH_PHOTOSPHERE)
    message("** Support for photo spheres enabled")
    message("** NOTE: When changing the WITH_PHOTOSPHERE option the source tree")
    message("**       might need to be cleaned before the next build")
    target_compile_definitions(previewqt PRIVATE PQMPHOTOSPHERE)
else()
    message("** Support for photo spheres DISABLED")
endif()

if(WITH_EPUB)
    message("** Support for EPUB enabled")
    target_compile_definitions(previewqt PRIVATE PQMEPUB)
else()
    message("** Support for EPUB DISABLED")
endif()

if(WITH_KF6SYNTAXHIGHLIGHT)

    message("** Using KSyntaxHighlight")
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
                message(FATAL_ERROR "Error: Source files are not adapted for Qt 6.4. You need to enable the ADAPT_SOURCE option to automatically convert them!")
            endif()
        endif()

    endforeach()

else()

    message("** KSyntaxHighlight DISABLED")

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
                message(FATAL_ERROR "Error: Source files are not adapted for Qt 6.4. You need to enable the ADAPT_SOURCE option to automatically convert them!")
            endif()
        endif()

    endforeach()

endif()

if(WITH_FLATPAKBUILD)
    message("** Enabling Flatpak workarounds")
    target_compile_definitions(previewqt PRIVATE PQMFLATPAKBUILD)
    target_include_directories(previewqt PRIVATE ${GLIB_INCLUDE_DIRS})
    target_link_libraries(previewqt PRIVATE ${GLIB_LIBRARIES})
    target_link_libraries(previewqt PRIVATE "gobject-2.0")
    target_link_libraries(previewqt PRIVATE "gio-2.0")
endif()

if(PORTABLETWEAKS)
    message("** Enabling Portable tweaks")
    target_compile_definitions(previewqt PRIVATE PQMPORTABLETWEAKS)
endif()

#######################
#### INSTALL FILES ####
#######################

if(UNIX)

    # Install executeable
    install(
        TARGETS previewqt
        DESTINATION bin/
    )

    # Install desktop file
    install(
        FILES org.previewqt.PreviewQt.desktop
        DESTINATION share/applications
    )

    # And install all the icons
    install(
        FILES icons/16x16/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/16x16/apps/
    )
    install(
        FILES icons/32x32/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/32x32/apps/
    )
    install(
        FILES icons/48x48/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/48x48/apps/
    )
    install(
        FILES icons/64x64/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/64x64/apps/
    )
    install(
        FILES icons/128x128/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/128x128/apps/
    )
    install(
        FILES icons/256x256/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/256x256/apps/
    )
    install(
        FILES icons/512x512/org.previewqt.PreviewQt.png
        DESTINATION share/icons/hicolor/512x512/apps/
    )
    if(NOT WITH_FLATPAKBUILD)
        install(
            FILES icons/1024x1024/org.previewqt.PreviewQt.png
            DESTINATION share/icons/hicolor/1024x1024/apps/
        )
    endif()
    install(
        FILES org.previewqt.PreviewQt.metainfo.xml
        DESTINATION share/metainfo/
    )

endif()


##########################
#### UNINSTALL TARGET ####
##########################

if(UNIX)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
    add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
