#include <pqc_specialactions.h>
#include <pqc_scripts.h>
#include <pqc_imageformats.h>
#include <pqc_loadimage.h>

#include <QMimeDatabase>
#include <QStandardPaths>
#include <QFileInfo>
#include <QImageReader>
#include <iostream>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QProcess>
#include <thread>

PQCSpecialActions::PQCSpecialActions() {}

void PQCSpecialActions::processOnly(QString path, int fileNumInside) {

    QString filename = PQCScripts::get().toAbsolutePath(PQCScripts::cleanPath(path));

    QString tmpfile = QString("%1/tmpfile.jpg").arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if(QFileInfo::exists(tmpfile))
        QFile::remove(tmpfile);

    if(!QFileInfo::exists(filename)) {
        std::cerr << "ERROR" << std::endl;
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

    return;

}

/******************************************************************/
/******************************************************************/
/******************************************************************/

QString PQCSpecialActions::getSelectedFile() {

    return getSelectedFile_dolphin();

}

QString PQCSpecialActions::getSelectedFile_dolphin() {

    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    /******************************************************************/
    // 1. Check if a dolphin window is currently active

    QString activeDolphinTarget = "";
    const QStringList lst = sessionBus.interface()->registeredServiceNames();
    for(const QString &l : lst) {
        if(l.contains("dolphin")) {

            // we don't print any error messages below as we don't care about them

            QDBusInterface dolphinInterface(l, "/dolphin/Dolphin_1", "org.qtproject.Qt.QWidget", sessionBus);

            if(dolphinInterface.isValid()) {
                // Call the isActiveWindow property
                QVariant reply = dolphinInterface.property("isActiveWindow");
                if(reply.isValid()) {
                    if(reply.toBool()) {
                        activeDolphinTarget = l;
                        break;
                    }
                }
            }

        }
    }

    // if not dolphin target is active -> stop
    if(activeDolphinTarget == "")
            return "";

    /******************************************************************/
    // 2. Backup current clipboard interface

    QString backupClipboard = clipboardGet();

    /******************************************************************/
    // 3. Prompt dolphin to copy selected location to clipboard

    // Creating a DBus interface for the specified destination service
    QDBusInterface dolphinInterface(activeDolphinTarget, "/dolphin/Dolphin_1/actions/copy_location", "org.qtproject.Qt.QAction", sessionBus);

    // Checking if the interface is valid
    if(dolphinInterface.isValid()) {

        // Calling the trigger method
        QDBusReply<void> reply = dolphinInterface.call("trigger");

        // Checking if the call was successful
        if(reply.isValid())
            qDebug() << "Triggered copy_location action successfully";
        else
            return "";

    } else
        return "";

    // we need to wait shortly here to ensure dolphin has finished copying the path to the clipboard
    std::this_thread::sleep_for(std::chrono::milliseconds(50));


    /******************************************************************/
    // 4. Get copied path from clipboard

    QString ret = clipboardGet();

    /******************************************************************/
    // 5. Reset previous clipboard content

    clipboardSet(backupClipboard);

    /******************************************************************/

    return ret;

}

QString PQCSpecialActions::clipboardGet() {

    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    // Creating a DBus interface for Klipper service
    QDBusInterface klipperInterface("org.kde.klipper", "/klipper", "org.kde.klipper.klipper", sessionBus);

    if(klipperInterface.isValid()) {

        // Calling the getClipboardContents method
        QDBusReply<QString> reply = klipperInterface.call("getClipboardContents");

        // Checking if the call was successful
        if(reply.isValid())
            return reply.value();

    }

    return "";

}

void PQCSpecialActions::clipboardSet(QString val) {

    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    // Creating a DBus interface for Klipper service
    QDBusInterface klipperInterface("org.kde.klipper", "/klipper", "org.kde.klipper.klipper", sessionBus);

    if(klipperInterface.isValid()) {

        // Calling the getClipboardContents method
        QDBusReply<QString> reply = klipperInterface.call("setClipboardContents", val);

    }

}
