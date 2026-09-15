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
    QString getDefaultAppOfficeDocuments() { return m_defaultAppOfficeDocuments; }
    QString getDefaultAppArchives() { return m_defaultAppArchives; }
    QString getDefaultAppVideos() { return m_defaultAppVideos; }
    QString getDefaultAppComicBooks() { return m_defaultAppComicBooks; }
    QString getDefaultAppEBooks() { return m_defaultAppEBooks; }
    QString getDefaultAppText() { return m_defaultAppText; }
    QString getDefaultAppUrl() { return m_defaultAppUrl; }

    QString getDefaultAppAudio() { return m_defaultAppAudio; }
    QString getDefaultAppDLL() { return m_defaultAppDLL; }
    QString getDefaultAppICC() { return m_defaultAppICC; }
    QString getDefaultAppMuseScore() { return m_defaultAppMuseScore; }
    QString getDefaultAppSQLite() { return m_defaultAppSQLite; }

    bool getCloseAfterDefaultApp() { return m_closeAfterDefaultApp; }

    QString getLastDownloadFolder() { return m_lastDownloadFolder; }
    void setLastDownloadFolder(QString val) { m_lastDownloadFolder = val; Q_EMIT lastDownloadFolderChanged(); }

    QString getExecutableYtDlp() { return m_executableYtDlp; }

    bool getCustomLibreOffice() { return m_customLibreOffice; }
    QString getCustomLibreOfficePath() { return m_customLibreOfficePath; }
    bool getCustomMuseScore() { return m_customMuseScore; }
    QString getCustomMuseScorePath() { return m_customMuseScorePath; }

