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

#ifndef PQCLoadFile_H
#define PQCLoadFile_H

#include <QImage>
#include <QDateTime>

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
#include <Magick++/Image.h>
#endif

class QSize;
class QString;

class PQCLoadFile {

public:
    static PQCLoadFile& get() {
        static PQCLoadFile instance;
        return instance;
    }
    ~PQCLoadFile();

    QString load(QString filename, QSize requestedSize, QSize &origSize, QImage &img);

private:
    PQCLoadFile();

    QImage cachedImage;
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    Magick::Image cachedImageMagick;
#endif
    QString cachedImageSource;
    QDateTime cachedImageLastModified;
};

#endif // PQCLoadFile_H
