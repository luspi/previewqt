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

#include <pqc_scriptsexternaltools.h>
#include <QObject>
#include <QQmlEngine>

class PQCScriptsExternalToolsQML : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(PQCScriptsExternalTools)

public:
    explicit PQCScriptsExternalToolsQML() {
        connect(&PQCScriptsExternalTools::get(), &PQCScriptsExternalTools::ytdlpReceivedStreamSupported,
                this, &PQCScriptsExternalToolsQML::ytdlpReceivedStreamSupported);
        connect(&PQCScriptsExternalTools::get(), &PQCScriptsExternalTools::ytdlpReceivedStreamURL,
                this, &PQCScriptsExternalToolsQML::ytdlpReceivedStreamURL);
        connect(&PQCScriptsExternalTools::get(), &PQCScriptsExternalTools::ytdlpReceivedStreamTitle,
                this, &PQCScriptsExternalToolsQML::ytdlpReceivedStreamTitle);
        connect(&PQCScriptsExternalTools::get(), &PQCScriptsExternalTools::ytdlpReceivedStreamError,
                this, &PQCScriptsExternalToolsQML::ytdlpReceivedStreamError);
    };
    ~PQCScriptsExternalToolsQML() {}

    Q_INVOKABLE void ytdlpRequestIsSupportedStream(QString url) {
        PQCScriptsExternalTools::get().ytdlpRequestIsSupportedStream(url);
    }

    Q_INVOKABLE void ytdlpRequestStreamURL(QString url) {
        PQCScriptsExternalTools::get().ytdlpRequestStreamURL(url);
    }

    Q_INVOKABLE void ytdlpRequestStreamTitle(QString url) {
        PQCScriptsExternalTools::get().ytdlpRequestStreamTitle(url);
    }


Q_SIGNALS:
    void ytdlpReceivedStreamSupported(bool supp);
    void ytdlpReceivedStreamURL(QString url);
    void ytdlpReceivedStreamTitle(QString title);
    void ytdlpReceivedStreamError(QString err);

};
