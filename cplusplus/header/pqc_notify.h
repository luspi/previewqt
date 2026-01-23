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

class PQCNotify : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit PQCNotify() : QObject() {}

Q_SIGNALS:
    void requestNewFile();
    void loadNewFile(QString path);

    void mainwindowToggleVisibility();
    void mainwindowToggleMaximized();
    void mainwindowShowFullscreen();
    void mainwindowShowNormal();
    void mainwindowStartSystemMove();
    void mainwindowKeyPress(int modifiers, int keycode);
    void mainwindowClose();
    void updateWindowSize(int w, int h);

    void closeAllMenus();
    void trayiconShowNotification(QString title, QString txt);
    void showMainContextMenu();
    void showSubWindow(QString wdw);
    void showExtNotSet();
    void resetFocus();

    void addRotation(int rot);
    void setImageAsync(bool async);

};
