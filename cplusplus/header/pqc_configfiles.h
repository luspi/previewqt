/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
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

    const QString FILEFORMATS_DB() {
        return QString("%1/fileformats.db").arg(m_CONFIG_DIR);
    }

    // This is the old filename for all supported formats.
    // It should not be used anywhere anymore.
    // Instead, the fileformats.db above should be used.
    const QString IMAGEFORMATS_DB() {
        return QString("%1/imageformats.db").arg(m_CONFIG_DIR);
    }

private:
    PQCConfigFiles() {

#ifdef PQMPORTABLETWEAKS
        previewqt_exe_basedir = qgetenv("PREVIEWQT_EXE_BASEDIR");
#endif

#ifdef PQMPORTABLETWEAKS
#ifdef Q_OS_WIN
        m_CONFIG_DIR = QString("%1/previewqt-data/config/").arg(previewqt_exe_basedir);
        m_CACHE_DIR = QString("%1/previewqt-data/cache/").arg(previewqt_exe_basedir);
#else
        m_CONFIG_DIR = QString("%1/.previewqt-data/config/").arg(previewqt_exe_basedir);
        m_CACHE_DIR = QString("%1/.previewqt-data/cache/").arg(previewqt_exe_basedir);
#endif
#else
        m_CONFIG_DIR = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        m_CACHE_DIR = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
#endif

        m_IMAGEFORMATS_DB = QString("%1/imageformats.db").arg(m_CONFIG_DIR);

    }

    QString m_CONFIG_DIR;
    QString m_CACHE_DIR;
    QString m_IMAGEFORMATS_DB;


#ifdef PQMPORTABLETWEAKS
    QString previewqt_exe_basedir;
#endif

};

#endif // PQCCONFIGFILES_H
