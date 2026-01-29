/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2026 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PhotoQt.                                        **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/
#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSize>

class PQCConstants : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit PQCConstants() : QObject() {

        m_currentSource = "";
        m_currentType = "";
        m_mainwindowWidth = 0;
        m_mainwindowHeight = 0;
        m_mainwindowManuallyResized = false;
        m_mainwindowOverrideTitle = "";
        m_mainwindowOverrideTitleSuffix = "";
        m_mainwindowIsFullscreen = false;
        m_mainwindowIsMaximized = false;
        m_toprowForceVisible = false;
        m_menuIsOpen = false;
        m_trayiconShowNotificationWhenReady = {"", ""};

        m_windowMainVisible = false;
        m_windowAboutVisible = false;
        m_windowHelpVisible = false;
        m_windowSettingsVisible = false;
        m_windowWelcomeVisible = false;
        m_windowEnterPathVisible = false;

        m_settingsTabNextTime = -1;

        m_imageStatus = 0;
        m_imagePaintedSize = QSize(0,0);
        m_imageAsynchronous = false;
        m_imageRotation = 0;
        m_imageAvailableSize = QSize(0,0);
        m_imageAvailableSizeDelay = QSize(0,0);
        m_currentStreamVideoDirectURL = "";
        m_downloadInProgress = false;

    }

    Q_PROPERTY(QString currentSource MEMBER m_currentSource NOTIFY currentSourceChanged)
    Q_PROPERTY(QString currentType MEMBER m_currentType NOTIFY currentTypeChanged)

    Q_PROPERTY(int mainwindowWidth MEMBER m_mainwindowWidth NOTIFY mainwindowWidthChanged)
    Q_PROPERTY(int mainwindowHeight MEMBER m_mainwindowHeight NOTIFY mainwindowHeightChanged)
    Q_PROPERTY(bool mainwindowManuallyResized MEMBER m_mainwindowManuallyResized NOTIFY mainwindowManuallyResizedChanged)
    Q_PROPERTY(QString mainwindowOverrideTitle MEMBER m_mainwindowOverrideTitle NOTIFY mainwindowOverrideTitleChanged)
    Q_PROPERTY(QString mainwindowOverrideTitleSuffix MEMBER m_mainwindowOverrideTitleSuffix NOTIFY mainwindowOverrideTitleSuffixChanged)
    Q_PROPERTY(bool mainwindowIsFullscreen MEMBER m_mainwindowIsFullscreen NOTIFY mainwindowIsFullscreenChanged)
    Q_PROPERTY(bool mainwindowIsMaximized MEMBER m_mainwindowIsMaximized NOTIFY mainwindowIsMaximizedChanged)

    Q_PROPERTY(bool windowMainVisible MEMBER m_windowMainVisible NOTIFY windowMainVisibleChanged)
    Q_PROPERTY(bool windowAboutVisible MEMBER m_windowAboutVisible NOTIFY windowAboutVisibleChanged)
    Q_PROPERTY(bool windowHelpVisible MEMBER m_windowHelpVisible NOTIFY windowHelpVisibleChanged)
    Q_PROPERTY(bool windowSettingsVisible MEMBER m_windowSettingsVisible NOTIFY windowSettingsVisibleChanged)
    Q_PROPERTY(bool windowWelcomeVisible MEMBER m_windowWelcomeVisible NOTIFY windowWelcomeVisibleChanged)
    Q_PROPERTY(bool windowEnterPathVisible MEMBER m_windowEnterPathVisible NOTIFY windowEnterPathVisibleChanged)

    Q_PROPERTY(int settingsTabNextTime MEMBER m_settingsTabNextTime NOTIFY settingsTabNextTimeChanged)

    Q_PROPERTY(bool toprowForceVisible MEMBER m_toprowForceVisible NOTIFY toprowForceVisibleChanged)
    Q_PROPERTY(bool menuIsOpen MEMBER m_menuIsOpen NOTIFY menuIsOpenChanged)
    Q_PROPERTY(QStringList trayiconShowNotificationWhenReady MEMBER m_trayiconShowNotificationWhenReady NOTIFY trayiconShowNotificationWhenReadyChanged)

    Q_PROPERTY(int imageStatus MEMBER m_imageStatus NOTIFY imageStatusChanged)
    Q_PROPERTY(QSize imagePaintedSize MEMBER m_imagePaintedSize NOTIFY imagePaintedSizeChanged)
    Q_PROPERTY(bool imageAsynchronous MEMBER m_imageAsynchronous NOTIFY imageAsynchronousChanged)
    Q_PROPERTY(int imageRotation MEMBER m_imageRotation NOTIFY imageRotationChanged)
    Q_PROPERTY(QSize imageAvailableSize MEMBER m_imageAvailableSize NOTIFY imageAvailableSizeChanged)
    Q_PROPERTY(QSize imageAvailableSizeDelay MEMBER m_imageAvailableSizeDelay NOTIFY imageAvailableSizeDelayChanged)
    Q_PROPERTY(QString currentStreamVideoDirectURL MEMBER m_currentStreamVideoDirectURL NOTIFY currentStreamVideoDirectURLChanged)
    Q_PROPERTY(bool downloadInProgress MEMBER m_downloadInProgress NOTIFY downloadInProgressChanged)

private:
    QString m_currentSource;
    QString m_currentType;
    int m_mainwindowWidth;
    int m_mainwindowHeight;
    bool m_mainwindowManuallyResized;
    QString m_mainwindowOverrideTitle;
    QString m_mainwindowOverrideTitleSuffix;
    bool m_mainwindowIsFullscreen;
    bool m_mainwindowIsMaximized;

    bool m_windowMainVisible;
    bool m_windowAboutVisible;
    bool m_windowHelpVisible;
    bool m_windowSettingsVisible;
    bool m_windowWelcomeVisible;
    bool m_windowEnterPathVisible;

    int m_settingsTabNextTime;

    int m_imageStatus;
    QSize m_imagePaintedSize;
    bool m_imageAsynchronous;
    int m_imageRotation;
    QSize m_imageAvailableSize;
    QSize m_imageAvailableSizeDelay;
    QString m_currentStreamVideoDirectURL;
    bool m_downloadInProgress;

    bool m_toprowForceVisible;
    bool m_menuIsOpen;
    QStringList m_trayiconShowNotificationWhenReady;

Q_SIGNALS:
    void currentSourceChanged();
    void currentTypeChanged();
    void mainwindowWidthChanged();
    void mainwindowHeightChanged();
    void mainwindowManuallyResizedChanged();
    void mainwindowOverrideTitleChanged();
    void mainwindowOverrideTitleSuffixChanged();
    void mainwindowIsFullscreenChanged();
    void mainwindowIsMaximizedChanged();
    void toprowForceVisibleChanged();
    void menuIsOpenChanged();
    void trayiconShowNotificationWhenReadyChanged();
    void windowMainVisibleChanged();
    void windowAboutVisibleChanged();
    void windowHelpVisibleChanged();
    void windowSettingsVisibleChanged();
    void windowWelcomeVisibleChanged();
    void windowEnterPathVisibleChanged();
    void settingsTabNextTimeChanged();
    void imageStatusChanged();
    void imagePaintedSizeChanged();
    void imageAsynchronousChanged();
    void imageRotationChanged();
    void imageAvailableSizeChanged();
    void imageAvailableSizeDelayChanged();
    void currentStreamVideoDirectURLChanged();
    void downloadInProgressChanged();

};
