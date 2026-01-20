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

#include <pqc_settings.h>
#include <pqc_configfiles.h>
#include <pqc_settingscpp.h>
#include <QSettings>
#include <QTimer>
#include <QFileInfo>
#include <QProcess>

PQCSettings::PQCSettings() {

    firstStart = false;
    if(!QFileInfo::exists(PQCConfigFiles::get().CONFIG_DIR() + "/settings"))
        firstStart = true;

    settings = new QSettings(PQCConfigFiles::get().CONFIG_DIR() + "/settings", QSettings::IniFormat);
    saveTimer = new QTimer;
    saveTimer->setInterval(100);
    saveTimer->setSingleShot(true);
    connect(saveTimer, &QTimer::timeout, this, &PQCSettings::saveSettings);

#ifndef Q_OS_WIN
    // not on windows we present options
    opt_img = {"photoqt", "gwenview", "nomacs", "eog", "feh", "gthumb", "mirage", "geeqie"};
    opt_doc = {"okular", "evince", "atril", "photoqt"};
    opt_arc = {"ark", "photoqt"};
    opt_com = {"okular", "photoqt"};
    opt_bok = {"ebook-viewer", "calibre", "okular"};
    opt_vid = {"vlc", "mplayer", "photoqt"};
    opt_txt = {"kate", "kwrite", "gedit", "sublime"};
#else
    // on windows custom tools are needed
    // we need empty entries here to not crash when loading the settings
    opt_img = {""};
    opt_doc = {""};
    opt_arc = {""};
    opt_com = {""};
    opt_bok = {""};
    opt_vid = {""};
    opt_txt = {""};
#endif

    // do this AFTER setting the above options
    loadSettings();

#ifndef Q_OS_WIN
    if(firstStart) {

        for(auto &ele : opt_img) {
            if(checkToolExistence(ele)) {
                m_defaultAppImages = ele;
                break;
            }
        }
        for(auto &ele : opt_doc) {
            if(checkToolExistence(ele)) {
                m_defaultAppDocuments = ele;
                break;
            }
        }
        for(auto &ele : opt_arc) {
            if(checkToolExistence(ele)) {
                m_defaultAppArchives = ele;
                break;
            }
        }
        for(auto &ele : opt_com) {
            if(checkToolExistence(ele)) {
                m_defaultAppComicBooks = ele;
                break;
            }
        }
        for(auto &ele : opt_bok) {
            if(checkToolExistence(ele)) {
                m_defaultAppEBooks = ele;
                break;
            }
        }
        for(auto &ele : opt_vid) {
            if(checkToolExistence(ele)) {
                m_defaultAppVideos = ele;
                break;
            }
        }
        for(auto &ele : opt_txt) {
            if(checkToolExistence(ele)) {
                m_defaultAppText = ele;
                break;
            }
        }

    }
#endif

    connect(&PQCSettingsCPP::get(), &PQCSettingsCPP::versionChanged, this, [=]() { m_version = PQCSettingsCPP::get().getVersion(); Q_EMIT versionChanged(); });

    connect(this, &PQCSettings::versionChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::languageChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::topBarAutoHideChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::hideToSystemTrayChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::launchHiddenToSystemTrayChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::notifyNextlaunchHiddenToSystemTrayChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::maximizeImageSizeAndAdjustWindowChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultWindowWidthChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultWindowHeightChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultWindowMaximizedChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppShortcutChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppImagesChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppDocumentsChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppArchivesChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppVideosChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppComicBooksChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppEBooksChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::defaultAppTextChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::closeAfterDefaultAppChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::filedialogLocationChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::closeWhenLosingFocusChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::textWordWrapChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::textFontPointSizeChanged, this, [=]() { saveTimer->start(); });
    connect(this, &PQCSettings::textSearchCaseSensitiveChanged, this, [=]() { saveTimer->start(); });

}

PQCSettings::~PQCSettings() {
    delete settings;
}

bool PQCSettings::checkToolExistence(QString tool) {

    qDebug() << "args: tool =" << tool;

    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << tool);
    which.waitForFinished();

    // success
    return !which.exitCode();
}

