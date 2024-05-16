/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2024 Lukas Spies                                       **
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

#include <pqc_scripts.h>
#include <pqc_imageformats.h>
#include <pqc_configfiles.h>
#include <pqc_settings.h>

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>
#include <QStringDecoder>
#include <QCollator>
#include <QMimeDatabase>
#include <QImageReader>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QColorSpace>
#include <thread>
#include <chrono>
#include <QXmlStreamReader>
#include <QBuffer>

#ifdef PQMQTPDF
#include <QtPdf/QPdfDocument>
#include <QtPdf/QtPdf>
#endif

#ifdef PQMLIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif

#ifdef PQMRAW
#include <libraw/libraw.h>
#endif

#ifdef PQMPOPPLER
#include <poppler/qt6/poppler-version.h>
#include <poppler/qt6/poppler-qt6.h>
#endif

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
#include <Magick++/Include.h>
#endif

#ifdef PQMDEVIL
#include <IL/il.h>
#endif

#ifdef PQMFREEIMAGE
#include <FreeImage.h>
#endif

#ifdef PQMVIDEOMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMEXIV2
#include <exiv2/exiv2.hpp>
#endif

#ifdef PQMLCMS2
#include <lcms2.h>
#endif

PQCScripts::PQCScripts() {
    m_onlyWriteToTempFile = "";
    m_startupMessage = "";
}
PQCScripts::~PQCScripts() {}

QString PQCScripts::cleanPath(QString path) {

#ifdef Q_OS_WIN
    return cleanPath_windows(path);
#else
    if(path.startsWith("file:////"))
        path = path.remove(0, 8);
    else if(path.startsWith("file:///"))
        path = path.remove(0, 7);
    else if(path.startsWith("file://"))
        path = path.remove(0, 6);
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    return QDir::cleanPath(path);
#endif

}

QString PQCScripts::cleanPath_windows(QString path) {

    if(path.startsWith("file:///"))
        path = path.remove(0, 8);
    else if(path.startsWith("file://"))
        path = path.remove(0, 7);
    else if(path.startsWith("file:/"))
        path = path.remove(0, 6);
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    return QDir::cleanPath(path);

}

bool PQCScripts::fileExists(QString path) {

    qDebug() << "args: path =" << path;

    QFileInfo info(path);
    return info.exists();

}

QString PQCScripts::getFilename(QString path) {

    qDebug() << "args: path =" << path;

    return QFileInfo(QByteArray::fromPercentEncoding(path.toUtf8())).fileName();

}

QStringList PQCScripts::listArchiveContent(QString path, bool insideFilenameOnly) {

    qDebug() << "args: path =" << path;

    QStringList ret;

    const QFileInfo info(path);

#ifndef Q_OS_WIN

    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << "unrar");
    which.waitForFinished();

    if(!which.exitCode()) {

        QProcess p;
        p.start("unrar", QStringList() << "lb" << info.absoluteFilePath());

        if(p.waitForStarted()) {

            QByteArray outdata = "";

            while(p.waitForReadyRead())
                outdata.append(p.readAll());

            auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
            QStringList allfiles = QString(toUtf16(outdata)).split('\n', Qt::SkipEmptyParts);

            allfiles.sort();

            if(insideFilenameOnly) {
                for(const QString &f : std::as_const(allfiles)) {
                    if(PQCImageFormats::get().getAllFormats().contains(QFileInfo(f).suffix()))
                        ret.append(f);
                }
            } else {
                for(const QString &f : std::as_const(allfiles)) {
                    if(PQCImageFormats::get().getAllFormats().contains(QFileInfo(f).suffix()))
                        ret.append(QString("%1::ARC::%2").arg(f, path));
                }
            }

        }

    }

    // this either means there is nothing in that archive
    // or something went wrong above with unrar
    if(ret.length() == 0) {

#endif

#ifdef PQMLIBARCHIVE

        // Create new archive handler
        struct archive *a = archive_read_new();

        // We allow any type of compression and format
        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);

        // Read file
        int r = archive_read_open_filename(a, info.absoluteFilePath().toLocal8Bit().data(), 10240);

        // If something went wrong, output error message and stop here
        if(r != ARCHIVE_OK) {
            qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
            return ret;
        }

        // Loop over entries in archive
        struct archive_entry *entry;
        QStringList allfiles;
        while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

            // Read the current file entry
            // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
            QString filenameinside = QString::fromWCharArray(archive_entry_pathname_w(entry));

            // If supported file format, append to temporary list
            if((PQCImageFormats::get().getAllFormats().contains(QFileInfo(filenameinside).suffix().toLower())))
                allfiles.append(filenameinside);

        }

        // Sort the temporary list and add to global list
        allfiles.sort();

        if(insideFilenameOnly) {
            ret = allfiles;
        } else {
            for(const QString &f : std::as_const(allfiles))
                ret.append(QString("%1::ARC::%2").arg(f, path));
        }

        // Close archive
        r = archive_read_free(a);
        if(r != ARCHIVE_OK)
            qWarning() << "ERROR: archive_read_free() returned code of" << r;

#endif

#ifndef Q_OS_WIN
    }
#endif

    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setIgnorePunctuation(true);
    collator.setNumericMode(true);

    std::sort(ret.begin(), ret.end(), [&collator](const QString &file1, const QString &file2) { return collator.compare(file1, file2) < 0; });

    return ret;

}

