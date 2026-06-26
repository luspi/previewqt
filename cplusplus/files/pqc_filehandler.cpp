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

#include <pqc_filehandler.h>
#include <pqc_configfiles.h>
#include <pqc_scriptsother.h>
#include <pqc_settingscpp.h>

#include <fileplugins/pqc_fileplugin_qt.h>
#include <fileplugins/pqc_fileplugin_resvg.h>
#include <fileplugins/pqc_fileplugin_pdf.h>
#include <fileplugins/pqc_fileplugin_libraw.h>
#include <fileplugins/pqc_fileplugin_libarchive.h>
#include <fileplugins/pqc_fileplugin_libsai.h>
#include <fileplugins/pqc_fileplugin_video.h>
#include <fileplugins/pqc_fileplugin_magick.h>
#include <fileplugins/pqc_fileplugin_devil.h>
#include <fileplugins/pqc_fileplugin_libvips.h>

#include <fileplugins/pqc_fileplugin_audio.h>
#include <fileplugins/pqc_fileplugin_text.h>
#include <fileplugins/pqc_fileplugin_ebook.h>

#include <QMimeDatabase>

PQCFileHandler::PQCFileHandler() {

    /*******************************************************/

    m_imagePluginOrder = QStringList()
#ifdef PQMRESVG
        << "resvg"
#endif
#if defined(PQMPOPPLER) || defined(PQMQTPDF)
        << "pdf"
#endif
        << "qt"
#ifdef PQMRAW
        << "libraw"
#endif
#ifdef PQMLIBARCHIVE
        << "libarchive"
#endif
#ifdef PQMLIBSAI
        << "libsai"
#endif
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
        << "magick"
#endif
#ifdef PQMLIBVIPS
        << "libvips"
#endif
#ifdef PQMDEVIL
        << "devil"
#endif
#ifdef PQMVIDEOQT
           << "video"
#endif
#ifdef PQMVIDEOMPV
           << "libmpv"
#endif
    ;

    m_pluginOrder = m_imagePluginOrder;
    m_pluginOrder
#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
        << "audio"
#endif
        << "text"
#ifdef PQMLIBARCHIVE
        << "ebook"
#endif
    ;

    /*******************************************************/

    m_plugins.insert("qt", new PQCFilePluginQt);
#ifdef PQMRESVG
    m_plugins.insert("resvg", new PQCFilePluginResvg);
#endif
#if defined(PQMPOPPLER) || defined(PQMQTPDF)
    m_plugins.insert("pdf", new PQCFilePluginPDF);
#endif
#ifdef PQMRAW
    m_plugins.insert("libraw", new PQCFilePluginLibraw);
#endif
#ifdef PQMLIBARCHIVE
    m_plugins.insert("libarchive", new PQCFilePluginLibarchive);
#endif
#ifdef PQMLIBSAI
    m_plugins.insert("libsai", new PQCFilePluginLibsai);
#endif
#ifdef PQMVIDEOQT
    m_plugins.insert("video", new PQCFilePluginVideo(false));
#endif
#ifdef PQMVIDEOMPV
    m_plugins.insert("libmpv", new PQCFilePluginVideo(true));
#endif
#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
    m_plugins.insert("magick", new PQCFilePluginMagick);
#endif
#ifdef PQMDEVIL
    m_plugins.insert("devil", new PQCFilePluginDevIL);
#endif
#ifdef PQMLIBVIPS
    m_plugins.insert("libvips", new PQCFilePluginLibVips);
#endif
#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
    m_plugins.insert("audio", new PQCFilePluginAudio);
#endif
    m_plugins.insert("text", new PQCFilePluginText);
#ifdef PQMLIBARCHIVE
    m_plugins.insert("ebook", new PQCFilePluginEBook);
#endif

    /*******************************************************/

    for(PQCFilePlugin *plugin : std::as_const(m_plugins)) {

        m_enabledIds += plugin->getFormats();
        m_enabledSuffixes += plugin->getSuffixes();
        m_enabledMimetypes += plugin->getMimetypes();
        m_suffix2id.insert(plugin->getSuffix2IdMapping());
        m_desc2id.insert(plugin->getDescription2IdMapping());

    }

    m_numEnabled = m_enabledSuffixes.size();

}

QSize PQCFileHandler::getSize(QString path) {

    QFileInfo info(path);
    const QString suffix1 = info.suffix().toLower();
    const QString suffix2 = info.completeSuffix().toLower();

    for(const QString &name : std::as_const(m_imagePluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        QSet<QString> suf = plugin->getSuffixes();
        if(suf.contains(suffix1) || suf.contains(suffix2)) {

            QSize sze = plugin->loadSize(path);
            if(!sze.isEmpty())
                return sze;

        }

    }

    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(path).name();

    for(const QString &name : std::as_const(m_imagePluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        QSet<QString> mim = plugin->getMimetypes();
        if(mim.contains(mimetype)) {

            QSize sze = plugin->loadSize(path);
            if(!sze.isEmpty())
                return sze;

        }

    }

    return QSize();

}

QImage PQCFileHandler::getImage(QString path, QSize requestedSize, QSize &origSize, QString &error) {

    if(path.trimmed().isEmpty())
        return QImage();

    QFileInfo info(path);

    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    const QString suffix1 = info.suffix().toLower();
    const QString suffix2 = info.completeSuffix().toLower();

    for(const QString &name : std::as_const(m_imagePluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        QSet<QString> suf = plugin->getSuffixes();
        if(suf.contains(suffix1) || suf.contains(suffix2)) {

            QImage img = plugin->loadImage(path, requestedSize, origSize, error);
            if(!img.isNull()) {
                return img;
            }

        }

    }

    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(path).name();

    for(const QString &name : std::as_const(m_imagePluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        QSet<QString> mim = plugin->getMimetypes();
        if(mim.contains(mimetype)) {

            QImage img = plugin->loadImage(path, requestedSize, origSize, error);
            if(!img.isNull()) {
                return img;
            }

        }

    }

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)

    QImage img = m_plugins["magick"]->loadImage(path, requestedSize, origSize, error);
    return img;

#endif

    return QImage();

}

QImage PQCFileHandler::getImageWithPlugin(QString plugin, QString path, QSize requestedSize, QSize &origSize, QString &error) {

    if(!m_imagePluginOrder.contains(plugin)) {
        qWarning() << "Requested plugin" << plugin << "not found.";
        return QImage();
    }

    return m_plugins.value(plugin)->loadImage(path, requestedSize, origSize, error);

}

QSet<int> PQCFileHandler::getFormats(QString category) {

    if(category == "all") return m_enabledIds;

    if(m_plugins.contains(category))
        return m_plugins.value(category)->getFormats();

    return {};

}

QSet<QString> PQCFileHandler::getSuffixes(QString category) {

    if(category == "all") return m_enabledSuffixes;

    if(m_plugins.contains(category))
        return m_plugins.value(category)->getSuffixes();

    return {};

}

QSet<QString> PQCFileHandler::getMimetypes(QString category) {

    if(category == "all") return m_enabledMimetypes;

    if(m_plugins.contains(category))
        return m_plugins.value(category)->getMimetypes();

    return {};

}
