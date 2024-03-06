#############################################
# CMakeLists for ViewQt: http://photoqt.org #
#############################################

cmake_minimum_required(VERSION 3.16)
project(viewqt LANGUAGES CXX)

##################################################################
####  GLOBAL VERSION STRING FOR ALL FILES (INCLUDING CPP/QML) ####
##################################################################

SET(APPVERSION "1.0")
add_definitions(-DVQMVERSION="${APPVERSION}")

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
set(viewqt_SRC "${d}/main.cpp")
set(d "cplusplus/singletons")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_scripts.cpp" "${d}/vqc_imageformats.cpp")
set(d "cplusplus/images")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage.cpp" "${d}/vqc_providerfull.cpp")
set(d "cplusplus/images/plugins")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage_qt.cpp")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage_archive.cpp" "${d}/vqc_loadimage_devil.cpp" "${d}/vqc_loadimage_freeimage.cpp")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage_libvips.cpp" "${d}/vqc_loadimage_magick.cpp" "${d}/vqc_loadimage_poppler.cpp")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage_qt.cpp" "${d}/vqc_loadimage_qtpdf.cpp" "${d}/vqc_loadimage_raw.cpp")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage_resvg.cpp" "${d}/vqc_loadimage_unrar.cpp" "${d}/vqc_loadimage_video.cpp")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_loadimage_xcf.cpp")
set(d "cplusplus/other")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_mpvobject.cpp")

set(d "cplusplus/header")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_imageformats.h" "${d}/vqc_scripts.h" "${d}/vqc_messagehandler.h")
set(viewqt_SRC ${viewqt_SRC} "${d}/vqc_mpvobject.h")

set(d "qml")
set(viewqt_QML "${d}/VQMainWindow.qml")

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

################################
#### FIND REQUIRED PACKAGES ####
################################

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick Widgets Svg Sql DBus)

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
    # set(viewqt_SOURCES ${viewqt_SOURCES} ${viewqt_libmpv_SOURCES})
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

qt_add_executable(viewqt ${viewqt_SRC})

# add qml files
qt_add_qml_module(viewqt URI src VERSION 1.0 QML_FILES ${viewqt_QML} RESOURCE_PREFIX)

