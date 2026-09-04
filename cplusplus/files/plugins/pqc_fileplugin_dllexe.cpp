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
    data["filesize"] = info.size();
    data["timeCreated"] = info.birthTime().toString();
    data["timeModified"] = info.lastModified().toString();

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to read file";
        return {true, data};
    }

    const QByteArray dllContent = file.readAll();
    if(dllContent.size() < 64)
        return {true, data};

    data["SHA256"] = QCryptographicHash::hash(dllContent, QCryptographicHash::Sha256).toHex();
    data["peHeaders"] = parsePEHeaders(path);
    data["versionInfo"] = parseWindowsMetadata(path);

    return {true, data};

}

QVariantMap PQCFilePluginDLLExe::parsePEHeaders(QString path) {

    // we prepare the map to make sure we always have all entries
    // for simpler parsing later-on
    QVariantMap ret = {
        {"architecture", ""},
        {"timestamp", 0},
        {"imageBase", ""},
        {"entryPoint", ""},
        {"sections", QStringList()}
    };

    // access file
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "unable to open DLL";
        return ret;
    }

    // read DOS Header
    quint16 mzSignature = 0;
    readUInt16LE(file, mzSignature);
    if(mzSignature != 0x5A4D) { // "MZ"
        qDebug() << "no a valid PE/DLL file";
        ret["architecture"] = "Not a valid PE/DLL file";
        return ret;
    }

    /****************************************************/

    // seek to e_lfanew to find PE signature
    file.seek(0x3C);
    quint32 eLfanew = 0;
    readUInt32LE(file, eLfanew);

    file.seek(eLfanew);
    quint32 peSignature = 0;
    readUInt32LE(file, peSignature);
    if(peSignature != 0x00004550) { // "PE\0\0"
        qDebug() << "wrong PE signature";
        return ret;
    }

    /****************************************************/

    // read COFF File Header
    uint16_t machine = 0;
    uint16_t numberOfSections = 0;
    uint32_t timeDateStamp = 0;
    readUInt16LE(file, machine);
    readUInt16LE(file, numberOfSections);
    readUInt32LE(file, timeDateStamp);

    ret["architecture"] = (machine == 0x8664 ?
                               "x86_64 (64-bit)" :
                               (machine == 0x014C ?
                                    "x86 (32-bit)" :
                                    QString("Unknown (Machine: %1)").arg(machine)));

    ret["timestamp"] = QDateTime::fromSecsSinceEpoch(timeDateStamp).toString();

    /****************************************************/

    // skip to Optional Header to grab image base & entry point
    // COFF header size is 20 bytes
    // optional header follows.
    uint16_t sizeOptionalHeader = 0;
    file.seek(eLfanew + 4 + 16); // PE sig (4) + COFF header fields before size_of_opt
    readUInt16LE(file, sizeOptionalHeader);

    // optional Header starts immediately after the 20-byte COFF header.
    const qint64 optionalHeaderOffset = eLfanew + 4 + 20;
    file.seek(optionalHeaderOffset);

    uint16_t optMagic = 0;
    readUInt16LE(file, optMagic);

    uint32_t entryPoint = 0;
    quint64 imageBase = 0;

    if(optMagic == 0x10b) {

        // PE32
        //
        // optional header structure:
        //
        //   +00 Magic
        //   +04 MajorLinkerVersion
        //   +05 MinorLinkerVersion
        //   +08 SizeOfCode
        //   ...
        //   +16 AddressOfEntryPoint
        //   +20 BaseOfCode
        //   +24 BaseOfData
        //   +28 ImageBase (uint32)

        file.seek(optionalHeaderOffset + 16);
        readUInt32LE(file, entryPoint);

        uint32_t imageBase32 = 0;
        file.seek(optionalHeaderOffset + 28);
        readUInt32LE(file, imageBase32);

        imageBase = imageBase32;

    } else if(optMagic == 0x20b) {

        // PE32+
        //
        // optional header structure:
        //
        //   +00 Magic
        //   +04 MajorLinkerVersion
        //   ...
        //   +16 AddressOfEntryPoint
        //   +20 BaseOfCode
        //   +24 ImageBase (uint64)

        file.seek(optionalHeaderOffset + 16);
        readUInt32LE(file, entryPoint);

        file.seek(optionalHeaderOffset + 24);
        readUInt64LE(file, imageBase);

    }

    ret["imageBase"] = QStringLiteral("0x%1").arg(imageBase, 0, 16);
    ret["entryPoint"] = QStringLiteral("0x%1").arg(entryPoint, 0, 16);

    /****************************************************/

    // read sections
    file.seek(eLfanew + 24 + sizeOptionalHeader);

    QVariantList sections = ret["sections"].toList();
    for (int i = 0; i < numberOfSections; ++i) {
        QByteArray sectionName = file.read(8);
        const qsizetype nullPos = sectionName.indexOf('\0');
        if (nullPos >= 0)
            sectionName.truncate(nullPos);
        sections.append(QString::fromLatin1(sectionName).trimmed());
        // section header is 40 bytes - we've already read 8
        file.seek(file.pos() + 32);
    }
    ret["sections"] = sections;

    return ret;

}

QVariantMap parseWindowsMetadata(const QString path) {

    QVariantMap ret;

    // TODO!!

    return ret;

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
    return QStringLiteral("%1.%2.%3.%4").arg(ms >> 16)
                                        .arg(ms & 0xffff)
                                        .arg(ls >> 16)
                                        .arg(ls & 0xffff);
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


