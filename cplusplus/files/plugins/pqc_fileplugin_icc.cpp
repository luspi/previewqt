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

#include <fileplugins/pqc_fileplugin_icc.h>
#include <QPainter>
#include <QPainterPath>
#include <QBuffer>

PQCFilePluginICC::PQCFilePluginICC() {

#ifdef PQMLCMS2
    setData({
        {21223,
         {{"ICC profile"}, {"icc", "icm"}, {"application/vnd.iccprofile"}}},
        });
#endif

}

const QVariantList PQCFilePluginICC::loadData(QString path) {

    QVariantList ret;

#ifdef PQMLCMS2

    QVariantMap data;

    QByteArray fullpath = QFile::encodeName(path);
    cmsHPROFILE profile = cmsOpenProfileFromFile(fullpath.constData(), "r");

    if(!profile) {
        qWarning() << "Unable to create imported color profile:" << fullpath;
        return {false, "Failed to create LCMS2 profile"};
    }

    QVariantList toGet = {
        "description", cmsInfoDescription,
        "copyright", cmsInfoCopyright,
        "model", cmsInfoModel,
        "manufacturer", cmsInfoManufacturer
    };

    for(int i = 0; i < toGet.length(); i += 2) {

        const int bufSize = 256;
        char buf[bufSize];

#if LCMS_VERSION >= 2160
        cmsGetProfileInfoUTF8(profile, static_cast<cmsInfoType>(toGet.value(i+1).toInt()),
                              cmsNoLanguage, cmsNoCountry,
                              buf, bufSize);
#else
        cmsGetProfileInfoASCII(profile, static_cast<cmsInfoType>(toGet.value(i+1).toInt()),
                               cmsNoLanguage, cmsNoCountry,
                               buf, bufSize);
#endif


        data[toGet.value(i).toString()] = QString::fromUtf8(buf);

    }

    cmsColorSpaceSignature sig = cmsGetColorSpace(profile);
    data["signature"] = colorSpaceSignatureToString(sig);

    cmsProfileClassSignature cls = cmsGetDeviceClass(profile);
    data["class"] = colorSpaceClassToString(cls);

    data["version"] = cmsGetProfileVersion(profile);

    data["renderingIntent"] = renderingIntentsToString(cmsGetHeaderRenderingIntent(profile));

    cmsUInt32Number size = 0;
    if(cmsSaveProfileToMem(profile, NULL, &size))
        data["profileSize"] = size;

    QVariantList redXY;
    QVariantList greenXY;
    QVariantList blueXY;

    if(data["signature"] == "RGB") {

        // get whitepoint and rgb x/y coordinates
        cmsCIEXYZ *whitepoint = static_cast<cmsCIEXYZ *>(cmsReadTag(profile, cmsSigMediaWhitePointTag));
        cmsCIEXYZ *redXYZ = static_cast<cmsCIEXYZ *>(cmsReadTag(profile, cmsSigRedColorantTag));
        cmsCIEXYZ *greenXYZ = static_cast<cmsCIEXYZ *>(cmsReadTag(profile, cmsSigGreenColorantTag));
        cmsCIEXYZ *blueXYZ = static_cast<cmsCIEXYZ *>(cmsReadTag(profile, cmsSigBlueColorantTag));
        if(whitepoint != NULL && redXYZ != NULL && greenXYZ != NULL && blueXYZ != NULL) {
            data["whitepoint"] = QVariantList({whitepoint->X, whitepoint->Y, whitepoint->Z});
            redXY = convertXYZ2XY(redXYZ);
            greenXY = convertXYZ2XY(greenXYZ);
            blueXY = convertXYZ2XY(blueXYZ);
            data["redXY"] = redXY;
            data["greenXY"] = greenXY;
            data["blueXY"] = blueXY;
        }

        // a plain empty gammut plot is available in the resources
        QImage gammut(":/gammut.jpg");

        // size of image
        // these are HARDCODED currently
        // changing these requires updating the gammut.jpg above
        const int sze = 1024;
        const int brd = 100;

        // the size of the plot points
        const int circleR = 15;
        const int circleD = 30;

        // start painting
        QPainter painter(&gammut);

        // general pen and font settings
        QPen pen;
        pen.setColor(Qt::black);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidth(1);
        QFont font;
        font.setPixelSize(sze/30);
        font.setBold(true);
        QFontMetrics mtrcs(font);

        // GAMMUT TRIANGLE
        // fill gammut triangle
        const int refSze = sze-2*brd;
        const int rx =     brd+redXY[0]  .toDouble()*refSze;
        const int ry = sze-brd-redXY[1]  .toDouble()*refSze;
        const int gx =     brd+greenXY[0].toDouble()*refSze;
        const int gy = sze-brd-greenXY[1].toDouble()*refSze;
        const int bx =     brd+blueXY[0] .toDouble()*refSze;
        const int by = sze-brd-blueXY[1] .toDouble()*refSze;
        QPainterPath path;
        path.moveTo(rx, ry);
        path.lineTo(gx, gy);
        path.lineTo(bx, by);
        path.lineTo(rx, ry);
        QColor col(50,50,50,100);
        painter.fillPath(path, QBrush(col));
        // draw gammut triangle outline
        static const QPointF gammutPoints[4] = {
            QPointF(rx, ry),
            QPointF(gx, gy),
            QPointF(bx, by),
            QPointF(rx, ry),
        };
        painter.drawPolyline(gammutPoints, 4);

        // draw RGB points (circles and labels)
        pen.setWidth(1);
        font.setPointSize(sze/20);
        painter.setFont(font);
        mtrcs = QFontMetrics(font);
        // "R"
        pen.setWidth(1);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.setBrush(QBrush(qRgb(255,50,50)));
        painter.drawEllipse(rx-circleR, ry-circleR, circleD, circleD);
        const QString rTxt = "R";
        QRect rR = mtrcs.tightBoundingRect(rTxt);
        QPainterPath rPath;
        rPath.addText(rx+circleD, ry+rR.height()/2, font, rTxt);
        painter.setBrush(Qt::black);
        pen.setWidth(2);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawPath(rPath);
        // "G"
        pen.setWidth(1);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.setBrush(QBrush(qRgb(50,255,50)));
        painter.drawEllipse(gx-circleR, gy-circleR, circleD, circleD);
        const QString gTxt = "G";
        QRect gR = mtrcs.tightBoundingRect(gTxt);
        QPainterPath gPath;
        gPath.addText(gx-gR.width()/2, gy-circleD, font, gTxt);
        painter.setBrush(Qt::black);
        pen.setWidth(2);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawPath(gPath);
        // "B"
        pen.setWidth(1);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.setBrush(QBrush(qRgb(50,50,255)));
        painter.drawEllipse(bx-circleR, by-circleR, circleD, circleD);
        const QString bTxt = "B";
        QRect bR = mtrcs.tightBoundingRect(bTxt);
        QPainterPath bPath;
        bPath.addText(bx-bR.width(), by+bR.height()+circleD, font, bTxt);
        painter.setBrush(Qt::black);
        pen.setWidth(2);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawPath(bPath);

        // done painting
        painter.end();

        // rescale to smaller size
        gammut = gammut.scaledToWidth(300, Qt::SmoothTransformation);

        // encode to base64-encoded string
        QByteArray bytes;
        QBuffer buffer(&bytes);
        if(!buffer.open(QIODevice::WriteOnly)) {
            qDebug() << "Unable to open buffer for gammut plot";
        } else {
            if(!gammut.save(&buffer, "PNG"))
                qDebug() << "unable to save gammut plot as png to buffer";
            else
                data["gammut"] = QString::fromLatin1(bytes.toBase64());
        }

    }

    // done reading data
    cmsCloseProfile(profile);

    ret = {true, data};

#endif

    return ret;

}

