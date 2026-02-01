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

#include <QImageReader>
#include <QFileInfo>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QMessageBox>
#include <QImageWriter>
#include <QMimeDatabase>
#include <pqc_fileformats.h>
#include <pqc_configfiles.h>

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
#include <Magick++/CoderInfo.h>
#include <Magick++/Exception.h>
#endif

PQCFileFormats::PQCFileFormats() {

    // connect to database
    if(QSqlDatabase::isDriverAvailable("QSQLITE3"))
        db = QSqlDatabase::addDatabase("QSQLITE3", "imageformats");
    else if(QSqlDatabase::isDriverAvailable("QSQLITE"))
        db = QSqlDatabase::addDatabase("QSQLITE", "imageformats");
    db.setDatabaseName(PQCConfigFiles::get().FILEFORMATS_DB());

    QFileInfo infodb(PQCConfigFiles::get().FILEFORMATS_DB());

    if(!infodb.exists() || !db.open()) {

        qWarning() << "ERROR opening database:" << db.lastError().text();
        qWarning() << "Will load built-in read-only database of imageformats";

        db.setConnectOptions("QSQLITE_OPEN_READONLY");

        QString tmppath = QStandardPaths::writableLocation(QStandardPaths::TempLocation)+"/imageformats.db";

        if(QFile::exists(tmppath))
            QFile::remove(tmppath);

        if(!QFile::copy(":/imageformats.db", tmppath)) {
            //: This is the window title of an error message box
            QMessageBox::critical(0, QCoreApplication::translate("PQCFileFormats", "ERROR getting default image formats"),
                                  QCoreApplication::translate("PQCFileFormats", "Not even a read-only version of the database of default image formats could be opened.") + QCoreApplication::translate("PQCFileFormats", "Something went terribly wrong somewhere!"));
            qCritical() << "ERROR copying read-only default database!";
            qApp->quit();
            return;
        }

        QFile f(tmppath);
        f.setPermissions(f.permissions()|QFileDevice::WriteOwner);

        db.setDatabaseName(tmppath);

        if(!db.open()) {
            QMessageBox::critical(0, QCoreApplication::translate("PQCFileFormats", "ERROR getting default image formats"),
                                  QCoreApplication::translate("PQCFileFormats", "Not even a read-only version of the database of default image formats could be opened.") + QCoreApplication::translate("PQCFileFormats", "Something went terribly wrong somewhere!"));
            qCritical() << "ERROR opening read-only default database!";
            qApp->quit();
            return;
        }

        readFromDatabase();

    } else {
        readFromDatabase();
    }

}

