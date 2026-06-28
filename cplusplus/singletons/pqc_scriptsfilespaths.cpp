/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
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
#include <pqc_scriptsfilespaths.h>
#include <pqc_filehandler.h>
#include <pqc_settingscpp.h>

// native api for executing commands
// we cannot use QProcess as even if started detached it keeps a
// link to the starting process and dies with it (if PreviewQt quits after executing).
#ifdef Q_OS_UNIX
#include <cstdlib>
#elif defined(Q_OS_WIN)
#include <windows.h>
#include <shellapi.h>
#endif

#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QProcess>
#include <thread>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>

PQCScriptsFilesPaths::PQCScriptsFilesPaths() {}

PQCScriptsFilesPaths::~PQCScriptsFilesPaths() {}

QString PQCScriptsFilesPaths::cleanPath(QString path) {

    if(path.startsWith("http:") || path.startsWith("https:"))
        return path.trimmed();

#ifdef Q_OS_WIN
    bool addslash = false;
    if(path.startsWith("//"))
        addslash = true;
#endif

    QUrl url(path);
    if(url.isLocalFile())
        path = url.toLocalFile();
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

#ifdef Q_OS_WIN
    path = QDir::cleanPath(path);
    if(addslash)
        return ("/"+path);
    return path;
#else
    return QDir::cleanPath(path);
#endif

}

QString PQCScriptsFilesPaths::getBasename(QString fullpath) {

    if(fullpath.isEmpty())
        return "";

    return QFileInfo(fullpath).baseName();

}

QString PQCScriptsFilesPaths::getFilename(QString path) {

    qDebug() << "args: path =" << path;

    if(path.startsWith("http://") || path.startsWith("https://"))
        return path;

    return QFileInfo(QByteArray::fromPercentEncoding(path.toUtf8())).fileName();

}

QString PQCScriptsFilesPaths::getDir(QString fullpath) {

    if(fullpath.isEmpty())
        return "";

    return QFileInfo(fullpath).absolutePath();

}

QString PQCScriptsFilesPaths::getSuffix(QString path) {

    if(path.isEmpty())
        return "";

    return QFileInfo(path).completeSuffix();

}

QString PQCScriptsFilesPaths::toPercentEncoding(QString str) {
    return QUrl::toPercentEncoding(str);
}

QString PQCScriptsFilesPaths::fromPercentEncoding(QByteArray str) {
    return QUrl::fromPercentEncoding(str);
}

bool PQCScriptsFilesPaths::isFileSupported(QString path) {

    qDebug() << "args: path =" << path;

    if(path.startsWith("http:") || path.startsWith("https:")) {
        qDebug() << "Remote URL detected: supported.";
        return true;
    }

    if(!QFileInfo::exists(cleanPath(path))) {
        qDebug() << "File does not exist: not supported.";
        return false;
    }

    const QString suffix = QFileInfo(path).suffix().toLower();
    if(PQCFileHandler::get().getSuffixes().contains(suffix)) {
        qDebug() << "Supported suffix detected.";
        return true;
    } else
        qDebug() << "Unknown suffix:" << suffix;

    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(path).name();
    if(PQCFileHandler::get().getMimetypes().contains(mimetype)) {
        qDebug() << "Supported mime-type detected.";
        return true;
    } else
        qDebug() << "Unknown mime-type:" << mimetype;

    qDebug() << "File is not supported by PreviewQt. Not yet.";

    return false;

}

QString PQCScriptsFilesPaths::toAbsolutePath(QString path) {

    if(path.startsWith("http:/") || path.startsWith("https:/"))
        return path;

    return QFileInfo(path).absoluteFilePath();

}

QString PQCScriptsFilesPaths::openNewFile() {

    // TODO: Add custom sorting model that takes both endings and mimetypes into account

    QFileDialog dlg;
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setDirectory(PQCSettingsCPP::get().getFiledialogLocation());

    int e = dlg.exec();

    if(e == QDialog::Accepted && dlg.selectedFiles().length()) {
        QStringList l = dlg.selectedFiles();
        return l.first();
    }

    return "";

}