#ifdef PQMLCMS2
const QString PQCFilePluginICC::colorSpaceSignatureToString(cmsColorSpaceSignature &sign) {

    if(sign == cmsSigXYZData)
        return "XYZ";
    if(sign == cmsSigLabData)
        return "Lab";
    if(sign == cmsSigLuvData)
        return "Luv";
    if(sign == cmsSigYCbCrData)
        return "YCbr";
    if(sign == cmsSigYxyData)
        return "Yxy";
    if(sign == cmsSigRgbData)
        return "RGB";
    if(sign == cmsSigGrayData)
        return "GRAY";
    if(sign == cmsSigHsvData)
        return "HSV";
    if(sign == cmsSigHlsData)
        return "HLS";
    if(sign == cmsSigCmykData)
        return "CMYK";
    if(sign == cmsSigCmyData)
        return "CMY";
    if(sign == cmsSigMCH1Data)
        return "MCH1";
    if(sign == cmsSigMCH2Data)
        return "MCH2";
    if(sign == cmsSigMCH3Data)
        return "MCH3";
    if(sign == cmsSigMCH4Data)
        return "MCH4";
    if(sign == cmsSigMCH5Data)
        return "MCH5";
    if(sign == cmsSigMCH6Data)
        return "MCH6";
    if(sign == cmsSigMCH7Data)
        return "MCH7";
    if(sign == cmsSigMCH8Data)
        return "MCH8";
    if(sign == cmsSigMCH9Data)
        return "MCH9";
    if(sign == cmsSigMCHAData)
        return "MCHA";
    if(sign == cmsSigMCHBData)
        return "MCHB";
    if(sign == cmsSigMCHCData)
        return "MCHC";
    if(sign == cmsSigMCHDData)
        return "MCHD";
    if(sign == cmsSigMCHEData)
        return "MCHE";
    if(sign == cmsSigMCHFData)
        return "MCHF";
    if(sign == cmsSigNamedData)
        return "nmcl";
    if(sign == cmsSig1colorData)
        return "1CLR";
    if(sign == cmsSig2colorData)
        return "2CLR";
    if(sign == cmsSig3colorData)
        return "3CLR";
    if(sign == cmsSig4colorData)
        return "4CLR";
    if(sign == cmsSig5colorData)
        return "5CLR";
    if(sign == cmsSig6colorData)
        return "6CLR";
    if(sign == cmsSig7colorData)
        return "7CLR";
    if(sign == cmsSig8colorData)
        return "8CLR";
    if(sign == cmsSig9colorData)
        return "9CLR";
    if(sign == cmsSig10colorData)
        return "ACLR";
    if(sign == cmsSig11colorData)
        return "BCLR";
    if(sign == cmsSig12colorData)
        return "CCLR";
    if(sign == cmsSig13colorData)
        return "DCLR";
    if(sign == cmsSig14colorData)
        return "ECLR";
    if(sign == cmsSig15colorData)
        return "FCLR";
    if(sign == cmsSigLuvKData)
        return "LuvK";

    return QString("Unknown (%1)").arg(sign);

}

