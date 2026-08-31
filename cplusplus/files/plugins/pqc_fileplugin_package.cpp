/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2026 Lukas Spies                                  **
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

#include <fileplugins/pqc_fileplugin_package.h>

#ifdef PQMLIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#include <QXmlStreamReader>
#endif

#ifdef PQMLIBAPPIMAGE
#include <appimage/core/AppImage.h>
#include <appimage/core/PayloadIterator.h>
#include <appimage/utils/ResourcesExtractor.h>
#endif

using namespace Qt::StringLiterals;

PQCFilePluginPackage::PQCFilePluginPackage() {

    setData({
#ifdef PQMLIBARCHIVE
        {88877,	{{"Debian package"}, {"deb"}, {"application/vnd.debian.binary-package"}}},
        {88876, {{"RPM package"}, {"rpm"}, {""}}},
#endif
#ifdef PQMLIBAPPIMAGE
        {12548, {{"AppImage file"}, {"appimage"}, {""}}},
#endif
    });


}

const QVariantList PQCFilePluginPackage::loadData(QString path) {

    QFileInfo info(path);
    const QString suffix = info.suffix().toLower();
    if(suffix == "deb") {
        return getDebianData(path);
    } else if(suffix == "rpm") {
        return getRPMData(path);
    } else if(suffix == "appimage") {
        return getAppImageData(path);
    }

    return {false, "Unknown file"};

}

