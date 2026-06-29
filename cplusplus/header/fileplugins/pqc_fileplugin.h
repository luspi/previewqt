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

#include <pqc_configfiles.h>
#include <pqc_helper.h>
#include <QObject>
#include <QSet>
#include <QImage>
#include <QFileInfo>

class QTimer;

// Every image plugin has to inherit this class and implement all its methods

class PQCFilePlugin : public QObject {

    Q_OBJECT

public:
    explicit PQCFilePlugin(QObject *parent = nullptr);
    ~PQCFilePlugin();

    // the printable name of this plugin
    virtual const QString name() = 0;

    // LOAD the size (resolution) of the image at the specified path
    virtual const QSize loadSize(QString path) = 0;

    // LOAD the image from the specified path at its requested Size
    // > origSize is set to the original size of the image (before scaling)
    // > error holding any potential error message
    virtual const QImage loadImage(QString path, QSize requestedSize, QSize &origSize, QString &error) = 0;

    /****************************************************/
    /****************************************************/

    // sets the data for this plugin
    void setData(const QHash<int, QList<QStringList> > dat);

    /****************************************************/
    /****************************************************/

    const QSet<int> getFormats()    { return m_enabledIds; }
    const QSet<QString> getSuffixes()   { return m_enabledSuffixes; }
    const QSet<QString> getMimetypes()  { return m_enabledMimetypes; }

    /****************************************************/
    /****************************************************/

    // the format for a suffix
    int getFormat(QString suffix) const { return m_suffix2id.value(suffix, -1); }
    int getFormatFromDescription(QString desc) const { return m_desc2id.value(desc, -1); }
    const QString getDescription(int id) { const QList<QStringList> tmp = m_id2data.value(id, {{""}}); return tmp[0][0]; }

    /****************************************************/
    /****************************************************/
    // the suffixes/mimetypes for a format

    const QHash<QString,int> getSuffix2IdMapping() {
        return m_suffix2id;
    }

    const QHash<QString,int> getDescription2IdMapping() {
        return m_desc2id;
    }

    const QHash<QString,int> getMimetypes2IdMapping() {
        return m_mime2id;
    }

    /****************************************************/
    /****************************************************/

    // whether this plugin supports the format based on its format
    bool supportsFormat(const int id) const {
        return m_id2data.contains(id);
    }

    /****************************************************/
    /****************************************************/

    bool loadImageFromCache(QString path, QImage &img, QSize requestedSize) {
        const QString curCacheId = QString("%1%2").arg(path).arg(QFileInfo(path).lastModified().toMSecsSinceEpoch());
        if(m_cacheId == curCacheId) {
            if(!requestedSize.isEmpty()) {
                img = m_cacheImg.scaled(m_cacheImg.size().scaled(requestedSize, Qt::KeepAspectRatio),
                                         Qt::IgnoreAspectRatio,
                                         Qt::SmoothTransformation);
            } else
                img = m_cacheImg.copy();
            return true;
        }
        return false;
    }

    bool loadSizeFromCache(QString path, QSize &sze) {
        const QString curCacheId = QString("%1%2").arg(path).arg(QFileInfo(path).lastModified().toMSecsSinceEpoch());
        if(m_cacheId == curCacheId) {
            sze = m_cacheImg.size();
            return true;
        }
        return false;
    }

    void saveImageToCache(QString path, QImage &img) {
        m_cacheId = QString("%1%2").arg(path).arg(QFileInfo(path).lastModified().toMSecsSinceEpoch());
        m_cacheImg = img.copy();
    }

private:
    QHash<int, QList<QStringList> > m_id2data;
    QHash<QString,int> m_suffix2id;
    QHash<QString,int> m_mime2id;
    QHash<QString,int> m_desc2id;

    QSet<int> m_enabledIds;
    QSet<QString> m_enabledSuffixes;
    QSet<QString> m_enabledMimetypes;

    QString m_cacheId;
    QImage m_cacheImg;

};
