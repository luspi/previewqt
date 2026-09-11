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

#include <fileplugins/pqc_fileplugin_image.h>

#include <fileplugins/images/pqc_imageplugin_devil.h>
#include <fileplugins/images/pqc_imageplugin_libraw.h>
#include <fileplugins/images/pqc_imageplugin_libsai.h>
#include <fileplugins/images/pqc_imageplugin_libvips.h>
#include <fileplugins/images/pqc_imageplugin_magick.h>
#include <fileplugins/images/pqc_imageplugin_openslide.h>
#include <fileplugins/images/pqc_imageplugin_qt.h>
#include <fileplugins/images/pqc_imageplugin_resvg.h>

PQCFilePluginImage::PQCFilePluginImage() {

#ifdef PQMRESVG
    m_plugins.append(new PQCImagePluginResvg);
#endif
#ifdef PQMOPENSLIDE
    m_plugins.append(new PQCImagePluginOpenSlide);
#endif
    m_plugins.append(new PQCImagePluginQt);
#ifdef PQMRAW
    m_plugins.append(new PQCImagePluginLibraw);
#endif
#ifdef PQMLIBSAI
    m_plugins.append(new PQCImagePluginLibsai);
#endif
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    m_plugins.append(new PQCImagePluginMagick);
#endif
#ifdef PQMLIBVIPS
    m_plugins.append(new PQCImagePluginLibVips);
#endif
#ifdef PQMDEVIL
    m_plugins.append(new PQCImagePluginDevIL);
#endif

    QHash<int, QList<QStringList> > dat;
    for(PQCFilePlugin *pl : std::as_const(m_plugins))
        dat.insert(pl->getData());

    setData(dat);

}

PQCFilePluginImage::~PQCFilePluginImage() {
    for(PQCFilePlugin *pl : std::as_const(m_plugins))
        delete pl;
}

const QSize PQCFilePluginImage::loadSize(QString path) {

    for(PQCFilePlugin *pl : std::as_const(m_plugins)) {
        const QSize sze = pl->loadSize(path);
        if(!sze.isEmpty()) return sze;
    }

    return QSize();

}

const QImage PQCFilePluginImage::loadImage(QString path, QSize requestedSize, QSize &origSize, QString &error) {

    for(PQCFilePlugin *pl : std::as_const(m_plugins)) {
        const QImage img = pl->loadImage(path, requestedSize, origSize, error);
        if(!img.isNull()) return img;
    }

    return QImage();

}

const QJsonObject PQCFilePluginImage::loadJSON(QString path, QVariantMap extraArguments) {

    for(PQCFilePlugin *pl : std::as_const(m_plugins)) {
        const QJsonObject json = pl->loadJSON(path, extraArguments);
        if(!json.isEmpty()) return json;
    }

    return {};

}