bool PQCScriptsFilesPaths::openInDefault(QString path) {

    qDebug() << "args: path =" << path;

    if(path.isEmpty())
        return true;

    QString exe = "";

    // some things do not go by file ending, check those before anything else
    if(path.startsWith("http:/") || path.startsWith("https:/")) {

        exe = PQCSettingsCPP::get().getDefaultAppUrl();

    } else {

        QFileInfo info(path);
        const QString suffix = info.suffix().toLower();

        // first check for file endings

        if(PQCFileHandler::get().getSuffixes("pdf").contains(suffix)) {

            exe = PQCSettingsCPP::get().getDefaultAppDocuments();

        } else if(PQCFileHandler::get().getSuffixes("libarchive").contains(suffix) &&
                   (suffix == "cbr" || suffix == "cbt" || suffix == "cbz" || suffix == "cb7")) {

            exe = PQCSettingsCPP::get().getDefaultAppComicBooks();

        } else if(PQCFileHandler::get().getSuffixes("ebook").contains(suffix)) {

            exe = PQCSettingsCPP::get().getDefaultAppEBooks();

        } else if(PQCFileHandler::get().getSuffixes("libarchive").contains(suffix)) {

            exe = PQCSettingsCPP::get().getDefaultAppArchives();

        } else if(PQCFileHandler::get().getSuffixes("libmpv").contains(suffix) || PQCFileHandler::get().getSuffixes("video").contains(suffix)) {

            exe = PQCSettingsCPP::get().getDefaultAppVideos();

        } else if(PQCFileHandler::get().getSuffixes("text").contains(suffix)) {

            exe = PQCSettingsCPP::get().getDefaultAppText();

        } else if(PQCFileHandler::get().getSuffixes().contains(suffix)) {

            exe = PQCSettingsCPP::get().getDefaultAppImages();

        }

        // if nothing found check mime types

        if(exe.isEmpty()) {

            QMimeDatabase db;
            QString mimetype = db.mimeTypeForFile(path).name();

            if(PQCFileHandler::get().getMimetypes("pdf").contains(mimetype)) {

                exe = PQCSettingsCPP::get().getDefaultAppDocuments();

            } else if(PQCFileHandler::get().getMimetypes("libarchive").contains(mimetype) &&
                       (suffix == "cbr" || suffix == "cbt" || suffix == "cbz" || suffix == "cb7")) {

                exe = PQCSettingsCPP::get().getDefaultAppComicBooks();

            } else if(PQCFileHandler::get().getMimetypes("ebook").contains(mimetype)) {

                exe = PQCSettingsCPP::get().getDefaultAppEBooks();

            } else if(PQCFileHandler::get().getMimetypes("libarchive").contains(mimetype)) {

                exe = PQCSettingsCPP::get().getDefaultAppArchives();

            } else if(PQCFileHandler::get().getMimetypes("libmpv").contains(mimetype) || PQCFileHandler::get().getMimetypes("video").contains(mimetype)) {

                exe = PQCSettingsCPP::get().getDefaultAppVideos();

            } else if(PQCFileHandler::get().getMimetypes("text").contains(mimetype)) {

                exe = PQCSettingsCPP::get().getDefaultAppText();

            } else if(PQCFileHandler::get().getMimetypes().contains(mimetype)) {

                exe = PQCSettingsCPP::get().getDefaultAppImages();

            }

        }

    }

    // if nothing found default to photoqt
    if(exe.isEmpty()) {
#ifdef WIN32
        exe = "photoqt.exe";
#else
        exe = "photoqt";
#endif
    }

    if(exe == "_default_") {

        QUrl def;

        if(!path.startsWith("http:/") && !path.startsWith("https:/")) {
            def = QUrl::fromLocalFile(cleanPath(path));
        } else {
            def = QUrl(path);
        }

        qDebug() << "Opening in system default application:" << def;

        if(!QDesktopServices::openUrl(def))
            qWarning() << "Failed to open in default application:" << def;

        for(int i = 0; i < 100; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            qApp->processEvents();
        }

    } else {

        exe = QDir::toNativeSeparators(exe);
        path = QDir::toNativeSeparators(path);

        qDebug() << "Executing:" << exe;
        qDebug() << "Path:" << path;

        // we cannot use QProcess as even if started detached it keeps a
        // link to the starting process and dies with it (if PreviewQt quits after executing)
#ifdef Q_OS_UNIX
        system(QString("\"%1\" \"%2\" &").arg(exe, path.replace("\"", "\\\"")).toStdString().c_str());
#elif defined(Q_OS_WIN)
        ShellExecuteW(nullptr, L"open", (wchar_t*)exe.utf16(), (wchar_t*)QString("\"%1\"").arg(path.replace("\"", "\\\"")).utf16(), nullptr, SW_SHOWNORMAL);
#endif
    }

    return true;

}

void PQCScriptsFilesPaths::deleteTemporaryFiles() {

    qDebug() << "";

    QDir dir(PQCConfigFiles::get().CACHE_DIR() % "/archive");
    dir.removeRecursively();

    QDir dir2(PQCConfigFiles::get().CACHE_DIR() % "/motionphotos");
    dir2.removeRecursively();

}

void PQCScriptsFilesPaths::copyTextToClipboard(QString txt) {

    qDebug() << "args: txt.length =" << txt.length();

    QApplication::clipboard()->setText(txt, QClipboard::Clipboard);

}

QString PQCScriptsFilesPaths::saveImageToTempFile(QImage &img) {
    QString path = PQCConfigFiles::get().CACHE_DIR() % "/tmpfile.jpg";
    if(QFileInfo::exists(path))
        QFile::remove(path);
    if(!img.save(path))
        return "";
    return path;
}