const QString PQCFilePluginICC::colorSpaceClassToString(cmsProfileClassSignature &sign) {

    if(sign == cmsSigInputClass)
        return "Input Device";
    if(sign == cmsSigDisplayClass)
        return "Display Device";
    if(sign == cmsSigOutputClass)
        return "Output Device";
    if(sign == cmsSigLinkClass)
        return "DeviceLink";
    if(sign == cmsSigAbstractClass)
        return "Abstract";
    if(sign == cmsSigColorSpaceClass)
        return "Color Space";
    if(sign == cmsSigNamedColorClass)
        return "Named Color";

    if(sign == cmsSigColorEncodingSpaceClass)
        return "Color Encoding Space";
    if(sign == cmsSigMultiplexIdentificationClass)
        return "Multiplex Identification";
    if(sign == cmsSigMultiplexLinkClass)
        return "Multiplex Link";
    if(sign == cmsSigMultiplexVisualizationClass)
        return "Multiplex Visualization";

    return "";

}

const QString PQCFilePluginICC::renderingIntentsToString(int intent) {

    if(intent == 0)
        return "Perceptual";
    if(intent == 1)
        return "Relative Colorimetric";
    if(intent == 2)
        return "Saturation";
    if(intent == 3)
        return "Absolute Colorimetric";
    if(intent == 10)
        return "Perceptual - preserve K only";
    if(intent == 11)
        return "Relative Colorimetric - preserve K only";
    if(intent == 12)
        return "Saturation - preserve K only";
    if(intent == 13)
        return "Perceptual - preserve K plane";
    if(intent == 14)
        return "Relative Colorimetric - preserve K plane";
    if(intent == 15)
        return "Saturation - preserve K plane";

    return QString("Unknown (%1)").arg(intent);

}

QVariantList PQCFilePluginICC::convertXYZ2XY(const cmsCIEXYZ *xyz) {

    double sum = xyz->X + xyz->Y + xyz->Z;

    if(sum == 0.0)
        return {0,0};

    return {xyz->X/sum, xyz->Y/sum};

}

#endif
