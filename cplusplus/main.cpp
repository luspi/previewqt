/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
 ** Contact: https://previewqt.org                                       **
 **                                                                      **
 ** This file is part of PreviewQt.                                      **
 **                                                                      **
 ** PreviewQt is free software: you can redistribute it and/or modify    **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PreviewQt is distributed in the hope that it will be useful,         **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.    **
 **                                                                      **
 **************************************************************************/

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFileInfo>
#include <QDir>
#include <clocale>

#ifdef PQMEXIV2
    #ifdef PQMEXIV2_ENABLE_BMFF
        #define EXV_ENABLE_BMFF
    #endif
#endif

// This needs to come early (in particular before the FreImage header)
#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

#include <pqc_messagehandler.h>
#include <pqc_scripts.h>
#include <pqc_fileformats.h>
#include <pqc_providerfull.h>
#include <pqc_providersvg.h>
#include <pqc_settings.h>
#include <pqc_singleinstance.h>
#include <pqc_configfiles.h>
#include <pqc_cache.h>
#include <pqc_textprocessing.h>

#ifdef Q_OS_WIN
#include <QQuickWindow>
#endif

#ifdef PQMGRAPHICSMAGICK
#include <GraphicsMagick/Magick++.h>
#endif

#ifdef PQMIMAGEMAGICK
#include <Magick++.h>
#endif

#ifdef PQMDEVIL
#include <IL/il.h>
#endif

#ifdef PQMLIBVIPS
#include <vips/vips.h>
#endif

#ifdef PQMVIDEOMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMFREEIMAGE
#include <FreeImage.h>
#endif

#ifdef PQMPHOTOSPHERE
#include <pqc_photosphere.h>
#endif

#ifdef PQMVIDEOMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMEPUB
#include <QtWebEngineQuick/QtWebEngineQuick>
#endif

#ifdef PQMEXIV2
#include <exiv2/exiv2.hpp>
#endif

