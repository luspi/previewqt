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
#pragma once

#include <QObject>
#include <QStandardPaths>

class PQCConfigFiles {

public:

    static PQCConfigFiles& get() {
        static PQCConfigFiles instance;
        return instance;
    }
    ~PQCConfigFiles() {}

    PQCConfigFiles(PQCConfigFiles const&)     = delete;
    void operator=(PQCConfigFiles const&) = delete;

    const QString CONFIG_DIR() {
        return m_CONFIG_DIR;
    }

    const QString CACHE_DIR() {
        return m_CACHE_DIR;
    }

private:
    PQCConfigFiles() {

#ifdef PQMPORTABLETWEAKS
        const QString portablefolder = qgetenv("PREVIEWQT_PORTABLE_DATA_LOCATION");
        m_CONFIG_DIR = QString("%1/config/").arg(portablefolder);
        m_CACHE_DIR = QString("%1/cache/").arg(portablefolder);
#else
        m_CONFIG_DIR = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        m_CACHE_DIR = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#endif

    }

    QString m_CONFIG_DIR;
    QString m_CACHE_DIR;

};
