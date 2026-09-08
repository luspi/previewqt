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

// VERSIONINFO constants from Win32 Ver.h
namespace VersionInfo {

    // dwFileFlagsMask
    constexpr quint32 FileFlagsMask = 0x0000003F;

    // dwFileFlags
    constexpr quint32 Debug        = 0x00000001;
    constexpr quint32 Prerelease   = 0x00000002;
    constexpr quint32 Patched      = 0x00000004;
    constexpr quint32 PrivateBuild = 0x00000008;
    constexpr quint32 InfoInferred = 0x00000010;
    constexpr quint32 SpecialBuild = 0x00000020;

    // dwFileOS
    constexpr quint32 OsUnknown         = 0x00000000;
    constexpr quint32 OsDos             = 0x00010000;
    constexpr quint32 OsOs216           = 0x00020000;
    constexpr quint32 OsOs232           = 0x00030000;
    constexpr quint32 OsNt              = 0x00040000;
    constexpr quint32 OsDosWindows16    = 0x00010001;
    constexpr quint32 OsDosWindows32    = 0x00010002;
    constexpr quint32 OsOs216Pm16       = 0x00020001;
    constexpr quint32 OsOs232Pm32       = 0x00030001;
    constexpr quint32 OsNtWindows32     = 0x00040004;

    // dwFileType
    constexpr quint32 TypeUnknown   = 0x00000000;
    constexpr quint32 TypeApp       = 0x00000001;
    constexpr quint32 TypeDll       = 0x00000002;
    constexpr quint32 TypeDriver    = 0x00000003;
    constexpr quint32 TypeFont      = 0x00000004;
    constexpr quint32 TypeVxd       = 0x00000005;
    constexpr quint32 TypeStaticLib = 0x00000007;

    // dwFileSubtype, depending on dwFileType
    constexpr quint32 SubtypeUnknown = 0x00000000;

    // driver subtypes
    constexpr quint32 DriverComm             = 0x00000002;
    constexpr quint32 DriverPrinter          = 0x00000001;
    constexpr quint32 DriverKeyboard         = 0x0000000B;
    constexpr quint32 DriverLanguage         = 0x00000003;
    constexpr quint32 DriverDisplay          = 0x00000004;
    constexpr quint32 DriverMouse            = 0x00000005;
    constexpr quint32 DriverNetwork          = 0x00000006;
    constexpr quint32 DriverSystem            = 0x00000007;
    constexpr quint32 DriverInstallable      = 0x00000008;
    constexpr quint32 DriverSound             = 0x00000009;
    constexpr quint32 DriverVersionedPrinter = 0x0000000C;

    // font subtypes
    constexpr quint32 FontRaster   = 0x00000001;
    constexpr quint32 FontVector   = 0x00000002;
    constexpr quint32 FontTrueType = 0x00000003;

    // common code pages
    constexpr quint16 CodePageUtf16Le = 1200;
    constexpr quint16 CodePageUtf16Be = 1201;
    constexpr quint16 CodePageUtf8    = 65001;

}

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

    inline bool readUInt16LE(QFile& file, quint16& value);
    inline bool readUInt32LE(QFile& file, quint32& value);
    inline bool readUInt64LE(QFile& file, quint64& value);
    inline quint64 align4(quint64 value);
    inline bool readUtf16String(QFile& file, QString& value);
    inline bool rvaToFileOffset(quint32 rva, const QList<PESection>& sections, quint64& fileOffset);
    inline bool readResourceDirectoryEntries(QFile& file, quint64 offset, QList<ResourceEntry>& entries);

    inline bool readBytes(QFile& file, qsizetype size, QByteArray& data);
    inline bool skipBytes(QFile& file, qsizetype size);

    inline QString formatVersion(quint32 ms, quint32 ls);
    inline QString formatFileFlags(quint32 fileFlags);
    inline QString formatFileFlagsMask(quint32 fileFlagsMask);
    inline QString formatFileOs(quint32 fileos);
    inline QString formatFileType(quint32 filetype);
    inline QString formatFileSubtype(quint32 type, quint32 subtype);
    inline QString formatFileDate(quint32 filedateMS, quint32 filedateLS);
    inline QString formatStructureVersion(quint32 strucver);
    QString formatLanguageName(quint16 langId);
    inline QString formatCodePage(quint16 codepage);

    QHash<int,QString> langId2Code;
    inline QString convertLanguageCodeToString(QString code);

    inline bool parseVersionBlock(QFile& file, quint64 blockOffset, quint64 availableSize, QVariantMap& ret, quint32 languageId, quint32 codePage);

};
