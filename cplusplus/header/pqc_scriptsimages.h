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
#include <QMap>
#include <QImage>
#include <QTranslator>

class QProcess;

class PQCScriptsImages : public QObject {

    Q_OBJECT

public:
    static PQCScriptsImages& get() {
        static PQCScriptsImages instance;
        return instance;
    }
    ~PQCScriptsImages();

    PQCScriptsImages(PQCScriptsImages const&)     = delete;
    void operator=(PQCScriptsImages const&) = delete;

    bool isArchive(QString path);
    bool isComicBook(QString path);
    bool isEpub(QString path);
    bool isTextDocument(QString path);
    bool isMpvVideo(QString path);
    bool isQtVideo(QString path);
    bool isPDFDocument(QString path);
    bool isSVG(QString path);
    bool isPhotoSphere(QString path);
    int isMotionPhoto(QString path);
    bool isItAnimated(QString filename);
    bool isURL(QString url);

    void requestIsSupportedStream(QString url);
    void requestStreamURL(QString url);
    void requestStreamTitle(QString url);

    int getDocumentPageCount(QString path);
    QString extractMotionPhoto(QString path);
    int getExifOrientation(QString path);
    QString getTextFileContents(QString path);

    QStringList getArchiveContent(QString path, bool insideFilenameOnly = false);

    QVariantList loadEPUB(QString path);
    void analyzeEpubMetaData(QString subfolder, QString txt, QString &title, QString &coverId, QMap<QString, QString> &outFileList, QStringList &outIdOrder);

private:
    PQCScriptsImages();

    QProcess *m_streamSupportedProc;
    QProcess *m_streamProc;
    QProcess *m_streamTitleProc;

    QMap<QString,QStringList> archiveContents;
    QString generateArchiveId(QString path);

Q_SIGNALS:
    void receivedStreamSupported(bool supp);
    void receivedStreamURL(QString url);
    void receivedStreamTitle(QString title);
    void receivedStreamError(QString err);

};
