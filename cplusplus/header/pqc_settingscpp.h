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
#pragma once

#include <QObject>
#include <pqc_configfiles.h>
#include <QSettings>
#include <QFileSystemWatcher>

class QSettings;
class QTimer;

class PQCSettingsCPP : public QObject {

    Q_OBJECT

public:
    static PQCSettingsCPP& get() {
        static PQCSettingsCPP instance;
        return instance;
    }
    ~PQCSettingsCPP() {
        delete settings;
    }

    PQCSettingsCPP(PQCSettingsCPP const&) = delete;
    void operator=(PQCSettingsCPP const&) = delete;

    QString getLanguage() { return m_language; }
    QString getVersion() { return m_version; }
    // the version is the ONLY one that can be changed from here and will be picked up in the PQCSettings class
    void setVersion(QString val) { m_version = val; Q_EMIT versionChanged(); }

    QString getFiledialogLocation() { return m_filedialogLocation; }

    QString getDefaultAppImages() { return m_defaultAppImages; }
    QString getDefaultAppDocuments() { return m_defaultAppDocuments; }
    QString getDefaultAppArchives() { return m_defaultAppArchives; }
    QString getDefaultAppVideos() { return m_defaultAppVideos; }
    QString getDefaultAppComicBooks() { return m_defaultAppComicBooks; }
    QString getDefaultAppEBooks() { return m_defaultAppEBooks; }
    QString getDefaultAppText() { return m_defaultAppText; }

    bool getCloseAfterDefaultApp() { return m_closeAfterDefaultApp; }

private:
    PQCSettingsCPP() {

        settings = new QSettings(PQCConfigFiles::get().CONFIG_DIR() + "/settings", QSettings::IniFormat);

        readSettings();

        watcher = new QFileSystemWatcher;
        watcher->addPath(settings->fileName());
        connect(watcher, &QFileSystemWatcher::fileChanged, this, [=](QString) { readSettings(); } );

    }

    void readSettings() {

        m_language = settings->value("language", "en").toString();
        m_version  = settings->value("version", "").toString();
        m_filedialogLocation = settings->value("filedialogLocation", "").toString();

        m_defaultAppImages = settings->value("defaultAppImages", "").toString();
        m_defaultAppDocuments = settings->value("defaultAppDocuments", "").toString();
        m_defaultAppArchives = settings->value("defaultAppArchives", "").toString();
        m_defaultAppVideos = settings->value("defaultAppVideos", "").toString();
        m_defaultAppComicBooks = settings->value("defaultAppComicBooks", "").toString();
        m_defaultAppEBooks = settings->value("defaultAppEBooks", "").toString();
        m_defaultAppText = settings->value("defaultAppText", "").toString();

        m_closeAfterDefaultApp = settings->value("closeAfterDefaultApp", true).toBool();

    }

    QSettings *settings;
    QFileSystemWatcher *watcher;

    QString m_language;
    QString m_version;
    QString m_filedialogLocation;

    QString m_defaultAppImages;
    QString m_defaultAppDocuments;
    QString m_defaultAppArchives;
    QString m_defaultAppVideos;
    QString m_defaultAppComicBooks;
    QString m_defaultAppEBooks;
    QString m_defaultAppText;

    bool m_closeAfterDefaultApp;

Q_SIGNALS:
    void versionChanged();

};
