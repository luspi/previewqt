/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
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
#include <pqc_fileformats.h>
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

#ifdef PQMFREEIMAGE
#include <FreeImage.h>
#endif

#ifdef PQMVIDEOMPV
#include <pqc_mpvobject.h>
#endif

#ifdef PQMEXIV2
#include <exiv2/exiv2.hpp>
#endif

PQCScriptsConfig::PQCScriptsConfig() {
    m_debug = false;
    trans = new QTranslator;
    currentTranslation = "en";
}

PQCScriptsConfig::~PQCScriptsConfig() {
    delete trans;
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

    txt += QString(" - Compiled with %1Qt %2%3, running with %4Qt %5%6%7").arg(bold1, QT_VERSION_STR, bold2, bold1, qVersion(), bold2, nl);

    txt += QString(" - Total of %1%2%3 supported file formats%4").arg(bold1).arg(PQCFileFormats::get().getNumFormats()).arg(bold2, nl);

#ifdef PQMEXIV2
    txt += QString(" - %1Exiv2%2: %3%4").arg(bold1, bold2, Exiv2::version(), nl);
#endif

#ifdef PQMRAW
    txt += QString(" - %1LibRaw%2: %3%4").arg(bold1, bold2, LibRaw::version(), nl);
#endif

#ifdef PQMPOPPLER
    txt += QString(" - %1Poppler%2: %3%4").arg(bold1, bold2, POPPLER_VERSION, nl);
#endif

#ifdef PQMQTPDF
    txt += QString(" - %1QtPDF%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMLIBARCHIVE
    txt += QString(" - %1LibArchive%2: %3%4").arg(bold1, bold2, ARCHIVE_VERSION_ONLY_STRING, nl);
#endif

#ifdef PQMIMAGEMAGICK
    txt += QString(" - %1ImageMagick%2: %3%4").arg(bold1, bold2, MagickLibVersionText, nl);
#endif

#ifdef PQMGRAPHICSMAGICK
    txt += QString(" - %1GraphicsMagick%2: %3%4").arg(bold1, bold2, MagickLibVersionText, nl);
#endif

#ifdef PQMFREEIMAGE
    txt += QString(" - %1FreeImage%2: %3.%4%5").arg(bold1, bold2).arg(FREEIMAGE_MAJOR_VERSION).arg(FREEIMAGE_MINOR_VERSION).arg(nl);
#endif

#ifdef PQMDEVIL
    txt += QString(" - %1DevIL%2: %3%4").arg(bold1, bold2).arg(IL_VERSION).arg(nl);
#endif

#ifdef PQMMOTIONPHOTO
    txt += QString(" - %1Motion Photo%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMPHOTOSPHERE
    txt += QString(" - %1Photosphere%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMEPUB
    txt += QString(" - %1E-books (epub)%2%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMVIDEOQT
    txt += QString(" - %1Video%2 through Qt%3").arg(bold1, bold2, nl);
#endif

#ifdef PQMVIDEOMPV
    mpv_handle *mpv = mpv_create();
    if(mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");
    txt += QString(" - %1libmpv%2: %3 (ffmpeg: %4)%5").arg(bold1, bold2, mpv::qt::get_property(mpv, "mpv-version").toString(), mpv::qt::get_property(mpv, "ffmpeg-version").toString(), nl);
#endif

    txt += QString(" - %1Qt%2 image formats available:%3%4").arg(bold1, bold2, nl, spacing);
    QImageReader reader;
    auto formats = reader.supportedImageFormats();
    for(int i = 0; i < formats.length(); ++i) {
        if(i != 0 && i%10 == 0)
            txt += QString("%1%2").arg(nl, spacing);
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
#ifdef PQMVIDEOMPV
    return true;
#endif
    return false;
}

bool PQCScriptsConfig::isQtVideoEnabled() {
#ifdef PQMVIDEOQT
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

void PQCScriptsConfig::updateTranslation() {

    qDebug() << "";

    QString code = PQCSettingsCPP::get().getLanguage();
    if(code == currentTranslation)
        return;

    if(!trans->isEmpty())
        qApp->removeTranslator(trans);

    const QStringList allcodes = code.split("/");

    for(const QString &c : allcodes) {

        if(QFile(":/lang/previewqt_" + c + ".qm").exists()) {

            if(trans->load(":/lang/previewqt_" + c))
                qApp->installTranslator(trans);
            else
                qWarning() << "Unable to install translator for language code" << c;

        } else if(c.contains("_")) {

            const QString cc = c.split("_").at(0);

            if(QFile(":/lang/previewqt_" + cc + ".qm").exists()) {

                if(trans->load(":/lang/previewqt_" + cc))
                    qApp->installTranslator(trans);
                else
                    qWarning() << "Unable to install translator for language code" << cc;

            }

        } else {

            const QString cc = QString("%1_%2").arg(c, c.toUpper());

            if(QFile(":/lang/previewqt_" + cc + ".qm").exists()) {

                if(trans->load(":/lang/previewqt_" + cc))
                    qApp->installTranslator(trans);
                else
                    qWarning() << "Unable to install translator for language code" << c;

            }
        }

    }

    // store current localization
    currentTranslation = code;

    QQmlEngine::contextForObject(this)->engine()->retranslate();

}