int PQCScripts::isMotionPhoto(QString path) {

    qDebug() << "args: path =" << path;

#ifndef PQMMOTIONPHOTO
    return 0;
#endif

    // 1 = Apple Live Photos
    // 2 = Motion Photo
    // 3 = Micro Video

    QFileInfo info(path);
    const QString suffix = info.suffix().toLower();

    if(suffix == "jpg" || suffix == "jpeg" || suffix == "heic" || suffix == "heif") {

        /***********************************/
        // check for Apply Live Photos

        QString videopath = QString("%1/%2.mov").arg(info.absolutePath(), info.baseName());
        QFileInfo videoinfo(videopath);
        if(videoinfo.exists())
            return 1;

        /***********************************/
        // Access EXIV2 data

#if defined(PQMEXIV2) && defined(PQMEXIV2_ENABLE_BMFF)

#if EXIV2_TEST_VERSION(0, 28, 0)
        Exiv2::Image::UniquePtr image;
#else
        Exiv2::Image::AutoPtr image;
#endif

        try {
            image = Exiv2::ImageFactory::open(path.toStdString());
            image->readMetadata();
        } catch (Exiv2::Error& e) {
            // An error code of kerFileContainsUnknownImageType (older version: 11) means unknown file type
            // Since we always try to read any file's meta data, this happens a lot
#if EXIV2_TEST_VERSION(0, 28, 0)
            if(e.code() != Exiv2::ErrorCode::kerFileContainsUnknownImageType)
#else
            if(e.code() != 11)
#endif
                qWarning() << "ERROR reading exiv data (caught exception):" << e.what();
            else
                qDebug() << "ERROR reading exiv data (caught exception):" << e.what();

            return 0;
        }

        Exiv2::XmpData xmpData;
        try {
            xmpData = image->xmpData();
        } catch(Exiv2::Error &e) {
            qDebug() << "ERROR: Unable to read xmp metadata:" << e.what();
            return 0;
        }

        for(Exiv2::XmpData::const_iterator it_xmp = xmpData.begin(); it_xmp != xmpData.end(); ++it_xmp) {

            QString familyName = QString::fromStdString(it_xmp->familyName());
            QString groupName = QString::fromStdString(it_xmp->groupName());
            QString tagName = QString::fromStdString(it_xmp->tagName());

            /***********************************/
            // check for Motion Photo
            if(familyName == "Xmp" && groupName == "GCamera" && tagName == "MotionPhoto") {

                // check value == 1
                if(QString::fromStdString(Exiv2::toString(it_xmp->value())) == "1")
                    return 2;
            }

            /***********************************/
            // check for Micro Video

            if(familyName == "Xmp" && groupName == "GCamera" && tagName == "MicroVideo") {

                // check value == 1
                if(QString::fromStdString(Exiv2::toString(it_xmp->value())) == "1")
                    return 3;

            }

        }

#endif

    }

    return 0;

}

QString PQCScripts::extractMotionPhoto(QString path) {

    qDebug() << "args: path =" << path;

    // at this point we assume that the check for google motion photo has already been done
    // and we wont need to check again

    // the approach taken in this function is inspired by the analysis found at:
    // https://linuxreviews.org/Google_Pixel_%22Motion_Photo%22

    QFileInfo info(path);
    if(!info.exists())
        return "";

    const QString videofilename = QString("%1/motionphotos/%2.mp4").arg(PQCConfigFiles::CACHE_DIR(), info.baseName());
    if(QFileInfo::exists(videofilename)) {
        return videofilename;
    }

    // we assume header for type==2
    QStringList headerbytes = {"00000018667479706d703432",
                               "0000001c6674797069736f6d"};

    char *data = new char[info.size()];

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file for reading";
        delete[] data;
        return "";
    }

    QDataStream in(&file);
    in.readRawData(data, info.size());

    // we look for the offset of the header of size 12
    // it looks like this: 00000018667479706d703432
    for(int i = 0; i < info.size()-12; ++i) {

        // we inspect the current 3
        QByteArray firstthree(&data[i], 3);

        if(firstthree.toHex() == "000000") {

            // read the full 12 bytes
            QByteArray array(&data[i], 12);

            // if it matches we found the video
            if(headerbytes.contains(array.toHex())) {

                // get the video data
                QByteArray videodata(&data[i], info.size()-i);

                // make sure cache folder exists
                QDir dir;
                dir.mkpath(QFileInfo(videofilename).absolutePath());

                // write video to temporary file
                QFile outfile(videofilename);
                outfile.open(QIODevice::WriteOnly);
                QDataStream out(&outfile);
                out.writeRawData(videodata, info.size()-i);
                outfile.close();

                delete[] data;

                return outfile.fileName();

            }
        }
    }

    delete[] data;

    return "";

}

int PQCScripts::getExifOrientation(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMEXIV2

#if EXIV2_TEST_VERSION(0, 28, 0)
    Exiv2::Image::UniquePtr image;
#else
    Exiv2::Image::AutoPtr image;
#endif
    try {
        image  = Exiv2::ImageFactory::open(path.toStdString());
        image->readMetadata();
    } catch (Exiv2::Error& e) {
        // An error code of kerFileContainsUnknownImageType (older version: 11) means unknown file type \
        // Since we always try to read any file's meta data, this happens a lot
#if EXIV2_TEST_VERSION(0, 28, 0)
        if(e.code() != Exiv2::ErrorCode::kerFileContainsUnknownImageType)
#else
        if(e.code() != 11)
#endif
            qWarning() << "ERROR reading exif data (caught exception):" << e.what();
        else
            qDebug() << "ERROR reading exif data (caught exception):" << e.what();

        return 1;
    }

    Exiv2::ExifData exifData;

    try {
        exifData = image->exifData();
    } catch(Exiv2::Error &e) {
        qDebug() << "ERROR: Unable to read exif metadata:" << e.what();
        return 1;
    }

    Exiv2::ExifData::iterator iter = exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
    if(iter != exifData.end()) {

        const int val = QString::fromStdString(Exiv2::toString(iter->value())).toInt();
        if(val >= 1 && val <= 8)
            return val;

    }

#endif

    return 1;

}

