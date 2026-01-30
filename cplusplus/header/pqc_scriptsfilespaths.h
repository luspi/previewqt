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
#pragma once

#include <QObject>

class QTranslator;

class PQCScriptsFilesPaths : public QObject {

    Q_OBJECT

public:
    static PQCScriptsFilesPaths& get() {
        static PQCScriptsFilesPaths instance;
        return instance;
    }
    ~PQCScriptsFilesPaths();

    PQCScriptsFilesPaths(PQCScriptsFilesPaths const&) = delete;
    void operator=(PQCScriptsFilesPaths const&) = delete;

    static QString cleanPath(QString path);
    static QString cleanPath_windows(QString path);
    QString getBasename(QString fullpath);
    QString getFilename(QString path);
    QString getDir(QString fullpath);
    QString getSuffix(QString path);
    QString toPercentEncoding(QString str);
    QString fromPercentEncoding(QByteArray str);
    bool isFileSupported(QString path);
    QString toAbsolutePath(QString path);
    QString openNewFile();
    void deleteTemporaryFiles();
    bool openInDefault(QString path);
    void copyTextToClipboard(QString txt);
    QString saveImageToTempFile(QImage &img);

private:
    PQCScriptsFilesPaths();

};
