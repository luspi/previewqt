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

#include <pqc_singleinstance.h>
#include <pqc_scripts.h>
#include <pqc_loadimage.h>
#include <pqc_imageformats.h>

PQCSingleInstance::PQCSingleInstance(int &argc, char *argv[]) : QApplication(argc, argv) {

    // some possible parameter handling
    QString file = "";
    bool processonly = false;
    int fileNumInside = 0;
    bool setDebug = false;

    for(int i = 1; i < argc; ++i) {

        QString arg = argv[i];

        if(arg == "-h" || arg == "--help") {

            std::cout << std::endl;
            std::cout << "Usage: previewqt [options] [filename]" << std::endl;
            std::cout << "Preview files." << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << std::setw(15) << std::right << "  -h, --help" << "   " << "Displays help on commandline options." << std::endl;
            std::cout << std::setw(15) << std::right << "  -v, --version" << "   " << "Displays version information." << std::endl;
            std::cout << std::setw(15) << std::right << "  --debug" << "   " << "Show debug messages." << std::endl;
            std::cout << std::setw(15) << std::right << "  --process-only" << "   " << "Process file, provide path to processed file, and print some information." << std::endl;
            std::cout << std::setw(15) << std::right << "  --file-num <num>" << "   " << "Which file/page to load inside of a document/archive." << std::endl;
            std::cout << std::endl;
            std::cout << "Arguments:" << std::endl;
            std::cout << std::setw(15) << std::right << "  [filename]" << "   " << "Image file to open." << std::endl;
            std::cout << std::endl;

            std::exit(0);
            return;

        } else if(arg == "-v" || arg == "--version") {

            std::cout << std::endl;
            std::cout << " PreviewQt " << PQMVERSION << std::endl;
            std::cout << std::endl;

            std::exit(0);
            return;

        } else if(arg == "--process-only") {

            processonly = true;

        } else if(arg == "--file-num" && i < argc-1) {

            fileNumInside = atoi(argv[++i]);

        } else if(arg == "--debug") {

            setDebug = true;

        } else {

            file = arg;

        }

    }

    QString filename = PQCScripts::get().toAbsolutePath(PQCScripts::cleanPath(file));

    // only process file, exit and stop executing after
    if(processonly && QFileInfo::exists(PQCScripts::get().cleanPath(file))) {

        QString tmpfile = QString("%1/tmpfile.jpg").arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        if(QFileInfo::exists(tmpfile))
            QFile::remove(tmpfile);

        if(!QFileInfo::exists(filename)) {
            std::cerr << "ERROR" << std::endl;
            std::exit(0);
            return;
        }

        // we print out some mime info
        QMimeDatabase db;

        // the suffix for convenience
        QString suffix = QFileInfo(filename).suffix().toLower();

        // is this a video?
        if(PQCImageFormats::get().getAllFormatsVideo().indexOf(suffix) > -1) {

            QString mime = db.mimeTypeForFile(filename).name();
            std::cout << "display mime: " << mime.toStdString() << std::endl
                      << "source mime: " << mime.toStdString() << std::endl
                      << "tmp path: " << filename.toStdString() << std::endl
                      << "file name: " << PQCScripts::get().getFilename(filename).toStdString() << std::endl
                      << "video: yes" << std::endl;

            std::exit(0);
            return;

        }

        // is this format supported by Qt?
        if(PQCImageFormats::get().getAllFormatsQt().indexOf(suffix) > -1) {

            QString mime = db.mimeTypeForFile(filename).name();
            std::cout << "display mime: " << mime.toStdString() << std::endl
                      << "source mime: " << mime.toStdString() << std::endl
                      << "tmp path: " << filename.toStdString() << std::endl
                      << "file name: " << PQCScripts::get().getFilename(filename).toStdString() << std::endl;

            QImageReader reader(filename);
            if(reader.supportsAnimation())
                std::cout << "animated: yes" << std::endl;

            std::exit(0);
            return;

        }

        bool isPDF = PQCScripts::get().isPDFDocument(filename);
        bool isARC = PQCScripts::get().isArchive(filename);

        QStringList archiveContent;
        if(isARC)
            archiveContent = PQCScripts::get().getArchiveContent(filename);
\
        QString filenameToLoad = filename;
        if(isPDF)
            filenameToLoad = QString("%1::PDF::%2").arg(fileNumInside).arg(filename);
        else if(isARC && fileNumInside < archiveContent.length())
            filenameToLoad = QString("%1::ARC::%2").arg(archiveContent[fileNumInside], filename);

        // process file
        QSize tmp;
        QImage img;
        PQCLoadImage::get().load(filenameToLoad, QSize(-1,-1), tmp, img);
        img.save(tmpfile);

        // display information
        std::cout << "display mime: image/jpeg" << std::endl
                  << "source mime: " << db.mimeTypeForFile(filename).name().toStdString() << std::endl
                  << "tmp path: " << QString("%1/tmpfile.jpg").arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).toStdString() << std::endl
                  << "file name: " << PQCScripts::get().getFilename(filename).toStdString() << std::endl;

        if(isPDF) {
            std::cout << "file number: " << fileNumInside << std::endl;
            std::cout << "file count: " << PQCScripts::get().getDocumentPageCount(filename) << std::endl;
        } else if(isARC) {
            std::cout << "file number: " << fileNumInside << std::endl;
            std::cout << "file name (inside): " << archiveContent[fileNumInside].toStdString() << std::endl;
            std::cout << "file count: " << archiveContent.length() << std::endl;
        }

        std::exit(0);
        return;

    }

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
