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

#include <pqc_providerfull.h>
#include <pqc_loadimage.h>
#include <pqc_scripts.h>
#include <QFileInfo>
#include <QCoreApplication>

PQCProviderFull::PQCProviderFull() : QQuickImageProvider(QQuickImageProvider::Image) {}

PQCProviderFull::~PQCProviderFull() {}

QImage PQCProviderFull::requestImage(const QString &url, QSize *origSize, const QSize &requestedSize) {

    qDebug() << "args: url =" << url;
    qDebug() << "args: requestedSize =" << requestedSize;

    QString filename = PQCScripts::cleanPath(QByteArray::fromPercentEncoding(url.toUtf8()));

    QString filenameForChecking = filename;
    if(filenameForChecking.contains("::PDF::"))
        filenameForChecking = filenameForChecking.split("::PDF::").at(1);
    if(filenameForChecking.contains("::ARC::"))
        filenameForChecking = filenameForChecking.split("::ARC::").at(1);

    if(!QFileInfo::exists(filenameForChecking)) {
        QString err = QCoreApplication::translate("imageprovider", "File failed to load, it does not exist!");
        qWarning() << "ERROR:" << err;
        qWarning() << "Filename:" << filenameForChecking;
        return QImage();
    }

    // Load image
    QImage ret;
    PQCLoadImage::get().load(filename, requestedSize, *origSize, ret);

    // if returned image is not an error image ...
    if(ret.isNull())
        return QImage();

    // return scaled version
    if(requestedSize.width() > 2 && requestedSize.height() > 2 && origSize->width() > requestedSize.width() && origSize->height() > requestedSize.height())
        return ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // return full version
    return ret;

}
