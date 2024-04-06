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
#include <QPoint>
#include <QSize>

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
    Q_INVOKABLE void storeConfiguration();

    Q_INVOKABLE bool verifyExecutable();
    Q_INVOKABLE void passToPreviewQt(QString path);

    Q_PROPERTY(bool showText READ getShowText WRITE setShowText NOTIFY showTextChanged)
    bool getShowText();
    void setShowText(bool val);

    Q_PROPERTY(QString previewQtExec READ getPreviewQtExec WRITE setPreviewQtExec NOTIFY previewQtExecChanged)
    QString getPreviewQtExec();
    void setPreviewQtExec(QString val);

    Q_INVOKABLE QString getPassedOnFilename() { return m_passedOnFilename; }
    Q_INVOKABLE void setPassedOnFilename(QString val) { m_passedOnFilename = val; }

    Q_INVOKABLE QPoint getWindowPos() { return m_windowPos; }
    Q_INVOKABLE void setWindowPos(QPoint val) { m_windowPos = val; }

    Q_INVOKABLE QSize getWindowSize() { return m_windowSize; }
    Q_INVOKABLE void setWindowSize(QSize val) { m_windowSize = val; }

    Q_INVOKABLE static QString cleanPath(QString path);
    Q_INVOKABLE static QString cleanPath_windows(QString path);
    Q_INVOKABLE QString getDir(QString fullpath);
    Q_INVOKABLE bool amIOnWindows();

    void setOpenConfigStart(bool val) { m_openConfigStart = val; }
    Q_INVOKABLE bool getOpenConfigStart() { return m_openConfigStart; }

private:
    PQCScripts();
    QProcess *proc;
    QSettings *set;

    bool m_showText;
    QString m_previewQtExec;
    QPoint m_windowPos;
    QSize m_windowSize;

    QString m_passedOnFilename;

    bool m_openConfigStart;

signals:
    void showTextChanged();
    void previewQtExecChanged();

};

#endif