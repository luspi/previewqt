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

#include <pqc_loadimage_qt.h>
#include <pqc_loadimage_resvg.h>
#include <pqc_loadimage_raw.h>
#include <pqc_loadimage_poppler.h>
#include <pqc_loadimage_qtpdf.h>
#include <pqc_loadimage_xcf.h>
#include <pqc_loadimage_magick.h>
#include <pqc_loadimage_libvips.h>
#include <pqc_loadimage_archive.h>
#include <pqc_loadimage_devil.h>
#include <pqc_loadimage_freeimage.h>
#include <pqc_loadimage_video.h>

#include <pqc_loadimage.h>
#include <pqc_imageformats.h>
#include <pqc_scripts.h>
#include <QSize>
#include <QImage>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QtDebug>

PQCLoadImage::PQCLoadImage() {}
PQCLoadImage::~PQCLoadImage() {}

QString PQCLoadImage::load(QString filename, QSize requestedSize, QSize &origSize, QImage &img) {

    if(filename.trimmed() == "")
        return "";

    QFileInfo info(filename);

    if(info.isSymLink() && info.exists())
        filename = info.symLinkTarget();

    QString err = "";

    // for easier access below
    QString suffix = info.suffix().toLower();

    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // first we check for filename suffix matches

    // resvg trumps Qt's SVG engine
#ifdef PQMRESVG
    if(PQCImageFormats::get().getAllFormatsResvg().contains(suffix))
        err = PQCLoadImageResvg::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMPOPPLER
    if(img.isNull() && PQCImageFormats::get().getAllFormatsPoppler().contains(suffix))
        err = PQCLoadImagePoppler::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMQTPDF
    if(img.isNull() && PQCImageFormats::get().getAllFormatsPoppler().contains(suffix))
        err = PQCLoadImageQtPDF::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && PQCImageFormats::get().getAllFormatsQt().contains(suffix))
        err = PQCLoadImageQt::load(filename, requestedSize, origSize, img);

#ifdef PQMRAW
    if(img.isNull() && PQCImageFormats::get().getAllFormatsLibRaw().contains(suffix))
        err = PQCLoadImageRAW::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMLIBARCHIVE
    if(img.isNull() && PQCImageFormats::get().getAllFormatsLibArchive().contains(suffix))
        err = PQCLoadImageArchive::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && PQCImageFormats::get().getAllFormatsXCFTools().contains(suffix))
        err = PQCLoadImageXCF::load(filename, requestedSize, origSize, img);

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    if(img.isNull() && PQCImageFormats::get().getAllFormatsMagick().contains(suffix))
        err = PQCLoadImageMagick::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMLIBVIPS
    if((err != "" || img.isNull()) && PQCImageFormats::get().getAllFormatsLibVips().contains(suffix))
        err = PQCLoadImageLibVips::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMFREEIMAGE
    if(img.isNull() && PQCImageFormats::get().getAllFormatsFreeImage().contains(suffix))
        err = PQCLoadImageFreeImage::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMDEVIL
    if(img.isNull() && PQCImageFormats::get().getAllFormatsDevIL().contains(suffix))
        err = PQCLoadImageDevil::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && (PQCImageFormats::get().getAllFormatsVideo().contains(suffix) || PQCImageFormats::get().getAllFormatsLibmpv().contains(suffix)))
        err = PQCLoadImageVideo::load(filename, requestedSize, origSize, img);


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // if that failed, then we check for mimetype matches
    if(img.isNull()) {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(filename).name();

        if(mimetype != "" && mimetype != "application/octet-stream") {

            // resvg trumps Qt's SVG engine
#ifdef PQMRESVG
            if(PQCImageFormats::get().getAllMimeTypesResvg().contains(suffix))
                err = PQCLoadImageResvg::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMPOPPLER
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesPoppler().contains(mimetype))
                err = PQCLoadImagePoppler::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMQTPDF
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesPoppler().contains(mimetype))
                err = PQCLoadImageQtPDF::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesQt().contains(mimetype))
                err = PQCLoadImageQt::load(filename, requestedSize, origSize, img);

#ifdef PQMRAW
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesLibRaw().contains(mimetype))
                err = PQCLoadImageRAW::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMLIBARCHIVE
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesLibArchive().contains(mimetype))
                err = PQCLoadImageArchive::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesXCFTools().contains(mimetype))
                err = PQCLoadImageXCF::load(filename, requestedSize, origSize, img);

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesMagick().contains(mimetype))
                err = PQCLoadImageMagick::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMLIBVIPS
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesLibVips().contains(mimetype))
                err = PQCLoadImageLibVips::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMFREEIMAGE
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesFreeImage().contains(mimetype))
                err = PQCLoadImageFreeImage::load(filename, requestedSize, origSize, img);
#endif

#ifdef PQMDEVIL
            if(img.isNull() && PQCImageFormats::get().getAllMimeTypesDevIL().contains(mimetype))
                err = PQCLoadImageDevil::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && (PQCImageFormats::get().getAllMimeTypesVideo().contains(mimetype) || PQCImageFormats::get().getAllMimeTypesLibmpv().contains(mimetype)))
                err = PQCLoadImageVideo::load(filename, requestedSize, origSize, img);

        }

    }


#if defined(PQMGRAPHICSMAGICK) || defined(PQMIMAGEMAGICK)
    // if everything failed, we make sure to try one more time with ImageMagick or GraphicsMagick to see what could be done
    // we do not do this for video files as it can lead to resource intensive ffmpeg processes that may persist after PreviewQt is closed
    if(img.isNull() && !PQCImageFormats::get().getAllFormatsVideo().contains(suffix) && !PQCImageFormats::get().getAllFormatsLibmpv().contains(suffix)) {

        qDebug() << "null image, try magick";

        // we do not override the old error message
        PQCLoadImageMagick::load(filename, requestedSize, origSize, img);

    }
#endif

    return err;

}
