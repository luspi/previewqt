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
#pragma once

#include <fileplugins/pqc_fileplugin.h>
#include <QSet>
#include <QSize>
#include <QImage>

class PQCFilePluginDLLExe : public PQCFilePlugin {

    struct PESection {
        quint32 virtualAddress = 0;
        quint32 virtualSize = 0;
        quint32 rawAddress = 0;
        quint32 rawSize = 0;
    };

    struct ResourceEntry {

        quint32 name = 0;
        quint32 offset = 0;

        bool isNamed() const {
            return name & 0x80000000u;
        }
        bool isDirectory() const {
            return offset & 0x80000000u;
        }
        quint32 id() const {
            return name & 0xffffu;
        }
        quint32 directoryOffset() const {
            return offset & 0x7fffffffu;
        }
    };

public:
    PQCFilePluginDLLExe();

    const QString name() override { return "DLL/EXE"; }
    const QSize loadSize(QString) override { return QSize(); };
    const QImage loadImage(QString, QSize, QSize&, QString&) override { return QImage(); };
    const QVariantList loadData(QString path) override;
    const int loadNumPages(QString path) override { return 1; }
    const QStringList loadContent(QString path) override { return {path}; }
    const QJsonObject loadJSON(QString path, QVariantMap extraArguments) override { return {}; };

private:
    QVariantMap parseDLLMetadata(const QString path);

    static bool readUInt16LE(QFile& file, quint16& value);
    static bool readUInt32LE(QFile& file, quint32& value);
    static bool readUInt64LE(QFile& file, quint64& value);
    static quint64 align4(quint64 value);
    static QString formatVersion(quint32 ms, quint32 ls);
    static bool readUtf16String(QFile& file, QString& value);
    static bool rvaToFileOffset(quint32 rva, const QList<PESection>& sections, quint64& fileOffset);
    static bool readResourceDirectoryEntries(QFile& file, quint64 offset, QList<ResourceEntry>& entries);

    static bool readBytes(QFile& file, qsizetype size, QByteArray& data);
    static bool skipBytes(QFile& file, qsizetype size);

    static bool parseVersionBlock(QFile& file, quint64 blockOffset, quint64 availableSize, QVariantMap& ret, quint32 languageId, quint32 codePage);

};
