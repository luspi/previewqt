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

#include <pqc_loadfile_qt.h>
#include <pqc_loadfile_resvg.h>
#include <pqc_loadfile_raw.h>
#include <pqc_loadfile_poppler.h>
#include <pqc_loadfile_qtpdf.h>
#include <pqc_loadfile_xcf.h>
#include <pqc_loadfile_magick.h>
#include <pqc_loadfile_libvips.h>
#include <pqc_loadfile_archive.h>
#include <pqc_loadfile_devil.h>
#include <pqc_loadfile_freeimage.h>
#include <pqc_loadfile_video.h>

#include <pqc_loadfile.h>
#include <pqc_fileformats.h>
#include <pqc_scripts.h>
#include <QSize>
#include <QImage>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QtDebug>

PQCLoadFile::PQCLoadFile() {}
PQCLoadFile::~PQCLoadFile() {}

QString PQCLoadFile::load(QString filename, QSize requestedSize, QSize &origSize, QImage &img) {

    if(filename.trimmed() == "")
        return "";

    QFileInfo info(filename);

    if(info.isSymLink() && info.exists())
        filename = info.symLinkTarget();

    // have a cached image -> all good!
    if(filename == cachedImageSource && info.lastModified() == cachedImageLastModified) {

        // a "normal" cached QImage
        if(!cachedImage.isNull()) {

            origSize = cachedImage.size();

            // Scale image if necessary
            if(requestedSize.width() != -1) {

                QSize finalSize = origSize;

                if(finalSize.width() > requestedSize.width() || finalSize.height() > requestedSize.height())
                    finalSize = finalSize.scaled(requestedSize, Qt::KeepAspectRatio);

                img = cachedImage.scaled(finalSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                return "";

            }

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
        // *Magick loads the full image only as its own image structure so that's what we cache
        } else if(cachedImageMagick.isValid()) {

            Magick::Image magickimage = cachedImageMagick;

            QSize finalSize = QSize(magickimage.columns(), magickimage.rows());
            origSize = finalSize;

            // Scale image if necessary
            if(requestedSize.width() != -1) {

                if(finalSize.width() > requestedSize.width() || finalSize.height() > requestedSize.height())
                    finalSize = finalSize.scaled(requestedSize, Qt::KeepAspectRatio);

                // For small images we can use the faster algorithm, as the quality is good enough for that
                if(finalSize.width() < 300 && finalSize.height() < 300)
                    magickimage.thumbnail(Magick::Geometry(finalSize.width(),finalSize.height()));
                else
                    magickimage.scale(Magick::Geometry(finalSize.width(),finalSize.height()));

            }

            // Write Magick as PPM to memory
            Magick::Blob ob;
            magickimage.magick("PPM");
            magickimage.write(&ob);

            // And load image from memory into QImage
            const QByteArray imgData((char*)(ob.data()),ob.length());
            img = QImage::fromData(imgData);

            return "";

#endif

        }
    }

    // reset cache files
    cachedImage = QImage();
    cachedImageSource = "";
    cachedImageLastModified = QDateTime();
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    cachedImageMagick = Magick::Image();
#endif

    QString err = "";

    // for easier access below
    QString suffix = info.suffix().toLower();

    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // first we check for filename suffix matches

    // resvg trumps Qt's SVG engine
#ifdef PQMRESVG
    if(PQCFileFormats::get().getAllFormatsResvg().contains(suffix))
        err = PQCLoadFileResvg::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMPOPPLER
    if(img.isNull() && PQCFileFormats::get().getAllFormatsPoppler().contains(suffix))
        err = PQCLoadFilePoppler::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMQTPDF
    if(img.isNull() && PQCFileFormats::get().getAllFormatsPoppler().contains(suffix))
        err = PQCLoadFileQtPDF::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && PQCFileFormats::get().getAllFormatsQt().contains(suffix))
        err = PQCLoadFileQt::load(filename, requestedSize, origSize, img, cachedImage);

#ifdef PQMRAW
    if(img.isNull() && PQCFileFormats::get().getAllFormatsLibRaw().contains(suffix))
        err = PQCLoadFileRAW::load(filename, requestedSize, origSize, img, cachedImage);
#endif

#ifdef PQMLIBARCHIVE
    if(img.isNull() && PQCFileFormats::get().getAllFormatsLibArchive().contains(suffix))
        err = PQCLoadFileArchive::load(filename, requestedSize, origSize, img, cachedImage);
#endif

    if(img.isNull() && PQCFileFormats::get().getAllFormatsXCFTools().contains(suffix))
        err = PQCLoadFileXCF::load(filename, requestedSize, origSize, img);

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    if(img.isNull() && PQCFileFormats::get().getAllFormatsMagick().contains(suffix))
        err = PQCLoadFileMagick::load(filename, requestedSize, origSize, img, cachedImageMagick);
#endif

#ifdef PQMLIBVIPS
    if((err != "" || img.isNull()) && PQCFileFormats::get().getAllFormatsLibVips().contains(suffix))
        err = PQCLoadFileLibVips::load(filename, requestedSize, origSize, img, cachedImage);
#endif

#ifdef PQMFREEIMAGE
    if(img.isNull() && PQCFileFormats::get().getAllFormatsFreeImage().contains(suffix))
        err = PQCLoadFileFreeImage::load(filename, requestedSize, origSize, img, cachedImage);
#endif

#ifdef PQMDEVIL
    if(img.isNull() && PQCFileFormats::get().getAllFormatsDevIL().contains(suffix))
        err = PQCLoadFileDevil::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && (PQCFileFormats::get().getAllFormatsVideo().contains(suffix) || PQCFileFormats::get().getAllFormatsLibmpv().contains(suffix)))
        err = PQCLoadFileVideo::load(filename, requestedSize, origSize, img);


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // if that failed, then we check for mimetype matches
    if(img.isNull()) {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(filename).name();

        if(mimetype != "" && mimetype != "application/octet-stream") {

            // resvg trumps Qt's SVG engine
#ifdef PQMRESVG
            if(PQCFileFormats::get().getAllMimeTypesResvg().contains(suffix))
                err = PQCLoadFileResvg::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMPOPPLER
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesPoppler().contains(mimetype))
                err = PQCLoadFilePoppler::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMQTPDF
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesPoppler().contains(mimetype))
                err = PQCLoadFileQtPDF::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesQt().contains(mimetype))
                err = PQCLoadFileQt::load(filename, requestedSize, origSize, img, cachedImage);

