/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PhotoQt.                                        **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/

#include <QImageReader>
#include <QFileInfo>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QMessageBox>
#include <QImageWriter>
#include <QMimeDatabase>
#include <vqc_imageformats.h>
#include <vqc_configfiles.h>

#if defined(VQMIMAGEMAGICK) || defined(VQMGRAPHICSMAGICK)
#include <Magick++/CoderInfo.h>
#include <Magick++/Exception.h>
#endif

VQCImageFormats::VQCImageFormats() {

    // connect to database
    if(QSqlDatabase::isDriverAvailable("QSQLITE3"))
        db = QSqlDatabase::addDatabase("QSQLITE3", "imageformats");
    else if(QSqlDatabase::isDriverAvailable("QSQLITE"))
        db = QSqlDatabase::addDatabase("QSQLITE", "imageformats");
    db.setDatabaseName(VQCConfigFiles::IMAGEFORMATS_DB());

    QFileInfo infodb(VQCConfigFiles::IMAGEFORMATS_DB());

    if(!infodb.exists() || !db.open()) {

        qWarning() << "ERROR opening database:" << db.lastError().text();
        qWarning() << "Will load built-in read-only database of imageformats";

        db.setConnectOptions("QSQLITE_OPEN_READONLY");

        QString tmppath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/imageformats.db";

        if(QFile::exists(tmppath))
            QFile::remove(tmppath);

        if(!QFile::copy(":/imageformats.db", tmppath)) {
            //: This is the window title of an error message box
            QMessageBox::critical(0, QCoreApplication::translate("VQCImageFormats", "ERROR getting default image formats"),
                                  QCoreApplication::translate("VQCImageFormats", "Not even a read-only version of the database of default image formats could be opened.") + QCoreApplication::translate("VQCImageFormats", "Something went terribly wrong somewhere!"));
            qCritical() << "ERROR copying read-only default database!";
            qApp->quit();
            return;
        }

        QFile f(tmppath);
        f.setPermissions(f.permissions()|QFileDevice::WriteOwner);

        db.setDatabaseName(tmppath);

        if(!db.open()) {
            QMessageBox::critical(0, QCoreApplication::translate("VQCImageFormats", "ERROR getting default image formats"),
                                  QCoreApplication::translate("VQCImageFormats", "Not even a read-only version of the database of default image formats could be opened.") + QCoreApplication::translate("VQCImageFormats", "Something went terribly wrong somewhere!"));
            qCritical() << "ERROR opening read-only default database!";
            qApp->quit();
            return;
        }

        readFromDatabase();

    } else {
        readFromDatabase();
    }

}