QString PQCScripts::getBasename(QString fullpath) {

    if(fullpath == "")
        return "";

    return QFileInfo(fullpath).baseName();

}

QString PQCScripts::getDir(QString fullpath) {

    if(fullpath == "")
        return "";

    return QFileInfo(fullpath).absolutePath();

}

bool PQCScripts::isPhotoSphere(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMPHOTOSPHERE

#if defined(PQMEXIV2) && defined(PQMEXIV2_ENABLE_BMFF)

#if EXIV2_TEST_VERSION(0, 28, 0)
    Exiv2::Image::UniquePtr image;
#else
    Exiv2::Image::AutoPtr image;
#endif

    try {
        image = Exiv2::ImageFactory::open(path.toStdString());
        image->readMetadata();
    } catch (Exiv2::Error& e) {
        // An error code of kerFileContainsUnknownImageType (older version: 11) means unknown file type
        // Since we always try to read any file's meta data, this happens a lot
#if EXIV2_TEST_VERSION(0, 28, 0)
        if(e.code() != Exiv2::ErrorCode::kerFileContainsUnknownImageType)
#else
        if(e.code() != 11)
#endif
            qWarning() << "ERROR reading exiv data (caught exception):" << e.what();
        else
            qDebug() << "ERROR reading exiv data (caught exception):" << e.what();

        return false;
    }

    Exiv2::XmpData xmpData;
    try {
        xmpData = image->xmpData();
    } catch(Exiv2::Error &e) {
        qDebug() << "ERROR: Unable to read xmp metadata:" << e.what();
        return false;
    }

    for(Exiv2::XmpData::const_iterator it_xmp = xmpData.begin(); it_xmp != xmpData.end(); ++it_xmp) {

        QString familyName = QString::fromStdString(it_xmp->familyName());
        QString groupName = QString::fromStdString(it_xmp->groupName());
        QString tagName = QString::fromStdString(it_xmp->tagName());

        /***********************************/
        // check for Motion Photo
        if(familyName == "Xmp" && groupName == "GPano" && tagName == "ProjectionType") {

            // check value == equirectangular
            if(QString::fromStdString(Exiv2::toString(it_xmp->value())) == "equirectangular")
                return true;
        }

    }

#endif

#endif

    return false;

}

bool PQCScripts::isQtAtLeast6_5() {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    return true;
#endif
    return false;
}

bool PQCScripts::isMpvVideo(QString path) {

    qDebug() << "args: path =" << path;

    bool supported = false;

#ifdef PQMVIDEOMPV

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCImageFormats::get().getAllFormatsLibmpv().contains(suf)) {

        supported = true;

    } else {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(path).name();
        if(PQCImageFormats::get().getAllMimeTypesLibmpv().contains(mimetype))
            supported = true;

    }

#endif

    return supported;

}

bool PQCScripts::isQtVideo(QString path) {

    qDebug() << "args: path =" << path;

    bool supported = false;

#ifdef PQMVIDEOQT

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCImageFormats::get().getAllFormatsVideo().contains(suf)) {

        supported = true;

    } else {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(path).name();
        if(PQCImageFormats::get().getAllMimeTypesVideo().contains(mimetype))
            supported = true;

    }

#endif

    return supported;

}

bool PQCScripts::isItAnimated(QString filename) {
    QImageReader reader(filename);
    return (reader.supportsAnimation()&&reader.imageCount()>1);
}

bool PQCScripts::isPDFDocument(QString path) {

    qDebug() << "args: path =" << path;

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCImageFormats::get().getAllFormatsPoppler().contains(suf))
        return true;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(path).name();
    if(PQCImageFormats::get().getAllMimeTypesPoppler().contains(mimetype))
        return true;

    return false;

}

int PQCScripts::getDocumentPageCount(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMQTPDF

    if(path.contains("::PDF::"))
        path = path.split("::PDF::").at(1);

    QPdfDocument doc;
    doc.load(path);

    QPdfDocument::Error err = doc.error();
    if(err != QPdfDocument::Error::None) {
        qWarning() << "Error occured loading PDF";
        return 0;
    }

    return doc.pageCount();

#elif PQMPOPPLER

    std::unique_ptr<Poppler::Document> document = Poppler::Document::load(path);
    if(!document || document->isLocked()) {
        qWarning() << "Invalid PDF document, unable to load!";
        return 0;
    }

    return document->numPages();

#endif

    return 0;

}

