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

#include <fileplugins/pqc_fileplugin_dllexe.h>
#include <QCryptographicHash>

PQCFilePluginDLLExe::PQCFilePluginDLLExe() {

    setData({
        {44125,
            {{"Windows DLL"}, {"dll"}, {"application/vnd.microsoft.portable-executable"}}},
        {44126,
            {{"Windows Executable"}, {"exe"}, {"application/vnd.microsoft.portable-executable"}}},
             });

}

const QVariantList PQCFilePluginDLLExe::loadData(QString path) {

    QFileInfo info(path);
    const QString mime = mimetypeForSupportedFile(path);
    if(mime.isEmpty() || !info.exists())
        return {};

    QVariantMap data;
    data["Filesize"] = info.size();
    data["TimeCreated"] = info.birthTime().toString();
    data["TimeModified"] = info.lastModified().toString();

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to read file";
        return {true, data};
    }

    const QByteArray dllContent = file.readAll();
    if(dllContent.size() < 64)
        return {true, data};

    data["SHA256"] = QCryptographicHash::hash(dllContent, QCryptographicHash::Sha256).toHex();
    data["VersionInfo"] = parseDLLMetadata(path);

    return {true, data};

}

QVariantMap PQCFilePluginDLLExe::parseDLLMetadata(const QString path) {

    QVariantMap ret;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "unable to open DLL:" << path;
        return ret;
    }

    /******************************************************/
    // DOS header

    quint16 mzSignature = 0;

    if(!readUInt16LE(file, mzSignature) || mzSignature != 0x5A4D) { // "MZ"
        qDebug() << "not a valid PE/DLL file";
        ret["Architecture"] = "Not a valid PE/DLL file";
        return ret;
    }

    // e_lfanew is at offset 0x3C.
    if(!file.seek(0x3C))
        return ret;

    quint32 eLfanew = 0;

    if(!readUInt32LE(file, eLfanew)) {
        qDebug() << "unable to read e_lfanew";
        return ret;
    }

    /******************************************************/
    // PE signature

    if(!file.seek(eLfanew)) {
        qDebug() << "unable to seek to e_lfanew";
        return ret;
    }

    quint32 peSignature = 0;

    if(!readUInt32LE(file, peSignature) || peSignature != 0x00004550) { // "PE\0\0"
        qDebug() << "wrong PE signature";
        return ret;
    }

    /******************************************************/
    // COFF file header

    quint16 machine = 0;
    quint16 numberOfSections = 0;
    quint32 timeDateStamp = 0;
    quint32 pointerToSymbolTable = 0;
    quint32 numberOfSymbols = 0;
    quint16 sizeOptionalHeader = 0;
    quint16 characteristics = 0;

    if(!readUInt16LE(file, machine)               || !readUInt16LE(file, numberOfSections) || !readUInt32LE(file, timeDateStamp) ||
        !readUInt32LE(file, pointerToSymbolTable) || !readUInt32LE(file, numberOfSymbols)  || !readUInt16LE(file, sizeOptionalHeader) ||
        !readUInt16LE(file, characteristics)) {
        qDebug() << "invalid COFF header";
        return ret;
    }

    Q_UNUSED(pointerToSymbolTable);
    Q_UNUSED(numberOfSymbols);
    Q_UNUSED(characteristics);

    /******************************************************/
    // architecture

    switch(machine) {
        case 0x014C:
            ret["Architecture"] = "x86 (32-bit)";
            break;
        case 0x8664:
            ret["Architecture"] = "x86_64 (64-bit)";
            break;
        case 0x01C0:
            ret["Architecture"] = "ARM (32-bit)";
            break;
        case 0xAA64:
            ret["Architecture"] = "ARM64 (64-bit)";
            break;
        default:
            ret["Architecture"] = QString("Unknown (Machine: %1)").arg(machine, 0, 16);
            break;
    }

    ret["Timestamp"] = QDateTime::fromSecsSinceEpoch(timeDateStamp).toString();

    /******************************************************/
    // optional header

    const quint64 optionalHeaderOffset = quint64(eLfanew) + 4 + 20;

    if(!file.seek(optionalHeaderOffset)) {
        qDebug() << "unable to seek to optional header";
        return ret;
    }

    quint16 optionalMagic = 0;

    if(!readUInt16LE(file, optionalMagic)) {
        qDebug() << "unable to seek to read optional magic";
        return ret;
    }

    quint32 entryPoint = 0;
    quint64 imageBase = 0;
    quint64 dataDirectoryOffset = 0;

    switch(optionalMagic) {

        case 0x10B: { // PE32

            if(!file.seek(optionalHeaderOffset + 16) || !readUInt32LE(file, entryPoint)) {
                qDebug() << "unable to read entry_point";
                return ret;
            }

            quint32 imageBase32 = 0;

            if(!file.seek(optionalHeaderOffset + 28) || !readUInt32LE(file, imageBase32)) {
                qDebug() << "unable to read image_base (32)";
                return ret;
            }

            // IMAGE_DATA_DIRECTORY starts at +96 for PE32
            dataDirectoryOffset = 96;

            break;
        }

        case 0x20B: { // PE32+

            if(!file.seek(optionalHeaderOffset + 16) || !readUInt32LE(file, entryPoint)) {
                qDebug() << "unable to read entry_point";
                return ret;
            }

            if(!file.seek(optionalHeaderOffset + 24) || !readUInt64LE(file, imageBase)) {
                qDebug() << "unable to read image_base";
                return ret;
            }

            // IMAGE_DATA_DIRECTORY starts at +112 for PE32+
            dataDirectoryOffset = 112;

            break;
        }

        default:
            qDebug() << "unknown PE optional header:" << Qt::hex << optionalMagic;
            return ret;

    }

    /******************************************************/
    // Resource Directory

    // IMAGE_DIRECTORY_ENTRY_RESOURCE
    const quint64 resourceDirectoryOffset = optionalHeaderOffset + dataDirectoryOffset + 2 * 8;

    quint32 resourceRva = 0;
    quint32 resourceSize = 0;

    if(!file.seek(resourceDirectoryOffset) || !readUInt32LE(file, resourceRva) || !readUInt32LE(file, resourceSize)) {
        resourceRva = 0;
        resourceSize = 0;
    }

    /******************************************************/
    // Section Headers

    QList<PESection> sections;
    QStringList sectionNames;

    const quint64 sectionTableOffset = quint64(eLfanew) + 4 + 20 + sizeOptionalHeader;

    if(!file.seek(sectionTableOffset)) {
        qDebug() << "unable to seek to section table offset";
        return ret;
    }

    for(quint16 i = 0; i < numberOfSections; ++i) {

        QByteArray sectionName;

        if(!readBytes(file, 8, sectionName)) {
            qDebug() << "unable to read section_name";
            return ret;
        }

        const qsizetype nullPos = sectionName.indexOf('\0');

        if(nullPos >= 0)
            sectionName.truncate(nullPos);

        sectionNames.append(QString::fromLatin1(sectionName).trimmed());

        PESection section;

        if(!readUInt32LE(file, section.virtualSize) || !readUInt32LE(file, section.virtualAddress) ||
            !readUInt32LE(file, section.rawSize)    || !readUInt32LE(file, section.rawAddress)) {
            qDebug() << "unable to read section related information";
            return ret;
        }

        // PointerToRelocations
        quint32 value32 = 0;

        if(!readUInt32LE(file, value32)) {
            qDebug() << "unable to read PointerToRelocations";
            return ret;
        }

        // PointerToLinenumbers
        if(!readUInt32LE(file, value32)) {
            qDebug() << "unable to read PointerToLinenumbers";
            return ret;
        }

        // NumberOfRelocations
        quint16 value16 = 0;

        if(!readUInt16LE(file, value16)) {
            qDebug() << "unable to read NumberOfRelocations";
            return ret;
        }

        // NumberOfLinenumbers
        if(!readUInt16LE(file, value16)) {
            qDebug() << "unable to NumberOfLinenumbers";
            return ret;
        }

        // Characteristics
        if(!readUInt32LE(file, value32)) {
            qDebug() << "unable to read Characteristics";
            return ret;
        }

        sections.append(section);
    }

    ret["Sections"] = sectionNames;

    /******************************************************/
    // find VERSION resource

    if(resourceRva == 0 || resourceSize == 0) {
        qDebug() << "bo VERSION resource found";
        return ret;
    }

    quint64 resourceFileOffset = 0;

    if(!rvaToFileOffset(resourceRva, sections, resourceFileOffset)) {
        qDebug() << "call to rvaToFileOffset() failed";
        return ret;
    }

    QList<ResourceEntry> rootEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset, rootEntries)) {
        qDebug() << "call to readResourceDirectoryEntries failed";
        return ret;
    }

    /******************************************************/
    // find RT_VERSION (resource ID 16)

    ResourceEntry versionType;
    bool foundVersion = false;

    for(const ResourceEntry& entry : std::as_const(rootEntries)) {
        if(!entry.isNamed() && entry.id() == 16 && entry.isDirectory()) {
            versionType = entry;
            foundVersion = true;
            break;
        }
    }

    if(!foundVersion) {
        qDebug() << "RT_VERSION not found";
        return ret;
    }

    // -------------------------------------------------------------------------
    // Version resource ID
    // -------------------------------------------------------------------------

    QList<ResourceEntry> versionEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + versionType.directoryOffset(), versionEntries)) {
        qDebug() << "call to readResourceDirectoryEntries failed";
        return ret;
    }

    if(versionEntries.isEmpty()) {
        qDebug() << "no version entries found";
        return ret;
    }

    const ResourceEntry versionResource = versionEntries.first();

    if(!versionResource.isDirectory()) {
        qDebug() << "version resource not a dir";
        return ret;
    }

    /******************************************************/
    // Language

    QList<ResourceEntry> languageEntries;

    if(!readResourceDirectoryEntries(file, resourceFileOffset + versionResource.directoryOffset(), languageEntries)) {
        qDebug() << "call to readResourceDirectoryEntries failed";
        return ret;
    }

    if(languageEntries.isEmpty()) {
        qDebug() << "no language entries found";
        return ret;
    }

    const ResourceEntry language = languageEntries.first();

    if(language.isDirectory()) {
        qDebug() << "language is a dir";
        return ret;
    }

    /******************************************************/
    // IMAGE_RESOURCE_DATA_ENTRY

    const quint64 dataEntryOffset = resourceFileOffset + language.offset;

    if(!file.seek(dataEntryOffset)) {
        qDebug() << "failed to seek to datae entry offset";
        return ret;
    }

    quint32 versionDataRva = 0;
    quint32 versionDataSize = 0;
    quint32 codePage = 0;
    quint32 reserved = 0;

    if(!readUInt32LE(file, versionDataRva) || !readUInt32LE(file, versionDataSize) ||
        !readUInt32LE(file, codePage) || !readUInt32LE(file, reserved)) {
        qDebug() << "failed to read version data info";
        return ret;
    }

    Q_UNUSED(reserved);

    quint64 versionFileOffset = 0;

    if(!rvaToFileOffset(versionDataRva, sections, versionFileOffset)) {
        qDebug() << "call to rvaToFileOffset failed";
        return ret;
    }

    /******************************************************/
    // Parse VS_VERSION_INFO

    if(!parseVersionBlock(file, versionFileOffset, versionDataSize, ret, language.id(), codePage))
        qDebug() << "unable to parse VERSIONINFO";

    return ret;

}