void VQCImageFormats::readFromDatabase() {

    qDebug() << "";

    m_formats.clear();
    m_formats_qt.clear();
    m_formats_resvg.clear();
    m_formats_libvips.clear();
    m_formats_magick.clear();
    m_formats_libraw.clear();
    m_formats_poppler.clear();
    m_formats_xcftools.clear();
    m_formats_devil.clear();
    m_formats_freeimage.clear();
    m_formats_archive.clear();
    m_formats_video.clear();
    m_formats_libmpv.clear();

    m_mimetypes_qt.clear();
    m_mimetypes_resvg.clear();
    m_mimetypes_libvips.clear();
    m_mimetypes_magick.clear();
    m_mimetypes_libraw.clear();
    m_mimetypes_poppler.clear();
    m_mimetypes_xcftools.clear();
    m_mimetypes_devil.clear();
    m_mimetypes_freeimage.clear();
    m_mimetypes_archive.clear();
    m_mimetypes_video.clear();
    m_mimetypes_libmpv.clear();

    const QList<QByteArray> qtSupported = QImageReader::supportedImageFormats();

    QSqlQuery query("SELECT * FROM imageformats ORDER BY description ASC", db);

    while(query.next()) {

        const QString endings = query.record().value("endings").toString();
        const QString mimetypes = query.record().value("mimetypes").toString();
        const QString desc = query.record().value("description").toString();
        const QString cat = query.record().value("category").toString();
        const int enabled = query.record().value("enabled").toInt();
        const int qt = query.record().value("qt").toInt();
#ifdef VQMLIBVIPS
        const int libvips = query.record().value("libvips").toInt();
#endif
#ifdef VQMRESVG
        const int resvg = query.record().value("resvg").toInt();
#endif
#ifdef VQMIMAGEMAGICK
        const int imgmmagick = query.record().value("imagemagick").toInt();
#elif defined(VQMGRAPHICSMAGICK)
        const int imgmmagick = query.record().value("graphicsmagick").toInt();
#endif
#ifdef VQMRAW
        const int libraw = query.record().value("libraw").toInt();
#endif
#if defined(VQMPOPPLER) || defined(VQMQTPDF)
        const int poppler = query.record().value("poppler").toInt();
#endif
        const int xcftools = query.record().value("xcftools").toInt();
#ifdef VQMDEVIL
        const int devil = query.record().value("devil").toInt();
#endif
#ifdef VQMFREEIMAGE
        const int freeimage = query.record().value("freeimage").toInt();
#endif
#ifdef VQMLIBARCHIVE
        const int archive = query.record().value("archive").toInt();
#endif
#ifdef VQMVIDEOQT
        const int video = query.record().value("video").toInt();
#endif
#ifdef VQMVIDEOMPV
        const int libmpv = query.record().value("libmpv").toInt();
#endif
#if defined(VQMIMAGEMAGICK) || defined(VQMGRAPHICSMAGICK)
        const QString im_gm_magick = query.record().value("im_gm_magick").toString();
#endif
        const QString qt_formatname = query.record().value("qt_formatname").toString();

        bool supportedByAnyLibrary = false;
        bool magickToBeAdded = false;

        QList<QVariant> all;
        all << endings;
        all << enabled;
        all << desc;
        all << cat;
        if(qt) {
            // we check the formats against the list of supported image formats
            // this list can vary depending on which plugins are installed
            if(qtSupported.contains(qt_formatname.toUtf8())) {
                supportedByAnyLibrary = true;
                all << "Qt";
                m_formats_qt << endings.split(",");
                if(mimetypes != "")
                    m_mimetypes_qt << mimetypes.split(",");
            }
        }

#ifdef VQMRESVG
        if(resvg) {
            supportedByAnyLibrary = true;
            all << "resvg";
            formats_resvg << endings.split(",");
            if(mimetypes != "")
                mimetypes_resvg << mimetypes.split(",");
        }
#endif

#ifdef VQMLIBVIPS
        if(libvips) {
            supportedByAnyLibrary = true;
            all << "libvips";
            formats_libvips << endings.split(",");
            if(mimetypes != "")
                mimetypes_libvips << mimetypes.split(",");
        }
#endif

        QStringList validImGmMagick;

#if defined(VQMIMAGEMAGICK) || defined(VQMGRAPHICSMAGICK)
        if(imgmmagick) {

            // we check with the Magick++ API to see if each format is readable
            // by default we assume it is and if either no codec is available (exception thrown)
            // or when it is reported as not readable, then we skip this format
            bool alright = true;
            if(im_gm_magick != "") {
                const QStringList tmp = im_gm_magick.split(",", Qt::SkipEmptyParts);
                for(const auto &t: tmp) {
                    try {
                        Magick::CoderInfo magickCoderInfo(t.toStdString());
                        if(magickCoderInfo.isReadable())
                            validImGmMagick << t;
                    } catch(...) {
                        // do nothing here
                    }
                }
                alright = (validImGmMagick.length()>0);
            }

            if(alright) {
                supportedByAnyLibrary = true;
                magickToBeAdded = true;
#ifdef VQMIMAGEMAGICK
                all << "ImageMagick";
#elif defined(VQMGRAPHICSMAGICK)
                all << "GraphicsMagick";
#endif
                m_formats_magick << endings.split(",");
                if(mimetypes != "")
                    m_mimetypes_magick << mimetypes.split(",");
            }
        }
#endif
#ifdef VQMRAW
        if(libraw) {
            supportedByAnyLibrary = true;
            all << "libraw";
            m_formats_libraw << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_libraw << mimetypes.split(",");
        }
#endif
#if defined(VQMPOPPLER) || defined(VQMQTPDF)
        if(poppler) {
            supportedByAnyLibrary = true;
            all << "Poppler";
            m_formats_poppler << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_poppler << mimetypes.split(",");
        }
#endif
        if(xcftools) {
            supportedByAnyLibrary = true;
            all << "XCFTools";
            m_formats_xcftools << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_xcftools << mimetypes.split(",");
        }
#ifdef VQMDEVIL
        if(devil) {
            supportedByAnyLibrary = true;
            all << "DevIL";
            m_formats_devil << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_devil << mimetypes.split(",");
        }
#endif
#ifdef VQMFREEIMAGE
        if(freeimage) {
            supportedByAnyLibrary = true;
            all << "FreeImage";
            m_formats_freeimage << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_freeimage << mimetypes.split(",");
        }
#endif
#ifdef VQMLIBARCHIVE
        if(archive) {
            supportedByAnyLibrary = true;
            all << "LibArchive";
            m_formats_archive << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_archive << mimetypes.split(",");
        }
#endif
#ifdef VQMVIDEOQT
        if(video) {
            supportedByAnyLibrary = true;
            all << "Video";
            m_formats_video << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_video << mimetypes.split(",");
        }
#endif
#ifdef VQMVIDEOMPV
        if(libmpv) {
            supportedByAnyLibrary = true;
            all << "libmpv";
            m_formats_libmpv << endings.split(",");
            if(mimetypes != "")
                m_mimetypes_libmpv << mimetypes.split(",");
        }
#endif

        if(supportedByAnyLibrary) {

            m_formats << endings.split(",");
            if(mimetypes != "")
                m_mimetypes << mimetypes.split(",");

            if(magickToBeAdded && validImGmMagick.length() > 0) {
                for(QString &e : endings.split(",")) {
                    if(magick.contains(e))
                        magick[e] = QStringList() << magick[e].toStringList() << validImGmMagick;
                    else
                        magick.insert(e, QStringList() << validImGmMagick);
                }
                for(QString &mt : mimetypes.split(",")) {
                    if(magick_mimetype.contains(mt))
                        magick_mimetype[mt] = QStringList() << magick_mimetype[mt].toStringList() << validImGmMagick;
                    else
                        magick_mimetype.insert(mt, QStringList() << validImGmMagick);
                }
            }

        }
    }

}
