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

#include <pqc_loadimage_xcf.h>
#include <QProcess>
#include <QDir>
#include <QImageReader>

PQCLoadImageXCF::PQCLoadImageXCF() {}

// loads the png and returns it
QString PQCLoadImageXCF::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize =" << maxSize;

    QString errormsg = "";

    // We first check if xcftools is actually installed
    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << "xcf2png");
    which.waitForFinished();
    // If it isn't -> display error
    if(which.exitCode()) {
        errormsg = "'xcftools' not found";
        qWarning() << errormsg;
        return errormsg;
    }

    // Convert xcf to png using xcf2png (part of xcftools)
    QProcess p;
    p.execute("xcf2png", QStringList() << filename << "-o" << QString(QDir::tempPath() + "/previewqt_xcf.png"));

    // And load it
    QImageReader reader(QDir::tempPath() + "/previewqt_xcf.png");

    origSize = reader.size();

    // Make sure image fits into size specified by maxSize
    if(maxSize.width() > 5 && maxSize.height() > 5) {
        if(origSize.width() > maxSize.width() || origSize.height() > maxSize.height())
            reader.setScaledSize(origSize.scaled(maxSize, Qt::KeepAspectRatio));
    }

    img = reader.read();

    if(img.isNull()) {
        errormsg = "Invalid PNG image rendered by xcftools.";
        qWarning() << errormsg;
        return errormsg;
    }

    return "";

}
