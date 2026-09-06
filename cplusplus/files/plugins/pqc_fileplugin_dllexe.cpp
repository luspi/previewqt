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

            imageBase = imageBase32;

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

                ret["FileFlagsMask"] = fileFlagsMask;
                ret["FileFlags"] = fileFlags;
                ret["FileOS"] = fileOS;
                ret["FileType"] = fileType;
                ret["FileSubtype"] = fileSubtype;
                ret["FileDateMS"] = fileDateMS;
                ret["FileDateLS"] = fileDateLS;

                ret["StructureVersion"] = structureVersion;
                ret["VersionLanguage"] = languageId;
                ret["VersionCodePage"] = codePage;

                const quint64 fileDate = (quint64(fileDateMS) << 32) | quint64(fileDateLS);

                ret["FileDate"] = fileDate;
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


QString PQCFilePluginDLLExe::formatVersion(quint32 ms, quint32 ls) {
    return QStringLiteral("%1.%2.%3.%4").arg(ms >> 16).arg(ms & 0xffff).arg(ls >> 16).arg(ls & 0xffff);
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

