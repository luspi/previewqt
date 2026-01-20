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
#include <pqc_scriptsfilespaths.h>
#include <pqc_fileformats.h>
#include <pqc_settingscpp.h>

#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QProcess>
#include <thread>
#include <QApplication>
#include <QClipboard>

PQCScriptsFilesPaths::PQCScriptsFilesPaths() {}

PQCScriptsFilesPaths::~PQCScriptsFilesPaths() {}

QString PQCScriptsFilesPaths::cleanPath(QString path) {

#ifdef Q_OS_WIN
    return cleanPath_windows(path);
#else
    if(path.startsWith("file:////"))
        path = path.remove(0, 8);
    else if(path.startsWith("file:///"))
        path = path.remove(0, 7);
    else if(path.startsWith("file://"))
        path = path.remove(0, 6);
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    return QDir::cleanPath(path);
#endif

}

QString PQCScriptsFilesPaths::cleanPath_windows(QString path) {

    if(path.startsWith("file:///"))
        path = path.remove(0, 8);
    else if(path.startsWith("file://"))
        path = path.remove(0, 7);
    else if(path.startsWith("file:/"))
        path = path.remove(0, 6);
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    return QDir::cleanPath(path);

}

QString PQCScriptsFilesPaths::getBasename(QString fullpath) {

    if(fullpath == "")
        return "";

    return QFileInfo(fullpath).baseName();

}

QString PQCScriptsFilesPaths::getFilename(QString path) {

    qDebug() << "args: path =" << path;

    return QFileInfo(QByteArray::fromPercentEncoding(path.toUtf8())).fileName();

}

QString PQCScriptsFilesPaths::getDir(QString fullpath) {

    if(fullpath == "")
        return "";

    return QFileInfo(fullpath).absolutePath();

}

QString PQCScriptsFilesPaths::getSuffix(QString path) {

    if(path == "")
        return "";

    return QFileInfo(path).completeSuffix();

}

QString PQCScriptsFilesPaths::toPercentEncoding(QString str) {
    return QUrl::toPercentEncoding(str);
}

QString PQCScriptsFilesPaths::fromPercentEncoding(QByteArray str) {
    return QUrl::fromPercentEncoding(str);
}

bool PQCScriptsFilesPaths::doesFileExist(QString path) {

    qDebug() << "args: path =" << path;

    return QFileInfo::exists(path);

}

bool PQCScriptsFilesPaths::isFileSupported(QString path) {

    qDebug() << "args: path =" << path;

    if(!QFileInfo::exists(path))
        return false;

    const QString suffix = QFileInfo(path).suffix().toLower();
    return PQCFileFormats::get().getAllFormats().contains(suffix);

}

QString PQCScriptsFilesPaths::toAbsolutePath(QString path) {

    return QFileInfo(path).absoluteFilePath();

}

QString PQCScriptsFilesPaths::openNewFile() {

    // TODO: Add custom sorting model that takes both endings and mimetypes into account

    QFileDialog dlg;
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setDirectory(PQCSettingsCPP::get().getFiledialogLocation());

    int e = dlg.exec();

    if(e == QDialog::Accepted && dlg.selectedFiles().length() > 0) {
        QStringList l = dlg.selectedFiles();
        return l.first();
    }

    return "";

}

