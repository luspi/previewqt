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
#include <QImage>

class QTranslator;

class PQCScriptsOther : public QObject {

    Q_OBJECT

public:
    static PQCScriptsOther& get() {
        static PQCScriptsOther instance;
        return instance;
    }
    ~PQCScriptsOther();

    PQCScriptsOther(PQCScriptsOther const&) = delete;
    void operator=(PQCScriptsOther const&) = delete;

    QString keycodeToString(Qt::KeyboardModifiers modifiers, Qt::Key keycode);
    QSize fitSizeInsideSize(int w, int h, int maxw, int maxh);
    int toLcmsFormat(QImage::Format fmt);
    bool applyEmbeddedColorProfile(QImage &img);
    QString getStartupMessage(){ return m_startupMessage; }
    void setStartupMessage(QString val) { m_startupMessage = val; }

private:
    PQCScriptsOther();

    QString m_startupMessage;

};
