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

#include <pqc_loadimage_unrar.h>
#include <pqc_scripts.h>
#include <QSize>
#include <QImage>
#include <QtDebug>
#include <QFileInfo>
#include <QProcess>

PQCLoadImageUNRAR::PQCLoadImageUNRAR() {}

QImage PQCLoadImageUNRAR::loadArchiveIntoImage(QString filename) {

    qDebug() << "args: filename =" << filename;

    // filter out name of archivefile and of compressed file inside
    QString archivefile = filename;
    QString compressedFilename = "";
    if(archivefile.contains("::ARC::")) {
        QStringList parts = archivefile.split("::ARC::");
        archivefile = parts.at(1);
        compressedFilename = parts.at(0);
    } else {
        QStringList cont = PQCScripts::get().listArchiveContent(archivefile);
        if(cont.length() == 0) {
            qWarning() << "Unable to list contents of archive file...";
            return QImage();
        }
        compressedFilename = cont.at(0).split("::ARC::").at(0);
    }

    if(!QFileInfo::exists(archivefile)) {
        qWarning() << "Unable to load RAR archive, file doesn't seem to exist...";
        return QImage();
    }

    // We first check if unrar is actually installed
    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << "unrar");
    which.waitForFinished();
    // If it isn't -> display error
    if(which.exitCode()) {
        qWarning() << "'unrar' not found";
        return QImage();
    }

    // Extract file to standard output (the -ierr flag moves any other output by unrar to standard error output -> ignored)
    QProcess p;
    p.start("unrar", QStringList() << "-ierr" << "p" << archivefile << compressedFilename);

    // Make sure everything starts off well
    if(!p.waitForStarted()) {
        qWarning() << "Unable to start 'unrar' process...";
        return QImage();
    }

    // This will hold the accumulated image data
    QByteArray imgdata = "";

    // if there is something to read, read it
    while(p.waitForReadyRead())
        imgdata.append(p.readAll());

    // And load image from the read data
    return QImage::fromData(imgdata);

}

QString PQCLoadImageUNRAR::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    img = loadArchiveIntoImage(filename);

    origSize = img.size();

    // If image data is invalid or something went wrong, show error image
    if(img.isNull()) {
        QString errormsg = "Extracted file is not valid image file...";
        qWarning() << errormsg;
        return errormsg;
    }

    // Scale image if necessary
    if(maxSize.width() != -1) {

        if(origSize.width() > maxSize.width() || origSize.height() > maxSize.height())
            img = img.scaled(origSize.scaled(maxSize, Qt::KeepAspectRatio), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    }

    return "";

}