/***************************************************************/

bool PQCFilePluginDLLExe::parseVersionBlock(QFile& file, quint64 blockOffset, quint64 availableSize, QVariantMap& ret, quint32 languageId, quint32 codePage) {

    if(availableSize < 6) {
        qDebug() << "availableSize to small:" << availableSize;
        return false;
    }

    if(!file.seek(blockOffset)) {
        qDebug() << "failed to seek to block offset:" << blockOffset;
        return false;
    }

    quint16 length = 0;
    quint16 valueLength = 0;
    quint16 type = 0;

    if(!readUInt16LE(file, length) || !readUInt16LE(file, valueLength) || !readUInt16LE(file, type)) {
        qDebug() << "failed to read length/valueLength/type";
        return false;
    }

    // a VS_VERSIONINFO block must at least contain:
    // - WORD wLength
    // - WORD wValueLength
    // - WORD wType
    if(length < 6 || quint64(length) > availableSize) {
        qDebug() << "length/availableSize failed check:" << length << "/" << availableSize;
        return false;
    }

    const quint64 actualBlockEnd = blockOffset + quint64(length);

    /***************************************************************/
    // Key

    QString key;

    if(!readUtf16String(file, key)) {
        qDebug() << "failed to read key";
        return false;
    }

    const quint64 valueOffset = align4(quint64(file.pos()));

    if(valueOffset > actualBlockEnd) {
        qDebug() << "value appears to be beyond block end:" << valueOffset << "/" << actualBlockEnd;
        return false;
    }

    /***************************************************************/
    // VS_FIXEDFILEINFO

    if(type == 0 && key == "VS_VERSION_INFO") {

        // VS_FIXEDFILEINFO is 13 DWORDs = 52 bytes.

        constexpr quint64 fixedFileInfoSize = 52;

        if(valueLength >= 52 && valueOffset + fixedFileInfoSize <= actualBlockEnd) {

            if(!file.seek(valueOffset)) {
                qDebug() << "failed to seek to value offset:" << valueOffset;
                return false;
            }

            quint32 signature = 0;
            quint32 structureVersion = 0;
            quint32 fileVersionMS = 0;
            quint32 fileVersionLS = 0;
            quint32 productVersionMS = 0;
            quint32 productVersionLS = 0;
            quint32 fileFlagsMask = 0;
            quint32 fileFlags = 0;
            quint32 fileOS = 0;
            quint32 fileType = 0;
            quint32 fileSubtype = 0;
            quint32 fileDateMS = 0;
            quint32 fileDateLS = 0;

            if(!readUInt32LE(file, signature)      || !readUInt32LE(file, structureVersion) || !readUInt32LE(file, fileVersionMS) ||
                !readUInt32LE(file, fileVersionLS) || !readUInt32LE(file, productVersionMS) || !readUInt32LE(file, productVersionLS) ||
                !readUInt32LE(file, fileFlagsMask) || !readUInt32LE(file, fileFlags)        || !readUInt32LE(file, fileOS) ||
                !readUInt32LE(file, fileType)      || !readUInt32LE(file, fileSubtype)      || !readUInt32LE(file, fileDateMS) ||
                !readUInt32LE(file, fileDateLS)) {
                qDebug() << "failed to read various quint32";
                return false;
            }

            // VS_FFI_SIGNATURE
            if(signature == 0xFEEF04BDu) {

                ret["FileVersion"] = formatVersion(fileVersionMS, fileVersionLS);
                ret["ProductVersion"] = formatVersion(productVersionMS, productVersionLS);

                ret["FileFlagsMask"] = formatFileFlagsMask(fileFlagsMask);
                ret["FileFlags"] = formatFileFlags(fileFlags);
                ret["FileOS"] = formatFileOs(fileOS);
                ret["FileType"] = formatFileType(fileType);
                ret["FileSubtype"] = formatFileSubtype(fileType, fileSubtype);

                ret["StructureVersion"] = formatStructureVersion(structureVersion);
                ret["VersionLanguage"] = formatLanguageName(languageId);
                ret["VersionCodePage"] = formatCodePage(codePage);

                ret["FileDate"] = formatFileDate(fileDateMS, fileDateLS);
            }
        }

    /***************************************************************/
    // String values

    // A VERSIONINFO string block has:
    //
    // - wType        = 1
    // - wValueLength = number of UTF-16 characters
    //
    // => value occupies wValueLength * 2 bytes

    } else if(type == 1 && valueLength > 0) {

        const quint64 maxBytes = actualBlockEnd > valueOffset ? (actualBlockEnd - valueOffset) : 0;
        const quint64 requestedBytes = quint64(valueLength) * 2;
        const quint64 bytesToRead = qMin(requestedBytes, maxBytes);

        if(bytesToRead > 0) {

            if(!file.seek(valueOffset)) {
                qDebug() << "failed to seek to value offset";
                return false;
            }

            QByteArray raw;

            if(!readBytes(file, static_cast<qsizetype>(bytesToRead), raw)) {
                qDebug() << "failed to read raw bytes:" << bytesToRead;
                return false;
            }

            const qsizetype charCount = raw.size() / 2;

            QVector<char16_t> chars;
            chars.reserve(charCount);

            for(qsizetype i = 0; i < charCount; ++i) {
                const uchar* p = reinterpret_cast<const uchar*>(raw.constData() + i * 2);
                const quint16 ch = (quint16(p[0]) | (quint16(p[1]) << 8));
                chars.append(static_cast<char16_t>(ch));
            }

            QString value = QString::fromUtf16(chars.constData(), chars.size());

            // some VERSIONINFO resources include a terminating \0
            // even though wValueLength already accounts for it
            const qsizetype nul = value.indexOf(QChar(u'\0'));

            if(nul >= 0)
                value.truncate(nul);

            if(!key.isEmpty()) {

                // FileVersion and ProductVersion from VS_FIXEDFILEINFO
                // are preferred over their StringFileInfo counterparts

                const QSet<QString> lookFor = {"Comments", "CompanyName", "FileDescription", "LegalCopyright", "OriginalFilename", "ProductName"};
                if(lookFor.contains(key))
                    ret[key] = value;

            }
        }
    }

    /***************************************************************/
    // Children

    const quint64 valueBytes = (type == 1 ? quint64(valueLength) * 2 : valueLength);

    // Children start after the value and are aligned to a DWORD boundary.

    quint64 childOffset = align4(valueOffset + valueBytes);

    while(childOffset + 6 <= actualBlockEnd) {

        if(!file.seek(childOffset)) {
            qDebug() << "failed to seek to child offset:" << childOffset;
            return false;
        }

        quint16 childLength = 0;

        if(!readUInt16LE(file, childLength)) {
            qDebug() << "failed to read childLength";
            return false;
        }

        // a child block must have at least the 6-byte header and must
        // fit entirely inside its parent

        if(childLength < 6 || childOffset + quint64(childLength) > actualBlockEnd)
            break;

        if(!parseVersionBlock(file, childOffset, childLength, ret, languageId, codePage)) {
            qDebug() << "recursive call to parseVersionBlock failed";
            return false;
        }

        // Each child occupies a DWORD-aligned size in its parent.
        childOffset += align4(quint64(childLength));

    }

    return true;

}

