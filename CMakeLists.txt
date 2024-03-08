#############################################
# CMakeLists for PreviewQt: http://photoqt.org #
#############################################

cmake_minimum_required(VERSION 3.16)
project(previewqt LANGUAGES CXX)

##################################################################
####  GLOBAL VERSION STRING FOR ALL FILES (INCLUDING CPP/QML) ####
##################################################################

SET(APPVERSION "1.0")
add_definitions(-DPQMVERSION="${APPVERSION}")

########################################
####  SOME SETTINGS/FLAGS FOR CMAKE ####
########################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#############################
####  SET list of files  ####
#############################

set(d "cplusplus")
set(previewqt_SRC "${d}/main.cpp")
set(d "cplusplus/singletons")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_scripts.cpp" "${d}/pqc_imageformats.cpp")
set(d "cplusplus/images")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage.cpp" "${d}/pqc_providerfull.cpp")
set(d "cplusplus/images/plugins")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage_qt.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage_archive.cpp" "${d}/pqc_loadimage_devil.cpp" "${d}/pqc_loadimage_freeimage.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage_libvips.cpp" "${d}/pqc_loadimage_magick.cpp" "${d}/pqc_loadimage_poppler.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage_qt.cpp" "${d}/pqc_loadimage_qtpdf.cpp" "${d}/pqc_loadimage_raw.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage_resvg.cpp" "${d}/pqc_loadimage_unrar.cpp" "${d}/pqc_loadimage_video.cpp")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_loadimage_xcf.cpp")
set(d "cplusplus/other")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_mpvobject.cpp" "${d}/pqc_photosphere.cpp" "${d}/pqc_photosphereitem.cpp" "${d}/pqc_photosphererenderer.cpp")


set(d "cplusplus/header")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_imageformats.h" "${d}/pqc_scripts.h" "${d}/pqc_messagehandler.h")
set(previewqt_SRC ${previewqt_SRC} "${d}/pqc_mpvobject.h" "${d}/pqc_photosphere.h" "${d}/pqc_photosphereitem.h" "${d}/pqc_photosphererenderer.h")

set(d "qml")
set(previewqt_QML "${d}/PQMainWindow.qml")
set(d "qml/elements")
set(previewqt_QML ${previewqt_QML} "${d}/PQIconButton.qml")
set(d "qml/components")
set(previewqt_QML ${previewqt_QML} "${d}/PQTopRow.qml" "${d}/PQImage.qml")
set(d "qml/components/imageitems")
set(previewqt_QML ${previewqt_QML} "${d}/PQImageNormal.qml" "${d}/PQVideoQt.qml" "${d}/PQVideoMpv.qml" "${d}/PQImageAnimated.qml")
set(previewqt_QML ${previewqt_QML} "${d}/PQDocument.qml" "${d}/PQArchive.qml")

if(PHOTOSPHERE)
    set(previewqt_QML ${previewqt_QML} "qml/components/imageitems/PQPhotoSphere.qml")
endif()

#############################################
#### OPTIONS THAT CAN BE SET BY THE USER ####
#############################################

option(RAW "Use libraw library" ON)
option(GRAPHICSMAGICK "Use GraphicsMagick library" OFF)
option(IMAGEMAGICK "Use ImageMagick library" ON)
option(DEVIL "Use DevIL library" ON)
option(FREEIMAGE "Use FreeImage library" ON)
option(POPPLER "Use Poppler library" ON)
option(QTPDF "Use QtPDF module instead of Poppler" OFF)
option(LIBARCHIVE "Enable features that require libarchive" ON)
option(VIDEO_QT "Enable video support through Qt" ON)
option(VIDEO_MPV "Enable video support through MPV" ON)
option(LIBVIPS "Use libvips" OFF)
option(RESVG "Replace Qt's SVG engine with resvg for better SVG support" OFF)

option(EXIV2 "Use exiv2 library" ON)
option(EXIV2_ENABLE_BMFF "Enable BMFF format support in Exiv2 (if available)" ON)

option(MOTIONPHOTO "Enable support for Google Motion Photos and Apple Live Photos" ON)
option(PHOTOSPHERE "Enable support for photo spheres and 360 degree panoramic views" ON)

################################
#### FIND REQUIRED PACKAGES ####
################################

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick Widgets Svg Sql)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

if(IMAGEMAGICK)
    find_package(ImageMagick COMPONENTS Magick++)
    if(GRAPHICSMAGICK)
        set(GRAPHICSMAGICK OFF)
        message("** ImageMagick and GraphicsMagick cannot be used at the same time.")
        message("** GraphicsMagick has been automatically disabled in favour of ImageMagick.")
    endif()
endif()

if(GRAPHICSMAGICK)
    find_package(GraphicsMagick)
endif()

if(EXIV2)
    find_package(exiv2 REQUIRED)
endif()

