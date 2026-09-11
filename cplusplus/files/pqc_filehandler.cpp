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

#include <fileplugins/pqc_fileplugin_image.h>
#include <fileplugins/pqc_fileplugin_pdf.h>
#include <fileplugins/pqc_fileplugin_libarchive.h>
#include <fileplugins/pqc_fileplugin_video.h>
#include <fileplugins/pqc_fileplugin_libreoffice.h>
#include <fileplugins/pqc_fileplugin_package.h>
#include <fileplugins/pqc_fileplugin_musescore.h>
#include <fileplugins/pqc_fileplugin_pqt.h>
#include <fileplugins/pqc_fileplugin_sqlite.h>
#include <fileplugins/pqc_fileplugin_dllexe.h>

#include <fileplugins/pqc_fileplugin_audio.h>
#include <fileplugins/pqc_fileplugin_text.h>
#include <fileplugins/pqc_fileplugin_ebook.h>

#include <QMimeDatabase>
#include <QJsonObject>

PQCFileHandler::PQCFileHandler() {

    /*******************************************************/

    m_pluginOrder = QStringList()
#if defined(PQMPOPPLER) || defined(PQMQTPDF)
        << "pdf"
#endif
#ifdef PQMLIBREOFFICE
        << "libreoffice"
#endif
        << "pqt"
#ifdef PQMLIBARCHIVE
        << "libarchive"
#endif
        << "image"
#ifdef PQMQTMULTIMEDIA
        << "video"
#endif
#ifdef PQMLIBMPV
        << "libmpv"
#endif
        << "musescore"
#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
        << "audio"
#endif
        << "sqlite"
        << "dllexe"
        << "text"
#ifdef PQMLIBARCHIVE
        << "ebook"
        << "package"
#endif
    ;

    /*******************************************************/

    m_plugins.insert("image", new PQCFilePluginImage);
#if defined(PQMPOPPLER) || defined(PQMQTPDF)
    m_plugins.insert("pdf", new PQCFilePluginPDF);
#endif
#ifdef PQMLIBARCHIVE
    m_plugins.insert("libarchive", new PQCFilePluginLibarchive);
#endif
#ifdef PQMQTMULTIMEDIA
    m_plugins.insert("video", new PQCFilePluginVideo(false));
#endif
#ifdef PQMLIBMPV
    m_plugins.insert("libmpv", new PQCFilePluginVideo(true));
#endif
#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
    m_plugins.insert("audio", new PQCFilePluginAudio);
#endif
    m_plugins.insert("text", new PQCFilePluginText);
#ifdef PQMLIBARCHIVE
    m_plugins.insert("ebook", new PQCFilePluginEBook);
#endif
#ifdef PQMLIBREOFFICE
    m_plugins.insert("libreoffice", new PQCFilePluginLibreOffice);
#endif
#ifdef PQMLIBARCHIVE
    m_plugins.insert("package", new PQCFilePluginPackage);
#endif
    m_plugins.insert("musescore", new PQCFilePluginMuseScore);
    m_plugins.insert("pqt", new PQCFilePluginPQT);
    m_plugins.insert("sqlite", new PQCFilePluginSQLite);
    m_plugins.insert("dllexe", new PQCFilePluginDLLExe);

    /*******************************************************/

    for(PQCFilePlugin *plugin : std::as_const(m_plugins)) {

        m_enabledIds += plugin->getFormats();
        m_enabledSuffixes += plugin->getSuffixes();
        m_enabledMimetypes += plugin->getMimetypes();
        m_suffix2id.insert(plugin->getSuffix2IdMapping());
        m_desc2id.insert(plugin->getDescription2IdMapping());

    }

    m_numEnabled = m_enabledSuffixes.size();
    m_numEnabledFormats = m_enabledIds.size();

    m_activePlugin = "";

}

