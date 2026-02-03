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
#include <pqc_scriptsexternaltools.h>
#include <pqc_settingscpp.h>

#include <QProcess>
#include <QtDebug>

PQCScriptsExternalTools::PQCScriptsExternalTools() {
    m_ytdlpStreamProc = nullptr;
    m_ytdlpStreamTitleProc = nullptr;
    m_ytdlpStreamSupportedProc = nullptr;
}

PQCScriptsExternalTools::~PQCScriptsExternalTools() {
    if(m_ytdlpStreamProc) delete m_ytdlpStreamProc;
    if(m_ytdlpStreamTitleProc) delete m_ytdlpStreamTitleProc;
    if(m_ytdlpStreamSupportedProc) delete m_ytdlpStreamSupportedProc;
}

void PQCScriptsExternalTools::ytdlpRequestIsSupportedStream(QString url) {

    qDebug() << "args: url =" << url;

    QProcess which;
    which.setStandardOutputFile(QProcess::nullDevice());
    which.start("which", QStringList() << PQCSettingsCPP::get().getExecutableYtDlp());
    which.waitForFinished();

    if(which.exitCode()) {
        qWarning() << "yt-dlp executable not found. Is it set up correctly?";
        Q_EMIT ytdlpReceivedStreamSupported(false);
        return;
    }

    const QStringList knownMatches = {"://youtube.com", "://www.youtube.com","://youtu.be", "://www.youtu.be",
                                      "://dailymotion.com", "://www.dailymotion.com"};
    for(const QString &m : knownMatches) {
        if(url.contains(m)) {
            Q_EMIT ytdlpReceivedStreamSupported(true);
            return;
        }
    }

    if(m_ytdlpStreamSupportedProc != nullptr)
        delete m_ytdlpStreamSupportedProc;
    m_ytdlpStreamSupportedProc = new QProcess;

    url = url.split("&list=")[0];
    url = url.split("&index=")[0];

    QString program = "yt-dlp";
    QStringList arguments = {"--simulate", url};

    m_ytdlpStreamSupportedProc->start(program, arguments);

    connect(m_ytdlpStreamSupportedProc, &QProcess::readyReadStandardOutput, this, [=]() {
        const QString ret = QString::fromLocal8Bit(m_ytdlpStreamSupportedProc->readAll().trimmed());
        if(ret.contains("Downloading") && !ret.contains("Downloading webpage"))
            Q_EMIT ytdlpReceivedStreamSupported(true);
    });
    connect(m_ytdlpStreamSupportedProc, &QProcess::readyReadStandardError, this, [=]() {
        const QString err = QString::fromLocal8Bit(m_ytdlpStreamSupportedProc->readAllStandardError().trimmed());
        if(err.contains("Unsupported URL"))
            Q_EMIT ytdlpReceivedStreamSupported(false);
    });

}

void PQCScriptsExternalTools::ytdlpRequestStreamURL(QString url) {

    qDebug() << "args: url =" << url;

    if(m_ytdlpStreamProc != nullptr)
        delete m_ytdlpStreamProc;
    m_ytdlpStreamProc = new QProcess;

    url = url.split("&list=")[0];
    url = url.split("&index=")[0];

    QString program = "yt-dlp";
    QStringList arguments;

    arguments << "-g";
    arguments << "-f" << "best[protocol^=http]/best";
    arguments << "--user-agent" << "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";

    if(url.contains("youtube.com") || url.contains("youtu.be"))
        arguments << "--referer" << "https://www.youtube.com/";
    else if(url.contains("dailymotion.com"))
        arguments << "--referer" << "https://www.dailymotion.com/";

    arguments << url;


    m_ytdlpStreamProc->start(program, arguments);

    connect(m_ytdlpStreamProc, &QProcess::readyReadStandardOutput, this, [=]() {
        const QString ret = m_ytdlpStreamProc->readAll().trimmed();
        if(ret == "")
            Q_EMIT ytdlpReceivedStreamError("no_stream_found");
        else
            Q_EMIT ytdlpReceivedStreamURL(ret);
    });

    connect(m_ytdlpStreamProc, &QProcess::readyReadStandardError, this, [=]() {
        const QString err = m_ytdlpStreamProc->readAllStandardError().trimmed();
        if(err.contains("Sign in to confirm you’re not a bot"))
            Q_EMIT ytdlpReceivedStreamError("signin_bot");
        else if(err.contains("HTTP Error 403: Forbidden") || err.contains("Failed to download") || err.contains("No video formats found!"))
            Q_EMIT ytdlpReceivedStreamError("plugin_error");
    });

    connect(m_ytdlpStreamProc, &QProcess::finished, this, [=]() { Q_EMIT ytdlpFinished(); });

}

void PQCScriptsExternalTools::ytdlpRequestStreamTitle(QString url) {

    qDebug() << "args: url =" << url;

    if(m_ytdlpStreamTitleProc != nullptr)
        delete m_ytdlpStreamTitleProc;
    m_ytdlpStreamTitleProc = new QProcess;

    url = url.split("&list=")[0];
    url = url.split("&index=")[0];

    QString program = "yt-dlp";
    QStringList arguments = {"--simulate",
                             "--print", "%(title)s",
                             url};

    m_ytdlpStreamTitleProc->start(program, arguments);

    connect(m_ytdlpStreamTitleProc, &QProcess::readyReadStandardOutput, this, [=]() {
        const QString ret = m_ytdlpStreamTitleProc->readAll().trimmed();
        qDebug() << "Received standard output:";
        qDebug() << ret;
        Q_EMIT ytdlpReceivedStreamTitle(ret);
    });

}
