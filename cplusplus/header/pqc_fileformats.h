/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2024 Lukas Spies                                       **
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

#ifndef PQCIMAGEFORMATS_H
#define PQCIMAGEFORMATS_H

#include <QObject>
#include <QVariantMap>
#include <QtSql/QSqlDatabase>

class PQCFileFormats : public QObject {

    Q_OBJECT

public:
    static PQCFileFormats& get() {
        static PQCFileFormats instance;
        return instance;
    }

    PQCFileFormats(PQCFileFormats const&)     = delete;
    void operator=(PQCFileFormats const&) = delete;

    Q_INVOKABLE void readDatabase() {
        readFromDatabase();
    }

    Q_INVOKABLE QStringList getAllFormats() {
        return m_formats;
    }

    Q_INVOKABLE int getNumFormats() {
        return m_num;
    }

    Q_INVOKABLE QStringList getAllMimeTypes() {
        return m_mimetypes;
    }

    Q_INVOKABLE int getNumFormatsQt() {
        return m_num_qt;
    }

    Q_INVOKABLE QStringList getAllFormatsQt() {
        return m_formats_qt;
    }

    Q_INVOKABLE QStringList getAllMimeTypesQt() {
        return m_mimetypes_qt;
    }

    Q_INVOKABLE int getNumFormatsResvg() {
        return m_num_resvg;
    }

    Q_INVOKABLE QStringList getAllFormatsResvg() {
        return m_formats_resvg;
    }

    Q_INVOKABLE QStringList getAllMimeTypesResvg() {
        return m_mimetypes_resvg;
    }

    Q_INVOKABLE int getNumFormatsLibVips() {
        return m_num_libvips;
    }

    Q_INVOKABLE QStringList getAllFormatsLibVips() {
        return m_formats_libvips;
    }

    Q_INVOKABLE QStringList getAllMimeTypesLibVips() {
        return m_mimetypes_libvips;
    }

    Q_INVOKABLE int getNumFormatsMagick() {
        return m_num_magick;
    }

    Q_INVOKABLE QStringList getAllFormatsMagick() {
        return m_formats_magick;
    }

    Q_INVOKABLE QStringList getAllMimeTypesMagick() {
        return m_mimetypes_magick;
    }

    Q_INVOKABLE int getNumFormatsLibRaw() {
        return m_num_libraw;
    }

    Q_INVOKABLE QStringList getAllFormatsLibRaw() {
        return m_formats_libraw;
    }

    Q_INVOKABLE QStringList getAllMimeTypesLibRaw() {
        return m_mimetypes_libraw;
    }

    Q_INVOKABLE int getNumFormatsPoppler() {
        return m_num_poppler;
    }

    Q_INVOKABLE QStringList getAllFormatsPoppler() {
        return m_formats_poppler;
    }

    Q_INVOKABLE QStringList getAllMimeTypesPoppler() {
        return m_mimetypes_poppler;
    }

    Q_INVOKABLE int getNumFormatsXCFTools() {
        return m_num_xcftools;
    }

    Q_INVOKABLE QStringList getAllFormatsXCFTools() {
        return m_formats_xcftools;
    }

    Q_INVOKABLE QStringList getAllMimeTypesXCFTools() {
        return m_mimetypes_xcftools;
    }

    Q_INVOKABLE int getNumFormatsDevIL() {
        return m_num_devil;
    }

    Q_INVOKABLE QStringList getAllFormatsDevIL() {
        return m_formats_devil;
    }

    Q_INVOKABLE QStringList getAllMimeTypesDevIL() {
        return m_mimetypes_devil;
    }

    Q_INVOKABLE int getNumFormatsFreeImage() {
        return m_num_freeimage;
    }

    Q_INVOKABLE QStringList getAllFormatsFreeImage() {
        return m_formats_freeimage;
    }

    Q_INVOKABLE QStringList getAllMimeTypesFreeImage() {
        return m_mimetypes_freeimage;
    }

    Q_INVOKABLE int getNumFormatsLibArchive() {
        return m_num_archive;
    }

    Q_INVOKABLE QStringList getAllFormatsLibArchive() {
        return m_formats_archive;
    }

    Q_INVOKABLE QStringList getAllMimeTypesLibArchive() {
        return m_mimetypes_archive;
    }

    Q_INVOKABLE int getNumFormatsVideo() {
        return m_num_video;
    }

    Q_INVOKABLE QStringList getAllFormatsVideo() {
        return m_formats_video;
    }

    Q_INVOKABLE QStringList getAllMimeTypesVideo() {
        return m_mimetypes_video;
    }

    Q_INVOKABLE int getNumFormatsLibmpv() {
        return m_num_libmpv;
    }

    Q_INVOKABLE QStringList getAllFormatsLibmpv() {
        return m_formats_libmpv;
    }

    Q_INVOKABLE QStringList getAllMimeTypesLibmpv() {
        return m_mimetypes_libmpv;
    }

    Q_INVOKABLE int getNumFormatsEBook() {
        return m_num_ebook;
    }

    Q_INVOKABLE QStringList getAllFormatsEBook() {
        return m_formats_ebook;
    }

    Q_INVOKABLE QStringList getAllMimeTypesEBook() {
        return m_mimetypes_ebook;
    }

    Q_INVOKABLE int getNumFormatsText() {
        return m_num_text;
    }

    Q_INVOKABLE QStringList getAllFormatsText() {
        return m_formats_text;
    }

    Q_INVOKABLE QStringList getAllMimeTypesText() {
        return m_mimetypes_text;
    }

    Q_INVOKABLE QVariantMap getMagick() {
        return magick;
    }

    Q_INVOKABLE QVariantMap getMagickMimeType() {
        return magick_mimetype;
    }

    void closeDatabase();

    void validate();

private:
    PQCFileFormats();

    void readFromDatabase();

    QSqlDatabase db;

    int m_num;
    QStringList m_formats;
    QStringList m_mimetypes;

    int m_num_qt;
    QStringList m_formats_qt;
    QStringList m_mimetypes_qt;
    int m_num_resvg;
    QStringList m_formats_resvg;
    QStringList m_mimetypes_resvg;
    int m_num_libvips;
    QStringList m_formats_libvips;
    QStringList m_mimetypes_libvips;
    int m_num_magick;
    QStringList m_formats_magick;
    QStringList m_mimetypes_magick;
    int m_num_libraw;
    QStringList m_formats_libraw;
    QStringList m_mimetypes_libraw;
    int m_num_poppler;
    QStringList m_formats_poppler;
    QStringList m_mimetypes_poppler;
    int m_num_xcftools;
    QStringList m_formats_xcftools;
    QStringList m_mimetypes_xcftools;
    int m_num_devil;
    QStringList m_formats_devil;
    QStringList m_mimetypes_devil;
    int m_num_freeimage;
    QStringList m_formats_freeimage;
    QStringList m_mimetypes_freeimage;
    int m_num_archive;
    QStringList m_formats_archive;
    QStringList m_mimetypes_archive;
    int m_num_video;
    QStringList m_formats_video;
    QStringList m_mimetypes_video;
    int m_num_libmpv;
    QStringList m_formats_libmpv;
    QStringList m_mimetypes_libmpv;
    int m_num_ebook;
    QStringList m_formats_ebook;
    QStringList m_mimetypes_ebook;
    int m_num_text;
    QStringList m_formats_text;
    QStringList m_mimetypes_text;

    QVariantMap magick;
    QVariantMap magick_mimetype;

};


#endif // PQCIMAGEFORMATS_H