void PQCFileFormats::readFromDatabase() {

    qDebug() << "";

    m_num = 0;
    m_num_qt = 0;
    m_num_resvg = 0;
    m_num_libvips = 0;
    m_num_magick = 0;
    m_num_libraw = 0;
    m_num_poppler = 0;
    m_num_xcftools = 0;
    m_num_devil = 0;
    m_num_freeimage = 0;
    m_num_archive = 0;
    m_num_video = 0;
    m_num_libmpv = 0;
    m_num_ebook = 0;
    m_num_text = 0;
    m_num_audio = 0;

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
    m_formats_ebook.clear();
    m_formats_text.clear();
    m_formats_audio.clear();

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
    m_mimetypes_ebook.clear();
    m_mimetypes_text.clear();
    m_mimetypes_audio.clear();

    const QList<QByteArray> qtSupported = QImageReader::supportedImageFormats();

    QSqlQuery query("SELECT * FROM imageformats ORDER BY description ASC", db);

    while(query.next()) {

        const QString endings = query.record().value("endings").toString();
        const QString mimetypes = query.record().value("mimetypes").toString();
        const QString desc = query.record().value("description").toString();
        const QString cat = query.record().value("category").toString();
        const int enabled = 1; //query.record().value("enabled").toInt(); // In PreviewQt, all formats are always enabled
        const int qt = query.record().value("qt").toInt();
#ifdef PQMLIBVIPS
        const int libvips = query.record().value("libvips").toInt();
#endif
#ifdef PQMRESVG
        const int resvg = query.record().value("resvg").toInt();
#endif
#ifdef PQMIMAGEMAGICK
        const int imgmmagick = query.record().value("imagemagick").toInt();
#elif defined(PQMGRAPHICSMAGICK)
        const int imgmmagick = query.record().value("graphicsmagick").toInt();
#endif
#ifdef PQMRAW
        const int libraw = query.record().value("libraw").toInt();
#endif
#if defined(PQMPOPPLER) || defined(PQMQTPDF)
        const int poppler = query.record().value("poppler").toInt();
#endif
        const int xcftools = query.record().value("xcftools").toInt();
#ifdef PQMDEVIL
        const int devil = query.record().value("devil").toInt();
#endif
#ifdef PQMFREEIMAGE
        const int freeimage = query.record().value("freeimage").toInt();
#endif
#ifdef PQMLIBARCHIVE
        const int archive = query.record().value("archive").toInt();
#endif
#ifdef PQMQTMULTIMEDIA
        const int video = query.record().value("video").toInt();
#endif
#ifdef PQMLIBMPV
        const int libmpv = query.record().value("libmpv").toInt();
#endif
        const int text = (cat=="txt" ? 1 : 0);
#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
        const int audio = (cat=="aud" ? 1 : 0);
#endif
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
        const QString im_gm_magick = query.record().value("im_gm_magick").toString();
#endif
        const QString qt_formatname = query.record().value("qt_formatname").toString();

        bool supportedByAnyLibrary = false;
        bool magickToBeAdded = false;

        QStringList endingsParts = endings.split(",");

        QMimeDatabase db;
        if(mimetypes != "") {
            const QStringList allmt = mimetypes.split(",");
            for(const QString &m : allmt) {
                QMimeType mt = db.mimeTypeForName(m);
                if(!mt.globPatterns().isEmpty())
                    endingsParts << mt.globPatterns();
            }
        }

        if(qt) {
            // we check the formats against the list of supported image formats
            // this list can vary depending on which plugins are installed
            if(qtSupported.contains(qt_formatname.toUtf8())) {
                m_num_qt += 1;
                supportedByAnyLibrary = true;
                m_formats_qt << endingsParts;
                if(mimetypes != "")
                    m_mimetypes_qt << mimetypes.split(",");
            }
        }

#ifdef PQMRESVG
        if(resvg) {
            m_num_resvg += 1;
            supportedByAnyLibrary = true;
            m_formats_resvg << endingsParts;
            if(mimetypes != "")
                m_mimetypes_resvg << mimetypes.split(",");
        }
#endif

#ifdef PQMLIBVIPS
        if(libvips) {
            m_num_libvips += 1;
            supportedByAnyLibrary = true;
            m_formats_libvips << endingsParts;
            if(mimetypes != "")
                m_mimetypes_libvips << mimetypes.split(",");
        }
#endif

        QStringList validImGmMagick;

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
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
                m_num_magick += 1;
                supportedByAnyLibrary = true;
                magickToBeAdded = true;
                m_formats_magick << endingsParts;
                if(mimetypes != "")
                    m_mimetypes_magick << mimetypes.split(",");
            }
        }
#endif
#ifdef PQMRAW
        if(libraw) {
            m_num_libraw += 1;
            supportedByAnyLibrary = true;
            m_formats_libraw << endingsParts;
            if(mimetypes != "")
                m_mimetypes_libraw << mimetypes.split(",");
        }
#endif
#if defined(PQMPOPPLER) || defined(PQMQTPDF)
        if(poppler) {
            m_num_poppler += 1;
            supportedByAnyLibrary = true;
            m_formats_poppler << endingsParts;
            if(mimetypes != "")
                m_mimetypes_poppler << mimetypes.split(",");
        }
