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

#include <pqc_scriptsother.h>
#include <QObject>
#include <QQmlEngine>

class QTranslator;

class PQCScriptsOtherQML : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(PQCScriptsOther)

public:
    explicit PQCScriptsOtherQML() {
        connect(&PQCScriptsOther::get(), &PQCScriptsOther::commandLineArgumentReceived,
                this, &PQCScriptsOtherQML::commandLineArgumentReceived);
        connect(&PQCScriptsOther::get(), &PQCScriptsOther::downloadStarted,
                this, &PQCScriptsOtherQML::downloadStarted);
        connect(&PQCScriptsOther::get(), &PQCScriptsOther::downloadFinished,
                this, &PQCScriptsOtherQML::downloadFinished);
        connect(&PQCScriptsOther::get(), &PQCScriptsOther::downloadProgress,
                this, &PQCScriptsOtherQML::downloadProgress);
        connect(&PQCScriptsOther::get(), &PQCScriptsOther::downloadCancelled,
                this, &PQCScriptsOtherQML::downloadCancelled);
    };
    ~PQCScriptsOtherQML() {}

    Q_INVOKABLE QString keycodeToString(Qt::KeyboardModifiers modifiers, Qt::Key keycode) {
        return PQCScriptsOther::get().keycodeToString(modifiers, keycode);
    }

    Q_INVOKABLE QSize fitSizeInsideSize(int w, int h, int maxw, int maxh) {
        return PQCScriptsOther::get().fitSizeInsideSize(w, h, maxw, maxh);
    }

    Q_INVOKABLE QString getClipboardContents() {
        return PQCScriptsOther::get().getClipboardContents();
    }

    Q_INVOKABLE QString getStartupMessage() {
        return PQCScriptsOther::get().getStartupMessage();
    }

    Q_INVOKABLE void startDownloadOfFile(QString url) {
        PQCScriptsOther::get().startDownloadOfFile(url);
    }

    Q_INVOKABLE void cancelDownloadOfFile() {
        PQCScriptsOther::get().cancelDownloadOfFile();
    }

Q_SIGNALS:
    // picked up from PQCScriptsImages and passed on to QML
    void commandLineArgumentReceived(QString msg);
    void downloadStarted();
    void downloadFinished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadCancelled();

};
