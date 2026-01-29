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
#include <pqc_scriptsother.h>

#include <QtDebug>
#include <QSize>
#include <QKeySequence>
#include <QColorSpace>
#include <QClipboard>
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileDialog>

#ifdef PQMLCMS2
#include <lcms2.h>
#endif

PQCScriptsOther::PQCScriptsOther() {
    m_startupMessage = "";
}

PQCScriptsOther::~PQCScriptsOther() {}

QString PQCScriptsOther::keycodeToString(Qt::KeyboardModifiers modifiers, Qt::Key keycode) {

    QStringList mods;
    QStringList keys;

    if(modifiers & Qt::ControlModifier)
        mods.append("Ctrl");
    if(modifiers & Qt::AltModifier)
        mods.append("Alt");
    if(modifiers & Qt::ShiftModifier)
        mods.append("Shift");
    if(modifiers & Qt::MetaModifier)
        mods.append("Meta");
    if(modifiers & Qt::KeypadModifier)
        mods.append("Keypad");

    switch(keycode) {
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_Shift:
    case Qt::Key_Meta:
        break;
    case Qt::Key_Escape:
        keys.append("Esc");
        break;
    case Qt::Key_Right:
        keys.append("Right");
        break;
    case Qt::Key_Left:
        keys.append("Left");
        break;
    case Qt::Key_Up:
        keys.append("Up");
        break;
    case Qt::Key_Down:
        keys.append("Down");
        break;
    case Qt::Key_Space:
        keys.append("Space");
        break;
    case Qt::Key_Delete:
        keys.append("Delete");
        break;
    case Qt::Key_Home:
        keys.append("Home");
        break;
    case Qt::Key_End:
        keys.append("End");
        break;
    case Qt::Key_PageUp:
        keys.append("Page Up");
        break;
    case Qt::Key_PageDown:
        keys.append("Page Down");
        break;
    case Qt::Key_Insert:
        keys.append("Insert");
        break;
    case Qt::Key_Tab:
        keys.append("Tab");
        break;
    case Qt::Key_Backtab:
        keys.append("Tab");
        break;
    case Qt::Key_Return:
        keys.append("Return");
        break;
    case Qt::Key_Enter:
        keys.append("Enter");
        break;
    case Qt::Key_Pause:
        keys.append("Pause");
        break;
    case Qt::Key_Print:
        keys.append("Print");
        break;
    case Qt::Key_SysReq:
        keys.append("SysReq");
        break;
    case Qt::Key_Clear:
        keys.append("Clear");
        break;
    case Qt::Key_CapsLock:
        keys.append("CapsLock");
        break;
    case Qt::Key_NumLock:
        keys.append("NumLock");
        break;
    case Qt::Key_ScrollLock:
        keys.append("ScrollLock");
        break;
    case Qt::Key_Super_L:
        keys.append("Super L");
        break;
    case Qt::Key_Super_R:
        keys.append("Super R");
        break;
    case Qt::Key_Menu:
        keys.append("Menu");
        break;
    case Qt::Key_Hyper_L:
        keys.append("Hyper L");
        break;
    case Qt::Key_Hyper_R:
        keys.append("Hyper R");
        break;
    case Qt::Key_Help:
        keys.append("Help");
        break;
    case Qt::Key_Direction_L:
        keys.append("Direction L");
        break;
    case Qt::Key_Direction_R:
        keys.append("Direction R");
        break;
    case Qt::Key_F1:
        keys.append("F1");
        break;
    case Qt::Key_F2:
        keys.append("F2");
        break;
    case Qt::Key_F3:
        keys.append("F3");
        break;
    case Qt::Key_F4:
        keys.append("F4");
        break;
    case Qt::Key_F5:
        keys.append("F5");
        break;
    case Qt::Key_F6:
        keys.append("F6");
        break;
    case Qt::Key_F7:
        keys.append("F7");
        break;
    case Qt::Key_F8:
        keys.append("F8");
        break;
    case Qt::Key_F9:
        keys.append("F9");
        break;
    case Qt::Key_F10:
        keys.append("F10");
        break;
    case Qt::Key_F11:
        keys.append("F11");
        break;
    case Qt::Key_F12:
        keys.append("F12");
        break;
    case Qt::Key_F13:
        keys.append("F13");
        break;
    case Qt::Key_F14:
        keys.append("F14");
        break;
    case Qt::Key_F15:
        keys.append("F15");
        break;
    case Qt::Key_F16:
        keys.append("F16");
        break;
    case Qt::Key_F17:
        keys.append("F17");
        break;
    case Qt::Key_F18:
        keys.append("F18");
        break;
    case Qt::Key_F19:
        keys.append("F19");
        break;
    case Qt::Key_F20:
        keys.append("F20");
        break;
    case Qt::Key_F21:
        keys.append("F21");
        break;
    case Qt::Key_F22:
        keys.append("F22");
        break;
    case Qt::Key_F23:
        keys.append("F23");
        break;
    case Qt::Key_F24:
        keys.append("F24");
        break;
    case Qt::Key_F25:
        keys.append("F25");
        break;
    case Qt::Key_F26:
        keys.append("F26");
        break;
    case Qt::Key_F27:
        keys.append("F27");
        break;
    case Qt::Key_F28:
        keys.append("F28");
        break;
    case Qt::Key_F29:
        keys.append("F29");
        break;
    case Qt::Key_F30:
        keys.append("F30");
        break;
    case Qt::Key_F31:
        keys.append("F31");
        break;
    case Qt::Key_F32:
        keys.append("F32");
        break;
    case Qt::Key_F33:
        keys.append("F33");
        break;
    case Qt::Key_F34:
        keys.append("F34");
        break;
    case Qt::Key_F35:
        keys.append("F35");
        break;
    default: {
        const QString k = QKeySequence(keycode).toString();
        if(k != "")
            keys.append(k);
    }

    }

    QString ret = mods.join("+");
    if(ret != "")
        ret += "+";
    ret += keys.join("+");

    return ret;

}