#endif
        if(xcftools) {
            m_num_xcftools += 1;
            supportedByAnyLibrary = true;
            m_formats_xcftools << endingsParts;
            if(mimetypes != "")
                m_mimetypes_xcftools << mimetypes.split(",");
        }
#ifdef PQMDEVIL
        if(devil) {
            m_num_devil += 1;
            supportedByAnyLibrary = true;
            m_formats_devil << endingsParts;
            if(mimetypes != "")
                m_mimetypes_devil << mimetypes.split(",");
        }
#endif
#ifdef PQMFREEIMAGE
        if(freeimage) {
            m_num_freeimage += 1;
            supportedByAnyLibrary = true;
            m_formats_freeimage << endingsParts;
            if(mimetypes != "")
                m_mimetypes_freeimage << mimetypes.split(",");
        }
#endif
#ifdef PQMLIBARCHIVE
        if(archive) {
            supportedByAnyLibrary = true;
            if(cat == "bok") {
                m_num_ebook += 1;
                m_formats_ebook << endingsParts;
                if(mimetypes != "")
                    m_mimetypes_ebook << mimetypes.split(",");
            } else {
                m_num_archive += 1;
                m_formats_archive << endings.split(",");
                if(mimetypes != "")
                    m_mimetypes_archive << mimetypes.split(",");
            }
        }
#endif
#ifdef PQMQTMULTIMEDIA
        if(video) {
            m_num_video += 1;
            supportedByAnyLibrary = true;
            m_formats_video << endingsParts;
            if(mimetypes != "")
                m_mimetypes_video << mimetypes.split(",");
        }
#endif
#ifdef PQMLIBMPV
        if(libmpv) {
            m_num_libmpv += 1;
            supportedByAnyLibrary = true;
            m_formats_libmpv << endingsParts;
            if(mimetypes != "")
                m_mimetypes_libmpv << mimetypes.split(",");
        }
#endif

        if(text) {
            m_num_text += 1;
            supportedByAnyLibrary = true;
            m_formats_text << endingsParts;
            if(mimetypes != "")
                m_mimetypes_text << mimetypes.split(",");
        }

#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
        if(audio) {
            m_num_audio += 1;
            supportedByAnyLibrary = true;
            m_formats_audio << endingsParts;
            if(mimetypes != "")
                m_mimetypes_audio << mimetypes.split(",");
        }
