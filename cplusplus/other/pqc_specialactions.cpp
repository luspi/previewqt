#include <pqc_specialactions.h>
#include <pqc_scriptsimages.h>
#include <pqc_scriptsfilespaths.h>
#include <pqc_filehandler.h>

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

    QString filename = PQCScriptsFilesPaths::get().toAbsolutePath(PQCScriptsFilesPaths::cleanPath(path));

    QString tmpfile = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) % "/tmpfile.jpg";
    if(QFileInfo::exists(tmpfile))
        QFile::remove(tmpfile);

    if(!QFileInfo::exists(filename)) {
        std::cerr << "ERROR" << std::endl;
        return;
    }

    // we print out some mime info
    QMimeDatabase db;

    // the suffix for convenience
    QFileInfo info(filename);
    const QString suffix1 = info.suffix().toLower();
    const QString suffix2 = info.completeSuffix().toLower();

    // is this a video?
    const QSet<QString> videoSuffixes = PQCFileHandler::get().getSuffixes("video");
    if(videoSuffixes.contains(suffix1) || videoSuffixes.contains(suffix2)) {

        QString mime = db.mimeTypeForFile(filename).name();
        std::cout << "display mime: " << mime.toStdString() << std::endl
                  << "source mime: " << mime.toStdString() << std::endl
                  << "tmp path: " << filename.toStdString() << std::endl
                  << "file name: " << PQCScriptsFilesPaths::get().getFilename(filename).toStdString() << std::endl
                  << "video: yes" << std::endl;

        return;

    }

    // is this format supported by Qt?
    const QSet<QString> qtSuffixes = PQCFileHandler::get().getSuffixes("qt");
    if(qtSuffixes.contains(suffix1) || qtSuffixes.contains(suffix2)) {

        QString mime = db.mimeTypeForFile(filename).name();
        std::cout << "display mime: " << mime.toStdString() << std::endl
                  << "source mime: " << mime.toStdString() << std::endl
                  << "tmp path: " << filename.toStdString() << std::endl
                  << "file name: " << PQCScriptsFilesPaths::get().getFilename(filename).toStdString() << std::endl;

        QImageReader reader(filename);
        if(reader.supportsAnimation())
            std::cout << "animated: yes" << std::endl;

        return;

    }

    bool isPDF = PQCScriptsImages::get().isPDFDocument(filename);
    bool isARC = PQCScriptsImages::get().isArchive(filename);

    QStringList archiveContent;
    if(isARC) {
        archiveContent = PQCScriptsImages::get().getArchiveContent(filename, true);
    }

    QString filenameToLoad = filename;
    if(isPDF) {
        filenameToLoad = QString("%1::PDF::%2").arg(fileNumInside).arg(filename);
    } else if(isARC && fileNumInside < archiveContent.length()) {
        filenameToLoad = QString("%1::ARC::%2").arg(archiveContent[fileNumInside], filename);
    }

    // process file
    QSize tmp;
    QString err;
    QImage img = PQCFileHandler::get().getImage(filenameToLoad, QSize(-1,-1), tmp, err);
    img.save(tmpfile);

    // display information
    std::cout << "display mime: image/jpeg" << std::endl
              << "source mime: " << db.mimeTypeForFile(filename).name().toStdString() << std::endl
              << "tmp path: " << QString(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) % "/tmpfile.jpg").toStdString() << std::endl
              << "file name: " << PQCScriptsFilesPaths::get().getFilename(filename).toStdString() << std::endl;

    if(isPDF) {
        std::cout << "file number: " << fileNumInside << std::endl
                  << "file count: " << PQCScriptsImages::get().getDocumentPageCount(filename) << std::endl;
    } else if(isARC) {
        std::cout << "file number: " << fileNumInside << std::endl
                  << "file name (inside): " << archiveContent[fileNumInside].toStdString() << std::endl
                  << "file count: " << archiveContent.length() << std::endl;
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
    if(activeDolphinTarget.isEmpty())
            return "";

    /******************************************************************/
    // 2. Backup current clipboard interface

    QString backupClipboard = clipboardGet();

    // Note: Don't set a temporary (e.g., empty) clipboard content!
    // It might make retrieving the file copied by dolphin impossible to retrieve.

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
    if(!QFileInfo(ret).isFile()) {
        qDebug() << "Failed to read copied file from clipboard";
        clipboardSet(backupClipboard);
        return "";
    }

    /******************************************************************/
    // 5. Reset previous clipboard content

    qDebug() << "Successfully read copied file from clipboard:" << ret;
    clipboardSet(backupClipboard);

    /******************************************************************/

    return ret;

}

QString PQCSpecialActions::clipboardGet() {

    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    // Creating a DBus interface for Klipper service
    QDBusInterface klipperInterface("org.kde.klipper", "/klipper", "org.kde.klipper.klipper", sessionBus);

    if(klipperInterface.isValid()) {

        qDebug() << "Valid response received from klipper";

        // Calling the getClipboardContents method
        QDBusReply<QString> reply = klipperInterface.call("getClipboardContents");

        // Checking if the call was successful
        if(reply.isValid()) {
            qDebug() << "Received valid klipper reply:" << reply;
            return reply.value();
        } else
            qDebug() << "Invalid klipper reply received:" << reply;

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