QSize PQCFileHandler::getSize(QString path) {

    QFileInfo info(path);
    const QString suffix1 = info.suffix().toLower();
    const QString suffix2 = info.completeSuffix().toLower();

    for(const QString &name : std::as_const(m_pluginOrder)) {

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

    for(const QString &name : std::as_const(m_pluginOrder)) {

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

    for(const QString &name : std::as_const(m_pluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        m_activePlugin = plugin->name();
        Q_EMIT activePluginChanged();

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

    for(const QString &name : std::as_const(m_pluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        m_activePlugin = plugin->name();
        Q_EMIT activePluginChanged();

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

    if(!m_pluginOrder.contains(plugin)) {
        qWarning() << "Requested plugin" << plugin << "not found.";
        return QImage();
    }

    PQCFilePlugin *pl = m_plugins.value(plugin);
    m_activePlugin = pl->name();
    Q_EMIT activePluginChanged();

    return pl->loadImage(path, requestedSize, origSize, error);

}

QVariantList PQCFileHandler::getData(QString path) {

    if(path.trimmed().isEmpty())
        return {};

    QFileInfo info(path);

    if(info.isSymLink() && info.exists())
        path = info.symLinkTarget();

    const QString suffix1 = info.suffix().toLower();
    const QString suffix2 = info.completeSuffix().toLower();

    for(const QString &name : std::as_const(m_pluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        m_activePlugin = plugin->name();
        Q_EMIT activePluginChanged();

        QSet<QString> suf = plugin->getSuffixes();
        if(suf.contains(suffix1) || suf.contains(suffix2)) {

            QVariantList lst = plugin->loadData(path);
            if(!lst.isEmpty())
                return lst;

        }

    }

    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(path).name();

    for(const QString &name : std::as_const(m_pluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        m_activePlugin = plugin->name();
        Q_EMIT activePluginChanged();

        QSet<QString> mim = plugin->getMimetypes();
        if(mim.contains(mimetype)) {

            QVariantList lst = plugin->loadData(path);
            if(!lst.isEmpty())
                return lst;

        }

    }

    return {};

}

QVariantList PQCFileHandler::getDataWithPlugin(QString plugin, QString path) {

    if(!m_pluginOrder.contains(plugin)) {
        qWarning() << "Requested plugin" << plugin << "not found.";
        return {};
    }

    PQCFilePlugin *pl = m_plugins.value(plugin);
    m_activePlugin = pl->name();
    Q_EMIT activePluginChanged();

    return pl->loadData(path);

}

int PQCFileHandler::getNumPages(QString plugin, QString path) {
    if(m_pluginOrder.contains(plugin))
        return m_plugins.value(plugin)->loadNumPages(path);
    return 1;
}

QStringList PQCFileHandler::getContent(QString plugin, QString path) {
    if(m_pluginOrder.contains(plugin))
        return m_plugins.value(plugin)->loadContent(path);
    return {};
}

QJsonObject PQCFileHandler::getJSON(QString path, QVariantMap extraArguments) {

    for(const QString &name : std::as_const(m_pluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        QJsonObject json = plugin->loadJSON(path, extraArguments);
        if(!json.isEmpty())
            return json;

    }

    QJsonObject json;
    json["supported"] = false;

    return json;

}

const QStringList PQCFileHandler::getPossiblePluginsFor(QString path) {

    qDebug() << "args: path =" << path;

    const QString suffix1 = QFileInfo(path).suffix().toLower();
    const QString suffix2 = QFileInfo(path).completeSuffix().toLower();
    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(path).name();

    QStringList ret;

    for(const QString &name : std::as_const(m_pluginOrder)) {

        if(!m_plugins.contains(name)) continue;

        PQCFilePlugin *plugin = m_plugins[name];

        const QSet<QString> mim = plugin->getMimetypes();
        const QSet<QString> suf = plugin->getSuffixes();

        if(suf.contains(suffix1) || suf.contains(suffix2) || mim.contains(mimetype))
            ret.append(name);

    }

    return ret;

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