#endif

        if(supportedByAnyLibrary) {

            m_num += 1;

            if(magickToBeAdded && validImGmMagick.length() > 0) {
                for(QString &e : endingsParts) {
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

    m_formats.clear();
    m_formats << m_formats_qt;
    m_formats << m_formats_resvg;
    m_formats << m_formats_libvips;
    m_formats << m_formats_magick;
    m_formats << m_formats_libraw;
    m_formats << m_formats_poppler;
    m_formats << m_formats_xcftools;
    m_formats << m_formats_devil;
    m_formats << m_formats_freeimage;
    m_formats << m_formats_archive;
    m_formats << m_formats_video;
    m_formats << m_formats_libmpv;
    m_formats << m_formats_ebook;
    m_formats << m_formats_text;
    m_formats << m_formats_audio;

    m_mimetypes.clear();
    m_mimetypes << m_mimetypes_qt;
    m_mimetypes << m_mimetypes_resvg;
    m_mimetypes << m_mimetypes_libvips;
    m_mimetypes << m_mimetypes_magick;
    m_mimetypes << m_mimetypes_libraw;
    m_mimetypes << m_mimetypes_poppler;
    m_mimetypes << m_mimetypes_xcftools;
    m_mimetypes << m_mimetypes_devil;
    m_mimetypes << m_mimetypes_freeimage;
    m_mimetypes << m_mimetypes_archive;
    m_mimetypes << m_mimetypes_video;
    m_mimetypes << m_mimetypes_libmpv;
    m_mimetypes << m_mimetypes_ebook;
    m_mimetypes << m_mimetypes_text;
    m_mimetypes << m_mimetypes_audio;

}

void PQCFileFormats::validate() {

    qDebug() << "";

    // the db does not exist -> create it and finish
    if(!QFile::exists(PQCConfigFiles::get().FILEFORMATS_DB())) {
        if(!QFile::copy(":/imageformats.db", PQCConfigFiles::get().FILEFORMATS_DB()))
            qWarning() << "Unable to (re-)create default imageformats database";
        else {
            QFile file(PQCConfigFiles::get().FILEFORMATS_DB());
            file.setPermissions(file.permissions()|QFileDevice::WriteOwner);
        }
        return;
    }

    // here we check all the image formats
    // we do so automatically by loading the default imageformats database and check that all items there are present in the actual one

    QSqlDatabase dbinstalled;
    if(QSqlDatabase::isDriverAvailable("QSQLITE3"))
        dbinstalled = QSqlDatabase::addDatabase("QSQLITE3", "validateimageformats");
    else if(QSqlDatabase::isDriverAvailable("QSQLITE"))
        dbinstalled = QSqlDatabase::addDatabase("QSQLITE", "validateimageformats");
    dbinstalled.setDatabaseName(PQCConfigFiles::get().FILEFORMATS_DB());

    if(!dbinstalled.open())
        qWarning() << "Error opening database:" << dbinstalled.lastError().text();

    QSqlDatabase dbdefault;
    if(QSqlDatabase::isDriverAvailable("QSQLITE3"))
        dbdefault = QSqlDatabase::addDatabase("QSQLITE3", "imageformatsdefault");
    else if(QSqlDatabase::isDriverAvailable("QSQLITE"))
        dbdefault = QSqlDatabase::addDatabase("QSQLITE", "imageformatsdefault");
    else {
        qCritical() << "ERROR: SQLite driver not available. Available drivers are:" << QSqlDatabase::drivers().join(",");
        qCritical() << "PreviewQt cannot function without SQLite available.";
        qApp->quit();
        return;
    }

    // open database
    QString tmpfile = PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db";
    if(QFileInfo::exists(tmpfile) && !QFile::remove(tmpfile))
        qWarning() << "Error removing old tmp file";
    if(!QFile::copy(":/fileformats.db", PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db"))
        qWarning() << "Error copying default db to tmp file";
    QFile::setPermissions(tmpfile,
                          QFileDevice::WriteOwner|QFileDevice::ReadOwner |
                              QFileDevice::ReadGroup);
    dbdefault.setDatabaseName(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
    if(!dbdefault.open())
        qWarning() << "Error opening default database:" << dbdefault.lastError().text();

    QSqlQuery query(dbdefault);

    // get default columns
    query.prepare("PRAGMA table_info(imageformats)");
    if(!query.exec()) {
        qWarning() << "Error getting default columns:" << query.lastError().text();
        query.clear();
        dbdefault.close();
        QFile::remove(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
        return;
    }

    // loop over default columns and make sure they all exist in installed db
    while(query.next()) {
        QString col = query.value(1).toString();
        QString type = query.value(2).toString();

        QSqlQuery query2(dbinstalled);
        query2.prepare("SELECT COUNT(*) AS count FROM pragma_table_info('imageformats') WHERE name=:name");
        query2.bindValue(":name", col);
        if(!query2.exec()) {
            qWarning() << "Error checking column existence:" << query2.lastError().text();
            query2.clear();
            dbdefault.close();
            QFile::remove(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
            return;
        }
        query2.next();
        int c = query2.value(0).toInt();

        // if column does not exist, add it
        if(c == 0) {
            QSqlQuery query3(dbinstalled);
            query3.prepare(QString("ALTER TABLE imageformats ADD %1 %2").arg(col, type));
            if(!query3.exec()) {
                qCritical() << "Error adding new column:" << query3.lastError().text();
                query3.clear();
                dbdefault.close();
                QFile::remove(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
                return;
            }
            query3.clear();
        }
        query2.clear();

    }

    query.clear();

    // get reference data
    query.prepare("SELECT endings,uniqueid,mimetypes,description,category,enabled,qt,resvg,libvips,imagemagick,graphicsmagick,libraw,poppler,xcftools,devil,freeimage,archive,video,libmpv,im_gm_magick,qt_formatname FROM 'imageformats'");
    if(!query.exec()) {
        qWarning() << "Error getting default data:" << query.lastError().text();
        query.clear();
        dbdefault.close();
        QFile::remove(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
        return;
    }

    // loop over reference data
    while(query.next()) {

        int c = 0;
        const QString endings = query.value(c++).toString();
        const int uniqueid = query.value(c++).toInt();
        const QString mimetypes = query.value(c++).toString();
        const QString description = query.value(c++).toString();
        const QString category = query.value(c++).toString();

        const QString enabled = query.value(c++).toString();
        const QString qt = query.value(c++).toString();
        const QString resvg = query.value(c++).toString();
        const QString libvips = query.value(c++).toString();
        const QString imagemagick = query.value(c++).toString();

        const QString graphicsmagick = query.value(c++).toString();
        const QString libraw = query.value(c++).toString();
        const QString poppler = query.value(c++).toString();
        const QString xcftools = query.value(c++).toString();
        const QString devil = query.value(c++).toString();

        const QString freeimage = query.value(c++).toString();
        const QString archive = query.value(c++).toString();
        const QString video = query.value(c++).toString();
        const QString libmpv = query.value(c++).toString();
        const QString im_gm_magick = query.value(c++).toString();

        const QString qt_formatname = query.value(c++).toString();

        // check whether an entry with that name exists in the in-production database
        QSqlQuery check(dbinstalled);
        check.prepare("SELECT count(uniqueid) FROM imageformats WHERE uniqueid=:uniqueid");
        check.bindValue(":uniqueid", uniqueid);
        if(!check.exec()) {
            qWarning() << QString("Error checking ending (%1):").arg(endings) << check.lastError().text();
            continue;
        }
        check.next();
        int count = check.value(0).toInt();
        check.clear();

        // if entry does not exist, add it
        if(count == 0) {

            QSqlQuery insquery(dbinstalled);
            insquery.prepare("INSERT INTO imageformats (endings,uniqueid,mimetypes,description,category,enabled,qt,resvg,libvips,imagemagick,graphicsmagick,libraw,poppler,xcftools,devil,freeimage,archive,video,libmpv,im_gm_magick,qt_formatname) VALUES(:endings,:uniqueid,:mimetypes,:description,:category,:enabled,:qt,:resvg,:libvips,:imagemagick,:graphicsmagick,:libraw,:poppler,:xcftools,:devil,:freeimage,:archive,:video,:libmpv,:im_gm_magick,:qt_formatname)");
            insquery.bindValue(":endings", endings);
            insquery.bindValue(":uniqueid", uniqueid);
            insquery.bindValue(":mimetypes", mimetypes);
            insquery.bindValue(":description", description);
            insquery.bindValue(":category", category);

            insquery.bindValue(":enabled", enabled);
            insquery.bindValue(":qt", qt);
            insquery.bindValue(":resvg", resvg);
            insquery.bindValue(":libvips", libvips);
            insquery.bindValue(":imagemagick", imagemagick);

            insquery.bindValue(":graphicsmagick", graphicsmagick);
            insquery.bindValue(":libraw",libraw );
            insquery.bindValue(":poppler", poppler);
            insquery.bindValue(":xcftools", xcftools);
            insquery.bindValue(":devil", devil);

            insquery.bindValue(":freeimage", freeimage);
            insquery.bindValue(":archive", archive);
            insquery.bindValue(":video", video);
            insquery.bindValue(":libmpv", libmpv);
            insquery.bindValue(":im_gm_magick", im_gm_magick);

            insquery.bindValue(":qt_formatname", qt_formatname);

            if(!insquery.exec()) {
                qWarning() << QString("ERROR inserting missing image format '%1':").arg(endings) << insquery.lastError().text();
                continue;
            }

            // if entry does exist, make sure defaultvalue and datatype is valid
        } else {

            QSqlQuery check(dbinstalled);
            check.prepare("UPDATE imageformats SET  endings=:endings, description=:description, mimetypes=:mimetypes, category=:category, qt=:qt, resvg=:resvg, libvips=:libvips, imagemagick=:imagemagick, graphicsmagick=:graphicsmagick, libraw=:libraw, poppler=:poppler, xcftools=:xcftools, devil=:devil, freeimage=:freeimage, archive=:archive, video=:video, libmpv=:libmpv, im_gm_magick=:im_gm_magick, qt_formatname=:qt_formatname WHERE uniqueid=:uniqueid");

            check.bindValue(":endings", endings);
            check.bindValue(":uniqueid", uniqueid);
            check.bindValue(":mimetypes", mimetypes);
            check.bindValue(":description", description);
            check.bindValue(":category", category);
            check.bindValue(":qt", qt);
            check.bindValue(":resvg", resvg);
            check.bindValue(":libvips", libvips);
            check.bindValue(":imagemagick", imagemagick);
            check.bindValue(":graphicsmagick", graphicsmagick);
            check.bindValue(":libraw", libraw);
            check.bindValue(":poppler", poppler);
            check.bindValue(":xcftools", xcftools);
            check.bindValue(":devil", devil);
            check.bindValue(":freeimage", freeimage);
            check.bindValue(":archive", archive);
            check.bindValue(":video", video);
            check.bindValue(":libmpv", libmpv);
            check.bindValue(":im_gm_magick", im_gm_magick);
            check.bindValue(":qt_formatname", qt_formatname);

            if(!check.exec()) {
                qWarning() << QString("Error updating defaultvalue and datatype '%1':").arg(endings) << check.lastError().text();
                continue;
            }
            check.clear();

        }

    }

    query.clear();


    QSqlQuery queryInst(dbinstalled);
    // now we check for entries that should be removed
    queryInst.prepare("SELECT endings FROM 'imageformats'");
    if(!queryInst.exec()) {
        qWarning() << "Error getting default data (endings):" << queryInst.lastError().text();
        queryInst.clear();
        dbdefault.close();
        QFile::remove(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
        return;
    }

    QStringList toBeRemoved;

    // loop over reference data
    while(queryInst.next()) {

        const QString endings = queryInst.value(0).toString();

        QSqlQuery check(dbdefault);
        check.prepare("SELECT count(endings) FROM imageformats WHERE endings=:endings");
        check.bindValue(":endings", endings);
        if(!check.exec()) {
            qWarning() << QString("Error checking for removed endings '%1':").arg(endings) << check.lastError().text();
            continue;
        }
        check.next();
        int count = check.value(0).toInt();
        if(count == 0)
            toBeRemoved << endings;

    }

    queryInst.clear();

    for(const auto &endings : std::as_const(toBeRemoved)) {

        QSqlQuery query(dbinstalled);
        query.prepare("DELETE FROM imageformats WHERE endings=:endings");
        query.bindValue(":endings", endings);
        if(!query.exec())
            qWarning() << QString("Error removing no longer used endings '%1':").arg(endings) << query.lastError().text();
        query.clear();

    }

    dbdefault.close();

    QFile file(PQCConfigFiles::get().CACHE_DIR()+"/previewqt_tmp.db");
    if(!file.remove())
        qWarning() << "ERROR: Unable to remove ref db:" << file.errorString();

}
