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

#include <fileplugins/pqc_fileplugin_ebook.h>

PQCFilePluginEBook::PQCFilePluginEBook() {

#ifdef PQMLIBARCHIVE
    setData({
         {45132,
            {{"Electronic Publication (EPUB)"}, {"epub"}, {"application/epub+zip"}}},
    });
#endif

}

const QJsonObject PQCFilePluginEBook::loadJSON(QString path, QVariantMap extraArguments) {

    const QString suffix1 = QFileInfo(path).suffix().toLower();
    const QString suffix2 = QFileInfo(path).completeSuffix().toLower();
    QMimeDatabase db;
    QString mime = db.mimeTypeForFile(path).name();

    if(!getSuffixes().contains(suffix1) && !getSuffixes().contains(suffix2) && !getMimetypes().contains(mime))
        return {};

    QJsonObject json;
    json["supported"] = true;
    json["filename"] = QFileInfo(path).fileName();
    json["type"] = "ebook";
    json["mimetype"] = mime;
    json["loadpath"] = path;

    return json;

}
