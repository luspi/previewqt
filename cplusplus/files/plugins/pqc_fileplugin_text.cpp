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

#include <fileplugins/pqc_fileplugin_text.h>

PQCFilePluginText::PQCFilePluginText() {

    setData({
         {88823,
          {{"Text document"}, {"txt"}, {"text/plain", "text/x-csrc", "application/vnd.kde.kxmlguirc"}}},
         {11445,
          {{"JSON file"}, {"json"}, {"application/json"}}},
         {88736,
          {{"XML/HTML files"}, {"html", "xml", "xhtml", "htm"}, {"application/xml", "text/xml"}}},
         {22765,
          {{"YAML files"}, {"yaml", "yml"}, {"application/x-yaml"}}},
         {76245,
          {{"SQL scripts"}, {"sql"}, {"application/sql"}}},
         {33387,
          {{"JavaScript"}, {"js"}, {"application/javascript"}}},
         {62526,
          {{"Shell/Bash scripts"}, {"sh", "bash", "fsh"}, {"application/x-sh"}}},
         {33876,
          {{"PHP scripts"}, {"php"}, {"application/x-httpd-php"}}},
         {27654,
          {{"LaTex files"}, {"tex"}, {"application/x-latex"}}},
#ifndef PQMLIBREOFFICE
         {23222,
          {{"Rich Text Format"}, {"rtf"}, {"application/rtf"}}},
#endif
         {33233,
          {{"Troff files"}, {"troff", "t", "man"}, {"application/x-troff", "application/x-troff-man"}}},
         {23232,
          {{"Desktop file"}, {"desktop"}, {"application/x-desktop"}}}});

}

const QJsonObject PQCFilePluginText::loadJSON(QString path, QVariantMap extraArguments) {

    const QString suffix1 = QFileInfo(path).suffix().toLower();
    const QString suffix2 = QFileInfo(path).completeSuffix().toLower();
    QMimeDatabase db;
    QString mime = db.mimeTypeForFile(path).name();

    if(!getSuffixes().contains(suffix1) && !getSuffixes().contains(suffix2) && !getMimetypes().contains(mime))
        return {};

    QJsonObject json;
    json["supported"] = true;
    json["filename"] = QFileInfo(path).fileName();
    json["type"] = "text";
    json["mimetype"] = mime;
    json["loadpath"] = path;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        json["data"] = "(failed to open file)";
        return json;
    }

    QTextStream in(&file);
    json["data"] = in.readAll();

    return json;

}
