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

#include "scripts.h"
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

PQCScripts::PQCScripts() {

    proc = new QProcess;
    set = new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings");

    m_passedOnFilename = "";
    m_showText = true;
    m_previewQtExec = "previewqt";
    m_openConfigStart = false;

    loadConfiguration();

}
PQCScripts::~PQCScripts() {
    delete proc;
    delete set;
}


bool PQCScripts::getShowText() {
    return m_showText;
}

void PQCScripts::setShowText(bool val) {
    if(val != m_showText) {
        m_showText = val;
        showTextChanged();
    }
}


QString PQCScripts::getPreviewQtExec() {
    return m_previewQtExec;
}

void PQCScripts::setPreviewQtExec(QString val) {
    if(m_previewQtExec != val) {
        m_previewQtExec = val;
        previewQtExecChanged();
    }
}

void PQCScripts::loadConfiguration() {

    m_previewQtExec = set->value("executable", "previewqt").toString();
    m_showText = set->value("showText", true).toBool();
    m_windowPos = set->value("windowPosition", QPoint(-1,-1)).toPoint();
    m_windowSize = set->value("windowSize", QSize(200,200)).toSize();

}

void PQCScripts::storeConfiguration() {

    set->setValue("executable", m_previewQtExec);
    set->setValue("showText", m_showText);
    set->setValue("windowPosition", m_windowPos);
    set->setValue("windowSize", m_windowSize);

}

void PQCScripts::passToPreviewQt(QString path) {

    proc->setProgram(m_previewQtExec);
    proc->setArguments({path});
    proc->startDetached();

}

bool PQCScripts::verifyExecutable() {

    QFileInfo info(m_previewQtExec);
    if(!m_previewQtExec.startsWith("/"))
        info.setFile("/usr/bin/" + m_previewQtExec);

    if(!info.exists())
        return false;

    if(!info.isExecutable())
        return false;

    return true;

}

QString PQCScripts::getDir(QString fullpath) {

    if(fullpath == "")
        return "";

    return QFileInfo(fullpath).absolutePath();

}

bool PQCScripts::amIOnWindows() {
#ifdef Q_OS_WIN
    return true;
#endif
    return false;
}

QString PQCScripts::cleanPath(QString path) {

#ifdef Q_OS_WIN
    return cleanPath_windows(path);
#else
    if(path.startsWith("file:////"))
        path = path.remove(0, 8);
    else if(path.startsWith("file:///"))
        path = path.remove(0, 7);
    else if(path.startsWith("file://"))
        path = path.remove(0, 6);
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    return QDir::cleanPath(path);
#endif

}

QString PQCScripts::cleanPath_windows(QString path) {

    if(path.startsWith("file:///"))
        path = path.remove(0, 8);
    else if(path.startsWith("file://"))
        path = path.remove(0, 7);
    else if(path.startsWith("file:/"))
        path = path.remove(0, 6);
    else if(path.startsWith("image://full/"))
        path = path.remove(0, 13);
    else if(path.startsWith("image://thumb/"))
        path = path.remove(0, 14);

    QFileInfo info(path);
    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    return QDir::cleanPath(path);

}
