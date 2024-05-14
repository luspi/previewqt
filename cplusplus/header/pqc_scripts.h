/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2024 Lukas Spies                                       **
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

#ifndef PQCSCRIPTS_H
#define PQCSCRIPTS_H

#include <QObject>
#include <QMap>
#include <QImage>

class QProcess;

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
    Q_INVOKABLE bool isQtAtLeast6_5();
    Q_INVOKABLE bool isMpvVideo(QString path);
    Q_INVOKABLE bool isQtVideo(QString path);
    Q_INVOKABLE bool isItAnimated(QString filename);
    Q_INVOKABLE bool isPDFDocument(QString path);
    Q_INVOKABLE bool isSVG(QString path);
    Q_INVOKABLE int getDocumentPageCount(QString path);
    Q_INVOKABLE QStringList getArchiveContent(QString path);
    Q_INVOKABLE bool isArchive(QString path);
    Q_INVOKABLE bool isComicBook(QString path);
    Q_INVOKABLE bool isEpub(QString path);
    Q_INVOKABLE QString toPercentEncoding(QString str);
    Q_INVOKABLE QString fromPercentEncoding(QByteArray str);
    Q_INVOKABLE QString keycodeToString(Qt::KeyboardModifiers modifiers, Qt::Key keycode);
    Q_INVOKABLE bool doesFileExist(QString path);
    Q_INVOKABLE bool isFileSupported(QString path);
    QString toAbsolutePath(QString path);
    Q_INVOKABLE QSize fitSizeInsideSize(int w, int h, int maxw, int maxh);

    Q_INVOKABLE bool isPhotoSphere(QString path);
    Q_INVOKABLE int isMotionPhoto(QString path);
    Q_INVOKABLE QString extractMotionPhoto(QString path);
    Q_INVOKABLE int getExifOrientation(QString path);

    Q_INVOKABLE QStringList listArchiveContent(QString path, bool insideFilenameOnly = false);

    Q_INVOKABLE bool openInDefault(QString path);

    Q_INVOKABLE bool amIOnWindows();

    Q_INVOKABLE void deleteTemporaryFiles();
    Q_INVOKABLE QString getVersion();

    Q_INVOKABLE QString getConfigInfo(bool formatHTML = true);
    Q_INVOKABLE void copyTextToClipboard(QString txt);

    Q_INVOKABLE QString getOnlyWriteToTempFile(){ return m_onlyWriteToTempFile; }
    void setOnlyWriteToTempFile(QString val) { m_onlyWriteToTempFile = val; }

    Q_INVOKABLE QString getStartupMessage(){ return m_startupMessage; }
    void setStartupMessage(QString val) { m_startupMessage = val; }

    int toLcmsFormat(QImage::Format fmt);
    bool applyEmbeddedColorProfile(QImage &img);

    bool isUpgrade();

    Q_INVOKABLE QVariantList loadEPUB(QString path);
    Q_INVOKABLE QString getTextFromFile(QString path);

private:
    PQCScripts();

    QMap<QString,QStringList> archiveContents;
    QString generateArchiveId(QString path);

    QString m_onlyWriteToTempFile;
    QString m_startupMessage;

signals:
    void commandLineArgumentReceived(QString msg);

};

#endif
