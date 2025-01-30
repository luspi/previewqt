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

#include <pqc_settings.h>
#include <pqc_configfiles.h>
#include <QSettings>
#include <QTimer>
#include <QFileInfo>
#include <QProcess>

PQCSettings::PQCSettings() {

    firstStart = false;
    if(!QFileInfo::exists(PQCConfigFiles::CONFIG_DIR() + "/settings"))
        firstStart = true;

    settings = new QSettings(PQCConfigFiles::CONFIG_DIR() + "/settings", QSettings::IniFormat);
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
#else
    // on windows custom tools are needed
    // we need empty entries here to not crash when loading the settings
    opt_img = {""};
    opt_doc = {""};
    opt_arc = {""};
    opt_com = {""};
    opt_bok = {""};
    opt_vid = {""};
#endif

    // do this AFTER setting the above options
    loadSettings();

#ifndef Q_OS_WIN
    if(firstStart) {

        for(auto &ele : opt_img) {
            if(checkToolExistence(ele)) {
                setDefaultAppImages(ele);
                break;
            }
        }
        for(auto &ele : opt_doc) {
            if(checkToolExistence(ele)) {
                setDefaultAppDocuments(ele);
                break;
            }
        }
        for(auto &ele : opt_arc) {
            if(checkToolExistence(ele)) {
                setDefaultAppArchives(ele);
                break;
            }
        }
        for(auto &ele : opt_com) {
            if(checkToolExistence(ele)) {
                setDefaultAppComicBooks(ele);
                break;
            }
        }
        for(auto &ele : opt_bok) {
            if(checkToolExistence(ele)) {
                setDefaultAppEBooks(ele);
                break;
            }
        }
        for(auto &ele : opt_vid) {
            if(checkToolExistence(ele)) {
                setDefaultAppVideos(ele);
                break;
            }
        }

    }
#endif

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

QString PQCSettings::getVersion() {
    return m_version;
}
void PQCSettings::setVersion(QString val) {
    if(m_version != val) {
        m_version = val;
        saveTimer->start();
        Q_EMIT versionChanged();
    }
}

QString PQCSettings::getLanguage() {
    return m_language;
}
void PQCSettings::setLanguage(QString val) {
    if(m_language != val) {
        m_language = val;
        saveTimer->start();
        Q_EMIT languageChanged();
    }
}

bool PQCSettings::getTopBarAutoHide() {
    return m_topBarAutoHide;
}
void PQCSettings::setTopBarAutoHide(bool val) {
    if(m_topBarAutoHide != val) {
        m_topBarAutoHide = val;
        saveTimer->start();
        Q_EMIT topBarAutoHideChanged();
    }
}

bool PQCSettings::getLaunchHiddenToSystemTray() {
    return m_launchHiddenToSystemTray;
}
void PQCSettings::setLaunchHiddenToSystemTray(bool val) {
    if(m_launchHiddenToSystemTray != val) {
        m_launchHiddenToSystemTray = val;
        saveTimer->start();
        Q_EMIT launchHiddenToSystemTrayChanged();
    }
}

bool PQCSettings::getMaximizeImageSizeAndAdjustWindow() {
    return m_maximizeImageSizeAndAdjustWindow;
}
void PQCSettings::setMaximizeImageSizeAndAdjustWindow(bool val) {
    if(m_maximizeImageSizeAndAdjustWindow != val) {
        m_maximizeImageSizeAndAdjustWindow = val;
        saveTimer->start();
        Q_EMIT maximizeImageSizeAndAdjustWindowChanged();
    }
}

int PQCSettings::getDefaultWindowWidth() {
    return m_defaultWindowWidth;
}
void PQCSettings::setDefaultWindowWidth(int val) {
    if(m_defaultWindowWidth != val) {
        m_defaultWindowWidth = val;
        saveTimer->start();
        Q_EMIT defaultWindowWidthChanged();
    }
}

int PQCSettings::getDefaultWindowHeight() {
    return m_defaultWindowHeight;
}
void PQCSettings::setDefaultWindowHeight(int val) {
    if(m_defaultWindowHeight != val) {
        m_defaultWindowHeight = val;
        saveTimer->start();
        Q_EMIT defaultWindowHeightChanged();
    }
}

bool PQCSettings::getDefaultWindowMaximized() {
    return m_defaultWindowMaximized;
}
void PQCSettings::setDefaultWindowMaximized(bool val) {
    if(m_defaultWindowMaximized != val) {
        m_defaultWindowMaximized = val;
        saveTimer->start();
        Q_EMIT defaultWindowMaximizedChanged();
    }
}

QString PQCSettings::getDefaultAppShortcut() {
    return m_defaultAppShortcut;
}
void PQCSettings::setDefaultAppShortcut(QString val) {
    if(m_defaultAppShortcut != val) {
        m_defaultAppShortcut = val;
        saveTimer->start();
        Q_EMIT defaultAppShortcutChanged();
    }
}

QString PQCSettings::getDefaultAppImages() {
    return m_defaultAppImages;
}
void PQCSettings::setDefaultAppImages(QString val) {
    if(m_defaultAppImages != val) {
        m_defaultAppImages = val;
        saveTimer->start();
        Q_EMIT defaultAppImagesChanged();
    }
}

QString PQCSettings::getDefaultAppDocuments() {
    return m_defaultAppDocuments;
}
void PQCSettings::setDefaultAppDocuments(QString val) {
    if(m_defaultAppDocuments != val) {
        m_defaultAppDocuments = val;
        saveTimer->start();
        Q_EMIT defaultAppDocumentsChanged();
    }
}

QString PQCSettings::getDefaultAppArchives() {
    return m_defaultAppArchives;
}
void PQCSettings::setDefaultAppArchives(QString val) {
    if(m_defaultAppArchives != val) {
        m_defaultAppArchives = val;
        saveTimer->start();
        Q_EMIT defaultAppArchivesChanged();
    }
}

QString PQCSettings::getDefaultAppVideos() {
    return m_defaultAppVideos;
}
void PQCSettings::setDefaultAppVideos(QString val) {
    if(m_defaultAppVideos != val) {
        m_defaultAppVideos = val;
        saveTimer->start();
        Q_EMIT defaultAppVideosChanged();
    }
}

QString PQCSettings::getDefaultAppComicBooks() {
    return m_defaultAppComicBooks;
}
void PQCSettings::setDefaultAppComicBooks(QString val) {
    if(m_defaultAppComicBooks != val) {
        m_defaultAppComicBooks = val;
        saveTimer->start();
        Q_EMIT defaultAppComicBooksChanged();
    }
}

QString PQCSettings::getDefaultAppEBooks() {
    return m_defaultAppEBooks;
}
void PQCSettings::setDefaultAppEBooks(QString val) {
    if(m_defaultAppEBooks != val) {
        m_defaultAppEBooks = val;
        saveTimer->start();
        Q_EMIT defaultAppEBooksChanged();
    }
}

bool PQCSettings::getCloseAfterDefaultApp() {
    return m_closeAfterDefaultApp;
}
void PQCSettings::setCloseAfterDefaultApp(bool val) {
    if(m_closeAfterDefaultApp != val) {
        m_closeAfterDefaultApp = val;
        saveTimer->start();
        Q_EMIT closeAfterDefaultAppChanged();
    }
}

QString PQCSettings::getFiledialogLocation() {
    return m_filedialogLocation;
}
void PQCSettings::setFiledialogLocation(QString val) {
    if(m_filedialogLocation != val) {
        m_filedialogLocation = val;
        saveTimer->start();
        Q_EMIT filedialogLocationChanged();
    }
}

bool PQCSettings::getCloseWhenLosingFocus() {
    return m_closeWhenLosingFocus;
}
void PQCSettings::setCloseWhenLosingFocus(bool val) {
    if(m_closeWhenLosingFocus != val) {
        m_closeWhenLosingFocus = val;
        saveTimer->start();
        Q_EMIT closeWhenLosingFocusChanged();
    }
}

bool PQCSettings::getTextWordWrap() {
    return m_textWordWrap;
}
void PQCSettings::setTextWordWrap(bool val) {
    if(m_textWordWrap != val) {
        m_textWordWrap = val;
        saveTimer->start();
        Q_EMIT textWordWrapChanged();
    }
}

int PQCSettings::getTextFontPointSize() {
    return m_textFontPointSize;
}
void PQCSettings::setTextFontPointSize(int val) {
    if(m_textFontPointSize != val) {
        m_textFontPointSize = val;
        saveTimer->start();
        Q_EMIT textFontPointSizeChanged();
    }
}

void PQCSettings::loadSettings() {

    setVersion(settings->value("version", "").toString());
    setLanguage(settings->value("language", "en").toString());
    setTopBarAutoHide(settings->value("topBarAutoHide", false).toBool());
    setLaunchHiddenToSystemTray(settings->value("launchHiddenToSystemTray", false).toBool());
    setMaximizeImageSizeAndAdjustWindow(settings->value("maximizeImageSizeAndAdjustWindow", true).toBool());
    setDefaultWindowWidth(settings->value("defaultWindowWidth", 500).toInt());
    setDefaultWindowHeight(settings->value("defaultWindowHeight", 400).toInt());
    setDefaultWindowMaximized(settings->value("defaultWindowMaximized", false).toBool());
    setDefaultAppShortcut(settings->value("defaultAppShortcut", "E").toString());
    setDefaultAppImages(settings->value("defaultAppImages", opt_img[0]).toString());
    setDefaultAppDocuments(settings->value("defaultAppDocuments", opt_doc[0]).toString());
    setDefaultAppArchives(settings->value("defaultAppArchives", opt_arc[0]).toString());
    setDefaultAppVideos(settings->value("defaultAppVideos", opt_vid[0]).toString());
    setDefaultAppComicBooks(settings->value("defaultAppComicBooks", opt_com[0]).toString());
    setDefaultAppEBooks(settings->value("defaultAppEBooks", opt_bok[0]).toString());
    setCloseAfterDefaultApp(settings->value("closeAfterDefaultApp", true).toBool());
    setFiledialogLocation(settings->value("filedialogLocation", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString());
    setCloseWhenLosingFocus(settings->value("closeWhenLosingFocus", false).toBool());
    setTextWordWrap(settings->value("textWordWrap", true).toBool());
    setTextFontPointSize(settings->value("textFontPointSize", 12).toInt());

}

void PQCSettings::saveSettings() {

    settings->setValue("version", m_version);
    settings->setValue("language", m_language);
    settings->setValue("topBarAutoHide", m_topBarAutoHide);
    settings->setValue("launchHiddenToSystemTray", m_launchHiddenToSystemTray);
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
    settings->setValue("closeAfterDefaultApp", m_closeAfterDefaultApp);
    settings->setValue("filedialogLocation", m_filedialogLocation);
    settings->setValue("closeWhenLosingFocus", m_closeWhenLosingFocus);
    settings->setValue("textWordWrap", m_textWordWrap);
    settings->setValue("textFontPointSize", m_textFontPointSize);

}
