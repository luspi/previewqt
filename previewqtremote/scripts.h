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

#ifndef SCRIPTS_H
#define SCRIPTS_H

#include <QObject>

class QProcess;
class QSettings;

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

    void loadConfiguration();
    void storeConfiguration();

    bool verifyExecutable();
    Q_INVOKABLE void passToPreviewQt(QString path);

    Q_INVOKABLE bool getShowText() { return m_showText; }
    Q_INVOKABLE void setShowText(bool val) { m_showText = val; }

    Q_INVOKABLE QString getPreviewQtExec() { return m_previewQtExec; }
    Q_INVOKABLE void setPreviewQtExec(QString val) { m_previewQtExec = val; }

    Q_INVOKABLE QString getPassedOnFilename() { return m_passedOnFilename; }
    Q_INVOKABLE void setPassedOnFilename(QString val) { m_passedOnFilename = val; }

private:
    PQCScripts();
    QProcess *proc;
    QSettings *set;

    bool m_showText;
    QString m_previewQtExec;

    QString m_passedOnFilename;

};

#endif