if(RAW)
    find_package(LibRaw)
endif()

if(NOT WIN32)
    if(FREEIMAGE)
        find_package(FreeImage)
    endif()
endif()

if(LIBARCHIVE)
    find_package(LibArchive)
endif()

if(NOT WIN32)
    if(POPPLER)
        find_package(Poppler COMPONENTS Qt6)
    endif()
endif()

if(DEVIL)
    find_package(DevIL)
endif()

if(CHROMECAST)
    find_package(Python3 COMPONENTS Interpreter)
endif()

if(LIBVIPS)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB REQUIRED glib-2.0 gobject-2.0)
endif()

if(VIDEO_MPV)
    find_package(Libmpv)
    # TODO
    # set(previewqt_SOURCES ${previewqt_SOURCES} ${previewqt_libmpv_SOURCES})
endif()

if(QTPDF)
    find_package(Qt6 COMPONENTS Pdf REQUIRED)
endif()

###############################
#### ADDITIONAL QT OPTIONS ####
###############################

# we always want to capture debug/log context information
add_definitions(-DQT_MESSAGELOGCONTEXT)

#############################
#### Add the executeable ####
#############################

qt_add_executable(previewqt ${previewqt_SRC} img/img.qrc misc/misc.qrc)

# add qml files
qt_add_qml_module(previewqt URI src VERSION 1.0 QML_FILES ${previewqt_QML} RESOURCE_PREFIX)

