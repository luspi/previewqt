/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2026 Lukas Spies                                  **
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

#include <pqc_imageproviderfull.h>
#include <pqc_filehandler.h>
#include <pqc_scriptsfilespaths.h>
#include <pqc_scriptsimages.h>
#include <QFileInfo>
#include <QApplication>
#include <QColorSpace>
#include <pqc_helper.h>
#include <QPainter>

#ifdef PQMLCMS2
#include <lcms2.h>
#endif

PQCImageProviderFull::PQCImageProviderFull() : QQuickImageProvider(QQuickImageProvider::Image) {}

PQCImageProviderFull::~PQCImageProviderFull() {}

QImage PQCImageProviderFull::requestImage(const QString &url, QSize *origSize, const QSize &requestedSize) {

    QString filename = PQCScriptsFilesPaths::get().cleanPath(QByteArray::fromPercentEncoding(url.toUtf8()));

    QString filenameForChecking = PQCHelper::extractInsideFilename(filename);

    if(!QFileInfo::exists(filenameForChecking)) {
        QString err = QApplication::translate("imageprovider", "File failed to load, it does not exist!");
        qWarning() << "ERROR:" << err;
        qWarning() << "Filename:" << filenameForChecking;
        return QImage();
    }

    // Load image
    QString error = "";
    QImage ret = PQCFileHandler::get().getImage(filename, requestedSize, *origSize, error);

    // if returned image is not a valid image
    if(ret.isNull()) {
        PQCScriptsImages::get().addFileLoadError(error);
        return ret;
    }

    PQCScriptsImages::get().setFileLoadError("");

    // return scaled version
    if(requestedSize.width() > 2 && requestedSize.height() > 2 && origSize->width() > requestedSize.width() && origSize->height() > requestedSize.height())
        return ret.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // return full version
    return ret;

}