QStringList PQCScripts::getArchiveContent(QString path) {

    qDebug() << "args: path =" << path;

    if(path.contains("::ARC::"))
        path = path.split("::ARC::")[1];

    QString theid = generateArchiveId(path);
    if(archiveContents.contains(theid)) {
        return archiveContents[theid];
    }

    QStringList ret;

    const QFileInfo info(path);

#ifndef Q_OS_WIN

    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << "unrar");
    which.waitForFinished();

    if(!which.exitCode()) {

        QProcess p;
        p.start("unrar", QStringList() << "lb" << info.absoluteFilePath());

        if(p.waitForStarted()) {

            QByteArray outdata = "";

            while(p.waitForReadyRead())
                outdata.append(p.readAll());

            auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
            QStringList allfiles = QString(toUtf16(outdata)).split('\n', Qt::SkipEmptyParts);

            allfiles.sort();

            for(const QString &f : std::as_const(allfiles)) {
                if(PQCImageFormats::get().getAllFormats().contains(QFileInfo(f).suffix()))
                    ret.append(f);
            }

        }

    }

    // this either means there is nothing in that archive
    // or something went wrong above with unrar
    if(ret.length() == 0) {

#endif

#ifdef PQMLIBARCHIVE

        // Create new archive handler
        struct archive *a = archive_read_new();

        // We allow any type of compression and format
        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);

        // Read file
        int r = archive_read_open_filename(a, info.absoluteFilePath().toLocal8Bit().data(), 10240);

        // If something went wrong, output error message and stop here
        if(r != ARCHIVE_OK) {
            qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
            return ret;
        }

        // Loop over entries in archive
        struct archive_entry *entry;
        QStringList allfiles;
        while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

            // Read the current file entry
            // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
            QString filenameinside = QString::fromWCharArray(archive_entry_pathname_w(entry));

            // If supported file format, append to temporary list
            if((PQCImageFormats::get().getAllFormats().contains(QFileInfo(filenameinside).suffix())))
                allfiles.append(filenameinside);

        }

        // Sort the temporary list and add to global list
        allfiles.sort();

        ret = allfiles;

        // Close archive
        r = archive_read_free(a);
        if(r != ARCHIVE_OK)
            qWarning() << "ERROR: archive_read_free() returned code of" << r;

#endif

#ifndef Q_OS_WIN
    }
#endif

    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setIgnorePunctuation(true);
    collator.setNumericMode(true);

    std::sort(ret.begin(), ret.end(), [&collator](const QString &file1, const QString &file2) { return collator.compare(file1, file2) < 0; });

    archiveContents[theid] = ret;

    return ret;

}

bool PQCScripts::isArchive(QString path) {

    qDebug() << "args: path =" << path;

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCImageFormats::get().getAllFormatsLibArchive().contains(suf))
        return true;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(path).name();
    if(PQCImageFormats::get().getAllMimeTypesLibArchive().contains(mimetype))
        return true;

    return false;

}

bool PQCScripts::isComicBook(QString path) {

    qDebug() << "args: path =" << path;

    const QString suffix = QFileInfo(path).suffix().toLower();

    return (suffix=="cbt" || suffix=="cbr" || suffix=="cbz" || suffix=="cb7");

}

bool PQCScripts::isEpub(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMEPUB

    const QString suf = QFileInfo(path).suffix().toLower();

    return (suf=="epub");

#endif

    return false;

}

QString PQCScripts::toPercentEncoding(QString str) {
    return QUrl::toPercentEncoding(str);
}

QString PQCScripts::fromPercentEncoding(QByteArray str) {
    return QUrl::fromPercentEncoding(str);
}

QString PQCScripts::generateArchiveId(QString path) {

    QFileInfo info(path);
    return QString("%1_%2").arg(info.lastModified().toMSecsSinceEpoch()).arg(info.absoluteFilePath());

}