# set some properties for executable
set_target_properties(previewqt PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION "${APPVERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${APPVERSION}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# set header files as include files
target_include_directories(previewqt PRIVATE "cplusplus/header")

# link executable
target_link_libraries(previewqt PRIVATE Qt6::Quick Qt6::Widgets Qt6::Svg Qt6::Sql)

########################
#### CUSTOM OPTIONS ####
########################

if(VIDEO_QT)
    add_definitions(-DPQMVIDEOQT)
endif()

if(LIBARCHIVE)
    if(NOT ${LibArchive_FOUND})
        message(FATAL_ERROR "** Unable to locate LibArchive... is it installed?")
    else()
        message("** Using LibArchive " ${LibArchive_VERSION})
        add_definitions(-DPQMLIBARCHIVE)
        include_directories(${LibArchive_INCLUDE_DIRS})
        target_link_libraries(previewqt PRIVATE ${LibArchive_LIBRARIES})
    endif()
else()
    message("** LibArchive disabled")
endif()

if(POPPLER)
    if(WIN32)
        message("** Using Poppler")
        add_definitions(-DPQMPOPPLER)
        target_link_libraries(previewqt PRIVATE "poppler")
        target_link_libraries(previewqt PRIVATE "poppler-qt6")
    else()
        if(NOT ${Poppler_FOUND})
            message(FATAL_ERROR "** Unable to locate Poppler... is it installed?")
        else()
            message("** Using Poppler ${Poppler_VERSION}")
            include_directories(${Poppler_INCLUDE_DIRS})
            add_definitions(-DPQMPOPPLER)
            target_link_libraries(previewqt PRIVATE ${Poppler_LIBRARIES})
        endif()
    endif()
else()
    message("** Poppler DISABLED")
endif()

if(QTPDF)
    message("** Using QtPDF module")
    add_definitions(-DPQMQTPDF)
endif()

if(DEVIL)
    if(WIN32)
        target_link_libraries(previewqt PRIVATE "DevIL")
        message("** Using DevIL")
        add_definitions(-DPQMDEVIL)
    else()
        if(NOT ${DevIL_FOUND})
            message(FATAL_ERROR "** Unable to locate DevIL... is it installed?")
        else()
            message("** Using DevIL")
            include_directories(${IL_INCLUDE_DIR})
            add_definitions(-DPQMDEVIL)
            target_link_libraries(previewqt PRIVATE ${IL_LIBRARIES})
        endif()
    endif()
else()
    message("** DevIL DISABLED")
endif()

if(FREEIMAGE)
    if(WIN32)
        target_link_libraries(previewqt PRIVATE "FreeImage")
        message("** Using FreeImage")
        add_definitions(-DPQMFREEIMAGE)
    else()
        if(NOT ${FREEIMAGE_FOUND})
            message(FATAL_ERROR "** Unable to locate FreeImage... is it installed?")
        else()
            message("** Using FreeImage")
            include_directories(${FREEIMAGE_INCLUDE_DIRS})
            add_definitions(-DPQMFREEIMAGE)
            target_link_libraries(previewqt PRIVATE ${FREEIMAGE_C_LIBRARY})
        endif()
    endif()
else()
    message("** FreeImage DISABLED")
endif()

if(RAW)
    if(NOT ${LIBRAW_FOUND})
        message(FATAL_ERROR "** Unable to locate LibRaw... is it installed?")
    else()
        message("** Using LibRaw")
        add_definitions(-DPQMRAW)
        include_directories(${LIBRAW_INCLUDE_DIR})
        if(WIN32)
            target_link_libraries(previewqt PRIVATE "libraw")
        else()
            target_link_libraries(previewqt PRIVATE "raw")
        endif()
    endif()
else()
    message("** LibRaw DISABLED")
endif()

if(GRAPHICSMAGICK)
    if(NOT ${MAGICK++_FOUND})
        message(FATAL_ERROR "** Unable to locate GraphicsMagick... is it installed?")
    else()
        message("** Using Graphicsmagick")
        include_directories(${MAGICK++_INCLUDE_DIR})
        add_definitions(-DPQMGRAPHICSMAGICK)
        target_link_libraries(previewqt PRIVATE "GraphicsMagick++")
    endif()
else()
    message("** Graphicsmagick DISABLED")
endif()

if(IMAGEMAGICK)
    if(NOT ${ImageMagick_FOUND})
        message(FATAL_ERROR "** Unable to locate ImageMagick... is it installed?")
    else()
        message("** Using ImageMagick ${ImageMagick_VERSION_STRING}")
        # These checks are necessary to "fix" compiling preViewQt with both ImageMagick 6 and 7 available
        if(ImageMagick_VERSION_STRING MATCHES "^6")
            string(REPLACE "libMagick++-7." "libMagick++-6." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
            string(REPLACE "ImageMagick-7" "ImageMagick-6" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
        else()
            string(REPLACE "libMagick++-6." "libMagick++-7." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
            string(REPLACE "ImageMagick-6" "ImageMagick-7" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
        endif()
        include_directories("${ImageMagick_INCLUDE_DIRS}")
        add_definitions(-DPQMIMAGEMAGICK)
        add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
        add_definitions(-DMAGICKCORE_HDRI_ENABLE=1)
        target_link_libraries(previewqt PRIVATE "${ImageMagick_LIBRARIES}")
    endif()
else()
    message("** ImageMagick DISABLED")
endif()

if(LIBVIPS)
    message("** Using libvips")
    include_directories(${GLIB_INCLUDE_DIRS})
    add_definitions(-DPQMLIBVIPS)
    target_link_libraries(previewqt PRIVATE ${GLIB_LIBRARIES})
    target_link_libraries(previewqt PRIVATE "vips")
    target_link_libraries(previewqt PRIVATE "gobject-2.0")
    target_link_libraries(previewqt PRIVATE "vips-cpp")
else()
    message("** libvips DISABLED")
endif()

if(VIDEO_MPV)
    if(NOT ${Libmpv_FOUND})
        message(FATAL_ERROR "** Unable to locate libmpv... is it installed?")
    else()
        message("** Using libmpv " ${Libmpv_VERSION})
        include_directories(${Libmpv_INCLUDE_DIRS})
        add_definitions(-DPQMVIDEOMPV)
        target_link_libraries(previewqt PRIVATE ${Libmpv_LIBRARIES})
    endif()
else()
    message("** libmpv DISABLED")
endif()

if(RESVG)
    target_link_libraries(previewqt PRIVATE "resvg")
    message("** Using resvg")
    add_definitions(-DPQMRESVG)
else()
    message("** resvg DISABLED")
endif()

if(EXIV2)
    if(NOT ${exiv2_FOUND})
        message(FATAL_ERROR "** Unable to locate Exiv2... is it installed?")
    else()
        message("** Using Exiv2 ${exiv2_VERSION}")
        include_directories(${EXIV2_INCLUDE_DIR})
        add_definitions(-DPQMEXIV2)
        target_link_libraries(previewqt PRIVATE "exiv2")
        if(EXIV2_ENABLE_BMFF)
            add_definitions(-DPQMEXIV2_ENABLE_BMFF)
        endif()
        if(WIN32)
            add_definitions("-DNOMINMAX")
        endif()
        # if exiv2 0.27.x is used some c++ features removed in c++17 need to be reenabled
        if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
            if(${exiv2_VERSION} MATCHES "^0\.27\.")
                message("** Enabling C++ features removed in C++17 for Exiv2 0.27.x")
                message("** Please update Exiv2 to at least 0.28.x!")
                add_definitions(-D_HAS_AUTO_PTR_ETC=1)
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

if(MOTIONPHOTO)
    message("** Support for Motion Photos and Apple Live Photos enabled")
    add_definitions(-DPQMMOTIONPHOTO)
else()
    message("** Support for Motion Photos and Apple Live Photos DISABLED")
endif()

if(PHOTOSPHERE)
    message("** Support for photo spheres enabled")
    add_definitions(-DPQMPHOTOSPHERE)
else()
    message("** Support for photo spheres DISABLED")
endif()


#######################
#### INSTALL FILES ####
#######################

if(UNIX)

    include(GNUInstallDirs)
    install(TARGETS previewqt
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()
