/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
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

#include <pqc_scripts.h>
#include <pqc_imageformats.h>

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>
#include <QStringDecoder>
#include <QCollator>

#ifdef PQMLIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif

PQCScripts::PQCScripts() {}
PQCScripts::~PQCScripts() {}

QString PQCScripts::cleanPath(QString path) {

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

QString PQCScripts::cleanPath_windows(QString path) {

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

bool PQCScripts::fileExists(QString path) {

    qDebug() << "args: path =" << path;

    QFileInfo info(cleanPath(path));
    return info.exists();

}

QString PQCScripts::getFilename(QString path) {

    qDebug() << "args: path =" << path;

    return QFileInfo(cleanPath(path)).fileName();

}

QString PQCScripts::openFile() {

    qDebug() << "";

    return QFileDialog::getOpenFileName(nullptr, "Select a file", QDir::homePath(), QString("Images (*.%1)").arg(PQCImageFormats::get().getAllFormats().join(" *.")));

}

QStringList PQCScripts::listArchiveContent(QString path, bool insideFilenameOnly) {

    qDebug() << "args: path =" << path;

    QStringList ret;

    const QFileInfo info(path);

#ifndef Q_OS_WIN

    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << "unrar");
    which.waitForFinished();

    if(!which.exitCode()) {

        QProcess p;
        p.start("unrar", QStringList() << "lb" << info.absoluteFilePath());

        if(p.waitForStarted()) {

            QByteArray outdata = "";

            while(p.waitForReadyRead())
                outdata.append(p.readAll());

            auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
            QStringList allfiles = QString(toUtf16(outdata)).split('\n', Qt::SkipEmptyParts);

            allfiles.sort();

            if(insideFilenameOnly) {
                for(const QString &f : std::as_const(allfiles)) {
                    if(PQCImageFormats::get().getAllFormats().contains(QFileInfo(f).suffix()))
                        ret.append(f);
                }
            } else {
                for(const QString &f : std::as_const(allfiles)) {
                    if(PQCImageFormats::get().getAllFormats().contains(QFileInfo(f).suffix()))
                        ret.append(QString("%1::ARC::%2").arg(f, path));
                }
            }

        }

    }

    // this either means there is nothing in that archive
    // or something went wrong above with unrar
    if(ret.length() == 0) {

#endif

#ifdef PQMLIBARCHIVE

        // Create new archive handler
        struct archive *a = archive_read_new();

        // We allow any type of compression and format
        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);

        // Read file
        int r = archive_read_open_filename(a, info.absoluteFilePath().toLocal8Bit().data(), 10240);

        // If something went wrong, output error message and stop here
        if(r != ARCHIVE_OK) {
            qWarning() << "ERROR: archive_read_open_filename() returned code of" << r;
            return ret;
        }

        // Loop over entries in archive
        struct archive_entry *entry;
        QStringList allfiles;
        while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

            // Read the current file entry
            // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
            QString filenameinside = QString::fromWCharArray(archive_entry_pathname_w(entry));

            // If supported file format, append to temporary list
            if((PQCImageFormats::get().getAllFormats().contains(QFileInfo(filenameinside).suffix())))
                allfiles.append(filenameinside);

        }

        // Sort the temporary list and add to global list
        allfiles.sort();

        if(insideFilenameOnly) {
            ret = allfiles;
        } else {
            for(const QString &f : std::as_const(allfiles))
                ret.append(QString("%1::ARC::%2").arg(f, path));
        }

        // Close archive
        r = archive_read_free(a);
        if(r != ARCHIVE_OK)
            qWarning() << "ERROR: archive_read_free() returned code of" << r;

#endif

#ifndef Q_OS_WIN
    }
#endif

    QCollator collator;
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setIgnorePunctuation(true);
    collator.setNumericMode(true);

    std::sort(ret.begin(), ret.end(), [&collator](const QString &file1, const QString &file2) { return collator.compare(file1, file2) < 0; });

    return ret;

}