/***************************************************************/

// various helper functions for the parser above

bool PQCFilePluginDLLExe::readUInt16LE(QFile& file, quint16& value) {

    const QByteArray data = file.read(2);
    if(data.size() != 2)
        return false;

    value = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.constData()));

    return true;

}

bool PQCFilePluginDLLExe::readUInt32LE(QFile& file, quint32& value) {

    const QByteArray data = file.read(4);
    if(data.size() != 4)
        return false;

    value = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.constData()));

    return true;

}

bool PQCFilePluginDLLExe::readUInt64LE(QFile& file, quint64& value) {

    const QByteArray data = file.read(8);
    if(data.size() != 8)
        return false;

    value = qFromLittleEndian<quint64>(reinterpret_cast<const uchar*>(data.constData()));

    return true;

}

bool PQCFilePluginDLLExe::readBytes(QFile& file, qsizetype size, QByteArray& data) {

    if (size < 0)
        return false;

    data = file.read(size);
    return data.size() == size;

}


bool PQCFilePluginDLLExe::readUtf16String(QFile& file, QString& value) {

    QVector<ushort> chars;

    while(true) {

        quint16 ch = 0;

        if(!readUInt16LE(file, ch))
            return false;

        if(ch == 0)
            break;

        chars.append(ch);
    }

    value = QString::fromUtf16(reinterpret_cast<const char16_t*>(chars.constData()), chars.size());

    return true;

}


bool PQCFilePluginDLLExe::skipBytes(QFile& file, qsizetype size) {

    if (size < 0)
        return false;

    return file.seek(file.pos() + size);

}


quint64 PQCFilePluginDLLExe::align4(quint64 value) {
    return (value + 3) & ~quint64(3);
}

bool PQCFilePluginDLLExe::rvaToFileOffset(quint32 rva, const QList<PESection>& sections, quint64& fileOffset) {

    for(const PESection& section : sections) {

        const quint32 size = qMax(section.virtualSize, section.rawSize);

        if(rva < section.virtualAddress)
            continue;

        if(quint64(rva) >= quint64(section.virtualAddress) + size)
            continue;

        fileOffset = quint64(section.rawAddress) + quint64(rva - section.virtualAddress);

        return true;
    }

    return false;
}

bool PQCFilePluginDLLExe::readResourceDirectoryEntries(QFile& file, quint64 offset, QList<ResourceEntry>& entries) {

    entries.clear();

    if(!file.seek(offset))
        return false;

    quint32 characteristics = 0;
    quint32 timeDateStamp = 0;
    quint16 majorVersion = 0;
    quint16 minorVersion = 0;
    quint16 numberOfNamedEntries = 0;
    quint16 numberOfIdEntries = 0;

    if(!readUInt32LE(file, characteristics) || !readUInt32LE(file, timeDateStamp)        || !readUInt16LE(file, majorVersion) ||
       !readUInt16LE(file, minorVersion)    || !readUInt16LE(file, numberOfNamedEntries) || !readUInt16LE(file, numberOfIdEntries)) {
        return false;
    }

    Q_UNUSED(characteristics);
    Q_UNUSED(timeDateStamp);
    Q_UNUSED(majorVersion);
    Q_UNUSED(minorVersion);

    const quint32 count = quint32(numberOfNamedEntries) + quint32(numberOfIdEntries);

    for(quint32 i = 0; i < count; ++i) {

        ResourceEntry entry;

        if(!readUInt32LE(file, entry.name) || !readUInt32LE(file, entry.offset))
            return false;

        entries.append(entry);

    }

    return true;
}

QString PQCFilePluginDLLExe::formatVersion(quint32 ms, quint32 ls) {
    return QString("%1.%2.%3.%4").arg(ms >> 16).arg(ms & 0xffff).arg(ls >> 16).arg(ls & 0xffff);
}

