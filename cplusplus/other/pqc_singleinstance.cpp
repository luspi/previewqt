/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
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

#include <QKeyEvent>
#include <QFileInfo>
#include <QDir>
#include <thread>
#include <iostream>
#include <qlogging.h>   // needed in this form to compile with Qt 6.2

#include <QQmlApplicationEngine>
#include <QLocalSocket>
#include <QLocalServer>

#include <pqc_singleinstance.h>
#include <pqc_scripts.h>

PQCSingleInstance::PQCSingleInstance(int &argc, char *argv[]) : QApplication(argc, argv) {

    // This is the message string that we send to a running instance (if it exists
    QString passedon = ":/:/:";
    if(argc == 2)
        passedon = argv[1];

    if(passedon == "-h" || passedon == "--help") {

        std::cout << std::endl;
        std::cout << "Usage: previewqt [options] [filename]" << std::endl;
        std::cout << "Preview files." << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << std::setw(15) << std::right << "  -h, --help" << "   " << "Displays help on commandline options." << std::endl;
        std::cout << std::setw(15) << std::right << "  -v, --version" << "   " << "Displays version information." << std::endl;
        std::cout << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << std::setw(15) << std::right << "  [filename]" << "   " << "Image file to open." << std::endl;
        std::cout << std::endl;

        std::exit(0);
        return;

    } else if(passedon == "-v" || passedon == "--version") {

        std::cout << std::endl;
        std::cout << " PreviewQt " << PQMVERSION << std::endl;
        std::cout << std::endl;

        std::exit(0);
        return;

    }

    QByteArray message = QUrl::toPercentEncoding(passedon);


    socket = nullptr;
    server = nullptr;

    /*****************/
    /* Server/Socket */
    /*****************/

    // Create server name
    QString server_str = "org.photoqt.PreviewQt";

    // Connect to a Local Server (if available)
    socket = new QLocalSocket();
    socket->connectToServer(server_str);

    // If this is successfull, then an instance is already running
    if(socket->waitForConnected(100)) {

        // Send composed message string
        socket->write(message);
        socket->flush();

        // Inform user
        std::cout << "Running instance of PreviewQt detected, connecting to existing instance." << std::endl;

        // Exit the code (need to use stdlib exit function to ensure an immediate exit)
        // We wait 100ms as otherwise this instance might return as a crash (even though it doesn't really)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::exit(0);

    } else {

        // Create a new local server
        server = new QLocalServer();
        server->removeServer(server_str);
        server->listen(server_str);
        connect(server, &QLocalServer::newConnection, this, &PQCSingleInstance::newConnection);

    }

}

void PQCSingleInstance::newConnection() {
    QLocalSocket *socket = server->nextPendingConnection();
    if(socket->waitForReadyRead(2000))
        handleMessage(socket->readAll());
    socket->close();
    delete socket;
}

void PQCSingleInstance::handleMessage(QString msg) {

    qDebug() << "args: msg =" << msg;

    emit PQCScripts::get().commandLineArgumentReceived(msg);

}

PQCSingleInstance::~PQCSingleInstance() {
    if(socket != nullptr)
        delete socket;
    if(server != nullptr) {
        server->close();
        delete server;
    }
}
