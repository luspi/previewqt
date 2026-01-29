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
#include <QNetworkAccessManager>

class QTranslator;
class QNetworkReply;
class QFile;

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
    QString getClipboardContents();
    QString getStartupMessage(){ return m_startupMessage; }
    void setStartupMessage(QString val) { m_startupMessage = val; }
    void startDownloadOfFile(QString url);
    void cancelDownloadOfFile();

private:
    PQCScriptsOther();

    QString m_startupMessage;
    QNetworkReply *m_downloadReply;
    QNetworkAccessManager m_downloadManager;
    QFile *downloadFile;

Q_SIGNALS:
    void commandLineArgumentReceived(QString msg);

    void downloadStarted();
    void downloadFinished();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadCancelled();

};
