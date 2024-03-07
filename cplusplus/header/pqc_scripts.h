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

#ifndef PQCSCRIPTS_H
#define PQCSCRIPTS_H

#include <QObject>
#include <QTranslator>
#include <QQmlEngine>

class PQCScripts : public QObject {

    Q_OBJECT

public:
    static PQCScripts& get() {
        static PQCScripts instance;
        return instance;
    }
    ~PQCScripts();

    PQCScripts(PQCScripts const&)     = delete;
    void operator=(PQCScripts const&) = delete;

    Q_INVOKABLE static QString cleanPath(QString path);
    Q_INVOKABLE static QString cleanPath_windows(QString path);
    Q_INVOKABLE bool fileExists(QString path);
    Q_INVOKABLE QString getBasename(QString fullpath);
    Q_INVOKABLE QString getFilename(QString path);
    Q_INVOKABLE QString getDir(QString fullpath);

    Q_INVOKABLE int isMotionPhoto(QString path);
    Q_INVOKABLE QString extractMotionPhoto(QString path);
    Q_INVOKABLE int getExifOrientation(QString path);

    Q_INVOKABLE QStringList listArchiveContent(QString path, bool insideFilenameOnly = false);

private:
    PQCScripts();

};

#endif
