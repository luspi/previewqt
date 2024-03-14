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

#include <pqc_loadimage_video.h>
#include <QImage>
#include <QImageReader>
#include <QProcess>
#include <QDir>

PQCLoadImageVideo::PQCLoadImageVideo() {}

PQCLoadImageVideo::~PQCLoadImageVideo() {}

QString PQCLoadImageVideo::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    qDebug() << "** filename =" << filename;
    qDebug() << "** maxSize =" << maxSize;

    QString errormsg = "";

#ifdef Q_OS_LINUX

    // the temp image thumbnail path (incl random int)
    QString tmp_path = QString("%1/previewqt_videothumb_%2.jpg").arg(QDir::tempPath()).arg(rand());

    // create thumbnail using ffmpegthumbnailer
    QProcess proc;
    int ret = proc.execute("ffmpegthumbnailer", QStringList() << "-i" << filename << "-s0" << "-o" << tmp_path);

    // without this it seems like zombie ffmpeg processes might appear
    proc.kill();

    if(ret != 0) {
        errormsg = QString("ffmpegthumbnailer ended with error code %1 - is it installed?").arg(ret);
        qWarning() << errormsg;
        return errormsg;
    }

    img = QImage(tmp_path);

    origSize = img.size();

    // remove temporary thumbnail file
    QFile::remove(tmp_path);

    // Scale image if necessary
    if(maxSize.width() != -1) {

        QSize finalSize = origSize;

        if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height())
            finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

        img = img.scaled(finalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    }

    // store in return variable
    return "";

#endif

    errormsg = "Video thumbnail creation currently only supported on Linux";
    qWarning() << errormsg;
    return errormsg;

}
