/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
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

#include <vqc_loadimage_resvg.h>
#include <QSize>
#include <QImage>
#include <QtDebug>
#ifdef VQMRESVG
#include <ResvgQt.h>
#endif

VQCLoadImageResvg::VQCLoadImageResvg() {}

QString VQCLoadImageResvg::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize =" << maxSize;

#ifdef VQMRESVG

    ResvgOptions opt;
    ResvgRenderer renderer(filename, opt);

    if(!renderer.isValid()) {
        QString errmsg = "Invalid SVG encountered";
        qWarning() << errmsg;
        return errmsg;
    }

    origSize = renderer.defaultSize();

    if(maxSize.isValid())
        img = renderer.renderToImage(maxSize);
    else
        img = renderer.renderToImage();

    return "";

#endif

    return "";

}
