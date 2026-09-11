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
#include <QMap>
#include <QImage>
#include <QTranslator>

class QProcess;
class QMediaPlayer;

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

    bool isArchive(QString path, bool insideArchive = false);
    bool isComicBook(QString path);
    bool isEpub(QString path);
    bool isTextDocument(QString path);
    bool isMpvVideo(QString path);
    bool isQtVideo(QString path);
    bool isPDFDocument(QString path);
    bool isOfficeDocument(QString path);
    bool isSVG(QString path);
    bool isPhotoSphere(QString path);
    int isMotionPhoto(QString path);
    bool isItAnimated(QString filename);
    bool isURL(QString url);
    bool isLocalURL(QString url);
    bool isAudio(QString path);
    bool isPackage(QString path);
    bool isMuseScore(QString path);
    bool isPQT(QString path);
    bool isSQLite(QString path);

    QString findCoverImageNextToFile(QString path);

    QString extractMotionPhoto(QString path);
    int getExifOrientation(QString path);
    void applyExifOrientation(const QString filename, QImage &img);
    QString getTextFileContents(QString path);
    QString prepareSphereFile(QString path);
    QVariantList getFaceTags(QString filename);

    QVariantList loadEPUB(QString path);
    void analyzeEpubMetaData(QString subfolder, QString txt, QString &title, QString &coverId, QMap<QString, QString> &outFileList, QStringList &outIdOrder);

    // other methods
    void setMaxTextureLimit(int lmt) { m_maxTextureLimit = lmt; }
    int getMaxTextureLimit() { return m_maxTextureLimit; }

    void addFileLoadError(QString err) { m_fileLoadError += err; }
    void setFileLoadError(QString err) { m_fileLoadError = err; }
    QString getFileLoadError() { return m_fileLoadError; }

private:
    PQCScriptsImages();

    int m_maxTextureLimit;

    QString m_fileLoadError;

};
