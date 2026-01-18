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

#include <QApplication>

class QQmlApplicationEngine;
class QLocalServer;
class QLocalSocket;

// Makes sure only one instance of PhotoQt is running, and enables remote communication
class PQCSingleInstance : public QApplication {

    Q_OBJECT

public:
    explicit PQCSingleInstance(int&, char *[]);
    ~PQCSingleInstance();

private Q_SLOTS:
    // A new application instance was started (notification to main instance)
    void newConnection();

private:
    QLocalSocket *socket;
    QLocalServer *server;

    // This one is used in main process, handling the message sent by sub-instances
    void handleMessage(QString msg);

    void showHelpMessage();

};