int main(int argc, char *argv[]) {

#ifdef Q_OS_WIN

#ifdef PQMEXIV2
    // Exiv2 0.28.x and above needs this locale in order to support proper unicode (e.g., CJK characters) in file names/paths
    setlocale(LC_ALL, ".UTF8");
#endif

    QFileInfo f(argv[0]);
    qputenv("PATH", QString("%1;%2").arg(qgetenv("PATH"),f.absolutePath().replace("/", "\\")).toLocal8Bit());
    qputenv("MAGICK_CODER_MODULE_PATH", QString("%1").arg(f.absolutePath().replace("/", "\\") + "\\imagemagick\\coders").toLocal8Bit());
    qputenv("MAGICK_FILTER_MODULE_PATH", QString("%1").arg(f.absolutePath().replace("/", "\\") + "\\imagemagick\\filters").toLocal8Bit());

    // This allows for semi-transparent windows
    // By default Qt6 uses Direct3D which does not seem to support this
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif

#ifdef PQMPORTABLETWEAKS
    if(argc > 1) {
        qputenv("PREVIEWQT_EXE_BASEDIR", argv[1]);
        // create directory and set hidden attribute
#ifdef Q_OS_WIN
        QString folder = QString("%1/previewqt-data").arg(argv[1]);
        QDir dir;
        dir.mkdir(folder);
        SetFileAttributesA(dir.toNativeSeparators(folder).toLocal8Bit(), FILE_ATTRIBUTE_HIDDEN);
#else
        QString folder = QString("%1/.previewqt-data").arg(argv[1]);
        QDir dir;
        dir.mkdir(folder);
#endif
    } else {
#ifndef Q_OS_WIN
        QFileInfo f(argv[0]);
#endif
        qputenv("PREVIEWQT_EXE_BASEDIR", f.absolutePath().toLocal8Bit());
    }
#endif

#ifdef PQMEPUB
    QtWebEngineQuick::initialize();
#endif

    // avoids warning for customizing native styles (observed in particular on Windows)
    qputenv("QT_QUICK_CONTROLS_IGNORE_CUSTOMIZATION_WARNINGS", "1");

    // Set app information
    QApplication::setApplicationName("PreviewQt");
    QApplication::setOrganizationName("");
    QApplication::setOrganizationDomain("previewqt.org");
    QApplication::setApplicationVersion(PQMVERSION);
    QApplication::setQuitOnLastWindowClosed(true);

    // custom message handler for qDebug/qLog/qInfo/etc.
    qInstallMessageHandler(pqcMessageHandler);

    // make sure config directory exists
    if(!QFileInfo::exists(PQCConfigFiles::get().CONFIG_DIR())) {
        QDir dir(PQCConfigFiles::get().CONFIG_DIR());
        if(!dir.mkpath(PQCConfigFiles::get().CONFIG_DIR())) {
            qCritical() << "Error creating config directory!";
            qCritical() << "Unable to continue.";
            std::exit(1);
        }
        if(!dir.mkpath(PQCConfigFiles::get().CACHE_DIR())) {
            qCritical() << "Error creating cache directory!";
            qCritical() << "Continuing, but not everything might work.";
        }
    }

    // make sure the fileformats database exists
    // if only the old database exist, attempt to copy it over (this change happened for v4.0)
    if(!QFileInfo::exists(PQCConfigFiles::get().FILEFORMATS_DB())) {
        bool copyNewDB = true;
        if(QFileInfo::exists(PQCConfigFiles::get().IMAGEFORMATS_DB())) {
            if(!QFile::copy(PQCConfigFiles::get().IMAGEFORMATS_DB(), PQCConfigFiles::get().FILEFORMATS_DB()))
                qWarning() << "Unable to copy imageformats.db to fileformats.db. Attempting to create new database file";
            else
                copyNewDB = false;
        }
        if(copyNewDB) {
            if(!QFile::copy(":/fileformats.db", PQCConfigFiles::get().FILEFORMATS_DB())) {
                qCritical() << "Unable to create default fileformats database!";
                std::exit(1);
            } else {
                QFile file(PQCConfigFiles::get().FILEFORMATS_DB());
                file.setPermissions(file.permissions()|QFileDevice::WriteOwner);
            }
        }
    }

#ifdef PQMPORTABLETWEAKS
    if(argc > 1) {
        for(int i = 2; i < argc; ++i) {
            argv[i-1] = argv[i];
        }
        argc -= 1;
    }
#endif

    PQCSingleInstance app(argc, argv);

#ifdef PQMVIDEOMPV
    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");
#endif

#ifdef PQMEXIV2
    #if EXIV2_TEST_VERSION(0, 28, 0)
        // In this case Exiv2::enableBMFF() defaults to true
        // and the call to it is deprecated
    #else
        #ifdef PQMEXIV2_ENABLE_BMFF
            Exiv2::enableBMFF(true);
        #endif
    #endif
#endif

// only one of them will be defined at a time
#if defined(PQMGRAPHICSMAGICK) || defined(PQMIMAGEMAGICK)
    // Initialise Magick as early as possible
    // this needs to happen BEFORE startup check as this might call into Magick
    Magick::InitializeMagick(*argv);
#endif

#ifdef PQMDEVIL
    ilInit();
#endif

#ifdef PQMFREEIMAGE
    FreeImage_Initialise();
#endif

#ifdef PQMLIBVIPS
    VIPS_INIT(argv[0]);
#endif

    PQCFileFormats::get().validate();

    // Check for upgrade to PreviewQt
    if(PQCScripts::get().isUpgrade()) {

        // Validate image formats database
        PQCFileFormats::get().validate();

        // Update stored version number
        PQCSettings::get().setVersion(PQMVERSION);

    }

    QQmlApplicationEngine engine;
    const QUrl url("qrc:/src/qml/PQMainWindow.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    qmlRegisterSingletonInstance("PQCSettings", 1, 0, "PQCSettings", &PQCSettings::get());
    qmlRegisterSingletonInstance("PQCCache", 1, 0, "PQCCache", &PQCCache::get());
    qmlRegisterSingletonInstance("PQCScripts", 1, 0, "PQCScripts", &PQCScripts::get());
    qmlRegisterSingletonInstance("PQCFileFormats", 1, 0, "PQCFileFormats", &PQCFileFormats::get());
    qmlRegisterSingletonInstance("PQCTextProcessing", 1, 0, "PQCTextProcessing", &PQCTextProcessing::get());

    engine.addImageProvider("full", new PQCProviderFull);
    engine.addImageProvider("svg", new PQCProviderSVG);

#ifdef PQMPHOTOSPHERE
    qmlRegisterType<PQCPhotoSphere>("PQCPhotoSphere", 1, 0, "PQCPhotoSphere");
#endif

#ifdef PQMVIDEOMPV
    qmlRegisterType<PQCMPVObject>("PQCMPVObject", 1, 0, "PQCMPVObject");
#endif

    engine.load(url);

    int ret = app.exec();

#ifdef PQMFREEIMAGE
    FreeImage_DeInitialise();
#endif

#ifdef PQMLIBVIPS
    vips_shutdown();
#endif

    return ret;

}
