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

#include <pqc_loadimage_qtpdf.h>
#include <QSize>
#include <QImage>
#include <QtDebug>

#ifdef PQMQTPDF
#include <QtPdf/QPdfDocument>
#include <QtPdf/QtPdf>
#endif

PQCLoadImageQtPDF::PQCLoadImageQtPDF() {}

QString PQCLoadImageQtPDF::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize =" << maxSize;

    QString errormsg = "";

#ifdef PQMQTPDF

    // extract page and totalpage value from filename (prepended to filename (after filepath))
    int page = 0;
    QString realFileName = filename;
    if(filename.contains("::PDF::")) {
        page = filename.split("::PDF::").at(0).toInt();
        realFileName = filename.split("::PDF::").at(1);
    }

    QPdfDocument doc;
    doc.load(realFileName);

    QPdfDocument::Error err = doc.error();
    if(err != QPdfDocument::Error::None) {
        errormsg = "Error occured loading PDF";
        qWarning() << errormsg;
        return errormsg;
    }

    QSizeF _pageSize = (doc.pagePointSize(page)/72.0*qApp->primaryScreen()->physicalDotsPerInch())*(100/72.0);
    origSize = QSize(_pageSize.width(), _pageSize.height());

    QImage p = doc.render(page, origSize);

    if(p.isNull()) {
        errormsg = QString("Unable to read page %1").arg(page);
        qWarning() << errormsg;
        return errormsg;
    }

    // some pdfs don't specify a background
    // in that case the resulting image will have a transparent background
    // to "fix" this we simply draw the image on top of a white image
    img = QImage(p.size(), p.format());
    img.fill(Qt::white);
    QPainter paint(&img);
    paint.drawImage(QRect(QPoint(0,0), img.size()), p);
    paint.end();

    // Scale image if necessary
    if(maxSize.width() != -1) {

        QSize finalSize = origSize;

        if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height())
            finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

        img = img.scaled(finalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    }

    return "";

#endif

    origSize = QSize(-1,-1);
    errormsg = "Failed to load image, QtPDF not supported by this build of PreviewQt!";
    qWarning() << errormsg;
    return errormsg;

}
