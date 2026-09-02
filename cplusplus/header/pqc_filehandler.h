/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2026 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PhotoQt.                                        **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/
#pragma once

#include <QObject>
#include <QString>
#include <QSet>
#include <QMutex>
#include <fileplugins/pqc_fileplugin.h>

class PQCFileHandler : public QObject {

    Q_OBJECT

public:
    static PQCFileHandler& get() {
        static PQCFileHandler instance;
        return instance;
    }

    PQCFileHandler(PQCFileHandler const&) = delete;
    void operator=(PQCFileHandler const&) = delete;

    QSize getSize(QString path);
    QImage getImage(QString path, QSize requestedSize, QSize &origSize, QString &error);
    QImage getImageWithPlugin(QString plugin, QString path, QSize requestedSize, QSize &origSize, QString &error);
    QVariantList getData(QString path);
    QVariantList getDataWithPlugin(QString plugin, QString path);
    int getNumPages(QString plugin, QString path);
    QStringList getContent(QString plugin, QString path);

    int getNumFormats() { return m_numEnabled; }
    int getNumFormatsFormats() { return m_numEnabledFormats; }

    const QStringList getPluginList() { return m_pluginOrder; }
    const QString getPluginName(const QString plugin) { if(m_plugins.contains(plugin)) return m_plugins.value(plugin)->name(); else return "Unknown plugin"; }
    const QStringList getPossiblePluginsFor(QString path);

    QSet<int> getFormats(QString category = "all");
    QSet<QString> getSuffixes(QString category = "all");
    QSet<QString> getMimetypes(QString category = "all");

private:
    PQCFileHandler();

    QStringList m_pluginOrder;
    QStringList m_imagePluginOrder;
    QHash<QString, PQCFilePlugin*> m_plugins;

    int m_numEnabled;
    int m_numEnabledFormats;

    QSet<int> m_enabledIds;
    QSet<QString> m_enabledSuffixes;
    QSet<QString> m_enabledMimetypes;

    QHash<QString, int> m_suffix2id;
    QHash<QString, int> m_desc2id;

};
