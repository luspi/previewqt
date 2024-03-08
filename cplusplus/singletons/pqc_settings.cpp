#include <pqc_settings.h>
#include <pqc_configfiles.h>
#include <QSettings>
#include <QTimer>

PQCSettings::PQCSettings() {

    settings = new QSettings(PQCConfigFiles::CONFIG_DIR() + "/settings", QSettings::IniFormat);
    saveTimer = new QTimer;
    saveTimer->setInterval(100);
    saveTimer->setSingleShot(true);
    connect(saveTimer, &QTimer::timeout, this, &PQCSettings::saveSettings);

    loadSettings();

}

PQCSettings::~PQCSettings() {
    delete settings;
}

bool PQCSettings::getTopBarAutoHide() {
    return m_topBarAutoHide;
}
void PQCSettings::setTopBarAutoHide(bool val) {
    if(m_topBarAutoHide != val) {
        m_topBarAutoHide = val;
        saveTimer->start();
        emit topBarAutoHideChanged();
    }
}

bool PQCSettings::getHideToSystemTray() {
    return m_hideToSystemTray;
}
void PQCSettings::setHideToSystemTray(bool val) {
    if(m_hideToSystemTray != val) {
        m_hideToSystemTray = val;
        saveTimer->start();
        emit hideToSystemTrayChanged();
    }
}

bool PQCSettings::getLaunchHiddenToSystemTray() {
    return m_launchHiddenToSystemTray;
}
void PQCSettings::setLaunchHiddenToSystemTray(bool val) {
    if(m_launchHiddenToSystemTray != val) {
        m_launchHiddenToSystemTray = val;
        saveTimer->start();
        emit launchHiddenToSystemTrayChanged();
    }
}

int PQCSettings::getDefaultWindowWidth() {
    return m_defaultWindowWidth;
}
void PQCSettings::setDefaultWindowWidth(int val) {
    if(m_defaultWindowWidth != val) {
        m_defaultWindowWidth = val;
        saveTimer->start();
        emit defaultWindowWidthChanged();
    }
}

int PQCSettings::getDefaultWindowHeight() {
    return m_defaultWindowHeight;
}
void PQCSettings::setDefaultWindowHeight(int val) {
    if(m_defaultWindowHeight != val) {
        m_defaultWindowHeight = val;
        saveTimer->start();
        emit defaultWindowHeightChanged();
    }
}

bool PQCSettings::getDefaultWindowMaximized() {
    return m_defaultWindowMaximized;
}
void PQCSettings::setDefaultWindowMaximized(bool val) {
    if(m_defaultWindowMaximized != val) {
        m_defaultWindowMaximized = val;
        saveTimer->start();
        emit defaultWindowMaximizedChanged();
    }
}

QString PQCSettings::getDefaultAppShortcut() {
    return m_defaultAppShortcut;
}
void PQCSettings::setDefaultAppShortcut(QString val) {
    if(m_defaultAppShortcut != val) {
        m_defaultAppShortcut = val;
        saveTimer->start();
        emit defaultAppShortcutChanged();
    }
}

QString PQCSettings::getDefaultAppImages() {
    return m_defaultAppImages;
}
void PQCSettings::setDefaultAppImages(QString val) {
    if(m_defaultAppImages != val) {
        m_defaultAppImages = val;
        saveTimer->start();
        emit defaultAppImagesChanged();
    }
}

QString PQCSettings::getDefaultAppDocuments() {
    return m_defaultAppDocuments;
}
void PQCSettings::setDefaultAppDocuments(QString val) {
    if(m_defaultAppDocuments != val) {
        m_defaultAppDocuments = val;
        saveTimer->start();
        emit defaultAppDocumentsChanged();
    }
}

QString PQCSettings::getDefaultAppArchives() {
    return m_defaultAppArchives;
}
void PQCSettings::setDefaultAppArchives(QString val) {
    if(m_defaultAppArchives != val) {
        m_defaultAppArchives = val;
        saveTimer->start();
        emit defaultAppArchivesChanged();
    }
}

QString PQCSettings::getDefaultAppVideos() {
    return m_defaultAppVideos;
}
void PQCSettings::setDefaultAppVideos(QString val) {
    if(m_defaultAppVideos != val) {
        m_defaultAppVideos = val;
        saveTimer->start();
        emit defaultAppVideosChanged();
    }
}

bool PQCSettings::getCloseAfterDefaultApp() {
    return m_closeAfterDefaultApp;
}
void PQCSettings::setCloseAfterDefaultApp(bool val) {
    if(m_closeAfterDefaultApp != val) {
        m_closeAfterDefaultApp = val;
        saveTimer->start();
        emit closeAfterDefaultAppChanged();
    }
}

void PQCSettings::loadSettings() {

    setTopBarAutoHide(settings->value("topBarAutoHide", false).toBool());
    setHideToSystemTray(settings->value("hideToSystemTray", false).toBool());
    setLaunchHiddenToSystemTray(settings->value("launchHiddenToSystemTray", false).toBool());
    setDefaultWindowWidth(settings->value("defaultWindowWidth", 800).toInt());
    setDefaultWindowHeight(settings->value("defaultWindowHeight", 600).toInt());
    setDefaultWindowMaximized(settings->value("defaultWindowMaximized", false).toBool());
    setDefaultAppShortcut(settings->value("defaultAppShortcut", "D").toString());
    setDefaultAppImages(settings->value("defaultAppImages", "photoqt").toString());
    setDefaultAppDocuments(settings->value("defaultAppDocuments", "okular").toString());
    setDefaultAppArchives(settings->value("defaultAppArchives", "ark").toString());
    setDefaultAppVideos(settings->value("defaultAppVideos", "vlc").toString());
    setCloseAfterDefaultApp(settings->value("closeAfterDefaultApp", true).toBool());

}

void PQCSettings::saveSettings() {

    settings->setValue("topBarAutoHide", m_topBarAutoHide);
    settings->setValue("hideToSystemTray", m_hideToSystemTray);
    settings->setValue("launchHiddenToSystemTray", m_launchHiddenToSystemTray);
    settings->setValue("defaultWindowWidth", m_defaultWindowWidth);
    settings->setValue("defaultWindowHeight", m_defaultWindowHeight);
    settings->setValue("defaultWindowMaximized", m_defaultWindowMaximized);
    settings->setValue("defaultAppShortcut", m_defaultAppShortcut);
    settings->setValue("defaultAppImages", m_defaultAppImages);
    settings->setValue("defaultAppDocuments", m_defaultAppDocuments);
    settings->setValue("defaultAppArchives", m_defaultAppArchives);
    settings->setValue("defaultAppVideos", m_defaultAppVideos);
    settings->setValue("closeAfterDefaultApp", m_closeAfterDefaultApp);

}