# set some properties for executable
set_target_properties(viewqt PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION "${APPVERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${APPVERSION}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# set header files as include files
target_include_directories(viewqt PRIVATE "cplusplus/header")

# link executable
target_link_libraries(viewqt PRIVATE Qt6::Quick Qt6::Widgets Qt6::Svg Qt6::Sql Qt6::DBus)

########################
#### CUSTOM OPTIONS ####
########################

if(VIDEO_QT)
    add_definitions(-DVQMVIDEOQT)
endif()

if(LIBARCHIVE)
    if(NOT ${LibArchive_FOUND})
        message(FATAL_ERROR "** Unable to locate LibArchive... is it installed?")
    else()
        message("** Using LibArchive " ${LibArchive_VERSION})
        add_definitions(-DVQMLIBARCHIVE)
        include_directories(${LibArchive_INCLUDE_DIRS})
        target_link_libraries(viewqt PRIVATE ${LibArchive_LIBRARIES})
    endif()
else()
    message("** LibArchive disabled")
endif()

if(POPPLER)
    if(WIN32)
        message("** Using Poppler")
        add_definitions(-DVQMPOPPLER)
        target_link_libraries(viewqt PRIVATE "poppler")
        target_link_libraries(viewqt PRIVATE "poppler-qt6")
    else()
        if(NOT ${Poppler_FOUND})
            message(FATAL_ERROR "** Unable to locate Poppler... is it installed?")
        else()
            message("** Using Poppler ${Poppler_VERSION}")
            include_directories(${Poppler_INCLUDE_DIRS})
            add_definitions(-DVQMPOPPLER)
            target_link_libraries(viewqt PRIVATE ${Poppler_LIBRARIES})
        endif()
    endif()
else()
    message("** Poppler DISABLED")
endif()

if(QTPDF)
    message("** Using QtPDF module")
    add_definitions(-DVQMQTPDF)
endif()

if(DEVIL)
    if(WIN32)
        target_link_libraries(viewqt PRIVATE "DevIL")
        message("** Using DevIL")
        add_definitions(-DVQMDEVIL)
    else()
        if(NOT ${DevIL_FOUND})
            message(FATAL_ERROR "** Unable to locate DevIL... is it installed?")
        else()
            message("** Using DevIL")
            include_directories(${IL_INCLUDE_DIR})
            add_definitions(-DVQMDEVIL)
            target_link_libraries(viewqt PRIVATE ${IL_LIBRARIES})
        endif()
    endif()
else()
    message("** DevIL DISABLED")
endif()

if(FREEIMAGE)
    if(WIN32)
        target_link_libraries(viewqt PRIVATE "FreeImage")
        message("** Using FreeImage")
        add_definitions(-DVQMFREEIMAGE)
    else()
        if(NOT ${FREEIMAGE_FOUND})
            message(FATAL_ERROR "** Unable to locate FreeImage... is it installed?")
        else()
            message("** Using FreeImage")
            include_directories(${FREEIMAGE_INCLUDE_DIRS})
            add_definitions(-DVQMFREEIMAGE)
            target_link_libraries(viewqt PRIVATE ${FREEIMAGE_C_LIBRARY})
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
        add_definitions(-DVQMRAW)
        include_directories(${LIBRAW_INCLUDE_DIR})
        if(WIN32)
            target_link_libraries(viewqt PRIVATE "libraw")
        else()
            target_link_libraries(viewqt PRIVATE "raw")
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
        add_definitions(-DVQMGRAPHICSMAGICK)
        target_link_libraries(viewqt PRIVATE "GraphicsMagick++")
    endif()
else()
    message("** Graphicsmagick DISABLED")
endif()

if(IMAGEMAGICK)
    if(NOT ${ImageMagick_FOUND})
        message(FATAL_ERROR "** Unable to locate ImageMagick... is it installed?")
    else()
        message("** Using ImageMagick ${ImageMagick_VERSION_STRING}")
        # These checks are necessary to "fix" compiling ViewQt with both ImageMagick 6 and 7 available
        if(ImageMagick_VERSION_STRING MATCHES "^6")
            string(REPLACE "libMagick++-7." "libMagick++-6." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
            string(REPLACE "ImageMagick-7" "ImageMagick-6" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
        else()
            string(REPLACE "libMagick++-6." "libMagick++-7." ImageMagick_LIBRARIES "${ImageMagick_LIBRARIES}")
            string(REPLACE "ImageMagick-6" "ImageMagick-7" ImageMagick_INCLUDE_DIRS "${ImageMagick_INCLUDE_DIRS}")
        endif()
        include_directories("${ImageMagick_INCLUDE_DIRS}")
        add_definitions(-DVQMIMAGEMAGICK)
        add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
        add_definitions(-DMAGICKCORE_HDRI_ENABLE=1)
        target_link_libraries(viewqt PRIVATE "${ImageMagick_LIBRARIES}")
    endif()
else()
    message("** ImageMagick DISABLED")
endif()

if(LIBVIPS)
    message("** Using libvips")
    include_directories(${GLIB_INCLUDE_DIRS})
    add_definitions(-DVQMLIBVIPS)
    target_link_libraries(viewqt PRIVATE ${GLIB_LIBRARIES})
    target_link_libraries(viewqt PRIVATE "vips")
    target_link_libraries(viewqt PRIVATE "gobject-2.0")
    target_link_libraries(viewqt PRIVATE "vips-cpp")
else()
    message("** libvips DISABLED")
endif()

if(VIDEO_MPV)
    if(NOT ${Libmpv_FOUND})
        message(FATAL_ERROR "** Unable to locate libmpv... is it installed?")
    else()
        message("** Using libmpv " ${Libmpv_VERSION})
        include_directories(${Libmpv_INCLUDE_DIRS})
        add_definitions(-DVQMVIDEOMPV)
        target_link_libraries(viewqt PRIVATE ${Libmpv_LIBRARIES})
    endif()
else()
    message("** libmpv DISABLED")
endif()

if(RESVG)
    target_link_libraries(viewqt PRIVATE "resvg")
    message("** Using resvg")
    add_definitions(-DVQMRESVG)
else()
    message("** resvg DISABLED")
endif()


#######################
#### INSTALL FILES ####
#######################

if(UNIX)

    include(GNUInstallDirs)
    install(TARGETS viewqt
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()