QVariantList PQCFilePluginPackage::getDebianData(QString path) {

#ifdef PQMLIBARCHIVE

    QFileInfo info(path);

    // Create new archive handler
    struct archive *a = archive_read_new();

    // We allow any type of compression and format
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    // Read file
#ifdef Q_OS_WIN
    if(archive_read_open_filename_w(a, reinterpret_cast<const wchar_t*>(path.utf16()), 10240) != ARCHIVE_OK) {
#else
    QByteArray tmpPath = QFile::encodeName(info.absoluteFilePath());
    if(archive_read_open_filename(a, tmpPath.constData(), 10240) != ARCHIVE_OK) {
#endif
        // If something went wrong, output error message and stop here
        const QString msg = "archive_read_open_filename() failed: " % QString(archive_error_string(a));
        qWarning() << msg;
        return {false, msg};
    }

    // Create a buffer of that size to hold the control archive
    QByteArray controlArchiveData;

    // Loop over entries in archive
    struct archive_entry *entry;
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        // Also, if the archives is malformed or there is an encoding issue then it is possible that this may return a nullptr
        // and PhotoQt might crash if not handled properly -> check before converting to QString
        const wchar_t *wpath = archive_entry_pathname_w(entry);
        if(!wpath) continue;
        QString filenameinside = QString::fromWCharArray(wpath);

        // look for control archive file
        if(filenameinside == "control.tar.zst") {

            // Find out the size of the data
            int64_t size = archive_entry_size(entry);

            if(size <= 0) {
                const QString msg = QString("Invalid image size of file in archive: %1").arg(size);
                qWarning() << msg;
                return {false, msg};
            }
            controlArchiveData.resize(size);

            // And finally read the file into the buffer in chunks
            char* ptr = controlArchiveData.data();
            qint64 total = 0;
            while (total < size) {
                la_ssize_t chunk = archive_read_data(a, ptr + total, size - total);
                if(chunk < 0) {
                    const QString msg = QString("Invalid chunk read: %1").arg(archive_error_string(a));
                    qWarning() << msg;
                    return {false, msg};
                }

                if (chunk == 0) {
                    break;
                }

                total += chunk;
            }

            if(total != size) {
                const QString msg = QString("Failed to read image data, read size (%1) doesn't match expected size (%2)...").arg(total).arg(size);
                qWarning() << msg;
                return {false, msg};
            }

            // Nothing more to do except some cleaning up below
            break;
        }

    }

    if(controlArchiveData.isEmpty()) {
        const QString msg = "Unable to extract control archive";
        qWarning() << msg;
        return {false, msg};
    }

    // Close archive
    if(archive_read_close(a) != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_close() failed" << archive_error_string(a);
    if(archive_read_free(a) != ARCHIVE_OK)
        qWarning() << "ERROR: archive_read_free() failed:" << archive_error_string(a);

    // EXTRACT CONTROL DATA

    // Create new archive handler
    a = archive_read_new();

    // We allow any type of compression and format
    archive_read_support_filter_zstd(a);
    archive_read_support_format_tar(a);

    if(archive_read_open_memory(a, controlArchiveData.constData(), controlArchiveData.size()) != ARCHIVE_OK) {
        // If something went wrong, output error message and stop here
        const QString msg = "archive_read_open_memory() failed: " % QString(archive_error_string(a));
        qWarning() << msg;
        return {false, msg};
    }

    // Create a buffer of that size to hold the control data
    QByteArray controlData;

    // Loop over entries in archive
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        // Also, if the archives is malformed or there is an encoding issue then it is possible that this may return a nullptr
        // and PhotoQt might crash if not handled properly -> check before converting to QString
        const wchar_t *wpath = archive_entry_pathname_w(entry);
        if(!wpath) continue;
        QString filenameinside = QString::fromWCharArray(wpath);

        // look for control archive file
        if(filenameinside == "control" || filenameinside == "./control") {

            // Find out the size of the data
            int64_t size = archive_entry_size(entry);

            if(size <= 0) {
                const QString msg = QString("Invalid image size of file in archive: %1").arg(size);
                qWarning() << msg;
                return {false, msg};
            }
            controlData.resize(size);

            // And finally read the file into the buffer in chunks
            char* ptr = controlData.data();
            qint64 total = 0;
            while (total < size) {
                la_ssize_t chunk = archive_read_data(a, ptr + total, size - total);
                if(chunk < 0) {
                    const QString msg = QString("Invalid chunk read: %1").arg(archive_error_string(a));
                    qWarning() << msg;
                    return {false, msg};
                }

                if (chunk == 0) {
                    break;
                }

                total += chunk;
            }

            if(total != size) {
                const QString msg = QString("Failed to read image data, read size (%1) doesn't match expected size (%2)...").arg(total).arg(size);
                qWarning() << msg;
                return {false, msg};
            }

            // Nothing more to do except some cleaning up below
            break;
        }

    }

    QVariantMap data = {{"name", ""},
                        {"version", ""},
                        {"architecture", ""},
                        {"maintainer", ""},
                        {"depends", ""},
                        {"numDepends", ""},
                        {"section", ""},
                        {"homepage", ""},
                        {"shortDescription", ""},
                        {"description", ""}};

    const QStringList parts = QString::fromUtf8(controlData).split("\n");

    bool previousOneWasDescription = false;

    for(const QString &entry : parts) {

        if(entry.startsWith("Package:")) {
            data["name"] = entry.split("Package:").at(1).trimmed();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Version:")) {
            data["version"] = entry.split("Version:").at(1).trimmed();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Architecture:")) {
            data["architecture"] = entry.split("Architecture:").at(1).trimmed();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Maintainer:")) {
            data["maintainer"] = entry.split("Maintainer:").at(1).trimmed();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Depends:")) {
            QStringList lst = entry.split("Depends:").at(1).trimmed().split(",", Qt::SkipEmptyParts);
            for(QString &s : lst)
                s = s.trimmed();
            lst.sort(Qt::CaseInsensitive);
            data["depends"] = lst.join("\n");
            data["numDepends"] = lst.length();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Section:")) {
            data["section"] = entry.split("Section:").at(1).trimmed();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Homepage:")) {
            data["homepage"] = entry.split("Homepage:").at(1).trimmed();
            previousOneWasDescription = false;
        } else if(entry.startsWith("Description:")) {
            data["shortDescription"] = entry.split("Description:").at(1).trimmed();
            previousOneWasDescription = true;
        } else if(entry.startsWith(" ") && previousOneWasDescription) {
            data["description"] = entry.trimmed();
            previousOneWasDescription = false;
        }

    }

    if(data["description"].toString().isEmpty())
        data["description"] = data["shortDescription"];

    return {true, data};

#endif

    return {};

}

