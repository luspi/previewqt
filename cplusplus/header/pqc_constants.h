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
        m_menuIsOpen = false;

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

    Q_PROPERTY(bool menuIsOpen MEMBER m_menuIsOpen NOTIFY menuIsOpenChanged)

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

    bool m_menuIsOpen;

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
    void menuIsOpenChanged();

};
