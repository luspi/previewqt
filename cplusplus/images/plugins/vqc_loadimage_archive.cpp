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

#include <thread>

#include <vqc_loadimage.h>
#include <vqc_loadimage_archive.h>
#include <vqc_configfiles.h>
#include <vqc_imageformats.h>
#include <vqc_scripts.h>
#include <QSize>
#include <QtDebug>
#include <QFileInfo>
#include <QImage>
#include <QProcess>
#include <QDir>

#ifdef VQMLIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif

VQCLoadImageArchive::VQCLoadImageArchive() {}

QString VQCLoadImageArchive::load(QString filename, QSize maxSize, QSize &origSize, QImage &img) {

    qDebug() << "args: filename =" << filename;
    qDebug() << "args: maxSize =" << maxSize;

    QString errormsg = "";

#ifdef VQMLIBARCHIVE

    // filter out name of archivefile and of compressed file inside
    QString archivefile = filename;
    QString compressedFilename = "";
    if(archivefile.contains("::ARC::")) {
        QStringList parts = archivefile.split("::ARC::");
        archivefile = parts.at(1);
        compressedFilename = parts.at(0);
    } else {
        QStringList cont = VQCScripts::get().listArchiveContent(archivefile);
        if(cont.length() == 0) {
            errormsg = "Unable to list contents of archive file...";
            qWarning() << errormsg;
            return errormsg;
        }
        compressedFilename = cont.at(0).split("::ARC::").at(0);
    }

    if(!QFileInfo::exists(archivefile)) {
        errormsg = "File doesn't seem to exist...";
        qWarning() << errormsg;
        return errormsg;
    }

    QFileInfo info(archivefile);

    if(info.suffix() == "cbr" || info.suffix() == "rar") {

        QProcess which;
        which.setStandardOutputFile(QProcess::nullDevice());
        which.start("which", QStringList() << "unrar");
        which.waitForFinished();

        if(!which.exitCode()) {

            qDebug() << "loading archive with unrar";

            const QString tmpDir = VQCConfigFiles::CACHE_DIR()+"/unrar/";

            QDir dir;
            if(dir.mkpath(tmpDir)) {

                QProcess p;
                p.start("unrar", QStringList() << "x" << "-y" << archivefile << compressedFilename << tmpDir);
                p.waitForFinished(15000);

                VQCLoadImage::get().load(tmpDir + compressedFilename, QSize(-1,-1), origSize, img);
                QDir dir(tmpDir);
                dir.removeRecursively();

                // Scale image if necessary
                if(maxSize.width() != -1) {

                    QSize finalSize = origSize;

                    if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height())
                        finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

                    img = img.scaled(finalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

                }

                return "";

            } else
                qWarning() << "unable to create temporary folder for unrar target:" << tmpDir;

        } else
            qWarning() << "unrar was not found in system path";

    }

    // Create new archive handler
    struct archive *a = archive_read_new();

    // We allow any type of compression and format
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    // Read file
    int r = archive_read_open_filename(a, archivefile.toLocal8Bit().data(), 10240);

    // If something went wrong, output error message and stop here
    if(r != ARCHIVE_OK) {
        errormsg = QString("archive_read_open_filename() returned code of %1").arg(r);
        qWarning() << errormsg;
        return errormsg;
    }

    // Loop over entries in archive
    struct archive_entry *entry;
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        QString filenameinside = QString::fromWCharArray(archive_entry_pathname_w(entry));

        // If this is the file we are looking for:
        if(filenameinside == compressedFilename || (compressedFilename == "" && QFileInfo(filenameinside).suffix() != "")) {

            // Find out the size of the data
            int64_t size = archive_entry_size(entry);

            // Create a uchar buffer of that size to hold the image data
            uchar *buff = new uchar[size];

            // And finally read the file into the buffer
#ifdef WIN32
            size_t r = archive_read_data(a, (void*)buff, size);
#else
            ssize_t r = archive_read_data(a, (void*)buff, size);
#endif
            if(r != size) {
                errormsg = QString("Failed to read image data, read size (%1) doesn't match expected size (%2)...").arg(r).arg(size);
                qWarning() << errormsg;
                return errormsg;
            }

            // and finish off by turning it into an image

            // we extract it to a temp location from where we can load it then
            const QString temppath = VQCConfigFiles::CACHE_DIR() + "/archive/" + filenameinside;

            // file handles
            QFile file(temppath);
            QFileInfo info(file);

            // remove it if it exists, there is no way to know if it's the same file or not
            if(file.exists()) file.remove();

            // make sure the path exists
            QDir dir(info.absolutePath());
            if(!dir.exists())
                dir.mkpath(info.absolutePath());

            // write buffer to file
            file.open(QIODevice::WriteOnly);
            QDataStream out(&file);   // we will serialize the data into the file
            out.writeRawData((const char*) buff,size);
            file.close();
            delete[] buff;

            // attempt to load file
            QString err = VQCLoadImage::get().load(temppath, QSize(-1,-1), origSize, img);
            if(err != "")
                qWarning() << "Failed to load image inside archive:" << filenameinside;

            // finally remove file again
            file.remove();

            // Nothing more to do except some cleaning up below
            break;
        }

    }

    // Close archive
    r = archive_read_free(a);
    if(r != ARCHIVE_OK)
        qWarning() << "PQLoadImage::Archive::load(): ERROR: archive_read_free() returned code of" << r;

    // Scale image if necessary
    if(maxSize.width() != -1) {

        QSize finalSize = origSize;

        if(finalSize.width() > maxSize.width() || finalSize.height() > maxSize.height())
            finalSize = finalSize.scaled(maxSize, Qt::KeepAspectRatio);

        img = img.scaled(finalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    }

    return "";

#else

    origSize = QSize(-1,-1);
    errormsg = "Failed to load archive, LibArchive not supported by this build of PhotoQt!";
    qWarning() << errormsg;
    return errormsg;

#endif

}
