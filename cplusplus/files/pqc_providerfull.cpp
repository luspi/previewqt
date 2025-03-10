/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
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
#include <pqc_loadfile.h>
#include <pqc_scripts.h>
#include <QFileInfo>
#include <QCoreApplication>

PQCProviderFull::PQCProviderFull() : QQuickImageProvider(QQuickImageProvider::Image) {}

PQCProviderFull::~PQCProviderFull() {}

QImage PQCProviderFull::requestImage(const QString &url, QSize *origSize, const QSize &requestedSize) {

    qDebug() << "args: url =" << url;
    qDebug() << "args: requestedSize =" << requestedSize;

    int rotation = 0;

    QString path = QByteArray::fromPercentEncoding(url.toUtf8());
    if(path.contains(":::")) {
        rotation = path.split(":::")[0].toInt();
        path = path.split(":::")[1];
    }
    QString filename = PQCScripts::cleanPath(path);

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

    QSize useRequestedSize = requestedSize;
    if(abs(rotation%180) == 90) {
        useRequestedSize = QSize(requestedSize.height(), requestedSize.width());
    }

    // Load image
    QImage ret;
    PQCLoadFile::get().load(filename, useRequestedSize, *origSize, ret);

    // if returned image is not an error image ...
    if(ret.isNull())
        return QImage();

    // check for embedded color profile
    PQCScripts::get().applyEmbeddedColorProfile(ret);

    // apply rotation
    QTransform transform;
    transform.rotate(rotation);

    // return scaled version
    if(requestedSize.width() > 2 && requestedSize.height() > 2 && origSize->width() > useRequestedSize.width() && origSize->height() > useRequestedSize.height())
        return ret.transformed(transform).scaled(useRequestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // return full version
    return ret.transformed(transform);

}
