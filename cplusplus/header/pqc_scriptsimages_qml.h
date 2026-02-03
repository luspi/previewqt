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

#include <pqc_scriptsimages.h>
#include <QObject>
#include <QMap>
#include <QImage>
#include <QTranslator>
#include <QQmlEngine>

class QProcess;

class PQCScriptsImagesQML : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(PQCScriptsImages)

public:
    PQCScriptsImagesQML() {
    }
    ~PQCScriptsImagesQML() {}

    Q_INVOKABLE bool isArchive(QString path) {
        return PQCScriptsImages::get().isArchive(path);
    }

    Q_INVOKABLE bool isComicBook(QString path) {
        return PQCScriptsImages::get().isComicBook(path);
    }

    Q_INVOKABLE bool isEpub(QString path) {
        return PQCScriptsImages::get().isEpub(path);
    }

    Q_INVOKABLE bool isTextDocument(QString path) {
        return PQCScriptsImages::get().isTextDocument(path);
    }

    Q_INVOKABLE bool isMpvVideo(QString path) {
        return PQCScriptsImages::get().isMpvVideo(path);
    }

    Q_INVOKABLE bool isQtVideo(QString path) {
        return PQCScriptsImages::get().isQtVideo(path);
    }

    Q_INVOKABLE bool isPDFDocument(QString path) {
        return PQCScriptsImages::get().isPDFDocument(path);
    }

    Q_INVOKABLE bool isSVG(QString path) {
        return PQCScriptsImages::get().isSVG(path);
    }

    Q_INVOKABLE bool isPhotoSphere(QString path) {
        return PQCScriptsImages::get().isPhotoSphere(path);
    }

    Q_INVOKABLE int isMotionPhoto(QString path) {
        return PQCScriptsImages::get().isMotionPhoto(path);
    }

    Q_INVOKABLE bool isItAnimated(QString filename) {
        return PQCScriptsImages::get().isItAnimated(filename);
    }

    Q_INVOKABLE bool isURL(QString url) {
        return PQCScriptsImages::get().isURL(url);
    }

    Q_INVOKABLE bool isLocalURL(QString url) {
        return PQCScriptsImages::get().isLocalURL(url);
    }

    Q_INVOKABLE int isAudio(QString path) {
        return PQCScriptsImages::get().isAudio(path);
    }

    Q_INVOKABLE QString findCoverImageNextToFile(QString path) {
        return PQCScriptsImages::get().findCoverImageNextToFile(path);
    }

    Q_INVOKABLE int getDocumentPageCount(QString path) {
        return PQCScriptsImages::get().getDocumentPageCount(path);
    }

    Q_INVOKABLE QString extractMotionPhoto(QString path) {
        return PQCScriptsImages::get().extractMotionPhoto(path);
    }

    Q_INVOKABLE int getExifOrientation(QString path) {
        return PQCScriptsImages::get().getExifOrientation(path);
    }

    Q_INVOKABLE QString getTextFileContents(QString path) {
        return PQCScriptsImages::get().getTextFileContents(path);
    }

    Q_INVOKABLE QStringList getArchiveContent(QString path, bool insideFilenameOnly = false) {
        return PQCScriptsImages::get().getArchiveContent(path);
    }

    Q_INVOKABLE QVariantList loadEPUB(QString path) {
        return PQCScriptsImages::get().loadEPUB(path);
    }

Q_SIGNALS:
    void receivedStreamSupported(bool supp);
    void receivedStreamURL(QString url);
    void receivedStreamTitle(QString title);
    void receivedStreamError(QString err);
    void foundAudioData(QString what, QString value);

};
