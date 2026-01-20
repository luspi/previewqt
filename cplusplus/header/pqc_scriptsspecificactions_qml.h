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

#include <pqc_scriptsspecificactions.h>
#include <QObject>
#include <QMap>
#include <QImage>
#include <QTranslator>
#include <QQmlEngine>

class QProcess;

class PQCScriptsSpecificActionsQML : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(PQCScriptsSpecificActions)

public:
    PQCScriptsSpecificActionsQML() {
        connect(&PQCScriptsSpecificActions::get(), &PQCScriptsSpecificActions::commandLineArgumentReceived,
                this, &PQCScriptsSpecificActionsQML::commandLineArgumentReceived);
    }
    ~PQCScriptsSpecificActionsQML() {}

    Q_INVOKABLE bool isArchive(QString path) {
        return PQCScriptsSpecificActions::get().isArchive(path);
    }

    Q_INVOKABLE bool isComicBook(QString path) {
        return PQCScriptsSpecificActions::get().isComicBook(path);
    }

    Q_INVOKABLE bool isEpub(QString path) {
        return PQCScriptsSpecificActions::get().isEpub(path);
    }

    Q_INVOKABLE bool isTextDocument(QString path) {
        return PQCScriptsSpecificActions::get().isTextDocument(path);
    }

    Q_INVOKABLE bool isMpvVideo(QString path) {
        return PQCScriptsSpecificActions::get().isMpvVideo(path);
    }

    Q_INVOKABLE bool isQtVideo(QString path) {
        return PQCScriptsSpecificActions::get().isQtVideo(path);
    }

    Q_INVOKABLE bool isPDFDocument(QString path) {
        return PQCScriptsSpecificActions::get().isPDFDocument(path);
    }

    Q_INVOKABLE bool isSVG(QString path) {
        return PQCScriptsSpecificActions::get().isSVG(path);
    }

    Q_INVOKABLE bool isPhotoSphere(QString path) {
        return PQCScriptsSpecificActions::get().isPhotoSphere(path);
    }

    Q_INVOKABLE int isMotionPhoto(QString path) {
        return PQCScriptsSpecificActions::get().isMotionPhoto(path);
    }

    Q_INVOKABLE bool isItAnimated(QString filename) {
        return PQCScriptsSpecificActions::get().isItAnimated(filename);
    }

    Q_INVOKABLE int getDocumentPageCount(QString path) {
        return PQCScriptsSpecificActions::get().getDocumentPageCount(path);
    }

    Q_INVOKABLE QString extractMotionPhoto(QString path) {
        return PQCScriptsSpecificActions::get().extractMotionPhoto(path);
    }

    Q_INVOKABLE int getExifOrientation(QString path) {
        return PQCScriptsSpecificActions::get().getExifOrientation(path);
    }

    Q_INVOKABLE QString getTextFileContents(QString path) {
        return PQCScriptsSpecificActions::get().getTextFileContents(path);
    }

    Q_INVOKABLE QStringList getArchiveContent(QString path, bool insideFilenameOnly = false) {
        return PQCScriptsSpecificActions::get().getArchiveContent(path);
    }

    Q_INVOKABLE QVariantList loadEPUB(QString path) {
        return PQCScriptsSpecificActions::get().loadEPUB(path);
    }


Q_SIGNALS:
    void commandLineArgumentReceived(QString msg);

};
