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

#include <QKeyEvent>
#include <QFileInfo>
#include <QDir>
#include <thread>
#include <iostream>
#include <qlogging.h>   // needed in this form to compile with Qt 6.2

#include <QQmlApplicationEngine>
#include <QLocalSocket>
#include <QLocalServer>
#include <QStandardPaths>
#include <QMimeDatabase>
#include <QImageReader>
#include <QtDBus/QtDBus>
#include <QClipboard>

#include <pqc_singleinstance.h>
#include <pqc_scripts.h>
#include <pqc_loadfile.h>
#include <pqc_fileformats.h>
#include <pqc_specialactions.h>

PQCSingleInstance::PQCSingleInstance(int &argc, char *argv[]) : QApplication(argc, argv) {

    // some possible parameter handling
    QString file = "";
    bool processonly = false;
    bool loadselected = false;
    int fileNumInside = 0;
    bool setDebug = false;

    for(int i = 1; i < argc; ++i) {

        QString arg = argv[i];

        if(arg == "-h" || arg == "--help") {

            showHelpMessage();

            std::exit(0);
            return;

        } else if(arg == "-v" || arg == "--version") {

            std::cout << std::endl;
            std::cout << " PreviewQt " << PQMVERSION << std::endl;
            std::cout << std::endl;

            std::exit(0);
            return;

        } else if(arg == "--show-info") {

#ifdef PQMVIDEOMPV
            // Qt sets the locale in the QGuiApplication constructor, but libmpv
            // requires the LC_NUMERIC category to be set to "C", so change it back.
            std::setlocale(LC_NUMERIC, "C");
#endif

            std::cout << std::endl
                      << " ** PreviewQt configuration:"
                      << std::endl << std::endl
                      << PQCScripts::get().getConfigInfo(false).toStdString()
                      << std::endl;
            std::exit(0);
            return;

        } else if(arg == "--process-only") {

            processonly = true;

#ifdef Q_OS_UNIX
        } else if(arg == "--load-selected-file") {

            loadselected = true;
#endif
        } else if(arg == "--file-num" && i < argc-1) {

            fileNumInside = atoi(argv[++i]);

        } else if(arg == "--debug") {

            setDebug = true;

        } else if(arg.startsWith("-")) {

            std::cerr << std::endl << " Unknown flag:" << arg.toStdString() << std::endl << std::endl;
            showHelpMessage();
            std::exit(0);
            return;

        } else {

            file = arg;

        }

    }

    // only process and provide generic way to access rendered file
    if(processonly && QFileInfo::exists(PQCScripts::get().cleanPath(file))) {

        PQCSpecialActions::processOnly(file, fileNumInside);
        std::exit(0);
        return;

    } else if(loadselected) {

        QString selectedFile = PQCSpecialActions::getSelectedFile();

        if(selectedFile.trimmed() == "") {
            std::cout << std::endl << " >> No active Dolphin window with a selected file has been found... Stopping here!" << std::endl << std::endl;
            std::exit(0);
            return;
        }

        std::cout << std::endl << " >> Loading file:" << selectedFile.toStdString() << std::endl << std::endl;

        file = selectedFile;

    }


    QString filename = PQCScripts::get().toAbsolutePath(PQCScripts::cleanPath(file));

    QByteArray message = "";
    if(fileNumInside > 0) {
        if(PQCScripts::get().isPDFDocument(filename)) {
            message = QUrl::toPercentEncoding(QString("%1:/:/:%2").arg(filename).arg(fileNumInside));
        } else if(PQCScripts::get().isArchive(filename)) {
            QStringList cont = PQCScripts::get().getArchiveContent(filename);
            if(fileNumInside < cont.length())
                message = QUrl::toPercentEncoding(QString("%1:/:/:%2").arg(filename, cont[fileNumInside]));
            else
                message = QUrl::toPercentEncoding(filename);
        } else
            message = QUrl::toPercentEncoding(filename);

    } else
        message = QUrl::toPercentEncoding(filename);

    if(message == "")
        message = "-";

    if(setDebug)
        PQCScripts::get().setDebug(true);

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

        PQCScripts::get().setStartupMessage(message);

    }

}

void PQCSingleInstance::showHelpMessage() {

    std::cout << std::endl;
    std::cout << "Usage: previewqt [options] [filename]" << std::endl;
    std::cout << "Preview files." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << std::setw(15) << std::right << "  -h, --help" << "   " << "Displays help on commandline options." << std::endl;
    std::cout << std::setw(15) << std::right << "  -v, --version" << "   " << "Displays version information." << std::endl;
    std::cout << std::setw(15) << std::right << "  --show-info" << "   " << "Show configuration overview." << std::endl;
    std::cout << std::setw(15) << std::right << "  --debug" << "   " << "Show debug messages." << std::endl;
    std::cout << std::setw(15) << std::right << "  --process-only" << "   " << "Process file, provide path to processed file, and print some information." << std::endl;
#ifdef Q_OS_UNIX
    std::cout << std::setw(15) << std::right << "  --load-selected-file" << "   " << "Load any file selected in the currently active file manager." << std::endl;
#endif
    std::cout << std::setw(15) << std::right << "  --file-num <num>" << "   " << "Which file/page to load inside of a document/archive." << std::endl;
    std::cout << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << std::setw(15) << std::right << "  [filename]" << "   " << "Image file to open." << std::endl;
    std::cout << std::endl;

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

    Q_EMIT PQCScripts::get().commandLineArgumentReceived(msg);

}

PQCSingleInstance::~PQCSingleInstance() {
    if(socket != nullptr)
        delete socket;
    if(server != nullptr) {
        server->close();
        delete server;
    }
}