QString PQCFilePluginDLLExe::formatFileFlags(quint32 fileFlags) {

    if(fileFlags == 0)
        return "NORMAL (0)";

    QStringList vals;
    if(fileFlags & VersionInfo::Debug)
        vals.append("DEBUG (0x00000001)");
    if(fileFlags & VersionInfo::Prerelease)
        vals.append("PRERELEASE (0x00000002)");
    if(fileFlags & VersionInfo::Patched)
        vals.append("PATCHED (0x00000004)");
    if(fileFlags & VersionInfo::PrivateBuild)
        vals.append("PRIVATE BUILD (0x00000008)");
    if(fileFlags & VersionInfo::InfoInferred)
        vals.append("INFO INFERRED (0x00000010)");
    if(fileFlags & 0x00000020)
        vals.append("SPECIAL BUILD (0x00000020)");

    return vals.isEmpty() ? "NONE" : vals.join(" | ");

    return vals.join(", ");
}

QString PQCFilePluginDLLExe::formatFileFlagsMask(quint32 fileFlagsMask) {

    if(fileFlagsMask == 0x0000003F)
        return "normal file flags mask (0x0000003F)";

    return QString("0x%1").arg(fileFlagsMask, 8, 16, '0');

}

QString PQCFilePluginDLLExe::formatFileOs(quint32 fileos) {

    switch(fileos) {

        case VersionInfo::OsUnknown:
            return "Unknown";

        case VersionInfo::OsDos:
            return "DOS";

        case VersionInfo::OsOs216:
            return "OS/2 16-bit";

        case VersionInfo::OsOs232:
            return "OS/2 32-bit";

        case VersionInfo::OsNt:
            return "Windows NT";

        case VersionInfo::OsDosWindows16:
            return "DOS / Windows 16-bit";

        case VersionInfo::OsDosWindows32:
            return "DOS / Windows 32-bit";

        case VersionInfo::OsOs216Pm16:
            return "OS/2 16-bit / Presentation Manager";

        case VersionInfo::OsOs232Pm32:
            return "OS/2 32-bit / Presentation Manager";

        case VersionInfo::OsNtWindows32:
            return "Windows NT / Windows 32-bit";

        default:
            return QString("Unknown (0x%1)").arg(fileos, 8, 16, '0');

    }

}

QString PQCFilePluginDLLExe::formatFileType(quint32 filetype) {

    switch (filetype) {

        case VersionInfo::TypeUnknown:
            return "Unknown";

        case VersionInfo::TypeApp:
            return "Application";

        case VersionInfo::TypeDll:
            return "DLL";

        case VersionInfo::TypeDriver:
            return "Driver";

        case VersionInfo::TypeFont:
            return "Font";

        case VersionInfo::TypeVxd:
            return "VxD";

        case VersionInfo::TypeStaticLib:
            return "Static library";

        default:
            return QString("Unknown (0x%1)").arg(filetype, 8, 16, QLatin1Char('0'));

    }

}

QString PQCFilePluginDLLExe::formatFileSubtype(quint32 type, quint32 subtype) {

    if(subtype == VersionInfo::SubtypeUnknown)
        return "Unknown";

    switch (type) {

        case VersionInfo::TypeDriver:

            switch (subtype) {
                case VersionInfo::DriverPrinter:
                    return "Printer";
                case VersionInfo::DriverComm:
                    return "Communications";
                case VersionInfo::DriverLanguage:
                    return "Language";
                case VersionInfo::DriverDisplay:
                    return "Display";
                case VersionInfo::DriverMouse:
                    return "Mouse";
                case VersionInfo::DriverNetwork:
                    return "Network";
                case VersionInfo::DriverSystem:
                    return "System";
                case VersionInfo::DriverInstallable:
                    return "Installable";
                case VersionInfo::DriverSound:
                    return "Sound";
                case VersionInfo::DriverKeyboard:
                    return "Keyboard";
                case VersionInfo::DriverVersionedPrinter:
                    return "Versioned printer";
                default:
                    break;
            }

            break;

        case VersionInfo::TypeFont:

            switch (subtype) {
                case VersionInfo::FontRaster:
                    return "Raster";
                case VersionInfo::FontVector:
                    return "Vector";
                case VersionInfo::FontTrueType:
                    return "TrueType";
                default:
                    break;
            }

            break;

        default:
            break;

    }

    return QString("Unknown (0x%1)").arg(subtype, 8, 16, QLatin1Char('0'));

}

QString PQCFilePluginDLLExe::formatFileDate(quint32 filedateMS, quint32 filedateLS) {

    if(filedateMS == 0 && filedateLS == 0)
        return "Not specified";

    const quint64 fileDate = (quint64(filedateMS) << 32) | quint64(filedateLS);

    return QDateTime::fromSecsSinceEpoch(fileDate).toString();

}

QString PQCFilePluginDLLExe::formatStructureVersion(quint32 strucver) {
    return QString("%1.%2").arg(strucver >> 16).arg(strucver & 0xffff);
}

QString PQCFilePluginDLLExe::convertLanguageCodeToString(QString code) {

    qDebug() << "args: code =" << code;

    const QLocale locale(code);

    const QString l = QLocale::languageToString(locale.language());
    const QString t = QLocale::territoryToString(locale.territory());

    return t.isEmpty() ? l : QString("%1 (%2)").arg(l, t);

}

