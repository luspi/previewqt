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

#ifndef PQCCACHE_H
#define PQCCACHE_H

#include <QObject>
#include <QVariant>

class QSettings;
class QTimer;

class PQCCache : public QObject {

    Q_OBJECT

public:
    static PQCCache& get() {
        static PQCCache instance;
        return instance;
    }
    ~PQCCache();

    PQCCache(PQCCache const&)     = delete;
    void operator=(PQCCache const&) = delete;

    Q_INVOKABLE void setEntry(QString path, QString value);
    Q_INVOKABLE QString getEntry(QString path);

private:
    PQCCache();

    QSettings *cache;

};

#endif // PQCCACHE_H
