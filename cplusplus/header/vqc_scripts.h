/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of ViewQt.                                         **
 **                                                                      **
 ** ViewQt is free software: you can redistribute it and/or modify       **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** ViewQt is distributed in the hope that it will be useful,            **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with ViewQt. If not, see <http://www.gnu.org/licenses/>.       **
 **                                                                      **
 **************************************************************************/

#ifndef VQCSCRIPTS_H
#define VQCSCRIPTS_H

#include <QObject>
#include <QTranslator>
#include <QQmlEngine>

class VQCScripts : public QObject {

    Q_OBJECT

public:
    static VQCScripts& get() {
        static VQCScripts instance;
        return instance;
    }
    ~VQCScripts();

    VQCScripts(VQCScripts const&)     = delete;
    void operator=(VQCScripts const&) = delete;

    Q_INVOKABLE static QString cleanPath(QString path);
    Q_INVOKABLE static QString cleanPath_windows(QString path);
    Q_INVOKABLE bool fileExists(QString path);
    Q_INVOKABLE QString getFilename(QString path);
    Q_INVOKABLE QString openFile();

    Q_INVOKABLE QStringList listArchiveContent(QString path, bool insideFilenameOnly = false);

private:
    VQCScripts();

};

#endif