QSize PQCScriptsOther::fitSizeInsideSize(int w, int h, int maxw, int maxh) {

    qDebug() << "args: w h maxw maxh =" << w << h << maxw << maxh;

    return QSize(w,h).scaled(QSize(maxw, maxh), Qt::KeepAspectRatio);

}

int PQCScriptsOther::toLcmsFormat(QImage::Format fmt) {

#ifdef PQMLCMS2
    switch (fmt) {

    case QImage::Format_ARGB32:  //  (0xAARRGGBB)
    case QImage::Format_RGB32:   //  (0xffRRGGBB)
        return TYPE_BGRA_8;

    case QImage::Format_RGB888:
        return TYPE_RGB_8;       // 24-bit RGB format (8-8-8).

    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
        return TYPE_RGBA_8;

    case QImage::Format_Grayscale8:
        return TYPE_GRAY_8;

    case QImage::Format_Grayscale16:
        return TYPE_GRAY_16;

    case QImage::Format_RGBA64:
    case QImage::Format_RGBX64:
        return TYPE_RGBA_16;

    case QImage::Format_BGR888:
        return TYPE_BGR_8;

    default:
        return 0;

    }
#endif

    return 0;

}

bool PQCScriptsOther::applyEmbeddedColorProfile(QImage &img) {

    qDebug() << "args: img";

#ifdef PQMLCMS2

    int lcms2SourceFormat = toLcmsFormat(img.format());

    QImage::Format targetFormat = img.format();
    // this format causes problems with lcms2
    // no error is caused but the resulting image is fully transparent
    // removing the alpha channel seems to fix this
    if(img.format() == QImage::Format_ARGB32)
        targetFormat = QImage::Format_RGB32;

    int lcms2targetFormat = toLcmsFormat(img.format());

    // Outputting an RGBA64 image with LCMS2 results in a blank rectangle.
    // Reading it seems to work just fine, however.
    // Thus we make sure to output the image in a working format here.
    if(img.format() == QImage::Format_RGBA64) {
        targetFormat = QImage::Format_RGB32;
        lcms2targetFormat = toLcmsFormat(QImage::Format_RGB32);
    }

    if(lcms2SourceFormat == 0 || lcms2targetFormat == 0) {
        qWarning() << "Unknown image format. Attempting to convert image to format known to LCMS2.";
        img.convertTo(QImage::Format_ARGB32);
        targetFormat = QImage::Format_RGB32;
        lcms2SourceFormat = toLcmsFormat(img.format());
        lcms2targetFormat = lcms2SourceFormat;
        if(img.isNull()) {
            qWarning() << "Error converting image to ARGB32. Not applying color profile.";
            return false;
        }
        if(lcms2targetFormat == 0) {
            qWarning() << "Unable to 'fix' image format. Not applying color profile.";
            return false;
        }
    }

    cmsHPROFILE targetProfile = cmsOpenProfileFromMem(img.colorSpace().iccProfile().constData(),
                                                      img.colorSpace().iccProfile().size());

    // Create a transformation from source (sRGB) to destination (provided ICC profile) color space
    cmsHTRANSFORM transform = cmsCreateTransform(targetProfile, lcms2SourceFormat, cmsCreate_sRGBProfile(), lcms2targetFormat, INTENT_PERCEPTUAL, 0);
    if (!transform) {
        // Handle error, maybe close profile and return original image or null image
        cmsCloseProfile(targetProfile);
        qWarning() << "Error creating transform for external color profile";
        return false;
    } else {

        // since the target format might not support alpha channels we use black instead of transparent to fill the initial image.
        // we don't have to fill the image for cmsDoTransform but it allows for additional checking whether cmsDoTransform succeeded.
        QImage ret(img.size(), targetFormat);
        ret.fill(Qt::black);

        // Perform color space conversion
        cmsDoTransform(transform, img.constBits(), ret.bits(), img.width() * img.height());

        // transform failed returning null image
        if(ret.isNull()) {
            qWarning() << "Failed to apply external color profile, null image returned";
            return false;
        }

        // check if image is all black -> transform failed
        bool allblack = true;
        for(int x = 0; x < img.width(); ++x) {
            for(int y = 0; y < img.height(); ++y) {
                if(ret.pixelColor(x,y).black() < 255) {
                    allblack = false;
                    break;
                }
            }
            if(!allblack) break;
        }

        if(allblack) {
            qWarning() << "Failed to apply external color profile, image completely black";
            return false;
        }

        const int bufSize = 100;
        char buf[bufSize];

#if LCMS_VERSION >= 2160
        cmsGetProfileInfoUTF8(targetProfile, cmsInfoDescription,
                              "en", "US",
                              buf, bufSize);
#else
        cmsGetProfileInfoASCII(targetProfile, cmsInfoDescription,
                               "en", "US",
                               buf, bufSize);
#endif

        // Release resources
        cmsDeleteTransform(transform);
        cmsCloseProfile(targetProfile);

        qDebug() << "Applying external color profile:" << buf;

        img = ret;

        return true;

    }

#endif

    return true;

}

