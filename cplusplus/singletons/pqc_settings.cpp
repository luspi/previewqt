#include <pqc_settings.h>
#include <pqc_configfiles.h>
#include <QSettings>
#include <QTimer>
#include <QFileInfo>
#include <QProcess>

PQCSettings::PQCSettings() {

#ifndef Q_OS_WIN
    bool detectDefaults = false;
    if(!QFileInfo::exists(PQCConfigFiles::CONFIG_DIR() + "/settings"))
        detectDefaults = true;
#endif

    settings = new QSettings(PQCConfigFiles::CONFIG_DIR() + "/settings", QSettings::IniFormat);
    saveTimer = new QTimer;
    saveTimer->setInterval(100);
    saveTimer->setSingleShot(true);
    connect(saveTimer, &QTimer::timeout, this, &PQCSettings::saveSettings);

    loadSettings();

#ifndef Q_OS_WIN
    if(detectDefaults) {

        const QStringList img = {"photoqt", "gwenview", "nomacs", "eog", "feh", "gthumb", "mirage", "geeqie"};
        const QStringList doc = {"okular", "evince", "atril", "photoqt"};
        const QStringList arc = {"ark", "photoqt"};
        const QStringList com = {"okular", "photoqt"};
        const QStringList vid = {"vlc", "mplayer", "photoqt"};

        for(auto &ele : img) {
            if(checkToolExistence(ele)) {
                setDefaultAppImages(ele);
                break;
            }
        }
        for(auto &ele : doc) {
            if(checkToolExistence(ele)) {
                setDefaultAppDocuments(ele);
                break;
            }
        }
        for(auto &ele : arc) {
            if(checkToolExistence(ele)) {
                setDefaultAppArchives(ele);
                break;
            }
        }
        for(auto &ele : com) {
            if(checkToolExistence(ele)) {
                setDefaultAppComicBooks(ele);
                break;
            }
        }
        for(auto &ele : vid) {
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

QString PQCSettings::getDefaultAppComicBooks() {
    return m_defaultAppComicBooks;
}
void PQCSettings::setDefaultAppComicBooks(QString val) {
    if(m_defaultAppComicBooks != val) {
        m_defaultAppComicBooks = val;
        saveTimer->start();
        emit defaultAppComicBooksChanged();
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

QString PQCSettings::getFiledialogLocation() {
    return m_filedialogLocation;
}
void PQCSettings::setFiledialogLocation(QString val) {
    if(m_filedialogLocation != val) {
        m_filedialogLocation = val;
        saveTimer->start();
        emit filedialogLocationChanged();
    }
}

bool PQCSettings::getCloseWhenLosingFocus() {
    return m_closeWhenLosingFocus;
}
void PQCSettings::setCloseWhenLosingFocus(bool val) {
    if(m_closeWhenLosingFocus != val) {
        m_closeWhenLosingFocus = val;
        saveTimer->start();
        emit closeWhenLosingFocusChanged();
    }
}

void PQCSettings::loadSettings() {

    setTopBarAutoHide(settings->value("topBarAutoHide", false).toBool());
    setLaunchHiddenToSystemTray(settings->value("launchHiddenToSystemTray", false).toBool());
    setDefaultWindowWidth(settings->value("defaultWindowWidth", 800).toInt());
    setDefaultWindowHeight(settings->value("defaultWindowHeight", 600).toInt());
    setDefaultWindowMaximized(settings->value("defaultWindowMaximized", false).toBool());
    setDefaultAppShortcut(settings->value("defaultAppShortcut", "E").toString());
    setDefaultAppImages(settings->value("defaultAppImages", "").toString());
    setDefaultAppDocuments(settings->value("defaultAppDocuments", "").toString());
    setDefaultAppArchives(settings->value("defaultAppArchives", "").toString());
    setDefaultAppVideos(settings->value("defaultAppVideos", "").toString());
    setDefaultAppComicBooks(settings->value("defaultAppComicBooks", "").toString());
    setCloseAfterDefaultApp(settings->value("closeAfterDefaultApp", true).toBool());
    setFiledialogLocation(settings->value("filedialogLocation", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString());
    setCloseWhenLosingFocus(settings->value("closeWhenLosingFocus", false).toBool());

}

void PQCSettings::saveSettings() {

    settings->setValue("topBarAutoHide", m_topBarAutoHide);
    settings->setValue("launchHiddenToSystemTray", m_launchHiddenToSystemTray);
    settings->setValue("defaultWindowWidth", m_defaultWindowWidth);
    settings->setValue("defaultWindowHeight", m_defaultWindowHeight);
    settings->setValue("defaultWindowMaximized", m_defaultWindowMaximized);
    settings->setValue("defaultAppShortcut", m_defaultAppShortcut);
    settings->setValue("defaultAppImages", m_defaultAppImages);
    settings->setValue("defaultAppDocuments", m_defaultAppDocuments);
    settings->setValue("defaultAppArchives", m_defaultAppArchives);
    settings->setValue("defaultAppVideos", m_defaultAppVideos);
    settings->setValue("defaultAppComicBooks", m_defaultAppComicBooks);
    settings->setValue("closeAfterDefaultApp", m_closeAfterDefaultApp);
    settings->setValue("filedialogLocation", m_filedialogLocation);
    settings->setValue("closeWhenLosingFocus", m_closeWhenLosingFocus);

}
