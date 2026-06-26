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

#include <fileplugins/pqc_fileplugin.h>
#include <QTimer>

PQCFilePlugin::PQCFilePlugin(QObject *parent) : QObject(parent) {}

PQCFilePlugin::~PQCFilePlugin() {}

void PQCFilePlugin::setData(const QHash<int, QList<QStringList > > dat) {

    m_id2data = dat;

    // this is VERY cheap and will make looking up a description much faster
    for(const auto &[key, value] : std::as_const(m_id2data).asKeyValueRange()) {
        m_desc2id.insert(value[0][0], key);
        m_enabledIds.insert(key);
        for(const QString &suffix : value[1]) {
            m_suffix2id.insert(suffix, key);
            m_enabledSuffixes.insert(suffix);
        }
        for(const QString &mime : value[2]) {
            m_mime2id.insert(mime, key);
            m_enabledMimetypes.insert(mime);
        }
    }

}
