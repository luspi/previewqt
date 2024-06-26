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

#ifndef PQCLOADIMAGE_H
#define PQCLOADIMAGE_H

#include <QImage>
#include <QDateTime>

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
#include <Magick++/Image.h>
#endif

class QSize;
class QString;

class PQCLoadImage {

public:
    static PQCLoadImage& get() {
        static PQCLoadImage instance;
        return instance;
    }
    ~PQCLoadImage();

    QString load(QString filename, QSize requestedSize, QSize &origSize, QImage &img);

private:
    PQCLoadImage();

    QImage cachedImage;
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    Magick::Image cachedImageMagick;
#endif
    QString cachedImageSource;
    QDateTime cachedImageLastModified;
};

#endif // PQCLOADIMAGE_H
