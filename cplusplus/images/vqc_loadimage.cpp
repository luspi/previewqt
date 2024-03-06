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

#include <vqc_loadimage_qt.h>
#include <vqc_loadimage_resvg.h>
#include <vqc_loadimage_raw.h>
#include <vqc_loadimage_poppler.h>
#include <vqc_loadimage_qtpdf.h>
#include <vqc_loadimage_xcf.h>
#include <vqc_loadimage_magick.h>
#include <vqc_loadimage_libvips.h>
#include <vqc_loadimage_archive.h>
#include <vqc_loadimage_devil.h>
#include <vqc_loadimage_freeimage.h>
#include <vqc_loadimage_video.h>

#include <vqc_loadimage.h>
#include <vqc_imageformats.h>
#include <vqc_scripts.h>
#include <QSize>
#include <QImage>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QtDebug>

VQCLoadImage::VQCLoadImage() {}
VQCLoadImage::~VQCLoadImage() {}

QString VQCLoadImage::load(QString filename, QSize requestedSize, QSize &origSize, QImage &img) {

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
#ifdef VQMRESVG
    if(VQCImageFormats::get().getAllFormatsResvg().contains(suffix))
        err = VQCLoadImageResvg::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMPOPPLER
    if(img.isNull() && VQCImageFormats::get().getAllFormatsPoppler().contains(suffix))
        err = VQCLoadImagePoppler::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMQTPDF
    if(img.isNull() && VQCImageFormats::get().getAllFormatsPoppler().contains(suffix))
        err = VQCLoadImageQtPDF::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && VQCImageFormats::get().getAllFormatsQt().contains(suffix))
        err = VQCLoadImageQt::load(filename, requestedSize, origSize, img);

#ifdef VQMRAW
    if(img.isNull() && VQCImageFormats::get().getAllFormatsLibRaw().contains(suffix))
        err = VQCLoadImageRAW::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMLIBARCHIVE
    if(img.isNull() && VQCImageFormats::get().getAllFormatsLibArchive().contains(suffix))
        err = VQCLoadImageArchive::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && VQCImageFormats::get().getAllFormatsXCFTools().contains(suffix))
        err = VQCLoadImageXCF::load(filename, requestedSize, origSize, img);

#if defined(VQMIMAGEMAGICK) || defined(VQMGRAPHICSMAGICK)
    if(img.isNull() && VQCImageFormats::get().getAllFormatsMagick().contains(suffix))
        err = VQCLoadImageMagick::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMLIBVIPS
    if((err != "" || img.isNull()) && VQCImageFormats::get().getAllFormatsLibVips().contains(suffix))
        err = VQCLoadImageLibVips::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMFREEIMAGE
    if(img.isNull() && VQCImageFormats::get().getAllFormatsFreeImage().contains(suffix))
        err = VQCLoadImageFreeImage::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMDEVIL
    if(img.isNull() && VQCImageFormats::get().getAllFormatsDevIL().contains(suffix))
        err = VQCLoadImageDevil::load(filename, requestedSize, origSize, img);
#endif

    if(img.isNull() && (VQCImageFormats::get().getAllFormatsVideo().contains(suffix) || VQCImageFormats::get().getAllFormatsLibmpv().contains(suffix)))
        err = VQCLoadImageVideo::load(filename, requestedSize, origSize, img);


    //////////////////////////////////////////////
    //////////////////////////////////////////////
    // if that failed, then we check for mimetype matches
    if(img.isNull()) {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(filename).name();

        if(mimetype != "" && mimetype != "application/octet-stream") {

            // resvg trumps Qt's SVG engine
#ifdef VQMRESVG
            if(VQCImageFormats::get().getAllMimeTypesResvg().contains(suffix))
                err = VQCLoadImageResvg::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMPOPPLER
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesPoppler().contains(mimetype))
                err = VQCLoadImagePoppler::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMQTPDF
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesPoppler().contains(mimetype))
                err = VQCLoadImageQtPDF::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesQt().contains(mimetype))
                err = VQCLoadImageQt::load(filename, requestedSize, origSize, img);

#ifdef VQMRAW
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesLibRaw().contains(mimetype))
                err = VQCLoadImageRAW::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMLIBARCHIVE
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesLibArchive().contains(mimetype))
                err = VQCLoadImageArchive::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesXCFTools().contains(mimetype))
                err = VQCLoadImageXCF::load(filename, requestedSize, origSize, img);

#if defined(VQMIMAGEMAGICK) || defined(VQMGRAPHICSMAGICK)
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesMagick().contains(mimetype))
                err = VQCLoadImageMagick::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMLIBVIPS
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesLibVips().contains(mimetype))
                err = VQCLoadImageLibVips::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMFREEIMAGE
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesFreeImage().contains(mimetype))
                err = VQCLoadImageFreeImage::load(filename, requestedSize, origSize, img);
#endif

#ifdef VQMDEVIL
            if(img.isNull() && VQCImageFormats::get().getAllMimeTypesDevIL().contains(mimetype))
                err = VQCLoadImageDevil::load(filename, requestedSize, origSize, img);
#endif

            if(img.isNull() && (VQCImageFormats::get().getAllMimeTypesVideo().contains(mimetype) || VQCImageFormats::get().getAllMimeTypesLibmpv().contains(mimetype)))
                err = VQCLoadImageVideo::load(filename, requestedSize, origSize, img);

        }

    }


#if defined(VQMGRAPHICSMAGICK) || defined(VQMIMAGEMAGICK)
    // if everything failed, we make sure to try one more time with ImageMagick or GraphicsMagick to see what could be done
    // we do not do this for video files as it can lead to resource intensive ffmpeg processes that may persist after PhotoQt is closed
    if(img.isNull() && !VQCImageFormats::get().getAllFormatsVideo().contains(suffix) && !VQCImageFormats::get().getAllFormatsLibmpv().contains(suffix)) {

        qDebug() << "null image, try magick";

        // we do not override the old error message
        VQCLoadImageMagick::load(filename, requestedSize, origSize, img);

    }
#endif

    return err;

}
