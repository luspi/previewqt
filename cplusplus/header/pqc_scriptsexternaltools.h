/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
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

class QProcess;

class PQCScriptsExternalTools : public QObject {

    Q_OBJECT

public:
    static PQCScriptsExternalTools& get() {
        static PQCScriptsExternalTools instance;
        return instance;
    }
    ~PQCScriptsExternalTools();

    PQCScriptsExternalTools(PQCScriptsExternalTools const&) = delete;
    void operator=(PQCScriptsExternalTools const&) = delete;

    void ytdlpRequestIsSupportedStream(QString url);
    void ytdlpRequestStreamURL(QString url);
    void ytdlpRequestStreamTitle(QString url);


private:
    PQCScriptsExternalTools();

    QProcess *m_ytdlpStreamSupportedProc;
    QProcess *m_ytdlpStreamProc;
    QProcess *m_ytdlpStreamTitleProc;

Q_SIGNALS:
    void ytdlpReceivedStreamSupported(bool supp);
    void ytdlpReceivedStreamURL(QString url);
    void ytdlpReceivedStreamTitle(QString title);
    void ytdlpReceivedStreamError(QString err);
    void ytdlpFinished();

};
