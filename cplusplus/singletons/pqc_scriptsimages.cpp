/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
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

#include <pqc_scriptsimages.h>
#include <pqc_configfiles.h>
#include <pqc_settingscpp.h>
#include <pqc_filehandler.h>

#ifdef WIN32
#include <WinSock2.h>
#endif

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
#include <QXmlStreamReader>
#include <QBuffer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QPainter>

#ifdef PQMEXIV2
#include <exiv2/exiv2.hpp>
#endif

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

#ifdef PQMLIBMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMLCMS2
#include <lcms2.h>
#endif

PQCScriptsImages::PQCScriptsImages() {
#ifdef PQMPHOTOSPHEREQRHI
    // a value of -1 makes a photo sphere wait until value is ready
    m_maxTextureLimit = -1;
#else
    m_maxTextureLimit = 0;
#endif
}

PQCScriptsImages::~PQCScriptsImages() {}

QStringList PQCScriptsImages::getArchiveContent(QString path, bool insideFilenameOnly) {

    qDebug() << "args: path =" << path;
    qDebug() << "args: insideFilenameOnly =" << insideFilenameOnly;

    path = PQCHelper::extractInsideARCFilename(path);

    QStringList ret;

    QString theid = generateArchiveId(path);

    if(archiveContents.contains(theid)) {

        if(insideFilenameOnly)
            return archiveContents[theid];

        const QStringList cached = archiveContents[theid];
        for(const QString &f : cached)
            ret.append(f % "::ARC::" % path);
        return ret;

    }

    QStringList listInsideOnly;

    const QFileInfo info(path);

#ifndef Q_OS_WIN

    const QString suffix = info.suffix();

    const QSet<QString> supportedSuffixes = PQCFileHandler::get().getSuffixes();

    if(suffix == "cbr" || suffix == "rar") {

        QProcess p;
        p.setProcessChannelMode(QProcess::MergedChannels);
        p.start("unrar", QStringList() << "lb" << info.absoluteFilePath());

        if(p.waitForStarted()) {

            if(p.waitForFinished()) {

                if(p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0) {

                    QStringList allfiles = QString::fromLocal8Bit(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);

                    // remove archives and unsupported files
                    ret.erase(std::remove_if(ret.begin(), ret.end(), [&](const QString &f) {
                                  QFileInfo info(f);
                                  return (isArchive(f) || (!supportedSuffixes.contains(info.suffix().toLower()) && !supportedSuffixes.contains(info.completeSuffix().toLower())));
                              }), ret.end());

                    allfiles.sort();

                    if(insideFilenameOnly) {
                        for(const QString &f : std::as_const(allfiles)) {
                            if(supportedSuffixes.contains(QFileInfo(f).suffix()))
                                ret.append(f);
                        }
                        listInsideOnly = ret;
                    } else {
                        for(const QString &f : std::as_const(allfiles)) {
                            if(supportedSuffixes.contains(QFileInfo(f).suffix())) {
                                ret.append(f % "::ARC::" % path);
                                listInsideOnly.append(f);
                            }
                        }
                    }

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
#ifdef Q_OS_WIN
        int r = archive_read_open_filename_w(a, reinterpret_cast<const wchar_t*>(info.absoluteFilePath().utf16()), 10240);
#else
        QByteArray tmpPath = QFile::encodeName(info.absoluteFilePath());
        int r = archive_read_open_filename(a, tmpPath.constData(), 10240);
#endif

        // If something went wrong, output error message and stop here
        if(r != ARCHIVE_OK) {
            qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
            qWarning() << "Archive:" << info.absoluteFilePath();
            archive_read_free(a);
            return ret;
        }

        // Loop over entries in archive
        struct archive_entry *entry;
        while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

            // Read the current file entry
            // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
            // Also, if the archives is malformed or there is an encoding issue then it is possible that this may return a nullptr
            // and PhotoQt might crash if not handled properly -> check before converting to QString
            const wchar_t *wpath = archive_entry_pathname_w(entry);
            if(!wpath) continue;
            QString filenameinside = QString::fromWCharArray(wpath);

            // If supported file format, append to temporary list
            QFileInfo info(filenameinside);
            if(!isArchive(filenameinside, true) && (supportedSuffixes.contains(info.suffix().toLower()) || supportedSuffixes.contains(info.completeSuffix().toLower())))
                listInsideOnly.append(filenameinside);

        }

        // Sort the temporary list and add to global list
        listInsideOnly.sort();

        if(insideFilenameOnly) {
            ret = listInsideOnly;
        } else {
            for(const QString &f : std::as_const(listInsideOnly))
                ret.append(f % "::ARC::" % path);
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
    collator.setLocale(QLocale::system());
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setIgnorePunctuation(true);
    collator.setNumericMode(true);

    std::sort(ret.begin(), ret.end(), [&collator](const QString &file1, const QString &file2) { return collator.compare(file1, file2) < 0; });
    std::sort(listInsideOnly.begin(), listInsideOnly.end(), [&collator](const QString &file1, const QString &file2) { return collator.compare(file1, file2) < 0; });

    archiveContents[theid] = listInsideOnly;

    return ret;

}

int PQCScriptsImages::isMotionPhoto(QString path) {

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

        const QString videopath = info.absolutePath() % "/" % info.baseName();
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

QString PQCScriptsImages::extractMotionPhoto(QString path) {

    qDebug() << "args: path =" << path;

    // at this point we assume that the check for google motion photo has already been done
    // and we won't need to check again

    QFileInfo info(path);
    if(!info.exists())
        return "";

    const QString videofilename = PQCConfigFiles::get().CACHE_DIR() % "/motionphotos/" % QString::number(qHash(info.baseName())) % ".mp4";
    if(QFileInfo::exists(videofilename)) {
        return videofilename;
    }

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file for reading";
        return "";
    }

    const qint64 fileSize = file.size();

    if(fileSize < 16) {
        qWarning() << "File too small to contain embedded video";
        return "";
    }

    uchar *data = file.map(0, fileSize);

    if(!data) {
        qWarning() << "Failed to memory-map file";
        return "";
    }

    // some common MP4 tags found in motion photos
    static const QList<QByteArray> validTags = {"mp42", "mp41", "isom", "iso2", "avc1", "M4V "};

    qint64 videoOffset = -1;

    // scan for MP4 ftyp atom
    for(qint64 i = 0; i < fileSize - 12; ++i) {

        // we are checking for:
        // [size:4]["ftyp":4][tag:4]

        // this is much faster than converting the four characters to a single string and comparing that
        if(data[i + 4] == 'f' && data[i + 5] == 't' &&
            data[i + 6] == 'y' && data[i + 7] == 'p') {

            // read atom size (big endian)
            quint32 atomSize = (quint32(data[i]) << 24) | (quint32(data[i + 1]) << 16) |
                               (quint32(data[i + 2]) << 8) | quint32(data[i + 3]);

            // some basic validation
            if(atomSize < 8 || atomSize > (fileSize - i))
                continue;

            QByteArray tag(reinterpret_cast<const char*>(data + i + 8), 4);

            if(!validTags.contains(tag))
                continue;

            videoOffset = i;

            break;
        }
    }

    file.unmap(data);

    if(videoOffset < 0) {
        qWarning() << "no embedded MP4 video found";
        return "";
    }

    // Ensure output directory exists
    QDir dir;
    dir.mkpath(QFileInfo(videofilename).absolutePath());

    if(!file.seek(videoOffset)) {
        qWarning() << "failed to seek to video offset";
        return "";
    }

    QFile outFile(videofilename);

    if(!outFile.open(QIODevice::WriteOnly)) {
        qWarning() << "failed to create output video file:" << videofilename;
        return "";
    }

    // use a buffer of 1MB
    constexpr qint64 bufferSize = 1024 * 1024;

    while(!file.atEnd()) {

        QByteArray chunk = file.read(bufferSize);

        if(chunk.isEmpty() && file.error() != QFile::NoError) {
            qWarning() << "error reading input file";
            outFile.close();
            outFile.remove();
            return "";
        }

        if(outFile.write(chunk) != chunk.size()) {
            qWarning() << "error writing output file";
            outFile.close();
            outFile.remove();
            return "";
        }
    }

    outFile.close();

    qDebug() << "extracted motion video to:" << videofilename;

    return outFile.fileName();

}

int PQCScriptsImages::getExifOrientation(QString path) {

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
        // An error code of kerFileContainsUnknownImageType (older version: 11) means unknown file type
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

void PQCScriptsImages::applyExifOrientation(const QString filename, QImage &img) {

    const int orientation = getExifOrientation(filename);

    QTransform transform;

    switch(orientation) {

    case 1:
        // no rotation, no mirror
        break;
    case 2:
        // no rotation, horizontal mirror
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        img = img.flipped(Qt::Horizontal);
#else
        img = img.mirrored(true, false);
#endif
        break;
    case 3:
        // 180 degree rotation, no mirror
        transform.rotate(180);
        img = img.transformed(transform);
        break;
    case 4:
        // 180 degree rotation, horizontal mirror
        transform.rotate(180);
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        img = img.flipped(Qt::Horizontal).transformed(transform);
#else
        img = img.mirrored(true, false).transformed(transform);
#endif
        break;
    case 5:
        // 90 degree rotation, horizontal mirror
        transform.rotate(90);
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        img = img.flipped(Qt::Horizontal).transformed(transform);
#else
        img = img.mirrored(true, false).transformed(transform);
#endif
        break;
    case 6:
        // 90 degree rotation, no mirror
        transform.rotate(90);
        img = img.transformed(transform);
        break;
    case 7:
        // 270 degree rotation, horizontal mirror
        transform.rotate(270);
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        img = img.flipped(Qt::Horizontal).transformed(transform);
#else
        img = img.mirrored(true, false).transformed(transform);
#endif
        break;
    case 8:
        // 270 degree rotation, no mirror
        transform.rotate(270);
        img = img.transformed(transform);
        break;
    default:
        qWarning() << "Unexpected orientation value received:" << orientation;
        break;

    }

}

bool PQCScriptsImages::isPhotoSphere(QString path) {

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

bool PQCScriptsImages::isMpvVideo(QString path) {

    qDebug() << "args: path =" << path;

    bool supported = false;

#ifdef PQMLIBMPV

    QFileInfo info(path);
    const QSet<QString> suffixes = PQCFileHandler::get().getSuffixes("libmpv");
    if(suffixes.contains(info.suffix().toLower()) || suffixes.contains(info.completeSuffix().toLower())) {

        supported = true;

    } else {

        QMimeDatabase db;
        if(PQCFileHandler::get().getMimetypes("libmpv").contains(db.mimeTypeForFile(path).name()))
            supported = true;

    }

#endif

    return supported;

}

bool PQCScriptsImages::isQtVideo(QString path) {

    qDebug() << "args: path =" << path;

    bool supported = false;

#ifdef PQMQTMULTIMEDIA

    QFileInfo info = QFileInfo(path);
    const QSet<QString> suffixes = PQCFileHandler::get().getSuffixes("video");
    if(suffixes.contains(info.suffix().toLower()) || suffixes.contains(info.completeSuffix().toLower())) {

        supported = true;

    } else {

        QMimeDatabase db;
        if(PQCFileHandler::get().getMimetypes("video").contains(db.mimeTypeForFile(path).name()))
            supported = true;

    }

#endif

    return supported;

}

bool PQCScriptsImages::isItAnimated(QString filename) {
    QImageReader reader(filename);
    return (reader.supportsAnimation()&&reader.imageCount()>1);
}

bool PQCScriptsImages::isPDFDocument(QString path) {

    qDebug() << "args: path =" << path;

#if defined(PQMPOPPLER) || defined(PQMQTPDF)

    const QSet<QString> suffixes = PQCFileHandler::get().getSuffixes("pdf");
    QFileInfo info(path);
    if(suffixes.contains(info.suffix().toLower()) || suffixes.contains(info.completeSuffix().toLower()))
        return true;

    QMimeDatabase db;
    if(PQCFileHandler::get().getMimetypes("pdf").contains(db.mimeTypeForFile(path).name()))
        return true;

#endif

    return false;

}

bool PQCScriptsImages::isURL(QString url) {
    return (url.startsWith("http:") || url.startsWith("https:") || isLocalURL(url));
}

bool PQCScriptsImages::isLocalURL(QString url) {
    return ((url.endsWith(".html") || url.endsWith(".xhtml") || url.endsWith(".htm")) &&
           (url.startsWith("file:/") || (!url.startsWith("http:/") && !url.startsWith("https:/"))));
}

bool PQCScriptsImages::isAudio(QString path) {

    const QSet<QString> suffixes = PQCFileHandler::get().getSuffixes("audio");
    QFileInfo info(path);
    if(suffixes.contains(info.suffix().toLower()) || suffixes.contains(info.completeSuffix().toLower()))
        return true;

    QMimeDatabase db;
    if(PQCFileHandler::get().getMimetypes("audio").contains(db.mimeTypeForFile(path).name()))
        return true;

    return false;

}

QString PQCScriptsImages::findCoverImageNextToFile(QString path) {

    QStringList possibleSuffix = {"jpg", "jpeg", "png"};

    QDir dir(QFileInfo(path).absolutePath());
    const QFileInfoList lst = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files);
    for(const QFileInfo &info : lst) {

        if(info.baseName().toLower() == "cover" && possibleSuffix.contains(info.suffix().toLower())) {
            return info.absoluteFilePath();
        }

    }

    return "";

}

int PQCScriptsImages::getDocumentPageCount(QString path) {

    qDebug() << "args: path =" << path;

    path = PQCHelper::extractInsidePDFFilename(path);

#ifdef PQMQTPDF

    QPdfDocument doc;

    if(doc.load(path) != QPdfDocument::Error::None) {
        qWarning() << "Error occurred loading PDF";
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

bool PQCScriptsImages::isArchive(QString path, bool insideArchive) {

    qDebug() << "args: path =" << path;

#ifdef PQMLIBARCHIVE

    QFileInfo info(path);
    const QSet<QString> supportedSuffixes = PQCFileHandler::get().getSuffixes("libarchive");
    if(supportedSuffixes.contains(info.suffix().toLower()) ||
       supportedSuffixes.contains(info.completeSuffix().toLower()))
        return true;

    if(!insideArchive) {
        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(path).name();
        if(PQCFileHandler::get().getMimetypes("libarchive").contains(mimetype))
            return true;
    }

#endif

    return false;

}

bool PQCScriptsImages::isComicBook(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMLIBARCHIVE

    const QString suffix = QFileInfo(path).suffix().toLower();

    return (suffix.compare("cbt", Qt::CaseInsensitive) == 0 ||
            suffix.compare("cbr", Qt::CaseInsensitive) == 0 ||
            suffix.compare("cbz", Qt::CaseInsensitive) == 0 ||
            suffix.compare("cb7", Qt::CaseInsensitive) == 0);

#endif

    return false;

}

bool PQCScriptsImages::isEpub(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMEPUB

    return (QFileInfo(path).suffix().toLower() == "epub");

#endif

    return false;

}

QString PQCScriptsImages::generateArchiveId(QString path) {

    QFileInfo info(path);
    return  QString("%1_%2").arg(info.lastModified().toMSecsSinceEpoch()).arg(info.absoluteFilePath());

}

bool PQCScriptsImages::isSVG(QString path) {

    qDebug() << "args: path =" << path;

    const QString suffix = QFileInfo(path).suffix().toLower();
    return (suffix.compare("svg", Qt::CaseInsensitive) == 0 || suffix.compare("svgz", Qt::CaseInsensitive) == 0);

}

QVariantList PQCScriptsImages::loadEPUB(QString path) {

    qDebug() << "args: path =" << path;

    QVariantList ret;

#ifdef PQMEPUB

    // clean up all old files
    QDir olddir(PQCConfigFiles::get().CACHE_DIR() % "/epub/");
    olddir.removeRecursively();

    const QFileInfo info(path);

    // Create new archive handler
    struct archive *a = archive_read_new();

    // We allow any type of compression and format
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // Read file
#ifdef Q_OS_WIN
    int r = archive_read_open_filename_w(a, reinterpret_cast<const wchar_t*>(info.absoluteFilePath().utf16()), 10240);
#else
    QByteArray tmpPath = QFile::encodeName(info.absoluteFilePath());
    int r = archive_read_open_filename(a, tmpPath.constData(), 10240);
#endif

    // If something went wrong, output error message and stop here
    if(r != ARCHIVE_OK) {
        qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
        qWarning() << "Archive:" << info.absoluteFilePath();
        archive_read_free(a);
        return {};
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

    QString packageDocumentFilename = "";

    // FIRST we need to find the filename of the OPF package document
    // an epub might contain more than one, but the root one is listed in the 'META-INF/container.xml' file
    struct archive_entry *entry;
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        // Also, if the archives is malformed or there is an encoding issue then it is possible that this may return a nullptr
        // and PhotoQt might crash if not handled properly -> check before converting to QString
        const wchar_t *wpath = archive_entry_pathname_w(entry);
        if(!wpath) continue;
        QString filenameinside = QString::fromWCharArray(wpath);

        if(filenameinside == "META-INF/container.xml") {

            qDebug() << "found container.xml";

            // Find out the size of the data
            int64_t size = archive_entry_size(entry);

            // empty file? ignore
            // size > max int size (limit for QByteArray)? ignore
            if(size <= 0 || size > std::numeric_limits<int>::max()) {
                qWarning() << "invalid container.xml found";
                break;
            }

            // Create a buffer of that size to hold the data
            QByteArray data;
            data.resize(size);

            // And finally read the file into the buffer in chunks
            char* ptr = data.data();
            qint64 total = 0;
            while (total < size) {
                la_ssize_t chunk = archive_read_data(a, ptr + total, size - total);
                if(chunk < 0) {
                    qWarning() << QString("Invalid chunk read: %1").arg(archive_error_string(a));
                    break;
                }

                if(chunk == 0) {
                    break;
                }

                total += chunk;
            }

            if(total != size) {
                qWarning() << QString("Failed to read image data, read size (%1) doesn't match expected size (%2)...").arg(total).arg(size);
                break;
            }

            // find the package document filename (usually something like metadata.opf)
            QXmlStreamReader xml(data);
            while(!xml.atEnd()) {
                xml.readNext();
                if(xml.isStartElement() && xml.name() == QLatin1StringView("rootfile")) {
                    packageDocumentFilename = xml.attributes().value("full-path").toString();
                    break;
                }
            }

        }

    }

    // re-open the archive as we're currently at EOF
    if(archive_read_close(a) != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_close() failed" << archive_error_string(a);
    if(archive_read_free(a) != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_free() failed:" << archive_error_string(a);

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

#ifdef Q_OS_WIN
    r = archive_read_open_filename_w(a, reinterpret_cast<const wchar_t*>(info.absoluteFilePath().utf16()), 10240);
#else
    r = archive_read_open_filename(a, tmpPath.constData(), 10240);
#endif

    // If something went wrong, output error message and stop here
    if(r != ARCHIVE_OK) {
        qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
        qWarning() << "Archive:" << info.absoluteFilePath();
        archive_read_free(a);
        return {};
    }

    // Loop over entries in archive
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        // Also, if the archives is malformed or there is an encoding issue then it is possible that this may return a nullptr
        // and PhotoQt might crash if not handled properly -> check before converting to QString
        const wchar_t *wpath = archive_entry_pathname_w(entry);
        if(!wpath) continue;
        QString filenameinside = QString::fromWCharArray(wpath);

        // we already read that file and don't need it anymore
        if(filenameinside.startsWith("META-INF"))
            continue;

        QFileInfo insideinfo(filenameinside);
        const QString suffix = insideinfo.suffix().toLower();

        // no suffix most likely means directory
        // in any case, we likely don't need this here
        // directories themselves are handled below already
        if(suffix == "") continue;

        // we extract it to a temp location from where we can load it then
        // we check to make sure we never write outside of the cache subfolder
        const QString tempRoot = PQCConfigFiles::get().CACHE_DIR() % "/epub/";

        // we create
        QDir dirTempRoot(tempRoot);
        if(!dirTempRoot.mkpath(tempRoot)) {
            qWarning() << "FAILED to create temporary directory:" << tempRoot;
            qWarning() << "Not sure what to do here...";
            return {};
        }

        const QString tempTarget = dirTempRoot.absoluteFilePath(filenameinside);
        if(!tempTarget.startsWith(tempRoot))
            continue;

        QFileInfo info(tempTarget);

        // make sure the path exists
        QDir dir(info.absolutePath());
        if(!dir.exists()) {
            if(!dir.mkpath(info.absolutePath())) {
                qWarning() << "FAILED to create temporary directory:" << info.absolutePath();
                qWarning() << "Not sure what to do here...";
                return {};
            }
        }

        // file handles
        QFile file(tempTarget);

        // write buffer to file
        if(!file.open(QIODevice::ReadWrite)) {
            qWarning() << "ERROR: Unable to write buffer to file:" << tempTarget;
            continue;
        }

        const void *buff;
        size_t size;
        la_int64_t offset;

        while(true) {

            int r = archive_read_data_block(a, &buff, &size, &offset);

            // finished!
            if(r == ARCHIVE_EOF)
                break;

            // error
            if(r != ARCHIVE_OK) {
                qWarning() << "FAILED reading next block of file in archive:" << archive_error_string(a);
                break;
            }

            // write chunk
            if(file.write(static_cast<const char*>(buff), size) != qint64(size)) {
                qWarning() << "FAILED to write next chunk";
                break;
            }
        }

        // check if this file is located in a subfolder
        // if it is, then we need to take that into account for paths listed inside
        QFileInfo mi(filenameinside);
        QString mp = mi.path();

        // this is the metadata
        if((!packageDocumentFilename.isEmpty() && filenameinside == packageDocumentFilename) || (packageDocumentFilename.isEmpty() && suffix == "opf" && (idToFile.size() == 0 || mp.isEmpty() || mp == "."))) {

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
            const QStringList coveroptions = {"cover", "_cover_", "coverimage", "cover_image", "_cover_image_", "_coverimage_", "coverimg", "_coverimg_"};

            if(coveroptions.contains(info.baseName().toLower()))
                imageFiles.append(tempTarget);

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

            if(!QFileInfo::exists(PQCConfigFiles::get().CACHE_DIR() % "/epub/" % fn))
                continue;

            QImage img(PQCConfigFiles::get().CACHE_DIR() % "/epub/" % fn);
            QBuffer buf;
            buf.open(QIODevice::WriteOnly);
            if(!img.save(&buf, "JPEG")) {
                qWarning() << "ERROR saving image";
                buf.close();
            } else {
                buf.close();
                // the image is stored in the second position
                ret.insert(1, buf.buffer().toBase64());
                addedcover = true;
            }

        // normal book file
        } else

        ret.append(QDir::cleanPath(PQCConfigFiles::get().CACHE_DIR() % "/epub/" % fn));
    }

    // If no cover was explicitely specified
    if(!addedcover) {

        // Take the first image file for this purpose (if any)
        if(!imageFiles.isEmpty() && QFileInfo::exists(imageFiles[0])) {
            QImage img(imageFiles[0]);
            QBuffer buf;
            buf.open(QIODevice::WriteOnly);
            if(!img.save(&buf, "JPEG")) {
                qWarning() << "ERROR saving image";
                buf.close();
            } else {
                buf.close();
                // the image is stored in the second position
                ret.insert(1, buf.buffer().toBase64());
            }

            // oh well, no cover image
        } else
            ret.insert(1, "");
    }

    // Close archive
    if(archive_read_close(a) != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_close() failed" << archive_error_string(a);
    if(archive_read_free(a) != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_free() failed:" << archive_error_string(a);

#endif

    return ret;

}

void PQCScriptsImages::analyzeEpubMetaData(QString subfolder, QString txt,
                                           QString &title, QString &coverId,
                                           QMap<QString, QString> &outFileList, QStringList &outIdOrder) {

    QPair<QString,QString> foundtitle = {"",""};

    const QSet<QString> coveroptions = {"cover", "_cover_", "coverimage", "cover_image", "_cover_image_", "_coverimage_", "coverimg", "_coverimg_"};
    const QSet<QString> titleopts = {"cover", "coverpage", "cover_page", "title", "titlepage", "title_page"};

    QXmlStreamReader reader(txt);
    while(!reader.atEnd()) {

        QXmlStreamReader::TokenType token = reader.readNext();

        if(token == QXmlStreamReader::StartElement) {

            const QString name = reader.name().toString();

            // Store the title in the return map directly
            if(name == "title") {

                title = reader.readElementText();

            // some file
            } else if(name == "item") {

                const QString id = reader.attributes().value("id").toString();
                const QString href = reader.attributes().value("href").toString();
                QFileInfo hrefInfo(href);
                const QString suffix = hrefInfo.suffix().toLower();
                const QString basename = hrefInfo.baseName().toLower();

                if(coverId.isEmpty() && (suffix == "jpeg" || suffix == "jpg")) {

                    if(coveroptions.contains(basename))
                        coverId = id;

                    continue;

                } else if(suffix != "xhtml" && suffix != "html" && suffix != "xml")
                    continue;

                // we ignore the title page IF we found the cover image
                // a title page typically also includes only the cover image
                // but we have more control over it when shown as normal image
                if(foundtitle.first.isEmpty()) {

                    for(const QString &t : titleopts) {
                        if(basename.endsWith(t)) {
                            foundtitle.first = id;
                            foundtitle.second = href;
                            break;
                        }
                    }

                    if(!foundtitle.first.isEmpty())
                        continue;

                }

                if(subfolder.isEmpty())
                    outFileList.insert(id, href);
                else
                    outFileList.insert(id, QString("%1/%2").arg(subfolder, href));

            // the current file (read in order)
            } else if(name == "itemref") {

                outIdOrder.append(reader.attributes().value("idref").toString());

            // the reference file we wantto ignore
            } else if(name == "reference") {

                const QString referencefile = reader.attributes().value("href").toString();
                const QString idForFile = outFileList.key(referencefile, "");
                if(!idForFile.isEmpty()) {
                    const int index = outIdOrder.indexOf(idForFile);
                    if(index > -1)
                        outIdOrder.remove(index);
                }

            // this might contain some information about the cover image
            } else if(name == "meta") {

                if(reader.attributes().value("name").toString() == "cover") {
                    coverId = reader.attributes().value("content").toString();

                    outIdOrder.append(reader.attributes().value("content").toString());
                }

            }

        }
    }

    if(reader.hasError()) {
        qWarning() << "ERROR parsing epub metadata:" << reader.errorString();
    }

    if(coverId.isEmpty() && !foundtitle.first.isEmpty()) {
        outFileList.insert(foundtitle.first, foundtitle.second);
    }

}

bool PQCScriptsImages::isTextDocument(QString path) {

    qDebug() << "args: path =" << path;

    const QSet<QString> suffixes = PQCFileHandler::get().getSuffixes("text");
    QFileInfo info(path);
    if(suffixes.contains(info.suffix().toLower()) || suffixes.contains(info.completeSuffix().toLower()))
        return true;

    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(path).name();
    qDebug() << "detected mime type:" << mimetype;
    if(mimetype.startsWith("text/") || PQCFileHandler::get().getMimetypes("text").contains(mimetype))
        return true;

    return false;

}

QString PQCScriptsImages::getTextFileContents(QString path) {

    qDebug() << "args: path =" << path;

    QFile f(path);
    if(!f.exists()) {
        qWarning() << "File does not exist:" << path;
        return "";
    }

    if(!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file for reading:" << path;
        return "";
    }

    QTextStream in(&f);
    return in.readAll();

}

QString PQCScriptsImages::prepareSphereFile(QString path) {

    qDebug() << "args: path " << path;

#if defined(PQMEXIV2)
// the following macro needs to come after the above check as the macro might not be defined
#if (EXIV2_TEST_VERSION(0, 28, 0) || defined(PQMEXIV2_ENABLE_BMFF))

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

        return path;
    }

    Exiv2::XmpData xmpData;
    try {
        xmpData = image->xmpData();
    } catch(Exiv2::Error &e) {
        qDebug() << "ERROR: Unable to read xmp metadata:" << e.what();
        return path;
    }

    int croppedW = 0, croppedH = 0;
    int fullW = 0, fullH = 0;

    for(Exiv2::XmpData::const_iterator it_xmp = xmpData.begin(); it_xmp != xmpData.end(); ++it_xmp) {

        QString familyName = QString::fromStdString(it_xmp->familyName());
        QString groupName = QString::fromStdString(it_xmp->groupName());
        QString tagName = QString::fromStdString(it_xmp->tagName());

        // check for actual and full dimensions of sphere
        if(familyName == "Xmp" && groupName == "GPano") {
            if(tagName == "CroppedAreaImageHeightPixels")
                croppedH = QString::fromStdString(Exiv2::toString(it_xmp->value())).toInt();
            else if(tagName == "CroppedAreaImageWidthPixels")
                croppedW = QString::fromStdString(Exiv2::toString(it_xmp->value())).toInt();
            else if(tagName == "FullPanoHeightPixels")
                fullH = QString::fromStdString(Exiv2::toString(it_xmp->value())).toInt();
            else if(tagName == "FullPanoWidthPixels")
                fullW = QString::fromStdString(Exiv2::toString(it_xmp->value())).toInt();
        }

    }

    // we add a small margin to allow for minor inaccuracies in creating the image
    // this will not affect the visible part of the image
    if(croppedW > 0 && croppedH > 0 && fullW > 0 && fullH > 0 && (croppedW < fullW-10 || croppedH < fullH-10)) {

        // image is cropped -> process

        QImage partialImage = QImage(path);

        double factor = 1.0;
        if(m_maxTextureLimit > 0 && (fullW > m_maxTextureLimit || fullH > m_maxTextureLimit))
            factor = qMin(static_cast<double>(m_maxTextureLimit)/static_cast<double>(fullW),
                          static_cast<double>(m_maxTextureLimit)/static_cast<double>(fullH));

        QImage fullimage(factor*fullW, factor*fullH, QImage::Format_RGB32);
        fullimage.fill(Qt::transparent);
        QPainter painter(&fullimage);
        painter.drawImage(factor*(fullW-croppedW)/2, factor*(fullH-croppedH)/2, partialImage);
        painter.end();

        const QString dir = PQCConfigFiles::get().CACHE_DIR() % "/sphere";
        if(QDir().mkpath(dir)) {
            const QString newPath = dir % "/" % QFileInfo(path).fileName();
            if(QFile(newPath).exists()) QFile::remove(newPath);
            fullimage.save(newPath);
            return newPath;
        }

        return path;

#ifdef PQMPHOTOSPHEREQRHI
    } else {

        if(m_maxTextureLimit > 0 && (fullW > m_maxTextureLimit || fullH > m_maxTextureLimit)) {

            qWarning() << "Image resolution exceeds maximum texture size of" << m_maxTextureLimit;
            qWarning() << "Reducing image size to fit within limit.";

            double factor = qMin(static_cast<double>(m_maxTextureLimit)/static_cast<double>(fullW),
                                 static_cast<double>(m_maxTextureLimit)/static_cast<double>(fullH));

            const QString dir = PQCConfigFiles::get().CACHE_DIR() % "/sphere";
            if(QDir().mkpath(dir)) {
                const QString newPath = dir % "/" % QFileInfo(path).fileName();
                if(QFile(newPath).exists()) QFile::remove(newPath);
                QImage img(path);
                img.scaled(fullW*factor, fullH*factor).save(newPath);
                return newPath;
            }

        }

#endif

    }

#endif
#endif

    return path;

}