bool PQCScriptsFilesPaths::openInDefault(QString path) {

    qDebug() << "args: path =" << path;

    if(path == "")
        return true;

    QFileInfo info(path);
    const QString suffix = info.suffix().toLower();

    QString exe = "";

    // first check for file endings

    if(PQCFileFormats::get().getAllFormatsPoppler().contains(suffix)) {

        exe = PQCSettingsCPP::get().getDefaultAppDocuments();

    } else if(PQCFileFormats::get().getAllFormatsLibArchive().contains(suffix) &&
               (suffix == "cbr" || suffix == "cbt" || suffix == "cbz" || suffix == "cb7")) {

        exe = PQCSettingsCPP::get().getDefaultAppComicBooks();

    } else if(PQCFileFormats::get().getAllFormatsEBook().contains(suffix)) {

        exe = PQCSettingsCPP::get().getDefaultAppEBooks();

    } else if(PQCFileFormats::get().getAllFormatsLibArchive().contains(suffix)) {

        exe = PQCSettingsCPP::get().getDefaultAppArchives();

    } else if(PQCFileFormats::get().getAllFormatsLibmpv().contains(suffix) || PQCFileFormats::get().getAllFormatsVideo().contains(suffix)) {

        exe = PQCSettingsCPP::get().getDefaultAppVideos();

    } else if(PQCFileFormats::get().getAllFormatsText().contains(suffix)) {

        exe = PQCSettingsCPP::get().getDefaultAppText();

    } else if(PQCFileFormats::get().getAllFormatsQt().contains(suffix) || PQCFileFormats::get().getAllFormatsFreeImage().contains(suffix) ||
               PQCFileFormats::get().getAllFormatsDevIL().contains(suffix) || PQCFileFormats::get().getAllFormatsLibRaw().contains(suffix) ||
               PQCFileFormats::get().getAllFormatsLibVips().contains(suffix) || PQCFileFormats::get().getAllFormatsMagick().contains(suffix) ||
               PQCFileFormats::get().getAllFormatsResvg().contains(suffix) || PQCFileFormats::get().getAllFormatsXCFTools().contains(suffix)) {

        exe = PQCSettingsCPP::get().getDefaultAppImages();

    }

    // if nothing found check mime types

    if(exe == "") {

        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(path).name();

        if(PQCFileFormats::get().getAllMimeTypesPoppler().contains(mimetype)) {

            exe = PQCSettingsCPP::get().getDefaultAppDocuments();

        } else if(PQCFileFormats::get().getAllMimeTypesLibArchive().contains(mimetype) &&
                   (suffix == "cbr" || suffix == "cbt" || suffix == "cbz" || suffix == "cb7")) {

            exe = PQCSettingsCPP::get().getDefaultAppComicBooks();

        } else if(PQCFileFormats::get().getAllMimeTypesEBook().contains(mimetype)) {

            exe = PQCSettingsCPP::get().getDefaultAppEBooks();

        } else if(PQCFileFormats::get().getAllMimeTypesLibArchive().contains(mimetype)) {

            exe = PQCSettingsCPP::get().getDefaultAppArchives();

        } else if(PQCFileFormats::get().getAllMimeTypesLibmpv().contains(mimetype) || PQCFileFormats::get().getAllMimeTypesVideo().contains(mimetype)) {

            exe = PQCSettingsCPP::get().getDefaultAppVideos();

        } else if(PQCFileFormats::get().getAllMimeTypesText().contains(mimetype)) {

            exe = PQCSettingsCPP::get().getDefaultAppText();

        } else if(PQCFileFormats::get().getAllMimeTypesQt().contains(mimetype) || PQCFileFormats::get().getAllMimeTypesFreeImage().contains(mimetype) ||
                   PQCFileFormats::get().getAllMimeTypesDevIL().contains(mimetype) || PQCFileFormats::get().getAllMimeTypesLibRaw().contains(mimetype) ||
                   PQCFileFormats::get().getAllMimeTypesLibVips().contains(mimetype) || PQCFileFormats::get().getAllMimeTypesMagick().contains(mimetype) ||
                   PQCFileFormats::get().getAllMimeTypesResvg().contains(mimetype) || PQCFileFormats::get().getAllMimeTypesXCFTools().contains(mimetype)) {

            exe = PQCSettingsCPP::get().getDefaultAppImages();

        }

    }

    // if nothing found default to photoqt
    if(exe == "") {
#ifdef WIN32
        exe = "photoqt.exe";
#else
        exe = "photoqt";
#endif
    }

    QProcess proc;
    proc.setProgram(QDir::toNativeSeparators(exe));
    proc.setArguments({cleanPath(path)});
    if(!proc.startDetached()) {
        qWarning() << "Could not start process!";
        return false;
    }

    if(PQCSettingsCPP::get().getCloseAfterDefaultApp())
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return true;

}

void PQCScriptsFilesPaths::deleteTemporaryFiles() {

    qDebug() << "";

    QDir dir(PQCConfigFiles::get().CACHE_DIR() + "/archive");
    dir.removeRecursively();

    QDir dir2(PQCConfigFiles::get().CACHE_DIR() + "/motionphotos");
    dir2.removeRecursively();

}

void PQCScriptsFilesPaths::copyTextToClipboard(QString txt) {

    qDebug() << "args: txt.length =" << txt.length();

    QApplication::clipboard()->setText(txt, QClipboard::Clipboard);

}
