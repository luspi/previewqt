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

#include <pqc_loadimage_magick.h>
#include <pqc_imageformats.h>
#include <QSize>
#include <QImage>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QtDebug>

PQCLoadImageMagick::PQCLoadImageMagick() {}

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
QString PQCLoadImageMagick::load(QString filename, QSize maxSize, QSize &origSize, QImage &img, Magick::Image &fullImage) {
#else
QString PQCLoadImageMagick::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {
#endif

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize =" << maxSize;

    QString errormsg = "";

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)

    QSize finalSize;

    // Prepare Magick
    QString suf = QFileInfo(filename).suffix().toUpper();
    Magick::Image image;

    QMimeDatabase db;
    QString mimetype = db.mimeTypeForFile(filename).name();

    QStringList mgs;
    if(suf != "") {
        mgs = QStringList() << suf.toUpper();
        if(PQCImageFormats::get().getMagick().keys().contains(suf.toLower()))
            mgs = PQCImageFormats::get().getMagick().value(suf.toLower()).toStringList();
    }
    if(mimetype != "") {
        if(PQCImageFormats::get().getMagickMimeType().keys().contains(mimetype)) {
            const QStringList lst = PQCImageFormats::get().getMagickMimeType().value(mimetype).toStringList();
            for(const QString &mt : lst)
                if(!mgs.contains(mt))
                    mgs << mt;
        }
    }

    // if nothing else worked try without any magick, maybe this will help...
    mgs << "";

    int howOftenFailed = 0;
    for(int i = 0; i < mgs.length(); ++i) {

        try {

            // set current magick
            image.magick(mgs.at(i).toUpper().toStdString());

            // Read image into Magick
            image.read(filename.toStdString());

            fullImage = image;

            // done with the loop if we manage to get here.
            break;

        } catch(Magick::Exception &e) {

            ++howOftenFailed;
            qWarning() << e.what();
            errormsg += QString("<div style='margin-bottom: 5px'>%1</div>").arg(e.what());

        }

    }

    // no attempt was successful -> stop here
    if(howOftenFailed == mgs.length()) {
        // no need to add anything to the errormsg variable here, it already contains the errors from the loop above
        qWarning() << "Failed to read image";
        return "Failed to read image";
    }

    try {

        finalSize = QSize(image.columns(), image.rows());
        origSize = finalSize;

        // Scale image if necessary
        if(maxSize.width() != -1) {

            if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height()) {
                finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

                // For small images we can use the faster algorithm, as the quality is good enough for that
                if(finalSize.width() < 300 && finalSize.height() < 300)
                    image.thumbnail(Magick::Geometry(finalSize.width(),finalSize.height()));
                else
                    image.scale(Magick::Geometry(finalSize.width(),finalSize.height()));

            }

        }

        // Write Magick as PPM to memory
        Magick::Blob ob;
        image.magick("PPM");
        image.write(&ob);

        // And load image from memory into QImage
        const QByteArray imgData((char*)(ob.data()),ob.length());
        img = QImage::fromData(imgData);

        // And we're done!
        return "";

    } catch(Magick::Exception &e) {
        errormsg = e.what();
        qWarning() << errormsg;
        return errormsg;
    }

#endif

    origSize = QSize(-1,-1);
    errormsg = "Failed to load image, ImageMagick/GraphicsMagick not supported by this build of PreviewQt!";
    qWarning() << errormsg;
    return errormsg;

}
