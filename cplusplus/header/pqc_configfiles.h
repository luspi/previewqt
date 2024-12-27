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

#ifndef PQCCONFIGFILES_H
#define PQCCONFIGFILES_H

#include <QObject>
#include <QStandardPaths>

class PQCConfigFiles {

public:

    static const QString CONFIG_DIR() {
        return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    }

    static const QString CACHE_DIR() {
        return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    }

    static const QString FILEFORMATS_DB() {
        return QString("%1/fileformats.db").arg(CONFIG_DIR());
    }

    // This is the old filename for all supported formats.
    // It should not be used anywhere anymore.
    // Instead, the fileformats.db above should be used.
    static const QString IMAGEFORMATS_DB() {
        return QString("%1/imageformats.db").arg(CONFIG_DIR());
    }

};

#endif // PQCCONFIGFILES_H
