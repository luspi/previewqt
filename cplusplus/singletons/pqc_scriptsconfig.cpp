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
#include <pqc_scriptsconfig.h>
#include <pqc_filehandler.h>
#include <pqc_settingscpp.h>

#include <QtDebug>
#include <QImageReader>
#include <QTranslator>
#include <QFile>
#include <QQmlEngine>
#include <QQmlContext>

#ifdef PQMLIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif

#ifdef PQMRAW
#include <libraw/libraw.h>
#endif

#ifdef PQMPOPPLER
#include <poppler/qt6/poppler-version.h>
#include <poppler/qt6/poppler-qt6.h>
#endif

#if defined(PQMIMAGEMAGICK) || defined(PQMGRAPHICSMAGICK)
#include <Magick++/Include.h>
#endif

#ifdef PQMDEVIL
#include <IL/il.h>
#endif

#ifdef PQMLIBMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMEXIV2
#include <exiv2/exiv2.hpp>
#endif

#ifdef PQMOPENSLIDE
#include <openslide/openslide.h>
#endif

PQCScriptsConfig::PQCScriptsConfig() {
    m_qmlEngine = nullptr;
    m_debug = false;
    currentTranslation = "en";
}

PQCScriptsConfig::~PQCScriptsConfig() {}

void PQCScriptsConfig::setQmlEngine(QQmlEngine &engine) {
    m_qmlEngine = &engine;
}

QString PQCScriptsConfig::getConfigInfo(bool formatHTML) {

    qDebug() << "";

    QString bold1 = "";
    QString bold2 = "";
    QString spacing = "    ";
    QString startLI = " - ";
    QString endLI = "\n";
    if(formatHTML) {
        bold1 = "<b>";
        bold2 = "</b>";
        spacing = "&nbsp;&nbsp;&nbsp;";
        startLI = "<li>";
        endLI = "</li>";
    }

    QString txt = "";

    txt += startLI % "Total of " % bold1 % QString::number(PQCFileHandler::get().getNumFormatsFormats()) % bold2 % " supported file formats." % endLI;

    txt += startLI % "Compiled with " % bold1 % "Qt " % QT_VERSION_STR % bold2 % ", running with " % bold1 % "Qt " % qVersion() % "." % bold2 % endLI;

    if(formatHTML)
        txt += "<br><h3>Configured options:</h3><ul>";
    else
        txt += "\nCONFIGURED OPTIONS:\n\n";

#ifdef PQMEXIV2
    txt += startLI % bold1 % "Exiv2" % bold2 % ": " % Exiv2::version() % endLI;
#endif

#ifdef PQMRAW
    txt += startLI % bold1 % "LibRaw" % bold2 % ": " % LibRaw::version() % endLI;
#endif

#ifdef PQMPOPPLER
    txt += startLI % bold1 % "Poppler" % bold2 % ": " % POPPLER_VERSION % endLI;
#endif

#ifdef PQMQTPDF
    txt += startLI % bold1 % "QtPDF" % bold2 % endLI;
#endif

#ifdef PQMLIBARCHIVE
    txt += startLI % bold1 % "LibArchive" % bold2 % ": " % ARCHIVE_VERSION_ONLY_STRING % endLI;
#endif

#ifdef PQMIMAGEMAGICK
    txt += startLI % bold1 % "ImageMagick" % bold2 % ": " % MagickLibVersionText % endLI;
#endif

#ifdef PQMGRAPHICSMAGICK
    txt += startLI % bold1 % "GraphicsMagick" % bold2 % ": " % MagickLibVersionText % endLI;
#endif

#ifdef PQMDEVIL
    txt += startLI % bold1 % "DevIL" % bold2 % ": " % QString::number(IL_VERSION) % endLI;
#endif

#ifdef PQMMOTIONPHOTO
    txt += startLI % bold1 % "Motion Photo" % bold2 % endLI;
#endif

#ifdef PQMPHOTOSPHERE
    txt += startLI % bold1 % "Photosphere" % bold2 % endLI;
#endif

#ifdef PQMEPUB
    txt += startLI % bold1 % "E-books (epub)" % bold2 % endLI;
#endif

#ifdef PQMQTMULTIMEDIA
    txt += startLI % bold1 % "Multimedia" % bold2 % " through Qt" % endLI;
#endif

#ifdef PQMLIBREOFFICE
    txt += startLI % bold1 % "LibreOffice" % bold2 % endLI;
#endif

#ifdef PQMOPENSLIDE
    txt += startLI % bold1 % "OpenSlide" % bold2 % ": " % openslide_get_version() % endLI;
#endif

#ifdef PQMLIBMPV
    mpv_handle *mpv = mpv_create();
    if(mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");
    txt += startLI % bold1 % "libmpv" % bold2 % ": " % mpv::qt::get_property(mpv, "mpv-version").toString() % " (ffmpeg: " % mpv::qt::get_property(mpv, "ffmpeg-version").toString() % ")" % endLI;
#endif

    if(formatHTML)
        txt += "</ul>";

    if(formatHTML)
        txt += "<h3>File Plugins:</h3><ul>";
    else
        txt += "\nFILE PLUGINS:\n\n";

    for(const QString plugin : PQCFileHandler::get().getPluginList()) {
        txt += startLI % bold1 % PQCFileHandler::get().getPluginName(plugin) % bold2 % ": " % QString::number(PQCFileHandler::get().getFormats(plugin).count()) % " file formats" % endLI;
    }

    if(formatHTML)
        txt += "</ul>";

    return txt;

}

QString PQCScriptsConfig::getVersion() {
    return PQMVERSION;
}

bool PQCScriptsConfig::isQtAtLeast6_5() {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    return true;
#endif
    return false;
}

bool PQCScriptsConfig::isMotionPhotoSupportEnabled() {
#ifdef PQMMOTIONPHOTO
    return true;
#endif
    return false;
}

bool PQCScriptsConfig::isMPVEnabled() {
#ifdef PQMLIBMPV
    return true;
#endif
    return false;
}

bool PQCScriptsConfig::isQtMultimediaEnabled() {
#ifdef PQMQTMULTIMEDIA
    return true;
#endif
    return false;
}

bool PQCScriptsConfig::amIOnWindows() {
#ifdef Q_OS_WIN
    return true;
#endif
    return false;
}

bool PQCScriptsConfig::isUpgrade() {

    return (PQCSettingsCPP::get().getVersion() != PQMVERSION || QString(PQMVERSION) == "dev");

}

bool PQCScriptsConfig::isDebug() {
    return m_debug;
}

void PQCScriptsConfig::setDebug(bool val) {
    m_debug = val;
}

void PQCScriptsConfig::updateTranslation(QString code) {

    qDebug() << "args: code =" << code;

    if(code == "") code = PQCSettingsCPP::get().getLanguage();
    qDebug() << "set language:" << code;

    if(code == currentTranslation)
        return;

    static QTranslator trans;
    qApp->removeTranslator(&trans);

    const QStringList allcodes = code.split("/");

    // we use this to detect whether a translation was found for the above language code
    currentTranslation = "";
    for(const QString &c : allcodes) {

        // the load() function will try various filename combinations
        if(trans.load(":/lang/previewqt_" % c % ".qm")) {
            currentTranslation = c;
            qApp->installTranslator(&trans);
            break;
        } else
            qWarning() << "Unable to install translator for language code" << c;

    }

    // no translation found -> store selected code
    if(currentTranslation.isEmpty())
        currentTranslation = code;

    if(m_qmlEngine != nullptr)
        m_qmlEngine->retranslate();

}