QString PQCScriptsOther::getClipboardContents() {

    QClipboard *c = qApp->clipboard();
    return c->text();

}

void PQCScriptsOther::startDownloadOfFile(QString url) {

    qDebug() << "args: url =" << url;

    QString fname = QFileDialog::getSaveFileName(nullptr, "Save file as", QDir::homePath());
    if(fname == "") {
        qDebug() << "No file selected, not downloading file.";
        return;
    }

    if(downloadFile != nullptr)
        delete downloadFile;
    downloadFile = new QFile(fname);
    if(!downloadFile->open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to open file for writing. Not downloading video.";
        return;
    }

    const QNetworkRequest& request = QNetworkRequest(url);
    m_downloadReply = m_downloadManager.get(request);
    Q_EMIT downloadStarted();

    connect(m_downloadReply, &QNetworkReply::readyRead, this, [=]() {
        downloadFile->write(m_downloadReply->read(m_downloadReply->bytesAvailable()));
    });

    connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &PQCScriptsOther::downloadProgress);

    connect(m_downloadReply, &QNetworkReply::finished, this, [=]() {
        downloadFile->close();
        Q_EMIT downloadFinished();
    });

}

void PQCScriptsOther::cancelDownloadOfFile() {

    if(m_downloadReply != nullptr) {
        Q_EMIT downloadCancelled();
        m_downloadReply->abort();
    }

}
