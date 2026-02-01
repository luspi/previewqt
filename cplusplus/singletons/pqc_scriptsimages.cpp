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

#include <pqc_scriptsimages.h>
#include <pqc_fileformats.h>
#include <pqc_configfiles.h>
#include <pqc_settingscpp.h>

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

#ifdef PQMLIBMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMEXIV2
#include <exiv2/exiv2.hpp>
#endif

#ifdef PQMLCMS2
#include <lcms2.h>
#endif

PQCScriptsImages::PQCScriptsImages() {}

PQCScriptsImages::~PQCScriptsImages() {}

QStringList PQCScriptsImages::getArchiveContent(QString path, bool insideFilenameOnly) {

    qDebug() << "args: path =" << path;

    if(path.contains("::ARC::"))
        path = path.split("::ARC::")[1];

    QStringList ret;

    QString theid = generateArchiveId(path);

    if(archiveContents.contains(theid)) {

        if(insideFilenameOnly)
            return archiveContents[theid];

        const QStringList cached = archiveContents[theid];
        for(const QString &f : cached)
            ret.append(QString("%1::ARC::%2").arg(f, path));
        return ret;

    }

    QStringList listInsideOnly;

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
                    if(PQCFileFormats::get().getAllFormats().contains(QFileInfo(f).suffix()))
                        ret.append(f);
                }
                listInsideOnly = ret;
            } else {
                for(const QString &f : std::as_const(allfiles)) {
                    if(PQCFileFormats::get().getAllFormats().contains(QFileInfo(f).suffix())) {
                        ret.append(QString("%1::ARC::%2").arg(f, path));
                        listInsideOnly.append(f);
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
        int r = archive_read_open_filename(a, info.absoluteFilePath().toLocal8Bit().data(), 10240);

        // If something went wrong, output error message and stop here
        if(r != ARCHIVE_OK) {
            qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
            return ret;
        }

        // Loop over entries in archive
        struct archive_entry *entry;
        while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

            // Read the current file entry
            // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
            QString filenameinside = QString::fromWCharArray(archive_entry_pathname_w(entry));

            // If supported file format, append to temporary list
            if((PQCFileFormats::get().getAllFormats().contains(QFileInfo(filenameinside).suffix().toLower())))
                listInsideOnly.append(filenameinside);

        }

        // Sort the temporary list and add to global list
        listInsideOnly.sort();

        if(insideFilenameOnly) {
            ret = listInsideOnly;
        } else {
            for(const QString &f : std::as_const(listInsideOnly))
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

    std::sort(ret.begin(), ret.end(), collator);
    std::sort(listInsideOnly.begin(), listInsideOnly.end(), collator);

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

QString PQCScriptsImages::extractMotionPhoto(QString path) {

    qDebug() << "args: path =" << path;

    // at this point we assume that the check for google motion photo has already been done
    // and we wont need to check again

    // the approach taken in this function is inspired by the analysis found at:
    // https://linuxreviews.org/Google_Pixel_%22Motion_Photo%22

    QFileInfo info(path);
    if(!info.exists())
        return "";

    const QString videofilename = QString("%1/motionphotos/%2.mp4").arg(PQCConfigFiles::get().CACHE_DIR(), info.baseName());
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
                if(!outfile.open(QIODevice::WriteOnly)) {
                    qWarning() << "ERROR: Unable to write video to file";
                    return "";
                }
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

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCFileFormats::get().getAllFormatsLibmpv().contains(suf)) {

        supported = true;

    } else {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(path).name();
        if(PQCFileFormats::get().getAllMimeTypesLibmpv().contains(mimetype))
            supported = true;

    }

#endif

    return supported;

}

bool PQCScriptsImages::isQtVideo(QString path) {

    qDebug() << "args: path =" << path;

    bool supported = false;

#ifdef PQMQTMULTIMEDIA

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCFileFormats::get().getAllFormatsVideo().contains(suf)) {

        supported = true;

    } else {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(path).name();
        if(PQCFileFormats::get().getAllMimeTypesVideo().contains(mimetype))
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

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCFileFormats::get().getAllFormatsPoppler().contains(suf))
        return true;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(path).name();
    if(PQCFileFormats::get().getAllMimeTypesPoppler().contains(mimetype))
        return true;

    return false;

}

bool PQCScriptsImages::isURL(QString url) {
    return (url.startsWith("http:") || url.startsWith("https:"));
}

bool PQCScriptsImages::isAudio(QString path) {

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCFileFormats::get().getAllFormatsAudio().contains(suf))
        return true;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(path).name();
    if(PQCFileFormats::get().getAllMimeTypesAudio().contains(mimetype))
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

bool PQCScriptsImages::isArchive(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMLIBARCHIVE

    QString suf = QFileInfo(path).suffix().toLower();
    if(PQCFileFormats::get().getAllFormatsLibArchive().contains(suf))
        return true;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(path).name();
    if(PQCFileFormats::get().getAllMimeTypesLibArchive().contains(mimetype))
        return true;

#endif

    return false;

}

bool PQCScriptsImages::isComicBook(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMLIBARCHIVE

    const QString suffix = QFileInfo(path).suffix().toLower();

    return (suffix=="cbt" || suffix=="cbr" || suffix=="cbz" || suffix=="cb7");

#endif

    return false;

}

bool PQCScriptsImages::isEpub(QString path) {

    qDebug() << "args: path =" << path;

#ifdef PQMEPUB

    const QString suf = QFileInfo(path).suffix().toLower();

    return (suf=="epub");

#endif

    return false;

}

QString PQCScriptsImages::generateArchiveId(QString path) {

    QFileInfo info(path);
    return QString("%1_%2").arg(info.lastModified().toMSecsSinceEpoch()).arg(info.absoluteFilePath());

}

bool PQCScriptsImages::isSVG(QString path) {

    qDebug() << "args: path =" << path;

    const QString suffix = QFileInfo(path).suffix().toLower();
    return (suffix == "svg" || suffix == "svgz");

}

QVariantList PQCScriptsImages::loadEPUB(QString path) {

    qDebug() << "args: path =" << path;

    QVariantList ret;

#ifdef PQMEPUB

    // clean up all old files
    QDir olddir(PQCConfigFiles::get().CACHE_DIR() + "/epub/");
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
        const QString temppath = PQCConfigFiles::get().CACHE_DIR() + "/epub/" + filenameinside;

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
        if(!file.open(QIODevice::ReadWrite)) {
            qWarning() << "ERROR: Unable to write buffer to file:" << temppath;
            continue;
        }
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
            const QStringList coveroptions = {"cover", "_cover_", "coverimage", "cover_image", "_cover_image_", "_coverimage_", "coverimg", "_coverimg_"};

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

            if(!QFileInfo::exists(PQCConfigFiles::get().CACHE_DIR() + "/epub/" + fn))
                continue;

            QImage img(PQCConfigFiles::get().CACHE_DIR() + "/epub/" + fn);
            QBuffer buf;
            buf.open(QIODevice::WriteOnly);
            img.save(&buf, "JPEG");
            buf.close();
            // the image is stored in the second position
            ret.insert(1, buf.buffer().toBase64());
            addedcover = true;

            // normal book file
        } else

        ret.append(QDir::cleanPath(PQCConfigFiles::get().CACHE_DIR() + "/epub/" + fn));
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

void PQCScriptsImages::analyzeEpubMetaData(QString subfolder, QString txt,
                                     QString &title, QString &coverId,
                                     QMap<QString, QString> &outFileList, QStringList &outIdOrder) {

    QStringList foundtitle;

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

                const QString id = reader.attributes().value("id").toString();
                const QString href = reader.attributes().value("href").toString();
                const QString suffix = QFileInfo(href).suffix().toLower();
                const QString basename = QFileInfo(href).baseName().toLower();

                if(coverId == "" && suffix == "jpeg" || suffix == "jpg") {

                    const QStringList coveroptions = {"cover", "_cover_", "coverimage", "cover_image", "_cover_image_", "_coverimage_", "coverimg", "_coverimg_"};

                    if(coveroptions.contains(basename))
                        coverId = id;

                    continue;

                } else if(suffix != "xhtml" && suffix != "html" && suffix != "xml")
                    continue;

                // we ignore the title page IF we found the cover image
                // a title page typically also includes only the cover image
                // but we have more control over it when shown as normal image
                if(foundtitle.length() == 0) {

                    const QStringList titleopts = {"cover", "coverpage", "cover_page", "title", "titlepage", "title_page"};
                    for(const QString &t : titleopts) {
                        if(basename.endsWith(t)) {
                            foundtitle.append(id);
                            foundtitle.append(href);
                            break;
                        }
                    }

                    if(foundtitle.length() > 0)
                        continue;

                }

                if(subfolder == "")
                    outFileList.insert(id, href);
                else
                    outFileList.insert(id, QString("%1/%2").arg(subfolder, href));

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

    if(coverId == "") {
        outFileList.insert(foundtitle[0], foundtitle[1]);
    }

}

bool PQCScriptsImages::isTextDocument(QString path) {

    qDebug() << "args: path =" << path;

    const QString suffix = QFileInfo(path).suffix();
    if(PQCFileFormats::get().getAllFormatsText().contains(suffix.toLower()))
        return true;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(path).name();
    qDebug() << "detected mime type:" << mimetype;
    if(mimetype.startsWith("text/") || PQCFileFormats::get().getAllMimeTypesText().contains(mimetype))
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
