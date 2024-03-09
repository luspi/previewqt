/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
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

#ifndef PQCSETTINGS_H
#define PQCSETTINGS_H

#include <QObject>

class QSettings;
class QTimer;

class PQCSettings : public QObject {

    Q_OBJECT

public:
    static PQCSettings& get() {
        static PQCSettings instance;
        return instance;
    }
    ~PQCSettings();

    PQCSettings(PQCSettings const&)     = delete;
    void operator=(PQCSettings const&) = delete;

    Q_PROPERTY(bool topBarAutoHide READ getTopBarAutoHide WRITE setTopBarAutoHide NOTIFY topBarAutoHideChanged)
    bool getTopBarAutoHide();
    void setTopBarAutoHide(bool val);

    Q_PROPERTY(bool launchHiddenToSystemTray READ getLaunchHiddenToSystemTray WRITE setLaunchHiddenToSystemTray NOTIFY launchHiddenToSystemTrayChanged)
    bool getLaunchHiddenToSystemTray();
    void setLaunchHiddenToSystemTray(bool val);

    Q_PROPERTY(int defaultWindowWidth READ getDefaultWindowWidth WRITE setDefaultWindowWidth NOTIFY defaultWindowWidthChanged)
    int getDefaultWindowWidth();
    void setDefaultWindowWidth(int val);

    Q_PROPERTY(int defaultWindowHeight READ getDefaultWindowHeight WRITE setDefaultWindowHeight NOTIFY defaultWindowHeightChanged)
    int getDefaultWindowHeight();
    void setDefaultWindowHeight(int val);

    Q_PROPERTY(bool defaultWindowMaximized READ getDefaultWindowMaximized WRITE setDefaultWindowMaximized NOTIFY defaultWindowMaximizedChanged)
    bool getDefaultWindowMaximized();
    void setDefaultWindowMaximized(bool val);

    Q_PROPERTY(QString defaultAppShortcut READ getDefaultAppShortcut WRITE setDefaultAppShortcut NOTIFY defaultAppShortcutChanged)
    QString getDefaultAppShortcut();
    void setDefaultAppShortcut(QString val);

    Q_PROPERTY(QString defaultAppImages READ getDefaultAppImages WRITE setDefaultAppImages NOTIFY defaultAppImagesChanged)
    QString getDefaultAppImages();
    void setDefaultAppImages(QString val);

    Q_PROPERTY(QString defaultAppDocuments READ getDefaultAppDocuments WRITE setDefaultAppDocuments NOTIFY defaultAppDocumentsChanged)
    QString getDefaultAppDocuments();
    void setDefaultAppDocuments(QString val);

    Q_PROPERTY(QString defaultAppArchives READ getDefaultAppArchives WRITE setDefaultAppArchives NOTIFY defaultAppArchivesChanged)
    QString getDefaultAppArchives();
    void setDefaultAppArchives(QString val);

    Q_PROPERTY(QString defaultAppVideos READ getDefaultAppVideos WRITE setDefaultAppVideos NOTIFY defaultAppVideosChanged)
    QString getDefaultAppVideos();
    void setDefaultAppVideos(QString val);

    Q_PROPERTY(QString defaultAppComicBooks READ getDefaultAppComicBooks WRITE setDefaultAppComicBooks NOTIFY defaultAppComicBooksChanged)
    QString getDefaultAppComicBooks();
    void setDefaultAppComicBooks(QString val);

    Q_PROPERTY(bool closeAfterDefaultApp READ getCloseAfterDefaultApp WRITE setCloseAfterDefaultApp NOTIFY closeAfterDefaultAppChanged)
    bool getCloseAfterDefaultApp();
    void setCloseAfterDefaultApp(bool val);

private:
    PQCSettings();

    bool m_topBarAutoHide;
    bool m_launchHiddenToSystemTray;
    int m_defaultWindowWidth;
    int m_defaultWindowHeight;
    bool m_defaultWindowMaximized;
    QString m_defaultAppShortcut;
    QString m_defaultAppImages;
    QString m_defaultAppDocuments;
    QString m_defaultAppArchives;
    QString m_defaultAppVideos;
    QString m_defaultAppComicBooks;
    bool m_closeAfterDefaultApp;

    QSettings *settings;
    QTimer *saveTimer;

    void loadSettings();

private slots:
    void saveSettings();

signals:
    void topBarAutoHideChanged();
    void launchHiddenToSystemTrayChanged();
    void defaultWindowWidthChanged();
    void defaultWindowHeightChanged();
    void defaultWindowMaximizedChanged();
    void defaultAppShortcutChanged();
    void defaultAppImagesChanged();
    void defaultAppDocumentsChanged();
    void defaultAppArchivesChanged();
    void defaultAppVideosChanged();
    void defaultAppComicBooksChanged();
    void closeAfterDefaultAppChanged();

};

#endif
