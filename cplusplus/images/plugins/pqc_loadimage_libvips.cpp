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

#include <pqc_loadimage_libvips.h>
#include <QSize>
#include <QtDebug>
#include <QImage>

#ifdef PQMLIBVIPS
#include <vips/vips.h>
#endif

PQCLoadImageLibVips::PQCLoadImageLibVips() {}

QString PQCLoadImageLibVips::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    qDebug() << "args: filename = " << filename;
    qDebug() << "args: maxSize = " << maxSize;

    QString errormsg = "";

#ifdef PQMLIBVIPS

    // we use the C API as the equivalent C++ API calls led to crash on subsequent call

    // The vips image object
    VipsImage *in;

    // attempt to the load the image
    if(!(in = vips_image_new_from_file(filename.toStdString().c_str(), NULL))) {
        g_object_unref(in);
        errormsg = "vips_image_new_from_file: failed to load image from file";
        qDebug() << errormsg;
        return errormsg;
    }

    // store original size
    origSize = QSize(vips_image_get_width(in), vips_image_get_height(in));

    // convert VipsImage to QImage
    img = QImage((uchar*)vips_image_get_data(in), vips_image_get_width(in), vips_image_get_height(in), VIPS_IMAGE_SIZEOF_LINE(in), QImage::Format_RGB888);
    if(img.isNull()) {
        errormsg = "converting VipsImage to QImage failed";
        qDebug() << errormsg;
        return errormsg;
    }

    g_object_unref(in);

    // Scale image if necessary
    if(maxSize.width() != -1) {

        QSize finalSize = origSize;

        if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height())
            finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

        img = img.scaled(finalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    }

    return "";

#endif
    origSize = QSize(-1,-1);
    errormsg = "Failed to load image, libvips not supported by this build of PreviewQt!";
    qDebug() << errormsg;
    return errormsg;

}
