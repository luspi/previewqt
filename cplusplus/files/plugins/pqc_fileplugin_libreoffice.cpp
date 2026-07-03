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

#include <fileplugins/pqc_fileplugin_libreoffice.h>

#ifdef PQMLIBREOFFICE
#define LOK_USE_UNSTABLE_API
#include <LibreOfficeKit/LibreOfficeKit.hxx>
#include <LibreOfficeKit/LibreOfficeKitEnums.h>
#endif

PQCFilePluginLibreOffice::PQCFilePluginLibreOffice() {

#ifdef PQMLIBREOFFICE
    setData({
        {99975, {{"Microsoft Word 97–2003"}, {"doc"}, {"application/msword"}}},
        {99974, {{"Microsoft Word"}, {"docx"}, {"application/vnd.openxmlformats-officedocument.wordprocessingml.document"}}},
        {99976, {{"OpenDocument Text"}, {"odt"}, {"application/vnd.oasis.opendocument.text"}}},
        {99973, {{"Template"}, {"ott"}, {"application/vnd.oasis.opendocument.text-template"}}},
        {99972, {{"Flat XML"}, {"fodt"}, {"application/vnd.oasis.opendocument.text-flat-xml"}}},
        {99971, {{"Word template"}, {"dot", "dotx"}, {"application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.template"}}},
        {23222, {{"Rich Text Format"}, {"rtf"}, {"application/rtf"}}},
        {99970, {{"OpenDocument Spreadsheet"}, {"ods"}, {"application/vnd.oasis.opendocument.spreadsheet"}}},
        {99969, {{"Excel 97–2003"}, {"xls"}, {"application/vnd.ms-excel"}}},
        {99968, {{"Excel"}, {"xlsx"}, {"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"}}},
        {99967, {{"Excel Macro-enabled"}, {"xlsm"}, {"application/vnd.ms-excel.sheet.macroEnabled.12"}}},
        {99966, {{"Excel Template"}, {"xlt", "xltx"}, {"application/vnd.ms-excel,application/vnd.openxmlformats-officedocument.spreadsheetml.template"}}},
        {99965, {{"OpenDocument Presentation"}, {"odp"}, {"application/vnd.oasis.opendocument.presentation"}}},
        {99964, {{"OpenDocument Presentation Template"}, {"otp"}, {"application/vnd.oasis.opendocument.presentation-template"}}},
        {99963, {{"OpenDocument Presentation Flat XML"}, {"fodp"}, {"application/vnd.oasis.opendocument.presentation-flat-xml"}}},
        {99962, {{"PowerPoint 97–2003"}, {"ppt"}, {"application/vnd.ms-powerpoint"}}},
        {99961, {{"PowerPoint"}, {"pptx"}, {"application/vnd.openxmlformats-officedocument.presentationml.presentation"}}},
        {99960, {{"PowerPoint Macro-enabled"}, {"pptm"}, {"application/vnd.ms-powerpoint.presentation.macroEnabled.12"}}},
        {99959, {{"PowerPoint Template"}, {"pot", "potx"}, {"application/vnd.ms-powerpoint", "application/vnd.openxmlformats-officedocument.presentationml.template"}}},
        {99958, {{"OpenDocument Drawing"}, {"odg"}, {"application/vnd.oasis.opendocument.graphics"}}},
        {99957, {{"OpenDocument Drawing Template"}, {"otg"}, {"application/vnd.oasis.opendocument.graphics-template"}}},
        {99956, {{"OpenDocument Drawing Flat XML"}, {"fodg"}, {"application/vnd.oasis.opendocument.graphics-flat-xml"}}},
        {99955, {{"Visio Documents"}, {"vsd", "vsdx"}, {"application/vnd.visio", "application/vnd.ms-visio.drawing"}}},
        {99954, {{"Visio Stencils"}, {"vss"}, {"application/vnd.ms-office"}}},
        {99953, {{"Visio Template"}, {"vst", "vstx"}, {"application/vnd.ms-office"}}}
    });

    m_suffixesWithNoFixedSize << "ods" << "xls" << "xlsx" << "xlsm" << "xlst" << "xltx";

    office = lok::lok_cpp_init(PQMLIBREOFFICE_LOPATH);

#endif

}

PQCFilePluginLibreOffice::~PQCFilePluginLibreOffice() {
#ifdef PQMLIBREOFFICE
    delete office;
#endif
}

const QSize PQCFilePluginLibreOffice::loadSize(QString path) {

#ifdef PQMLIBREOFFICE

    QSize sze;
    if(loadSizeFromCache(path, sze))
        return sze;

    // extract page and totalpage value from path (prepended to path (after filepath))
    const int idx = path.indexOf("::DOC::");
    if(idx != -1)
        path = path.mid(idx+7);

    lok::Document *lodoc = office->documentLoad(path.toStdString().c_str());

    long w = 0, h = 0;
    lodoc->getDocumentSize(&w, &h);

    delete lodoc;

    return QSize(w, h);

#endif

    return QSize();

}

const QImage PQCFilePluginLibreOffice::loadImage(QString path, QSize requestedSize, QSize &origSize, QString &error) {

#ifdef PQMLIBREOFFICE

    const QString origPath = path;

    QFileInfo info(path);
    const QString suffix = info.suffix().toLower();

    QImage cch;
    if(loadImageFromCache(path, cch, requestedSize))
        return cch.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // extract page and totalpage value from path (prepended to path (after filepath))
    int page = 0;
    const int idx = path.indexOf("::DOC::");
    if(idx != -1) {
        page = path.mid(0,idx).toInt();
        path = path.mid(idx+7);
    }

    lok::Document *lodoc = office->documentLoad(path.toStdString().c_str());

    lodoc->setPart(page);

    lodoc->setClientZoom(100, 100, requestedSize.width(), requestedSize.height());

    long pageWidthTwips;
    long pageHeightTwips;

    lodoc->getDocumentSize(&pageWidthTwips, &pageHeightTwips);

    if(m_suffixesWithNoFixedSize.contains(suffix)) {
        QSize maxSize = QSize(10000, 10000*(static_cast<double>(requestedSize.height())/static_cast<double>(requestedSize.width())));
        pageWidthTwips = qMin(maxSize.width(), pageWidthTwips);
        pageHeightTwips = qMin(maxSize.height(), pageHeightTwips);
    }

    // we render to 1/4 of the twips size
    // a typical twips size for an A4 page is 11906x16838
    // rendering it to roughly a quarter of that equates to about 300 dpi (not exact, but close enough)
    // if we immediately go to the requestedSize (potentially much smaller), then the result tends to be blurry
    QSize pageSize(pageWidthTwips, pageHeightTwips);
    QImage img(pageSize, QImage::Format_ARGB32);
    img.fill(Qt::white);

    lodoc->paintTile(reinterpret_cast<unsigned char*>(img.bits()),
                     img.width(), img.height(),
                     0, 0, pageWidthTwips, pageHeightTwips);

    delete lodoc;

    saveImageToCache(origPath, img);

    origSize = img.size();

    if(img.width() != requestedSize.width() || img.height() != requestedSize.height())
        return img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return img;

#endif

    return QImage();

}