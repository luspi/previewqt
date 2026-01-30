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

#include <pqc_scriptsfilespaths.h>
#include <QObject>
#include <QQmlEngine>
#include <QImage>

class QTranslator;

class PQCScriptsFilesPathsQML : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(PQCScriptsFilesPaths)

public:
    explicit PQCScriptsFilesPathsQML() {};
    ~PQCScriptsFilesPathsQML() {}

    Q_INVOKABLE static QString cleanPath(QString path) {
        return PQCScriptsFilesPaths::get().cleanPath(path);
    }

    Q_INVOKABLE static QString cleanPath_windows(QString path) {
        return PQCScriptsFilesPaths::get().cleanPath_windows(path);
    }

    Q_INVOKABLE QString getBasename(QString fullpath) {
        return PQCScriptsFilesPaths::get().getBasename(fullpath);
    }

    Q_INVOKABLE QString getFilename(QString path) {
        return PQCScriptsFilesPaths::get().getFilename(path);
    }

    Q_INVOKABLE QString getDir(QString fullpath) {
        return PQCScriptsFilesPaths::get().getDir(fullpath);
    }

    Q_INVOKABLE QString getSuffix(QString path) {
        return PQCScriptsFilesPaths::get().getSuffix(path);
    }

    Q_INVOKABLE QString toPercentEncoding(QString str) {
        return PQCScriptsFilesPaths::get().toPercentEncoding(str);
    }

    Q_INVOKABLE QString fromPercentEncoding(QByteArray str) {
        return PQCScriptsFilesPaths::get().fromPercentEncoding(str);
    }

    Q_INVOKABLE bool isFileSupported(QString path) {
        return PQCScriptsFilesPaths::get().isFileSupported(path);
    }

    Q_INVOKABLE QString openNewFile() {
        return PQCScriptsFilesPaths::get().openNewFile();
    }

    Q_INVOKABLE bool openInDefault(QString path) {
        return PQCScriptsFilesPaths::get().openInDefault(path);
    }

    Q_INVOKABLE void deleteTemporaryFiles() {
        PQCScriptsFilesPaths::get().deleteTemporaryFiles();
    }

    Q_INVOKABLE void copyTextToClipboard(QString txt) {
        PQCScriptsFilesPaths::get().copyTextToClipboard(txt);
    }

    Q_INVOKABLE QString saveImageToTempFile(QImage img) {
        return PQCScriptsFilesPaths::get().saveImageToTempFile(img);
    }


};
