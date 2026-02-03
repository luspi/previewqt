/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
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

#include <pqc_cache.h>
#include <pqc_configfiles.h>
#include <QSettings>
#include <QTimer>
#include <QFileInfo>
#include <QCryptographicHash>

PQCCache::PQCCache() {
    cache = new QSettings(PQCConfigFiles::get().CACHE_DIR() + "/poscache", QSettings::IniFormat);
}

PQCCache::~PQCCache() {
    delete cache;
}

void PQCCache::setEntry(QString path, QString value) {

    QFileInfo info(path);
    if(!info.exists())
        return;

    const QString key = QCryptographicHash::hash(QString("%1::%2").arg(info.lastModified().toMSecsSinceEpoch()).arg(path).toUtf8(), QCryptographicHash::Md5);

    cache->setValue(key, value);

}

QString PQCCache::getEntry(QString path) {

    QFileInfo info(path);
    if(!info.exists())
        return "";

    const QString key = QCryptographicHash::hash(QString("%1::%2").arg(info.lastModified().toMSecsSinceEpoch()).arg(path).toUtf8(), QCryptographicHash::Md5);

    if(cache->contains(key))
        return cache->value(key, "").toString();

    return "";

}
