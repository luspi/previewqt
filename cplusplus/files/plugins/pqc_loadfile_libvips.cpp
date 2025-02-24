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

#include <pqc_loadfile_libvips.h>
#include <QSize>
#include <QtDebug>
#include <QImage>

#ifdef PQMLIBVIPS
#include <vips/vips.h>
#endif

PQCLoadFileLibVips::PQCLoadFileLibVips() {}

QString PQCLoadFileLibVips::load(QString filename, QSize maxSize, QSize &origSize, QImage &img, QImage &fullImage) {

    qDebug() << "args: filename = " << filename;
    qDebug() << "args: maxSize = " << maxSize;

    QString errormsg = "";

#ifdef PQMLIBVIPS

    // we use the C API as the equivalent C++ API calls led to crash on subsequent call

    // attempt to the load the image
    VipsImage *vimg = vips_image_new_from_file(filename.toStdString().c_str(), "memory", true, NULL);
    if(vimg == NULL) {
        errormsg = "vips_image_new_from_file: failed to load image from file";
        qDebug() << errormsg;
        return errormsg;
    }

    // store original size
    origSize = QSize(vimg->Xsize, vimg->Ysize);

    void *buf = nullptr;
    size_t len = 0;
    vips_image_write_to_buffer(vimg, ".png", &buf, &len, NULL);

    // Convert VipsImage to raw data
    img.loadFromData(reinterpret_cast<const uchar *>(buf), len);
    if(img.isNull()) {
        errormsg = "converting VipsImage to QImage failed";
        qDebug() << errormsg;
        return errormsg;
    }

    g_object_unref(vimg);
    g_free(buf);

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
    errormsg = "Failed to load image, libvips not supported by this build of PhotoQt!";
    qDebug() << errormsg;
    return errormsg;

}
