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

#include <pqc_providersvg.h>
#ifdef PQMRESVG
#include <pqc_loadfile_resvg.h>
#else
#endif
#include <QSvgRenderer>
#include <QPainter>

PQCProviderSVG::PQCProviderSVG() : QQuickImageProvider(QQuickImageProvider::Image) {}

PQCProviderSVG::~PQCProviderSVG() {}

QImage PQCProviderSVG::requestImage(const QString &url, QSize *origSize, const QSize &requestedSize) {

    // we don't have a debug statement here as there would be A LOT of output all the time

    QImage ret;

    if(requestedSize.isNull()) {
        ret = QImage(1,1, QImage::Format_ARGB32);
        ret.fill(Qt::transparent);
        return ret;
    }

#ifdef PQMRESVG

    PQCLoadFileResvg::load(url, requestedSize, *origSize, ret);
    return ret;

#endif

    // For reading SVG files
    QSvgRenderer svg;

    // Loading SVG file
    svg.load(url);

    // Invalid vector graphic
    if(!svg.isValid()) {
        qWarning() << "Error: invalid svg file:" << url;
        return QImage();
    }

    // Store the width/height for later use
    *origSize = svg.defaultSize();

    QSize use = requestedSize;
    if(requestedSize.width() < 2 || requestedSize.height() < 2)
        use = svg.defaultSize();

    QSize fitSize = origSize->scaled(use, Qt::KeepAspectRatio);
    QRect fitRect(QPoint(0,0), fitSize);

    // Render SVG into pixmap
    ret = QImage(fitSize, QImage::Format_ARGB32);
    ret.fill(::Qt::transparent);
    QPainter painter(&ret);
    svg.render(&painter, fitRect);
    painter.end();

    return ret;

}
