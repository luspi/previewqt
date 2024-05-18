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
#include <QSize>
#include <QImage>
#include <QFileInfo>
#include <QSvgRenderer>
#include <QImageReader>
#include <QPainter>
#include <QtDebug>
#include <QMimeDatabase>

PQCLoadImageQt::PQCLoadImageQt() {}

QString PQCLoadImageQt::load(QString filename, QSize maxSize, QSize &origSize, QImage &img, QImage &fullImage) {

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize =" << maxSize;

    QString errormsg = "";

    // Suffix, for easier access later-on
    QString suffix = QFileInfo(filename).suffix().toLower();

    if(suffix == "svg" || suffix == "svgz") {

        // For reading SVG files
        QSvgRenderer svg;

        // Loading SVG file
        svg.load(filename);

        // Invalid vector graphic
        if(!svg.isValid()) {
            errormsg = "Error: invalid svg file";
            qWarning() << errormsg;
            return errormsg;
        }

        // Store the width/height for later use
        origSize = svg.defaultSize();

        // Render SVG into pixmap
        if(maxSize.width() > 5 || maxSize.height() > 5)
            img = QImage(svg.defaultSize().scaled(maxSize, Qt::KeepAspectRatio), QImage::Format_ARGB32);
        else
            img = QImage(svg.defaultSize(), QImage::Format_ARGB32);
        img.fill(::Qt::transparent);
        QPainter painter(&img);
        svg.render(&painter);

        return "";

    } else {

        // For all other supported file types
        QImageReader reader;

        // disable allocation limit check
        // there is a bug in Qt 6.6 with a disabled allocation limit and ICNS files:
        // https://bugreports.qt.io/browse/QTBUG-118797
        if(suffix != "icns")
            reader.setAllocationLimit(0);

        // Setting QImageReader
        reader.setFileName(filename);

        // this loads the image properly even if the extension is wrong
        QMimeDatabase db;
        QMimeType mimetype = db.mimeTypeForFile(filename, QMimeDatabase::MatchContent);
        if(!mimetype.isValid()) {
            errormsg = "invalid mime type received";
            qWarning() << "Error:" << errormsg;
            return errormsg;
        }
        QStringList mime = mimetype.name().split("/");
        if(mime.size() == 2 && mime.at(0) == "image")
            reader.setFormat(mime.at(1).toUtf8());

        reader.setAutoTransform(true);

        bool imgAlreadyLoaded = false;

        // Store the width/height for later use
        origSize = reader.size();
        // check if we need to read the image in full to get the original size
        if(!origSize.isValid()) {
            reader.read(&fullImage);
            imgAlreadyLoaded = true;
            origSize = fullImage.size();
        }

        // check if we need to scale the image
        if(maxSize.isValid() && origSize.isValid() && !maxSize.isNull() && !origSize.isNull()) {

            if(origSize.width() > maxSize.width() || origSize.height() > maxSize.height()) {
                if(imgAlreadyLoaded)
                    img = fullImage.scaled(origSize.scaled(maxSize, Qt::KeepAspectRatio), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                else
                    reader.setScaledSize(origSize.scaled(maxSize, Qt::KeepAspectRatio));
            }

        }

        // Eventually load the image
        if(!reader.canRead()) {
            errormsg = "image reader unable to read image";
            qWarning() << errormsg;
            return errormsg;
        }

        if(!imgAlreadyLoaded) {
            reader.read(&img);
        }

        // If an error occured
        if(img.isNull()) {
            errormsg = reader.errorString();
            qWarning() << errormsg;
            return errormsg;
        }

        return "";

    }

}
