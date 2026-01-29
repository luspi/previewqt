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
#include <QDir>
#include <QTimer>

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
    QString getDefaultAppUrl() { return m_defaultAppUrl; }

    bool getCloseAfterDefaultApp() { return m_closeAfterDefaultApp; }

    QString getLastDownloadFolder() { return m_lastDownloadFolder; }
    void setLastDownloadFolder(QString val) { m_lastDownloadFolder = val; Q_EMIT lastDownloadFolderChanged(); }

    QString getExecutableYtDlp() { return m_executableYtDlp; }

private:
    PQCSettingsCPP() {

#ifndef Q_OS_WIN
        // these NEED TO BE DUPLICATED in pqc_settings.h/cpp
        opt_img = {"_default_", "photoqt", "gwenview", "nomacs", "eog", "feh", "gthumb", "mirage", "geeqie"};
        opt_doc = {"_default_", "okular", "evince", "atril", "photoqt"};
        opt_arc = {"_default_", "ark", "photoqt"};
        opt_com = {"_default_", "okular", "photoqt"};
        opt_bok = {"_default_", "ebook-viewer", "calibre", "okular"};
        opt_vid = {"_default_", "vlc", "mplayer", "photoqt"};
        opt_txt = {"_default_", "kate", "kwrite", "gedit", "sublime"};
        opt_url = {"_default_", "firefox", "chrome", "chromium"};
#else
        // on windows custom tools are needed
        // we need empty entries here to not crash when loading the settings
        opt_img = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_doc = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_arc = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_com = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_bok = {"_default_", ""};
        opt_vid = {"_default_", ""};
        opt_txt = {"_default_", ""};
        opt_url = {"_default_", ""};
#endif

        settings = new QSettings(PQCConfigFiles::get().CONFIG_DIR() + "/settings", QSettings::IniFormat);

        readSettings();

        watcher = new QFileSystemWatcher;
        watcher->addPath(settings->fileName());
        connect(watcher, &QFileSystemWatcher::fileChanged, this, [=](QString) {
            readSettings();
            QTimer::singleShot(250, [=]() { watcher->removePath(settings->fileName()); watcher->addPath(settings->fileName()); });
        } );

    }

    QStringList opt_img;
    QStringList opt_doc;
    QStringList opt_arc;
    QStringList opt_com;
    QStringList opt_bok;
    QStringList opt_vid;
    QStringList opt_txt;
    QStringList opt_url;

    void readSettings() {

        m_language = settings->value("language", "en").toString();
        m_version  = settings->value("version", "").toString();
        m_filedialogLocation = settings->value("filedialogLocation", "").toString();

        m_defaultAppImages = settings->value("defaultAppImages", opt_img[1]).toString();
        m_defaultAppDocuments = settings->value("defaultAppDocuments", opt_doc[0]).toString();
        m_defaultAppArchives = settings->value("defaultAppArchives", opt_arc[0]).toString();
        m_defaultAppVideos = settings->value("defaultAppVideos", opt_vid[0]).toString();
        m_defaultAppComicBooks = settings->value("defaultAppComicBooks", opt_com[0]).toString();
        m_defaultAppEBooks = settings->value("defaultAppEBooks", opt_bok[0]).toString();
        m_defaultAppText = settings->value("defaultAppText", opt_txt[0]).toString();
        m_defaultAppUrl = settings->value("defaultAppUrl", opt_url[0]).toString();

        if(m_defaultAppImages == "") m_defaultAppImages = "_default_";
        if(m_defaultAppDocuments == "") m_defaultAppDocuments = "_default_";
        if(m_defaultAppArchives == "") m_defaultAppArchives = "_default_";
        if(m_defaultAppVideos == "") m_defaultAppVideos = "_default_";
        if(m_defaultAppComicBooks == "") m_defaultAppComicBooks = "_default_";
        if(m_defaultAppEBooks == "") m_defaultAppEBooks = "_default_";
        if(m_defaultAppText == "") m_defaultAppText = "_default_";
        if(m_defaultAppUrl == "") m_defaultAppUrl = "_default_";

        m_closeAfterDefaultApp = settings->value("closeAfterDefaultApp", true).toBool();

        m_lastDownloadFolder = settings->value("lastDownloadFolder", QDir::homePath()).toString();
#ifdef Q_OS_WIN
    m_executableYtDlp = settings->value("executableYtDlp", "C:/Program Files/ytdlp/ytdlp.exe").toString();
#else
    m_executableYtDlp = settings->value("executableYtDlp", "yt-dlp").toString();
#endif

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
    QString m_defaultAppUrl;

    bool m_closeAfterDefaultApp;
    QString m_lastDownloadFolder;
    QString m_executableYtDlp;

Q_SIGNALS:
    void versionChanged();
    void lastDownloadFolderChanged();

};