QString PQCScripts::keycodeToString(Qt::KeyboardModifiers modifiers, Qt::Key keycode) {

    QStringList mods;
    QStringList keys;

    if(modifiers & Qt::ControlModifier)
        mods.append("Ctrl");
    if(modifiers & Qt::AltModifier)
        mods.append("Alt");
    if(modifiers & Qt::ShiftModifier)
        mods.append("Shift");
    if(modifiers & Qt::MetaModifier)
        mods.append("Meta");
    if(modifiers & Qt::KeypadModifier)
        mods.append("Keypad");

    switch(keycode) {
        case Qt::Key_Control:
        case Qt::Key_Alt:
        case Qt::Key_Shift:
        case Qt::Key_Meta:
            break;
        case Qt::Key_Escape:
            keys.append("Esc");
            break;
        case Qt::Key_Right:
            keys.append("Right");
            break;
        case Qt::Key_Left:
            keys.append("Left");
            break;
        case Qt::Key_Up:
            keys.append("Up");
            break;
        case Qt::Key_Down:
            keys.append("Down");
            break;
        case Qt::Key_Space:
            keys.append("Space");
            break;
        case Qt::Key_Delete:
            keys.append("Delete");
            break;
        case Qt::Key_Home:
            keys.append("Home");
            break;
        case Qt::Key_End:
            keys.append("End");
            break;
        case Qt::Key_PageUp:
            keys.append("Page Up");
            break;
        case Qt::Key_PageDown:
            keys.append("Page Down");
            break;
        case Qt::Key_Insert:
            keys.append("Insert");
            break;
        case Qt::Key_Tab:
            keys.append("Tab");
            break;
        case Qt::Key_Backtab:
            keys.append("Tab");
            break;
        case Qt::Key_Return:
            keys.append("Return");
            break;
        case Qt::Key_Enter:
            keys.append("Enter");
            break;
        case Qt::Key_Pause:
            keys.append("Pause");
            break;
        case Qt::Key_Print:
            keys.append("Print");
            break;
        case Qt::Key_SysReq:
            keys.append("SysReq");
            break;
        case Qt::Key_Clear:
            keys.append("Clear");
            break;
        case Qt::Key_CapsLock:
            keys.append("CapsLock");
            break;
        case Qt::Key_NumLock:
            keys.append("NumLock");
            break;
        case Qt::Key_ScrollLock:
            keys.append("ScrollLock");
            break;
        case Qt::Key_Super_L:
            keys.append("Super L");
            break;
        case Qt::Key_Super_R:
            keys.append("Super R");
            break;
        case Qt::Key_Menu:
            keys.append("Menu");
            break;
        case Qt::Key_Hyper_L:
            keys.append("Hyper L");
            break;
        case Qt::Key_Hyper_R:
            keys.append("Hyper R");
            break;
        case Qt::Key_Help:
            keys.append("Help");
            break;
        case Qt::Key_Direction_L:
            keys.append("Direction L");
            break;
        case Qt::Key_Direction_R:
            keys.append("Direction R");
            break;
        case Qt::Key_F1:
            keys.append("F1");
            break;
        case Qt::Key_F2:
            keys.append("F2");
            break;
        case Qt::Key_F3:
            keys.append("F3");
            break;
        case Qt::Key_F4:
            keys.append("F4");
            break;
        case Qt::Key_F5:
            keys.append("F5");
            break;
        case Qt::Key_F6:
            keys.append("F6");
            break;
        case Qt::Key_F7:
            keys.append("F7");
            break;
        case Qt::Key_F8:
            keys.append("F8");
            break;
        case Qt::Key_F9:
            keys.append("F9");
            break;
        case Qt::Key_F10:
            keys.append("F10");
            break;
        case Qt::Key_F11:
            keys.append("F11");
            break;
        case Qt::Key_F12:
            keys.append("F12");
            break;
        case Qt::Key_F13:
            keys.append("F13");
            break;
        case Qt::Key_F14:
            keys.append("F14");
            break;
        case Qt::Key_F15:
            keys.append("F15");
            break;
        case Qt::Key_F16:
            keys.append("F16");
            break;
        case Qt::Key_F17:
            keys.append("F17");
            break;
        case Qt::Key_F18:
            keys.append("F18");
            break;
        case Qt::Key_F19:
            keys.append("F19");
            break;
        case Qt::Key_F20:
            keys.append("F20");
            break;
        case Qt::Key_F21:
            keys.append("F21");
            break;
        case Qt::Key_F22:
            keys.append("F22");
            break;
        case Qt::Key_F23:
            keys.append("F23");
            break;
        case Qt::Key_F24:
            keys.append("F24");
            break;
        case Qt::Key_F25:
            keys.append("F25");
            break;
        case Qt::Key_F26:
            keys.append("F26");
            break;
        case Qt::Key_F27:
            keys.append("F27");
            break;
        case Qt::Key_F28:
            keys.append("F28");
            break;
        case Qt::Key_F29:
            keys.append("F29");
            break;
        case Qt::Key_F30:
            keys.append("F30");
            break;
        case Qt::Key_F31:
            keys.append("F31");
            break;
        case Qt::Key_F32:
            keys.append("F32");
            break;
        case Qt::Key_F33:
            keys.append("F33");
            break;
        case Qt::Key_F34:
            keys.append("F34");
            break;
        case Qt::Key_F35:
            keys.append("F35");
            break;
        default: {
            const QString k = QKeySequence(keycode).toString();
            if(k != "")
                keys.append(k);
        }

    }

    QString ret = mods.join("+");
    if(ret != "")
        ret += "+";
    ret += keys.join("+");

    return ret;

}

bool PQCScripts::openInDefault(QString path) {

    qDebug() << "args: path =" << path;

    if(path == "")
        return true;

    QFileInfo info(path);
    const QString suffix = info.suffix().toLower();

    QString exe = "photoqt";

    if(PQCImageFormats::get().getAllFormatsPoppler().contains(suffix)) {

        exe = PQCSettings::get().getDefaultAppDocuments();

    } else if(PQCImageFormats::get().getAllFormatsLibArchive().contains(suffix) &&
               (suffix == "cbr" || suffix == "cbt" || suffix == "cbz" || suffix == "cb7")) {

        exe = PQCSettings::get().getDefaultAppComicBooks();

    } else if(PQCImageFormats::get().getAllFormatsEBook().contains(suffix)) {

        exe = PQCSettings::get().getDefaultAppEBooks();

    } else if(PQCImageFormats::get().getAllFormatsLibArchive().contains(suffix)) {

        exe = PQCSettings::get().getDefaultAppArchives();

    } else if(PQCImageFormats::get().getAllFormatsLibmpv().contains(suffix) || PQCImageFormats::get().getAllFormatsVideo().contains(suffix)) {

        exe = PQCSettings::get().getDefaultAppVideos();

    } else if(PQCImageFormats::get().getAllFormatsQt().contains(suffix) || PQCImageFormats::get().getAllFormatsFreeImage().contains(suffix) ||
        PQCImageFormats::get().getAllFormatsDevIL().contains(suffix) || PQCImageFormats::get().getAllFormatsLibRaw().contains(suffix) ||
        PQCImageFormats::get().getAllFormatsLibVips().contains(suffix) || PQCImageFormats::get().getAllFormatsMagick().contains(suffix) ||
        PQCImageFormats::get().getAllFormatsResvg().contains(suffix) || PQCImageFormats::get().getAllFormatsXCFTools().contains(suffix)) {

        exe = PQCSettings::get().getDefaultAppImages();

    }

    QProcess proc;
    proc.setProgram(QDir::toNativeSeparators(exe));
    proc.setArguments({cleanPath(path)});
    if(!proc.startDetached()) {
        qWarning() << "Could not start process!";
        return false;
    }

    if(PQCSettings::get().getCloseAfterDefaultApp())
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return true;

}