#ifdef PQMRAW
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesLibRaw().contains(mimetype))
                err = PQCLoadFileRAW::load(filename, requestedSize, origSize, img, cachedImage);
#endif

#ifdef PQMLIBARCHIVE
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesLibArchive().contains(mimetype))
                err = PQCLoadFileArchive::load(filename, requestedSize, origSize, img, cachedImage);
#endif

            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesXCFTools().contains(mimetype))
                err = PQCLoadFileXCF::load(filename, requestedSize, origSize, img);

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesMagick().contains(mimetype))
                err = PQCLoadFileMagick::load(filename, requestedSize, origSize, img, cachedImageMagick);
#endif

#ifdef PQMLIBVIPS
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesLibVips().contains(mimetype))
                err = PQCLoadFileLibVips::load(filename, requestedSize, origSize, img, cachedImage);
#endif

#ifdef PQMFREEIMAGE
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesFreeImage().contains(mimetype))
                err = PQCLoadFileFreeImage::load(filename, requestedSize, origSize, img, cachedImage);
#endif

#ifdef PQMDEVIL
            if(img.isNull() && PQCFileFormats::get().getAllMimeTypesDevIL().contains(mimetype))
                err = PQCLoadFileDevil::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && (PQCFileFormats::get().getAllMimeTypesVideo().contains(mimetype) || PQCFileFormats::get().getAllMimeTypesLibmpv().contains(mimetype)))
                err = PQCLoadFileVideo::load(filename, requestedSize, origSize, img);


        }

    }

#if defined(PQMGRAPHICSMAGICK) || defined(PQMIMAGEMAGICK)
    // if everything failed, we make sure to try one more time with ImageMagick or GraphicsMagick to see what could be done
    // we do not do this for video files as it can lead to resource intensive ffmpeg processes that may persist after PreviewQt is closed
    if(img.isNull() && !PQCFileFormats::get().getAllFormatsVideo().contains(suffix) && !PQCFileFormats::get().getAllFormatsLibmpv().contains(suffix)) {

        qDebug() << "null image, try magick";

        // we do not override the old error message
        PQCLoadFileMagick::load(filename, requestedSize, origSize, img, cachedImageMagick);

    }
#endif

    // have a full image cached -> store filename and timestamp
    if(!cachedImage.isNull()) {
        cachedImageLastModified = info.lastModified();
        cachedImageSource = filename;
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    } else if(cachedImageMagick.isValid()) {
        cachedImageLastModified = info.lastModified();
        cachedImageSource = filename;
#endif
    }

    return err;

}
