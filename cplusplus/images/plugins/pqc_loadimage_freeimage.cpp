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

#include <pqc_loadimage_freeimage.h>

#include <QString>
#include <QImage>
#include <QSize>
#include <QtDebug>

#ifdef PQMFREEIMAGE
#include <FreeImage.h>
#endif

PQCLoadImageFreeImage::PQCLoadImageFreeImage() {}

QString PQCLoadImageFreeImage::load(QString filename, QSize maxSize, QSize &origSize, QImage &img, QImage &fullImage) {

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize = " << maxSize;

#ifdef PQMFREEIMAGE

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.toStdString().c_str());
    if(fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename.toStdString().c_str());
    if(fif == FIF_UNKNOWN) {
        QString errormsg = "Unknown file type (FIF_UNKNOWN)";
        qWarning() << errormsg;
        return errormsg;
    }

    FIBITMAP *image = FreeImage_Load(fif, filename.toStdString().c_str());
    if(!image) {
        QString errormsg = "Error loading image: " + filename;
        qWarning() << errormsg;
        return errormsg;
    }

    const unsigned int width = FreeImage_GetWidth(image);
    const unsigned int height = FreeImage_GetHeight(image);

    // the width/height of the image, needed to ensure we respect the maxSize further down
    origSize = QSize(width, height);

    // This will be the access handler for the data that we can load into QImage
    FIMEMORY *stream = FreeImage_OpenMemory();
    if(!FreeImage_SaveToMemory(FIF_PPM, image, stream, 0)) {
        QString errormsg = "Unable to save to memory";
        qWarning() << errormsg;
        return errormsg;
    }

    // These will be the raw data (and its size) that we are after
    BYTE *mem_buffer = nullptr;
    DWORD size_in_bytes = 0;

    FreeImage_AcquireMemory(stream, &mem_buffer, &size_in_bytes);
    if(size_in_bytes == 0) {
        QString errormsg = "Unable to load image from memory";
        qWarning() << errormsg;
        return errormsg;
    }

    // Load data into QImage
    fullImage = QImage::fromData(QByteArray::fromRawData((char*)mem_buffer, size_in_bytes));

    if(fullImage.isNull()) {
        QString errormsg = "Loading FreeImage image into QImage resulted in NULL image";
        qWarning() << errormsg;
        return errormsg;
    }

    // Scale image if necessary
    if(maxSize.width() != -1) {

        QSize finalSize = origSize;

        if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height())
            finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

        img = fullImage.scaled(finalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    } else
        img = fullImage;

    // return full image
    return "";

#endif

    origSize = QSize(-1,-1);
    QString errormsg = "Failed to load image, FreeImage not supported by this build of PreviewQt!";
    qWarning() << errormsg;
    return errormsg;

}