bool PQCScripts::amIOnWindows() {
#ifdef Q_OS_WIN
    return true;
#endif
    return false;
}

bool PQCScripts::doesFileExist(QString path) {

    qDebug() << "args: path =" << path;

    return QFileInfo::exists(path);

}

void PQCScripts::deleteTemporaryFiles() {

    qDebug() << "";

    QDir dir(PQCConfigFiles::CACHE_DIR() + "/archive");
    dir.removeRecursively();

    QDir dir2(PQCConfigFiles::CACHE_DIR() + "/motionphotos");
    dir2.removeRecursively();

}

bool PQCScripts::isFileSupported(QString path) {

    qDebug() << "args: path =" << path;

    if(!QFileInfo::exists(path))
        return false;

    const QString suffix = QFileInfo(path).suffix().toLower();
    return PQCImageFormats::get().getAllFormats().contains(suffix);

}

QString PQCScripts::getVersion() {
    return PQMVERSION;
}

QString PQCScripts::getConfigInfo(bool formatHTML) {

    qDebug() << "";

    QString bold1 = "";
    QString bold2 = "";
    QString nl = "\n";
    QString spacing = "    ";
    if(formatHTML) {
        bold1 = "<b>";
        bold2 = "</b>";
        nl = "<br>";
        spacing = "&nbsp;&nbsp;&nbsp;";
    }

    QString txt = "";

    txt += QString(" - Compiled with %1Qt %2%3, running with %4Qt %5%6%7").arg(bold1, QT_VERSION_STR, bold2, bold1, qVersion(), bold2, nl);

#ifdef PQMEXIV2
    txt += QString(" - %1Exiv2%2: %3%4").arg(bold1, bold2, Exiv2::version(), nl);
#endif

#ifdef PQMRAW
    txt += QString(" - %1LibRaw%2: %3%4").arg(bold1, bold2, LibRaw::version(), nl);
#endif

#ifdef PQMPOPPLER
    txt += QString(" - %1Poppler%2: %3%4").arg(bold1, bold2, POPPLER_VERSION, nl);
#endif

#ifdef PQMQTPDF
    txt += QString(" - %1QtPDF%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMLIBARCHIVE
    txt += QString(" - %1LibArchive%2: %3%4").arg(bold1, bold2, ARCHIVE_VERSION_ONLY_STRING, nl);
#endif

#ifdef PQMIMAGEMAGICK
    txt += QString(" - %1ImageMagick%2: %3%4").arg(bold1, bold2, MagickLibVersionText, nl);
#endif

#ifdef PQMGRAPHICSMAGICK
    txt += QString(" - %1GraphicsMagick%2: %3%4").arg(bold1, bold2, MagickLibVersionText, nl);
#endif

#ifdef PQMFREEIMAGE
    txt += QString(" - %1FreeImage%2: %3.%4%5").arg(bold1, bold2).arg(FREEIMAGE_MAJOR_VERSION).arg(FREEIMAGE_MINOR_VERSION).arg(nl);
#endif

#ifdef PQMDEVIL
    txt += QString(" - %1DevIL%2: %3%4").arg(bold1, bold2).arg(IL_VERSION).arg(nl);
#endif

#ifdef PQMMOTIONPHOTO
    txt += QString(" - %1Motion Photo%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMPHOTOSPHERE
    txt += QString(" - %1Photosphere%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMVIDEOQT
    txt += QString(" - %1Video%2 through Qt%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMVIDEOMPV
    mpv_handle *mpv = mpv_create();
    if(mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");
    txt += QString(" - %1libmpv%2: %3 (ffmpeg: %4)%5").arg(bold1, bold2, mpv::qt::get_property(mpv, "mpv-version").toString(), mpv::qt::get_property(mpv, "ffmpeg-version").toString(), nl);
#endif

    txt += QString(" - %1Qt%2 image formats available:%3%4").arg(bold1, bold2, nl, spacing);
    QImageReader reader;
    auto formats = reader.supportedImageFormats();
    for(int i = 0; i < formats.length(); ++i) {
        if(i != 0 && i%10 == 0)
            txt += QString("%1%2").arg(nl, spacing);
        txt += QString("%1, ").arg(QString(formats[i]), 5);
    }

    txt += nl;

    return txt;

}

void PQCScripts::copyTextToClipboard(QString txt) {

    qDebug() << "args: txt.length =" << txt.length();

    QApplication::clipboard()->setText(txt, QClipboard::Clipboard);

}

QString PQCScripts::toAbsolutePath(QString path) {

    return QFileInfo(path).absoluteFilePath();

}

bool PQCScripts::applyEmbeddedColorProfile(QImage &img) {

    qDebug() << "args: img";

    bool manualSelectionCausedError = false;

    // if no color space is set then there obviously is no embedded one
    bool colorSpaceManuallySet = false;
    if(!img.colorSpace().isValid())
        return true;

#ifdef PQMLCMS2

    qDebug() << "Checking for embedded color profiles";

    cmsHPROFILE targetProfile = cmsOpenProfileFromMem(img.colorSpace().iccProfile().constData(),
                                                      img.colorSpace().iccProfile().size());

    if(targetProfile) {

        int lcms2format = toLcmsFormat(img.format());

        // Create a transformation from source (sRGB) to destination (provided ICC profile) color space
        cmsHTRANSFORM transform = cmsCreateTransform(cmsCreate_sRGBProfile(), lcms2format, targetProfile, lcms2format, INTENT_PERCEPTUAL, 0);
        if (!transform) {
            // Handle error, maybe close profile and return original image or null image
            cmsCloseProfile(targetProfile);
            qWarning() << "Error creating transform for external color profile";
            return false;
        } else {

            QImage ret(img.size(), img.format());
            ret.fill(Qt::transparent);
            // Perform color space conversion
            cmsDoTransform(transform, img.constBits(), ret.bits(), img.width() * img.height());

            const int bufSize = 100;
            char buf[bufSize];

#if LCMS_VERSION >= 2160
            cmsGetProfileInfoUTF8(targetProfile, cmsInfoDescription,
                                  "en", "US",
                                  buf, bufSize);
#else
            cmsGetProfileInfoASCII(targetProfile, cmsInfoDescription,
                                   "en", "US",
                                   buf, bufSize);
#endif

            // Release resources
            cmsDeleteTransform(transform);
            cmsCloseProfile(targetProfile);

            qDebug() << "Applying external color profile:" << buf;

            img = ret;

            return true;

        }

    } else
        return false;

#endif

    return true;

}

int PQCScripts::toLcmsFormat(QImage::Format fmt) {

#ifdef PQMLCMS2
    switch (fmt) {

    case QImage::Format_ARGB32:  //  (0xAARRGGBB)
    case QImage::Format_RGB32:   //  (0xffRRGGBB)
        return TYPE_BGRA_8;

    case QImage::Format_RGB888:
        return TYPE_RGB_8;       // 24-bit RGB format (8-8-8).

    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
        return TYPE_RGBA_8;

    case QImage::Format_Grayscale8:
        return TYPE_GRAY_8;

    case QImage::Format_Grayscale16:
        return TYPE_GRAY_16;

    case QImage::Format_RGBA64:
    case QImage::Format_RGBX64:
        return TYPE_RGBA_16;

    case QImage::Format_BGR888:
        return TYPE_BGR_8;

    default:
        return 0;

    }
#endif

    return 0;

}

bool PQCScripts::isSVG(QString path) {

    qDebug() << "args: path =" << path;

    const QString suffix = QFileInfo(path).suffix().toLower();
    return (suffix == "svg" || suffix == "svgz");

}

QSize PQCScripts::fitSizeInsideSize(int w, int h, int maxw, int maxh) {

    qDebug() << "args: w h maxw maxh =" << w << h << maxw << maxh;

    return QSize(w,h).scaled(QSize(maxw, maxh), Qt::KeepAspectRatio);

}

bool PQCScripts::isUpgrade() {

    return (PQCSettings::get().getVersion() != PQMVERSION);

}

QVariantList PQCScripts::loadEPUB(QString path) {

    qDebug() << "args: path =" << path;

    QVariantList ret;

#ifdef PQMEPUB

    // clean up all old files
    QDir olddir(PQCConfigFiles::CACHE_DIR() + "/epub/");
    olddir.removeRecursively();

    const QFileInfo info(path);

    // Create new archive handler
    struct archive *a = archive_read_new();

    // We allow any type of compression and format
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // Read file
    int r = archive_read_open_filename(a, info.absoluteFilePath().toLocal8Bit().data(), 10240);

    // If something went wrong, output error message and stop here
    if(r != ARCHIVE_OK) {
        qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
        return ret;
    }

    // we keep a list of all images found so that in case no cover image is explicitely specified we simply use the first image in that list for this purpose
    QStringList imageFiles;

    // Title string
    QString title = "";
    // compose some palatable overview of the book
    QMap<QString,QString> idToFile;
    QStringList idOrder;
    // the file id for the cover image (if present)
    QString coverid = "";

    // Loop over entries in archive
    struct archive_entry *entry;
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        QString filenameinside = QString::fromWCharArray(archive_entry_pathname_w(entry));

        QFileInfo insideinfo(filenameinside);
        const QString suffix = insideinfo.suffix().toLower();

        // Find out the size of the data
        int64_t size = archive_entry_size(entry);

        // empty file? ignore
        if(size == 0)
            continue;

        // Create a uchar buffer of that size to hold the image data
        uchar *buff = new uchar[size];

        // And finally read the file into the buffer
        la_ssize_t r = archive_read_data(a, (void*)buff, size);

        // Something went wrong reading file
        if(r != size) {
            qWarning() << "Unable to read file:" << filenameinside;
            qWarning() << "EPUB cannot be displayed!";
            return ret;
        }

        // we extract it to a temp location from where we can load it then
        const QString temppath = PQCConfigFiles::CACHE_DIR() + "/epub/" + filenameinside;

        // file handles
        QFile file(temppath);
        QFileInfo info(file);

        // remove it if it exists, there is no way to know if it's the same file or not
        // This should never happen as we remove the directory at the start, but better safe than sorry
        if(file.exists()) file.remove();

        // make sure the path exists
        QDir dir(info.absolutePath());
        if(!dir.exists())
            dir.mkpath(info.absolutePath());

        // write buffer to file
        file.open(QIODevice::ReadWrite);
        QDataStream out(&file);   // we will serialize the data into the file
        out.writeRawData((const char*) buff,size);
        delete[] buff;

        // check if this file is located in a subfolder
        // if it is, then we need to take that into account for paths listed inside
        QFileInfo mi(filenameinside);
        QString mp = mi.path();

        // this is the metadata
        if(suffix == "opf" && (idToFile.size() == 0 || mp == "" || mp == ".")) {

            if(mp == ".")
                mp = "";

            file.seek(0);
            QTextStream in(&file);

            idToFile.clear();
            idOrder.clear();
            analyzeEpubMetaData(mp, in.readAll(), title, coverid, idToFile, idOrder);

        // this is an image, possibly a cover image
        } else if(suffix == "jpg" || suffix == "jpeg") {

            // Any image file with one of these basenames is given preferential treatment if no cover image is explicitely specified
            QStringList coveroptions = {"cover", "_cover_", "coverimage", "cover_image", "_cover_image_", "_coverimage_", "coverimg", "_coverimg_"};

            if(coveroptions.contains(info.baseName().toLower()))
                imageFiles.append(temppath);

        }

        file.close();

    }

    bool addedcover = false;

    ret.append(title);

    // loop through all files in the given order
    for(auto &id : std::as_const(idOrder)) {

        // file was not actually found -> something went wrong
        if(!idToFile.contains(id)) {
            qDebug() << "ID not found:" << id;
            continue;
        }

        // the filename
        const QString fn = idToFile.value(id);

        // if this is the image cover then read file and store as base64 string
        if(id == coverid) {

            if(!QFileInfo::exists(PQCConfigFiles::CACHE_DIR() + "/epub/" + fn))
                continue;

            QImage img(PQCConfigFiles::CACHE_DIR() + "/epub/" + fn);
            QBuffer buf;
            buf.open(QIODevice::WriteOnly);
            img.save(&buf, "JPEG");
            buf.close();
            // the image is stored in the second position
            ret.insert(1, buf.buffer().toBase64());
            addedcover = true;

        // normal book file
        } else

            ret.append(QDir::cleanPath(PQCConfigFiles::CACHE_DIR() + "/epub/" + fn));
    }

    // If no cover was explicitely specified
    if(!addedcover) {

        // Take the first image file for this purpose (if any)
        if(imageFiles.length() > 0 && QFileInfo::exists(imageFiles[0])) {
            QImage img(imageFiles[0]);
            QBuffer buf;
            buf.open(QIODevice::WriteOnly);
            img.save(&buf, "JPEG");
            buf.close();
            // the image is stored in the second position
            ret.insert(1, buf.buffer().toBase64());

        // oh well, no cover image
        } else
            ret.insert(1, "");
    }

    // Close archive
    r = archive_read_free(a);
    if(r != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_free() returned code of" << r;

#endif

    return ret;

}

void PQCScripts::analyzeEpubMetaData(QString subfolder, QString txt,
                                     QString &title, QString &coverId,
                                     QMap<QString, QString> &outFileList, QStringList &outIdOrder) {

    bool foundtitle = false;

    QXmlStreamReader reader(txt);
    while(!reader.atEnd()) {

        QXmlStreamReader::TokenType token = reader.readNext();

        if(token == QXmlStreamReader::StartElement) {

            const QString name = reader.name().toString();

            // Store the title in the return map directly
            if(name == "title") {

                reader.readNext();
                title = reader.text().toString();

            // some file
            } else if(name == "item") {

                const QString href = reader.attributes().value("href").toString();
                const QString suffix = QFileInfo(href).suffix().toLower();
                const QString basename = QFileInfo(href).baseName().toLower();

                if(suffix != "xhtml" && suffix != "html" && suffix != "xml")
                    continue;

                // we ignore the title page IF we found the cover image
                // a title page typically also includes only the cover image
                // but we have more control over it when shown as normal image
                if(!foundtitle && coverId != "") {

                    const QStringList titleopts = {"cover", "coverpage", "cover_page", "title", "titlepage", "title_page"};
                    for(const QString &t : titleopts) {
                        if(basename.endsWith(t)) {
                            foundtitle = true;
                            break;
                        }
                    }

                    if(foundtitle)
                        continue;

                }

                if(subfolder == "")
                    outFileList.insert(reader.attributes().value("id").toString(), href);
                else
                    outFileList.insert(reader.attributes().value("id").toString(), QString("%1/%2").arg(subfolder, href));

            // the current file (read in order)
            } else if(name == "itemref") {

                outIdOrder.append(reader.attributes().value("idref").toString());

            // the reference file we want to ignore
            } else if(name == "reference") {

                const QString referencefile = reader.attributes().value("href").toString();
                if(outIdOrder.contains(referencefile))
                    outIdOrder.remove(outIdOrder.indexOf(referencefile));

                // this might contain some information about the cover image
            } else if(name == "meta") {

                if(reader.attributes().value("name").toString() == "cover") {
                    coverId = reader.attributes().value("content").toString();

                    outIdOrder.append(reader.attributes().value("content").toString());
                }

            }

        }
    }

}

QString PQCScripts::getTextFromFile(QString path) {

    qDebug() << "args: path =" << path;

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);
    QString txt = in.readAll();

    file.close();

    return txt;

}