private:
    PQCSettingsCPP() {

#ifndef Q_OS_WIN
        // these NEED TO BE DUPLICATED in pqc_settings.h/cpp
        opt_img = {"_default_", "photoqt", "gwenview", "nomacs", "eog", "feh", "gthumb", "mirage", "geeqie"};
        opt_doc = {"_default_", "okular", "evince", "atril", "photoqt"};
        opt_off = {"_default_", "libreoffice", "onlyoffice", "collabora"};
        opt_arc = {"_default_", "ark", "photoqt"};
        opt_com = {"_default_", "okular", "photoqt"};
        opt_bok = {"_default_", "ebook-viewer", "calibre", "okular"};
        opt_vid = {"_default_", "vlc", "mplayer", "photoqt"};
        opt_txt = {"_default_", "kate", "kwrite", "gedit", "sublime"};
        opt_url = {"_default_", "firefox", "chrome", "chromium"};
        opt_aud = {"_default_", "vlc"};
        opt_dll = {"_default_", ""};
        opt_icc = {"_default_", ""};
        opt_mus = {"_default_", "mscore"};
        opt_slt = {"_default_", "sqlitebrowser"};
#else
        // on windows custom tools are needed
        // we need empty entries here to not crash when loading the settings
        opt_img = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_doc = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_off = {"_default_", ""};
        opt_arc = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_com = {"_default_", "C:/Program Files/PhotoQt/photoqt.exe", ""};
        opt_bok = {"_default_", ""};
        opt_vid = {"_default_", ""};
        opt_txt = {"_default_", ""};
        opt_url = {"_default_", ""};
        opt_aud = {"_default_", ""};
        opt_dll = {"_default_", ""};
        opt_icc = {"_default_", ""};
        opt_mus = {"_default_", ""};
        opt_slt = {"_default_", ""};
#endif

        settings = new QSettings(PQCConfigFiles::get().CONFIG_DIR() + "/settings", QSettings::IniFormat);

        readSettings();

        watcher = new QFileSystemWatcher;
        watcher->addPath(settings->fileName());
        connect(watcher, &QFileSystemWatcher::fileChanged, this, [this](QString) {
            readSettings();
            QTimer::singleShot(250, this, [this]() { watcher->removePath(settings->fileName()); watcher->addPath(settings->fileName()); });
        });

    }

    QStringList opt_img;
    QStringList opt_doc;
    QStringList opt_off;
    QStringList opt_arc;
    QStringList opt_com;
    QStringList opt_bok;
    QStringList opt_vid;
    QStringList opt_txt;
    QStringList opt_url;
    QStringList opt_aud;
    QStringList opt_dll;
    QStringList opt_icc;
    QStringList opt_mus;
    QStringList opt_slt;

    void readSettings() {

        m_language = settings->value("language", "en").toString();
        m_version  = settings->value("version", "").toString();
        m_filedialogLocation = settings->value("filedialogLocation", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();

        m_defaultAppImages = settings->value("defaultAppImages", opt_img[1]).toString();
        m_defaultAppDocuments = settings->value("defaultAppDocuments", opt_doc[0]).toString();
        m_defaultAppOfficeDocuments = settings->value("defaultAppOfficeDocuments", opt_off[0]).toString();
        m_defaultAppArchives = settings->value("defaultAppArchives", opt_arc[0]).toString();
        m_defaultAppVideos = settings->value("defaultAppVideos", opt_vid[0]).toString();
        m_defaultAppComicBooks = settings->value("defaultAppComicBooks", opt_com[0]).toString();
        m_defaultAppEBooks = settings->value("defaultAppEBooks", opt_bok[0]).toString();
        m_defaultAppText = settings->value("defaultAppText", opt_txt[0]).toString();
        m_defaultAppUrl = settings->value("defaultAppUrl", opt_url[0]).toString();

        m_defaultAppAudio = settings->value("defaultAppAudio", opt_aud[0]).toString();
        m_defaultAppDLL = settings->value("defaultAppDLL", opt_dll[0]).toString();
        m_defaultAppICC = settings->value("defaultAppICC", opt_icc[0]).toString();
        m_defaultAppMuseScore = settings->value("defaultAppMuseScore", opt_mus[0]).toString();
        m_defaultAppSQLite = settings->value("defaultAppSQLite", opt_slt[0]).toString();

        if(m_defaultAppImages         .isEmpty()) m_defaultAppImages = "_default_";
        if(m_defaultAppDocuments      .isEmpty()) m_defaultAppDocuments = "_default_";
        if(m_defaultAppOfficeDocuments.isEmpty()) m_defaultAppOfficeDocuments = "_default_";
        if(m_defaultAppArchives       .isEmpty()) m_defaultAppArchives = "_default_";
        if(m_defaultAppComicBooks     .isEmpty()) m_defaultAppComicBooks = "_default_";
        if(m_defaultAppEBooks         .isEmpty()) m_defaultAppEBooks = "_default_";
        if(m_defaultAppVideos         .isEmpty()) m_defaultAppVideos = "_default_";
        if(m_defaultAppText           .isEmpty()) m_defaultAppText = "_default_";
        if(m_defaultAppUrl            .isEmpty()) m_defaultAppUrl = "_default_";
        if(m_defaultAppAudio          .isEmpty()) m_defaultAppAudio = "_default_";
        if(m_defaultAppDLL            .isEmpty()) m_defaultAppDLL = "_default_";
        if(m_defaultAppICC            .isEmpty()) m_defaultAppICC = "_default_";
        if(m_defaultAppMuseScore      .isEmpty()) m_defaultAppMuseScore = "_default_";
        if(m_defaultAppSQLite         .isEmpty()) m_defaultAppSQLite = "_default_";

        m_closeAfterDefaultApp = settings->value("closeAfterDefaultApp", true).toBool();

        m_lastDownloadFolder = settings->value("lastDownloadFolder", QDir::homePath()).toString();
#ifdef Q_OS_WIN
        m_executableYtDlp = settings->value("executableYtDlp", "C:/Program Files/ytdlp/ytdlp.exe").toString();
#else
        m_executableYtDlp = settings->value("executableYtDlp", "yt-dlp").toString();
#endif

        m_customLibreOffice = settings->value("customLibreOffice", false).toBool();
        m_customLibreOfficePath = settings->value("customLibreOfficePath", "").toString();
        m_customMuseScore = settings->value("customMuseScore", false).toBool();
        m_customMuseScorePath = settings->value("customMuseScorePath", "").toString();

        Q_EMIT settingsReloaded();

    }

    QSettings *settings;
    QFileSystemWatcher *watcher;

    QString m_language;
    QString m_version;
    QString m_filedialogLocation;

    QString m_defaultAppImages;
    QString m_defaultAppDocuments;
    QString m_defaultAppOfficeDocuments;
    QString m_defaultAppArchives;
    QString m_defaultAppVideos;
    QString m_defaultAppComicBooks;
    QString m_defaultAppEBooks;
    QString m_defaultAppText;
    QString m_defaultAppUrl;
    QString m_defaultAppAudio;
    QString m_defaultAppDLL;
    QString m_defaultAppICC;
    QString m_defaultAppMuseScore;
    QString m_defaultAppSQLite;

    bool m_customLibreOffice;
    QString m_customLibreOfficePath;
    bool m_customMuseScore;
    QString m_customMuseScorePath;

    bool m_closeAfterDefaultApp;
    QString m_lastDownloadFolder;
    QString m_executableYtDlp;

Q_SIGNALS:
    void versionChanged();
    void lastDownloadFolderChanged();
    void settingsReloaded();

};
