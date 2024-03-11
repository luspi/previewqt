/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
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

#include <pqc_messagehandler.h>
#include <pqc_scripts.h>
#include <pqc_imageformats.h>
#include <pqc_providerfull.h>
#include <pqc_settings.h>
#include <pqc_singleinstance.h>
#include <pqc_configfiles.h>

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

int main(int argc, char *argv[]) {

    // avoids warning for customizing native styles (observed in particular on Windows)
    qputenv("QT_QUICK_CONTROLS_IGNORE_CUSTOMIZATION_WARNINGS", "1");

    // Set app information
    QApplication::setApplicationName("PreviewQt");
    QApplication::setOrganizationName("");
    QApplication::setOrganizationDomain("photoqt.org");
    QApplication::setApplicationVersion(PQMVERSION);
    QApplication::setQuitOnLastWindowClosed(true);

    // custom message handler for qDebug/qLog/qInfo/etc.
    qInstallMessageHandler(pqcMessageHandler);

    // make sure config directory exists and contains imageformats file
    if(!QFileInfo::exists(PQCConfigFiles::CONFIG_DIR())) {
        QDir dir(PQCConfigFiles::CONFIG_DIR());
        if(!dir.mkpath(PQCConfigFiles::CONFIG_DIR())) {
            qFatal() << "Error creating config directory!";
            qFatal() << "Unable to continue.";
            std::exit(1);
        }
        if(!dir.mkpath(PQCConfigFiles::CACHE_DIR())) {
            qCritical() << "Error creating cache directory!";
            qCritical() << "Continuing, but not everything might work.";
        }
        if(!QFileInfo::exists(PQCConfigFiles::IMAGEFORMATS_DB())) {
            if(!QFile::copy(":/imageformats.db", PQCConfigFiles::IMAGEFORMATS_DB())) {
                qFatal() << "Unable to create default imageformats database!";
                std::exit(1);
            } else {
                QFile file(PQCConfigFiles::IMAGEFORMATS_DB());
                file.setPermissions(file.permissions()|QFileDevice::WriteOwner);
            }
        }
    }


    PQCSingleInstance app(argc, argv);

#ifdef PQMVIDEOMPV
    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");
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

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/src/qml/PQMainWindow.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    qmlRegisterSingletonInstance("PQCSettings", 1, 0, "PQCSettings", &PQCSettings::get());
    qmlRegisterSingletonInstance("PQCScripts", 1, 0, "PQCScripts", &PQCScripts::get());
    qmlRegisterSingletonInstance("PQCImageFormats", 1, 0, "PQCImageFormats", &PQCImageFormats::get());

    engine.addImageProvider("full", new PQCProviderFull);

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
