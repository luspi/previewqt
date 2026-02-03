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

#include <pqc_scriptsconfig.h>
#include <QObject>
#include <QQmlEngine>

class QTranslator;

class PQCScriptsConfigQML : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(PQCScriptsConfig)

public:
    explicit PQCScriptsConfigQML() {};
    ~PQCScriptsConfigQML() {}

    Q_INVOKABLE QString getConfigInfo(bool formatHTML = true) {
        return PQCScriptsConfig::get().getConfigInfo(formatHTML);
    }

    Q_INVOKABLE QString getVersion()  {
        return PQCScriptsConfig::get().getVersion();
    }

    Q_INVOKABLE bool isQtAtLeast6_5()  {
        return PQCScriptsConfig::get().isQtAtLeast6_5();
    }

    Q_INVOKABLE bool isMotionPhotoSupportEnabled() {
        return PQCScriptsConfig::get().isMotionPhotoSupportEnabled();
    }

    Q_INVOKABLE bool isMPVEnabled() {
        return PQCScriptsConfig::get().isMPVEnabled();
    }

    Q_INVOKABLE bool isQtMultimediaEnabled() {
        return PQCScriptsConfig::get().isQtMultimediaEnabled();
    }

    Q_INVOKABLE bool amIOnWindows() {
        return PQCScriptsConfig::get().amIOnWindows();
    }

    Q_INVOKABLE void updateTranslation(QString code = "") {
        return PQCScriptsConfig::get().updateTranslation(code);
    }

};
