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
    QString nl = "\n";
    QString spacing = "    ";
    if(formatHTML) {
        bold1 = "<b>";
        bold2 = "</b>";
        nl = "<br>";
        spacing = "&nbsp;&nbsp;&nbsp;";
    }

    QString txt = "";

    txt += " - Compiled with " % bold1 % "Qt " % QT_VERSION_STR % bold2 % ", running with " % bold1 % "Qt " % qVersion() % bold2 % nl;

    txt += " - Total of " % bold1 % QString::number(PQCFileHandler::get().getNumFormats()) % bold2 % " supported file formats" % nl;

#ifdef PQMEXIV2
    txt += " - " % bold1 % "Exiv2" % bold2 % ": " % Exiv2::version() % nl;
#endif

#ifdef PQMRAW
    txt += " - " % bold1 % "LibRaw" % bold2 % ": " % LibRaw::version() % nl;
#endif

#ifdef PQMPOPPLER
    txt += " - " % bold1 % "Poppler" % bold2 % ": " % POPPLER_VERSION % nl;
#endif

#ifdef PQMQTPDF
    txt += " - " % bold1 % "QtPDF" % bold2 % nl;
#endif

#ifdef PQMLIBARCHIVE
    txt += " - " % bold1 % "LibArchive" % bold2 % ": " % ARCHIVE_VERSION_ONLY_STRING % nl;
#endif

#ifdef PQMIMAGEMAGICK
    txt += " - " % bold1 % "ImageMagick" % bold2 % ": " % MagickLibVersionText % nl;
#endif

#ifdef PQMGRAPHICSMAGICK
    txt += " - " % bold1 % "GraphicsMagick" % bold2 % ": " % MagickLibVersionText % nl;
#endif

#ifdef PQMDEVIL
    txt += " - " % bold1 % "DevIL" % bold2 % ": " % QString::number(IL_VERSION) % nl;
#endif

#ifdef PQMMOTIONPHOTO
    txt += " - " % bold1 % "Motion Photo" % bold2 % nl;
#endif

#ifdef PQMPHOTOSPHERE
    txt += " - " % bold1 % "Photosphere" % bold2 % nl;
#endif

#ifdef PQMEPUB
    txt += " - " % bold1 % "E-books (epub)" % bold2 % nl;
#endif

#ifdef PQMQTMULTIMEDIA
    txt += " - " % bold1 % "Multimedia" % bold2 % " through Qt" % nl;
#endif

#ifdef PQMLIBMPV
    mpv_handle *mpv = mpv_create();
    if(mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");
    txt += " - " % bold1 % "libmpv" % bold2 % ": " % mpv::qt::get_property(mpv, "mpv-version").toString() % " (ffmpeg: " % mpv::qt::get_property(mpv, "ffmpeg-version").toString() % ")" % nl;
#endif

    txt += " - " % bold1 % "Qt" % bold2 % " image formats available:" % nl % spacing;
    QImageReader reader;
    auto formats = reader.supportedImageFormats();
    for(int i = 0; i < formats.length(); ++i) {
        if(i != 0 && i%10 == 0)
            txt += nl % spacing;
        txt += QString("%1, ").arg(QString(formats[i]), 5);
    }

    txt += nl;

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