void PQCSettings::loadSettings() {

    m_version = settings->value("version", "").toString();
    if(PQCSettingsCPP::get().getVersion() != "" && m_version != PQCSettingsCPP::get().getVersion()) m_version = PQCSettingsCPP::get().getVersion();
    m_language = settings->value("language", "en").toString();
    m_topBarAutoHide = settings->value("topBarAutoHide", false).toBool();
    m_hideToSystemTray = settings->value("hideToSystemTray", true).toBool();
    m_launchHiddenToSystemTray = settings->value("launchHiddenToSystemTray", false).toBool();
    m_notifyNextLaunchHiddenToSystemTray = settings->value("notifyNextlaunchHiddenToSystemTray", true).toBool();
    m_maximizeImageSizeAndAdjustWindow = settings->value("maximizeImageSizeAndAdjustWindow", true).toBool();
    m_defaultWindowWidth = settings->value("defaultWindowWidth", 500).toInt();
    m_defaultWindowHeight = settings->value("defaultWindowHeight", 400).toInt();
    m_defaultWindowMaximized = settings->value("defaultWindowMaximized", false).toBool();
    m_defaultAppShortcut = settings->value("defaultAppShortcut", "E").toString();
    m_defaultAppImages = settings->value("defaultAppImages", opt_img[0]).toString();
    m_defaultAppDocuments = settings->value("defaultAppDocuments", opt_doc[0]).toString();
    m_defaultAppArchives = settings->value("defaultAppArchives", opt_arc[0]).toString();
    m_defaultAppVideos = settings->value("defaultAppVideos", opt_vid[0]).toString();
    m_defaultAppComicBooks = settings->value("defaultAppComicBooks", opt_com[0]).toString();
    m_defaultAppEBooks = settings->value("defaultAppEBooks", opt_bok[0]).toString();
    m_defaultAppText = settings->value("defaultAppText", opt_txt[0]).toString();
    m_closeAfterDefaultApp = settings->value("closeAfterDefaultApp", true).toBool();
    m_filedialogLocation = settings->value("filedialogLocation", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
    m_closeWhenLosingFocus = settings->value("closeWhenLosingFocus", false).toBool();
    m_textWordWrap = settings->value("textWordWrap", true).toBool();
    m_textFontPointSize = settings->value("textFontPointSize", 12).toInt();
    m_textSearchCaseSensitive = settings->value("textSearchCaseSensitive", false).toBool();

    Q_EMIT versionChanged();
    Q_EMIT languageChanged();
    Q_EMIT topBarAutoHideChanged();
    Q_EMIT hideToSystemTrayChanged();
    Q_EMIT launchHiddenToSystemTrayChanged();
    Q_EMIT notifyNextlaunchHiddenToSystemTrayChanged();
    Q_EMIT maximizeImageSizeAndAdjustWindowChanged();
    Q_EMIT defaultWindowWidthChanged();
    Q_EMIT defaultWindowHeightChanged();
    Q_EMIT defaultWindowMaximizedChanged();
    Q_EMIT defaultAppShortcutChanged();
    Q_EMIT defaultAppImagesChanged();
    Q_EMIT defaultAppDocumentsChanged();
    Q_EMIT defaultAppArchivesChanged();
    Q_EMIT defaultAppVideosChanged();
    Q_EMIT defaultAppComicBooksChanged();
    Q_EMIT defaultAppEBooksChanged();
    Q_EMIT defaultAppTextChanged();
    Q_EMIT closeAfterDefaultAppChanged();
    Q_EMIT filedialogLocationChanged();
    Q_EMIT closeWhenLosingFocusChanged();
    Q_EMIT textWordWrapChanged();
    Q_EMIT textFontPointSizeChanged();
    Q_EMIT textSearchCaseSensitiveChanged();

}

void PQCSettings::saveSettings() {

    settings->setValue("version", m_version);
    settings->setValue("language", m_language);
    settings->setValue("topBarAutoHide", m_topBarAutoHide);
    settings->setValue("hideToSystemTray", m_hideToSystemTray);
    settings->setValue("launchHiddenToSystemTray", m_launchHiddenToSystemTray);
    settings->setValue("notifyNextlaunchHiddenToSystemTray", m_notifyNextLaunchHiddenToSystemTray);
    settings->setValue("maximizeImageSizeAndAdjustWindow", m_maximizeImageSizeAndAdjustWindow);
    settings->setValue("defaultWindowWidth", m_defaultWindowWidth);
    settings->setValue("defaultWindowHeight", m_defaultWindowHeight);
    settings->setValue("defaultWindowMaximized", m_defaultWindowMaximized);
    settings->setValue("defaultAppShortcut", m_defaultAppShortcut);
    settings->setValue("defaultAppImages", m_defaultAppImages);
    settings->setValue("defaultAppDocuments", m_defaultAppDocuments);
    settings->setValue("defaultAppArchives", m_defaultAppArchives);
    settings->setValue("defaultAppVideos", m_defaultAppVideos);
    settings->setValue("defaultAppComicBooks", m_defaultAppComicBooks);
    settings->setValue("defaultAppEBooks", m_defaultAppEBooks);
    settings->setValue("defaultAppText", m_defaultAppText);
    settings->setValue("closeAfterDefaultApp", m_closeAfterDefaultApp);
    settings->setValue("filedialogLocation", m_filedialogLocation);
    settings->setValue("closeWhenLosingFocus", m_closeWhenLosingFocus);
    settings->setValue("textWordWrap", m_textWordWrap);
    settings->setValue("textFontPointSize", m_textFontPointSize);

}