QString PQCFilePluginDLLExe::formatLanguageName(quint16 langId) {

    // based on Microsoft's current LCID/LANGID tables
    // returns BCP-47-style locale name as those are unambiguous
    // see: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/63d3d639-7fd2-4afb-abbe-0d5b5551eef8

    switch (langId) {

        // neutral / special
        case 0x0000: return "Neutral";
        case 0x007f: return "Invariant";

        // Arabic
        case 0x0001: return convertLanguageCodeToString("ar");
        case 0x0401: return convertLanguageCodeToString("ar-SA");
        case 0x0801: return convertLanguageCodeToString("ar-IQ");
        case 0x0c01: return convertLanguageCodeToString("ar-EG");
        case 0x1001: return convertLanguageCodeToString("ar-LY");
        case 0x1401: return convertLanguageCodeToString("ar-DZ");
        case 0x1801: return convertLanguageCodeToString("ar-MA");
        case 0x1c01: return convertLanguageCodeToString("ar-TN");
        case 0x2001: return convertLanguageCodeToString("ar-OM");
        case 0x2401: return convertLanguageCodeToString("ar-YE");
        case 0x2801: return convertLanguageCodeToString("ar-SY");
        case 0x2c01: return convertLanguageCodeToString("ar-JO");
        case 0x3001: return convertLanguageCodeToString("ar-LB");
        case 0x3401: return convertLanguageCodeToString("ar-KW");
        case 0x3801: return convertLanguageCodeToString("ar-AE");
        case 0x3c01: return convertLanguageCodeToString("ar-BH");
        case 0x4001: return convertLanguageCodeToString("ar-QA");
        case 0x4401: return convertLanguageCodeToString("ar-SA");
        case 0x4801: return convertLanguageCodeToString("ar");

        // Bulgarian
        case 0x0002: return convertLanguageCodeToString("bg");
        case 0x0402: return convertLanguageCodeToString("bg-BG");

        // Catalan
        case 0x0003: return convertLanguageCodeToString("ca");
        case 0x0403: return convertLanguageCodeToString("ca-ES");
        case 0x0803: return convertLanguageCodeToString("ca-ES");

        // Chinese
        case 0x0004: return convertLanguageCodeToString("zh-Hans");
        case 0x0404: return convertLanguageCodeToString("zh-TW");
        case 0x0804: return convertLanguageCodeToString("zh-CN");
        case 0x0c04: return convertLanguageCodeToString("zh-HK");
        case 0x1004: return convertLanguageCodeToString("zh-SG");
        case 0x1404: return convertLanguageCodeToString("zh-MO");
        case 0x7804: return convertLanguageCodeToString("zh");
        case 0x7c04: return convertLanguageCodeToString("zh-Hant");

        // Czech
        case 0x0005: return convertLanguageCodeToString("cs");
        case 0x0405: return convertLanguageCodeToString("cs-CZ");

        // Danish
        case 0x0006: return convertLanguageCodeToString("da");
        case 0x0406: return convertLanguageCodeToString("da-DK");

        // German
        case 0x0007: return convertLanguageCodeToString("de");
        case 0x0407: return convertLanguageCodeToString("de-DE");
        case 0x0807: return convertLanguageCodeToString("de-CH");
        case 0x0c07: return convertLanguageCodeToString("de-AT");
        case 0x1007: return convertLanguageCodeToString("de-LU");
        case 0x1407: return convertLanguageCodeToString("de-LI");

        // Greek
        case 0x0008: return convertLanguageCodeToString("el");
        case 0x0408: return convertLanguageCodeToString("el-GR");

        // English
        case 0x0009: return convertLanguageCodeToString("en");
        case 0x0409: return convertLanguageCodeToString("en-US");
        case 0x0809: return convertLanguageCodeToString("en-GB");
        case 0x0c09: return convertLanguageCodeToString("en-AU");
        case 0x1009: return convertLanguageCodeToString("en-CA");
        case 0x1409: return convertLanguageCodeToString("en-NZ");
        case 0x1809: return convertLanguageCodeToString("en-IE");
        case 0x1c09: return convertLanguageCodeToString("en-ZA");
        case 0x2009: return convertLanguageCodeToString("en-JM");
        case 0x2409: return convertLanguageCodeToString("en-029");
        case 0x2809: return convertLanguageCodeToString("en-BZ");
        case 0x2c09: return convertLanguageCodeToString("en-TT");
        case 0x3009: return convertLanguageCodeToString("en-ZW");
        case 0x3409: return convertLanguageCodeToString("en-PH");
        case 0x3809: return convertLanguageCodeToString("en-ID");
        case 0x3c09: return convertLanguageCodeToString("en-HK");
        case 0x4009: return convertLanguageCodeToString("en-IN");
        case 0x4409: return convertLanguageCodeToString("en-MY");
        case 0x4809: return convertLanguageCodeToString("en-SG");
        case 0x4c09: return convertLanguageCodeToString("en-AE");
        case 0x5009: return convertLanguageCodeToString("en-BH");
        case 0x5409: return convertLanguageCodeToString("en-EG");
        case 0x5809: return convertLanguageCodeToString("en-JO");
        case 0x5c09: return convertLanguageCodeToString("en-KW");
        case 0x6009: return convertLanguageCodeToString("en-TR");
        case 0x6409: return convertLanguageCodeToString("en-YE");

        // Spanish
        case 0x000a: return convertLanguageCodeToString("es");
        case 0x040a: return convertLanguageCodeToString("es-ES");
        case 0x080a: return convertLanguageCodeToString("es-MX");
        case 0x0c0a: return convertLanguageCodeToString("es-ES");
        case 0x100a: return convertLanguageCodeToString("es-GT");
        case 0x140a: return convertLanguageCodeToString("es-CR");
        case 0x180a: return convertLanguageCodeToString("es-PA");
        case 0x1c0a: return convertLanguageCodeToString("es-DO");
        case 0x200a: return convertLanguageCodeToString("es-VE");
        case 0x240a: return convertLanguageCodeToString("es-CO");
        case 0x280a: return convertLanguageCodeToString("es-PE");
        case 0x2c0a: return convertLanguageCodeToString("es-AR");
        case 0x300a: return convertLanguageCodeToString("es-EC");
        case 0x340a: return convertLanguageCodeToString("es-CL");
        case 0x380a: return convertLanguageCodeToString("es-UY");
        case 0x3c0a: return convertLanguageCodeToString("es-PY");
        case 0x400a: return convertLanguageCodeToString("es-BO");
        case 0x440a: return convertLanguageCodeToString("es-SV");
        case 0x480a: return convertLanguageCodeToString("es-HN");
        case 0x4c0a: return convertLanguageCodeToString("es-NI");
        case 0x500a: return convertLanguageCodeToString("es-PR");
        case 0x540a: return convertLanguageCodeToString("es-US");
        case 0x580a: return convertLanguageCodeToString("es-419");
        case 0x5c0a: return convertLanguageCodeToString("es-CU");

        // Finnish
        case 0x000b: return convertLanguageCodeToString("fi");
        case 0x040b: return convertLanguageCodeToString("fi-FI");

        // French
        case 0x000c: return convertLanguageCodeToString("fr");
        case 0x040c: return convertLanguageCodeToString("fr-FR");
        case 0x080c: return convertLanguageCodeToString("fr-BE");
        case 0x0c0c: return convertLanguageCodeToString("fr-CA");
        case 0x100c: return convertLanguageCodeToString("fr-CH");
        case 0x140c: return convertLanguageCodeToString("fr-LU");
        case 0x180c: return convertLanguageCodeToString("fr-MC");
        case 0x1c0c: return convertLanguageCodeToString("fr-029");
        case 0x200c: return convertLanguageCodeToString("fr-RE");
        case 0x240c: return convertLanguageCodeToString("fr-CD");
        case 0x280c: return convertLanguageCodeToString("fr-SN");
        case 0x2c0c: return convertLanguageCodeToString("fr-CM");
        case 0x300c: return convertLanguageCodeToString("fr-CI");
        case 0x340c: return convertLanguageCodeToString("fr-ML");
        case 0x380c: return convertLanguageCodeToString("fr-MA");
        case 0x3c0c: return convertLanguageCodeToString("fr-HT");
        case 0xe40c: return convertLanguageCodeToString("fr-015");

        // Hebrew
        case 0x000d: return convertLanguageCodeToString("he");
        case 0x040d: return convertLanguageCodeToString("he-IL");

        // Hungarian
        case 0x000e: return convertLanguageCodeToString("hu");
        case 0x040e: return convertLanguageCodeToString("hu-HU");

        // Icelandic
        case 0x000f: return convertLanguageCodeToString("is");
        case 0x040f: return convertLanguageCodeToString("is-IS");

        // Italian
        case 0x0010: return convertLanguageCodeToString("it");
        case 0x0410: return convertLanguageCodeToString("it-IT");
        case 0x0810: return convertLanguageCodeToString("it-CH");

        // Japanese
        case 0x0011: return convertLanguageCodeToString("ja");
        case 0x0411: return convertLanguageCodeToString("ja-JP");
        case 0x0811: return convertLanguageCodeToString("ja-JP");

        // Korean
        case 0x0012: return convertLanguageCodeToString("ko");
        case 0x0412: return convertLanguageCodeToString("ko-KR");

        // Dutch
        case 0x0013: return convertLanguageCodeToString("nl");
        case 0x0413: return convertLanguageCodeToString("nl-NL");
        case 0x0813: return convertLanguageCodeToString("nl-BE");

        // Norwegian
        case 0x0014: return convertLanguageCodeToString("no");
        case 0x0414: return convertLanguageCodeToString("nb-NO");
        case 0x0814: return convertLanguageCodeToString("nn-NO");
        case 0x7814: return convertLanguageCodeToString("nn");

        // Polish
        case 0x0015: return convertLanguageCodeToString("pl");
        case 0x0415: return convertLanguageCodeToString("pl-PL");

        // Portuguese
        case 0x0016: return convertLanguageCodeToString("pt");
        case 0x0416: return convertLanguageCodeToString("pt-BR");
        case 0x0816: return convertLanguageCodeToString("pt-PT");

        // Romansh
        case 0x0017: return convertLanguageCodeToString("rm");
        case 0x0417: return convertLanguageCodeToString("rm-CH");

        // Romanian
        case 0x0018: return convertLanguageCodeToString("ro");
        case 0x0418: return convertLanguageCodeToString("ro-RO");
        case 0x0818: return convertLanguageCodeToString("ro-MD");

        // Russian
        case 0x0019: return convertLanguageCodeToString("ru");
        case 0x0419: return convertLanguageCodeToString("ru-RU");
        case 0x0819: return convertLanguageCodeToString("ru-MD");

        // Croatian / Serbian / Bosnian
        case 0x001a: return "Croatian";
        case 0x041a: return convertLanguageCodeToString("hr-HR");
        case 0x081a: return convertLanguageCodeToString("sr-Latn");
        case 0x0c1a: return convertLanguageCodeToString("sr-Cyrl");
        case 0x101a: return convertLanguageCodeToString("hr-BA");
        case 0x141a: return convertLanguageCodeToString("bs-Latn-BA");
        case 0x181a: return convertLanguageCodeToString("sr-Latn-BA");
        case 0x1c1a: return convertLanguageCodeToString("sr-Cyrl-BA");
        case 0x201a: return convertLanguageCodeToString("bs-Cyrl-BA");
        case 0x241a: return convertLanguageCodeToString("sr-Latn-RS");
        case 0x281a: return convertLanguageCodeToString("sr-Cyrl-RS");
        case 0x2c1a: return convertLanguageCodeToString("sr-Latn-ME");
        case 0x301a: return convertLanguageCodeToString("sr-Cyrl-ME");
        case 0x641a: return convertLanguageCodeToString("bs-Cyrl");
        case 0x681a: return convertLanguageCodeToString("bs-Latn");
        case 0x6c1a: return convertLanguageCodeToString("sr-Cyrl");
        case 0x701a: return convertLanguageCodeToString("sr-Latn");
        case 0x781a: return convertLanguageCodeToString("bs");
        case 0x7c1a: return convertLanguageCodeToString("sr");

        // Slovak
        case 0x001b: return convertLanguageCodeToString("sk");
        case 0x041b: return convertLanguageCodeToString("sk-SK");

        // Albanian
        case 0x001c: return convertLanguageCodeToString("sq");
        case 0x041c: return convertLanguageCodeToString("sq-AL");

        // Swedish
        case 0x001d: return convertLanguageCodeToString("sv");
        case 0x041d: return convertLanguageCodeToString("sv-SE");
        case 0x081d: return convertLanguageCodeToString("sv-FI");

        // Thai
        case 0x001e: return convertLanguageCodeToString("th");
        case 0x041e: return convertLanguageCodeToString("th-TH");

        // Turkish
        case 0x001f: return convertLanguageCodeToString("tr");
        case 0x041f: return convertLanguageCodeToString("tr-TR");

        // Urdu
        case 0x0020: return convertLanguageCodeToString("ur");
        case 0x0420: return convertLanguageCodeToString("ur-PK");
        case 0x0820: return convertLanguageCodeToString("ur-IN");

        // Indonesian
        case 0x0021: return convertLanguageCodeToString("id");
        case 0x0421: return convertLanguageCodeToString("id-ID");

        // Ukrainian
        case 0x0022: return convertLanguageCodeToString("uk");
        case 0x0422: return convertLanguageCodeToString("uk-UA");

        // Belarusian
        case 0x0023: return convertLanguageCodeToString("be");
        case 0x0423: return convertLanguageCodeToString("be-BY");

        // Slovenian
        case 0x0024: return convertLanguageCodeToString("sl");
        case 0x0424: return convertLanguageCodeToString("sl-SI");

        // Estonian
        case 0x0025: return convertLanguageCodeToString("et");
        case 0x0425: return convertLanguageCodeToString("et-EE");

        // Latvian
        case 0x0026: return convertLanguageCodeToString("lv");
        case 0x0426: return convertLanguageCodeToString("lv-LV");

        // Lithuanian
        case 0x0027: return convertLanguageCodeToString("lt");
        case 0x0427: return convertLanguageCodeToString("lt-LT");

        // Tajik
        case 0x0028: return convertLanguageCodeToString("tg");
        case 0x0428: return convertLanguageCodeToString("tg-Cyrl-TJ");
        case 0x7c28: return convertLanguageCodeToString("tg-Cyrl");

        // Persian
        case 0x0029: return convertLanguageCodeToString("fa");
        case 0x0429: return convertLanguageCodeToString("fa-IR");

        // Vietnamese
        case 0x002a: return convertLanguageCodeToString("vi");
        case 0x042a: return convertLanguageCodeToString("vi-VN");

        // Armenian
        case 0x002b: return convertLanguageCodeToString("hy");
        case 0x042b: return convertLanguageCodeToString("hy-AM");

        // Azerbaijani
        case 0x002c: return convertLanguageCodeToString("az");
        case 0x042c: return convertLanguageCodeToString("az-Latn-AZ");
        case 0x082c: return convertLanguageCodeToString("az-Cyrl-AZ");
        case 0x742c: return convertLanguageCodeToString("az-Cyrl");
        case 0x782c: return convertLanguageCodeToString("az-Latn");

        // Basque
        case 0x002d: return convertLanguageCodeToString("eu");
        case 0x042d: return convertLanguageCodeToString("eu-ES");

        // Sorbian
        case 0x002e: return convertLanguageCodeToString("hsb");
        case 0x042e: return convertLanguageCodeToString("hsb-DE");
        case 0x082e: return convertLanguageCodeToString("dsb-DE");
        case 0x7c2e: return convertLanguageCodeToString("dsb");

        // Macedonian
        case 0x002f: return convertLanguageCodeToString("mk");
        case 0x042f: return convertLanguageCodeToString("mk-MK");

        // Southern Sotho
        case 0x0030: return convertLanguageCodeToString("st");
        case 0x0430: return convertLanguageCodeToString("st-ZA");

        // Tsonga
        case 0x0031: return convertLanguageCodeToString("ts");
        case 0x0431: return convertLanguageCodeToString("ts-ZA");

        // Tswana
        case 0x0032: return convertLanguageCodeToString("tn");
        case 0x0432: return convertLanguageCodeToString("tn-ZA");
        case 0x0832: return convertLanguageCodeToString("tn-BW");

        // Venda
        case 0x0033: return convertLanguageCodeToString("ve");
        case 0x0433: return convertLanguageCodeToString("ve-ZA");

        // Xhosa
        case 0x0034: return convertLanguageCodeToString("xh");
        case 0x0434: return convertLanguageCodeToString("xh-ZA");

        // Zulu
        case 0x0035: return convertLanguageCodeToString("zu");
        case 0x0435: return convertLanguageCodeToString("zu-ZA");

        // Afrikaans
        case 0x0036: return convertLanguageCodeToString("af");
        case 0x0436: return convertLanguageCodeToString("af-ZA");

        // Georgian
        case 0x0037: return convertLanguageCodeToString("ka");
        case 0x0437: return convertLanguageCodeToString("ka-GE");

        // Faroese
        case 0x0038: return convertLanguageCodeToString("fo");
        case 0x0438: return convertLanguageCodeToString("fo-FO");

        // Hindi
        case 0x0039: return convertLanguageCodeToString("hi");
        case 0x0439: return convertLanguageCodeToString("hi-IN");

        // Maltese
        case 0x003a: return convertLanguageCodeToString("mt");
        case 0x043a: return convertLanguageCodeToString("mt-MT");

        // Sami
        case 0x003b: return "Sami";
        case 0x043b: return convertLanguageCodeToString("se-NO");
        case 0x083b: return convertLanguageCodeToString("se-SE");
        case 0x0c3b: return convertLanguageCodeToString("se-FI");
        case 0x103b: return convertLanguageCodeToString("smj-NO");
        case 0x143b: return convertLanguageCodeToString("smj-SE");
        case 0x183b: return convertLanguageCodeToString("sma-NO");
        case 0x1c3b: return convertLanguageCodeToString("sma-SE");
        case 0x203b: return convertLanguageCodeToString("sms-FI");
        case 0x243b: return convertLanguageCodeToString("smn-FI");
        case 0x703b: return convertLanguageCodeToString("smn");
        case 0x743b: return convertLanguageCodeToString("sms");
        case 0x783b: return convertLanguageCodeToString("sma");
        case 0x7c3b: return convertLanguageCodeToString("smj");

        // Irish
        case 0x003c: return convertLanguageCodeToString("ga");
        case 0x043c: return convertLanguageCodeToString("ga-IE");
        case 0x083c: return convertLanguageCodeToString("ga-IE");

        // Yiddish
        case 0x003d: return convertLanguageCodeToString("yi");
        case 0x043d: return convertLanguageCodeToString("yi-001");

        // Malay
        case 0x003e: return convertLanguageCodeToString("ms");
        case 0x043e: return convertLanguageCodeToString("ms-MY");
        case 0x083e: return convertLanguageCodeToString("ms-BN");

        // Kazakh
        case 0x003f: return convertLanguageCodeToString("kk");
        case 0x043f: return convertLanguageCodeToString("kk-KZ");
        case 0x083f: return convertLanguageCodeToString("kk-Latn-KZ");
        case 0x783f: return convertLanguageCodeToString("kk-Cyrl");

        // Kyrgyz
        case 0x0040: return convertLanguageCodeToString("ky");
        case 0x0440: return convertLanguageCodeToString("ky-KG");

        // Swahili
        case 0x0041: return convertLanguageCodeToString("sw");
        case 0x0441: return convertLanguageCodeToString("sw-KE");

        // Turkmen
        case 0x0042: return convertLanguageCodeToString("tk");
        case 0x0442: return convertLanguageCodeToString("tk-TM");

        // Uzbek
        case 0x0043: return convertLanguageCodeToString("uz");
        case 0x0443: return convertLanguageCodeToString("uz-Latn-UZ");
        case 0x0843: return convertLanguageCodeToString("uz-Cyrl-UZ");
        case 0x7843: return convertLanguageCodeToString("uz-Cyrl");
        case 0x7c43: return convertLanguageCodeToString("uz-Latn");

        // Tatar
        case 0x0044: return convertLanguageCodeToString("tt");
        case 0x0444: return convertLanguageCodeToString("tt-RU");

        // Bengali / Bangla
        case 0x0045: return convertLanguageCodeToString("bn");
        case 0x0445: return convertLanguageCodeToString("bn-IN");
        case 0x0845: return convertLanguageCodeToString("bn-BD");

        // Punjabi
        case 0x0046: return convertLanguageCodeToString("pa");
        case 0x0446: return convertLanguageCodeToString("pa-IN");
        case 0x0846: return convertLanguageCodeToString("pa-Arab-PK");
        case 0x7c46: return convertLanguageCodeToString("pa-Arab");

        // Gujarati
        case 0x0047: return convertLanguageCodeToString("gu");
        case 0x0447: return convertLanguageCodeToString("gu-IN");

        // Odia
        case 0x0048: return convertLanguageCodeToString("or");
        case 0x0448: return convertLanguageCodeToString("or-IN");

        // Tamil
        case 0x0049: return convertLanguageCodeToString("ta");
        case 0x0449: return convertLanguageCodeToString("ta-IN");
        case 0x0849: return convertLanguageCodeToString("ta-LK");

        // Telugu
        case 0x004a: return convertLanguageCodeToString("te");
        case 0x044a: return convertLanguageCodeToString("te-IN");

        // Kannada
        case 0x004b: return convertLanguageCodeToString("kn");
        case 0x044b: return convertLanguageCodeToString("kn-IN");

        // Malayalam
        case 0x004c: return convertLanguageCodeToString("ml");
        case 0x044c: return convertLanguageCodeToString("ml-IN");

        // Assamese
        case 0x004d: return convertLanguageCodeToString("as");
        case 0x044d: return convertLanguageCodeToString("as-IN");

        // Marathi
        case 0x004e: return convertLanguageCodeToString("mr");
        case 0x044e: return convertLanguageCodeToString("mr-IN");

        // Sanskrit
        case 0x004f: return convertLanguageCodeToString("sa");
        case 0x044f: return convertLanguageCodeToString("sa-IN");

        // Mongolian
        case 0x0050: return convertLanguageCodeToString("mn");
        case 0x0450: return convertLanguageCodeToString("mn-MN");
        case 0x0850: return convertLanguageCodeToString("mn-Mong-CN");
        case 0x0c50: return convertLanguageCodeToString("mn-Mong-MN");
        case 0x7850: return convertLanguageCodeToString("mn-Cyrl");
        case 0x7c50: return convertLanguageCodeToString("mn-Mong");

        // Tibetan
        case 0x0051: return convertLanguageCodeToString("bo");
        case 0x0451: return convertLanguageCodeToString("bo-CN");
        case 0x0851: return convertLanguageCodeToString("bo-BT");

        // Welsh
        case 0x0052: return convertLanguageCodeToString("cy");
        case 0x0452: return convertLanguageCodeToString("cy-GB");

        // Khmer
        case 0x0053: return convertLanguageCodeToString("km");
        case 0x0453: return convertLanguageCodeToString("km-KH");

        // Lao
        case 0x0054: return convertLanguageCodeToString("lo");
        case 0x0454: return convertLanguageCodeToString("lo-LA");

        // Burmese
        case 0x0055: return convertLanguageCodeToString("my");
        case 0x0455: return convertLanguageCodeToString("my-MM");

        // Galician
        case 0x0056: return convertLanguageCodeToString("gl");
        case 0x0456: return convertLanguageCodeToString("gl-ES");

        // Konkani
        case 0x0057: return convertLanguageCodeToString("kok");
        case 0x0457: return convertLanguageCodeToString("kok-IN");

        // Manipuri
        case 0x0058: return "Manipuri";

        // Sindhi
        case 0x0059: return convertLanguageCodeToString("sd");
        case 0x0859: return convertLanguageCodeToString("sd-Arab-PK");
        case 0x7c59: return convertLanguageCodeToString("sd-Arab");

        // Syriac
        case 0x005a: return convertLanguageCodeToString("syr");
        case 0x045a: return convertLanguageCodeToString("syr-SY");

        // Sinhala
        case 0x005b: return convertLanguageCodeToString("si");
        case 0x045b: return convertLanguageCodeToString("si-LK");

        // Cherokee
        case 0x005c: return convertLanguageCodeToString("chr");
        case 0x045c: return convertLanguageCodeToString("chr-Cher");
        case 0x7c5c: return convertLanguageCodeToString("chr-Cher");

        // Inuktitut
        case 0x005d: return convertLanguageCodeToString("iu");
        case 0x045d: return convertLanguageCodeToString("iu-Latn-CA");
        case 0x085d: return convertLanguageCodeToString("iu-Latn-CA");
        case 0x785d: return convertLanguageCodeToString("iu-Cans");
        case 0x7c5d: return convertLanguageCodeToString("iu-Latn");

        // Amharic
        case 0x005e: return convertLanguageCodeToString("am");
        case 0x045e: return convertLanguageCodeToString("am-ET");

        // Tamazight
        case 0x005f: return convertLanguageCodeToString("tzm");
        case 0x045f: return convertLanguageCodeToString("tzm-Latn-DZ");
        case 0x085f: return convertLanguageCodeToString("tzm-Latn-DZ");
        case 0x105f: return convertLanguageCodeToString("tzm-Tfng-MA");
        case 0x5c0f: return convertLanguageCodeToString("tzm");
        case 0x785f: return convertLanguageCodeToString("tzm-Tfng");
        case 0x7c5f: return convertLanguageCodeToString("tzm-Latn");

        // Kashmiri
        case 0x0060: return convertLanguageCodeToString("ks");
        case 0x0460: return convertLanguageCodeToString("ks-Deva-IN");

        // Nepali
        case 0x0061: return convertLanguageCodeToString("ne");
        case 0x0461: return convertLanguageCodeToString("ne-IN");

        // Frisian
        case 0x0062: return convertLanguageCodeToString("fy");
        case 0x0462: return convertLanguageCodeToString("fy-NL");

        // Pashto
        case 0x0063: return convertLanguageCodeToString("ps");
        case 0x0463: return convertLanguageCodeToString("ps-AF");

        // Filipino
        case 0x0064: return convertLanguageCodeToString("fil");
        case 0x0464: return convertLanguageCodeToString("fil-PH");

        // Divehi
        case 0x0065: return convertLanguageCodeToString("dv");
        case 0x0465: return convertLanguageCodeToString("dv-MV");

        // Central European / pseudo language
        case 0x0066: return "Central European";

        // Fulah
        case 0x0067: return convertLanguageCodeToString("ff");
        case 0x0867: return convertLanguageCodeToString("ff-Latn-SN");
        case 0x7c67: return convertLanguageCodeToString("ff-Latn");

        // Hausa
        case 0x0068: return convertLanguageCodeToString("ha");
        case 0x0468: return convertLanguageCodeToString("ha-Latn-NG");
        case 0x7c68: return convertLanguageCodeToString("ha-Latn");

        // Yoruba
        case 0x006a: return convertLanguageCodeToString("yo");
        case 0x046a: return convertLanguageCodeToString("yo-NG");

        // Quechua
        case 0x006b: return convertLanguageCodeToString("qu");
        case 0x046b: return convertLanguageCodeToString("quz-PE");
        case 0x086b: return convertLanguageCodeToString("quz-EC");
        case 0x0c6b: return convertLanguageCodeToString("quz-PE");

        // Northern Sotho
        case 0x006c: return convertLanguageCodeToString("nso");
        case 0x046c: return convertLanguageCodeToString("nso-ZA");

        // Bashkir
        case 0x006d: return convertLanguageCodeToString("ba");
        case 0x046d: return convertLanguageCodeToString("ba-RU");

        // Luxembourgish
        case 0x006e: return convertLanguageCodeToString("lb");
        case 0x046e: return convertLanguageCodeToString("lb-LU");

        // Greenlandic
        case 0x006f: return convertLanguageCodeToString("kl");
        case 0x046f: return convertLanguageCodeToString("kl-GL");

        // Igbo
        case 0x0070: return convertLanguageCodeToString("ig");
        case 0x0470: return convertLanguageCodeToString("ig-NG");

        // Kanuri
        case 0x0071: return "Kanuri";

        // Oromo
        case 0x0072: return "Oromo";

        // Tigrinya
        case 0x0073: return convertLanguageCodeToString("ti");
        case 0x0473: return convertLanguageCodeToString("ti-ER");

        // Guarani
        case 0x0074: return "Guarani";

        // Hawaiian
        case 0x0075: return convertLanguageCodeToString("haw");
        case 0x0475: return convertLanguageCodeToString("haw-US");

        // Latin
        case 0x0076: return "Latin";

        // Somali
        case 0x0077: return convertLanguageCodeToString("so");
        case 0x0477: return convertLanguageCodeToString("so-SO");

        // Yi
        case 0x0078: return convertLanguageCodeToString("ii");
        case 0x0478: return convertLanguageCodeToString("ii-CN");

        // Papiamento
        case 0x0079: return "Papiamento";

        // Mapudungun
        case 0x007a: return convertLanguageCodeToString("arn");
        case 0x047a: return convertLanguageCodeToString("arn-CL");

        // Mohawk
        case 0x007c: return convertLanguageCodeToString("moh");
        case 0x047c: return convertLanguageCodeToString("moh-CA");

        // Breton
        case 0x007e: return convertLanguageCodeToString("br");
        case 0x047e: return convertLanguageCodeToString("br-FR");

        // Uyghur
        case 0x0080: return convertLanguageCodeToString("ug");
        case 0x0480: return convertLanguageCodeToString("ug-CN");

        // Māori
        case 0x0081: return convertLanguageCodeToString("mi");
        case 0x0481: return convertLanguageCodeToString("mi-NZ");

        // Occitan
        case 0x0082: return convertLanguageCodeToString("oc");
        case 0x0482: return convertLanguageCodeToString("oc-FR");

        // Corsican
        case 0x0083: return convertLanguageCodeToString("co");
        case 0x0483: return convertLanguageCodeToString("co-FR");

        // Alsatian
        case 0x0084: return convertLanguageCodeToString("gsw");
        case 0x0484: return convertLanguageCodeToString("gsw-FR");

        // Sakha / Yakut
        case 0x0085: return convertLanguageCodeToString("sah");
        case 0x0485: return convertLanguageCodeToString("sah-RU");

        // K'iche'
        case 0x0086: return convertLanguageCodeToString("qut");
        case 0x0486: return convertLanguageCodeToString("qut-GT");

        // Kinyarwanda
        case 0x0087: return convertLanguageCodeToString("rw");
        case 0x0487: return convertLanguageCodeToString("rw-RW");

        // Wolof
        case 0x0088: return convertLanguageCodeToString("wo");
        case 0x0488: return convertLanguageCodeToString("wo-SN");

        // Dari
        case 0x008c: return convertLanguageCodeToString("prs");
        case 0x048c: return convertLanguageCodeToString("prs-AF");

        // Scottish Gaelic
        case 0x0091: return convertLanguageCodeToString("gd");
        case 0x0491: return convertLanguageCodeToString("gd-GB");

        // Central Kurdish
        case 0x0092: return convertLanguageCodeToString("ku");
        case 0x0492: return convertLanguageCodeToString("ku-Arab-IQ");
        case 0x7c92: return convertLanguageCodeToString("ku-Arab");

        // reserved / pseudo locales appearing in Microsoft's LANGID tables
        case 0x0501: return "Pseudo-localization";
        case 0x05fe: return "Pseudo-localization (mirrored)";
        case 0x09ff: return "Pseudo-localization (mirror)";
        case 0xf2ee: return "Reserved";
        case 0xeeee: return "Reserved";


        // Sublanguage IDs 0x20 to 0x3f are available for user-defined languages.
        default:
            return QString("Unknown / custom language (LANGID 0x%1)").arg(langId, 4, 16, '0');

    }

    return "";

}

QString PQCFilePluginDLLExe::formatCodePage(quint16 codepage) {

    switch(codepage) {

        case VersionInfo::CodePageUtf16Le:
            return "UTF-16LE (1200)";
        case VersionInfo::CodePageUtf16Be:
                return "UTF-16BE (1201)";
        case VersionInfo::CodePageUtf8:
                return "UTF-8 (65001)";
        case 1250:
        case 1251:
        case 1252:
        case 1253:
        case 1254:
        case 1255:
        case 1256:
        case 1257:
        case 1258:
            return QString("Windows-%1 (%1)").arg(codepage);
        case 932:
            return "Shift-JIS (932)";
        case 936:
            return "GBK (936)";
        case 949:
            return "KS C 5601 (949)";
        case 950:
            return "Big5 (950)";
        default:
            return QString("Code page %1").arg(codepage);

    }

}
