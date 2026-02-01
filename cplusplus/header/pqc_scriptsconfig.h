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

class QTranslator;
class QQmlEngine;

class PQCScriptsConfig : public QObject {

    Q_OBJECT

public:
    static PQCScriptsConfig& get() {
        static PQCScriptsConfig instance;
        return instance;
    }
    ~PQCScriptsConfig();

    PQCScriptsConfig(PQCScriptsConfig const&) = delete;
    void operator=(PQCScriptsConfig const&) = delete;

    QString getConfigInfo(bool formatHTML = true);
    QString getVersion();
    bool isQtAtLeast6_5();
    bool isMotionPhotoSupportEnabled();
    bool isMPVEnabled();
    bool isQtMultimediaEnabled();
    bool amIOnWindows();
    bool isUpgrade();
    bool isDebug();
    void setDebug(bool val);
    void updateTranslation(QString code);

    void setQmlEngine(QQmlEngine &engine);

private:
    PQCScriptsConfig();

    bool m_debug;
    QTranslator *trans;
    QString currentTranslation;

    QQmlEngine *m_qmlEngine;

};