QVariantList PQCFilePluginPackage::getRPMData(QString path) {

#ifdef PQMLIBARCHIVE

    QFileInfo info(path);

    // Create new archive handler
    struct archive *a = archive_read_new();

    // We allow any type of compression and format
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    // Read file
#ifdef Q_OS_WIN
    if(archive_read_open_filename_w(a, reinterpret_cast<const wchar_t*>(path.utf16()), 10240) != ARCHIVE_OK) {
#else
    QByteArray tmpPath = QFile::encodeName(info.absoluteFilePath());
    if(archive_read_open_filename(a, tmpPath.constData(), 10240) != ARCHIVE_OK) {
#endif
        // If something went wrong, output error message and stop here
        const QString msg = "archive_read_open_filename() failed: " % QString(archive_error_string(a));
        qWarning() << msg;
        return {false, msg};
    }

    // Create a buffer of that size to hold the control archive
    QByteArray metainfoData;

    // Loop over entries in archive
    struct archive_entry *entry;
    while(archive_read_next_header(a, &entry) == ARCHIVE_OK) {

        // Read the current file entry
        // We use the '_w' variant here, as otherwise on Windows this call causes a segfault when a file in an archive contains non-latin characters
        // Also, if the archives is malformed or there is an encoding issue then it is possible that this may return a nullptr
        // and PhotoQt might crash if not handled properly -> check before converting to QString
        const wchar_t *wpath = archive_entry_pathname_w(entry);
        if(!wpath) continue;
        QString filenameinside = QString::fromWCharArray(wpath);

        // look for control archive file
        if(filenameinside.startsWith("./usr/share/metainfo/") && filenameinside.endsWith(".metainfo.xml")) {

            // Find out the size of the data
            int64_t size = archive_entry_size(entry);

            if(size <= 0) {
                const QString msg = QString("Invalid image size of file in archive: %1").arg(size);
                qWarning() << msg;
                return {false, msg};
            }
            metainfoData.resize(size);

            // And finally read the file into the buffer in chunks
            char* ptr = metainfoData.data();
            qint64 total = 0;
            while (total < size) {
                la_ssize_t chunk = archive_read_data(a, ptr + total, size - total);
                if(chunk < 0) {
                    const QString msg = QString("Invalid chunk read: %1").arg(archive_error_string(a));
                    qWarning() << msg;
                    return {false, msg};
                }

                if (chunk == 0) {
                    break;
                }

                total += chunk;
            }

            if(total != size) {
                const QString msg = QString("Failed to read image data, read size (%1) doesn't match expected size (%2)...").arg(total).arg(size);
                qWarning() << msg;
                return {false, msg};
            }

            // Nothing more to do except some cleaning up below
            break;
        }

    }

    return parseMetaInfo(metainfoData);

#endif

    return {};

}

QVariantList PQCFilePluginPackage::getAppImageData(QString path) {

#ifdef PQMLIBAPPIMAGE

    appimage::core::AppImage appImage(path.toStdString());

    // first find the metainfo files present
    // we also (while we're at it) look for the desktop files
    // as multiple metainfo files might be present but only one desktop file should be
    // this allows us to detect the authoritative metainfo file if necessary
    QStringList metainfoFiles, desktopFiles;
    auto files = appImage.files();
    const std::string prefixMetainfo = "usr/share/metainfo/";
    const std::string prefixDesktop = "usr/share/applications/";
    for(const std::string &path : files) {
        if(path.rfind(prefixMetainfo, 0) == 0)
            metainfoFiles.append(QString::fromStdString(path));
        else if(path.rfind(prefixDesktop, 0) == 0)
            desktopFiles.append(QString::fromStdString(path));
    }

    if(metainfoFiles.length() == 0) {

        // no metainfo.xml file -> not much we can do

        const QString msg = "No metainfo.xml files found, unable to find out anything about this AppImage";
        qWarning() << msg;
        return {false, msg};

    } else if(metainfoFiles.length() == 1 || desktopFiles.length() == 0) {

        // exactly one metainfo.xml file, or no .desktop files -> use first metainfo.xml file in list

        appimage::utils::ResourcesExtractor extractor(appImage);
        const auto data = extractor.extract(metainfoFiles.first().toStdString());
        QByteArray dat(data.data(), data.size());
        return parseMetaInfo(dat);

    } else {

        // multiple metainfo.xml files -> check whether one of them matches the pattern of the .desktop file (if only one found)
        // otherwise we use simply the first one in the list of metainfo.xml files

        QString xmlFilename = desktopFiles.first();
        // 23 == prefixDesktop.length()
        // 8 == ".desktop".length()
        xmlFilename = "usr/share/metainfo/" % xmlFilename.sliced(23, xmlFilename.length()-23-8) % ".metainfo.xml";
        QString toParse = metainfoFiles.first();
        if(metainfoFiles.contains(xmlFilename))
            toParse = xmlFilename;

        appimage::utils::ResourcesExtractor extractor(appImage);
        const auto data = extractor.extract(toParse.toStdString());
        QByteArray dat(data.data(), data.size());
        return parseMetaInfo(dat);

    }

#endif

    const QString msg = "AppImage support wasn't enabled at compile time.";
    qWarning() << msg;
    return {false, msg};

}

QVariantList PQCFilePluginPackage::parseMetaInfo(QString content) {

    QVariantMap data = {{"name", ""},
                        {"license", ""},
                        {"version",""},
                        {"releaseDate", ""},
                        {"author", ""},
                        {"homepage", ""},
                        {"keywords", ""},
                        {"id", ""},
                        {"summary", ""},
                        {"description", ""}};

    bool insideDeveloper = false;

    QStringList keywords;

    QXmlStreamReader xmlReader(content);
    while(!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if(token == QXmlStreamReader::StartElement) {

            if(xmlReader.name() == "name"_L1) {
                if(insideDeveloper)
                    data["author"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
                else
                    data["name"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
            } else if(xmlReader.name() == "developer"_L1) {
                insideDeveloper = true;
            } else if(xmlReader.name() == "project_license"_L1)
                data["license"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
            else if(xmlReader.name() == "summary"_L1)
                data["summary"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
            else if(xmlReader.name() == "description"_L1 && data["description"].toString().isEmpty())
                data["description"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
            else if(xmlReader.name() == "url"_L1) {
                const QString tpe = xmlReader.attributes().value("type").toString();
                if(tpe == "homepage")
                    data["homepage"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
            } else if(xmlReader.name() == "release"_L1 && data["version"].toString().isEmpty()) {
                data["version"] = xmlReader.attributes().value("version").toString();
                data["releaseDate"] = xmlReader.attributes().value("date").toString();
            } else if(xmlReader.name() == "keyword"_L1)
                keywords << xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
            else if(xmlReader.name() == "id"_L1 && data["id"].toString().isEmpty())
                data["id"] = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
        }
    }
    if(xmlReader.hasError()) {
        const QString msg = "XML error: " + xmlReader.errorString();
        qWarning() << msg;
        return {false, msg};
    }

    // get the full description including any HTML tags
    // this cannot be obtained through QXmlStreamReader (yet)
    // if this check fails we stick to what (if anything) we found in the loop above
    if(content.contains("<description>") && content.contains("</description>"))
        data["description"] = content.split("<description>")[1].split("</description>")[0].trimmed();

    data["keywords"] = keywords.join(", ");

    return {true, data};

}
