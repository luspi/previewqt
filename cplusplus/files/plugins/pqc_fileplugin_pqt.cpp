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

#include <fileplugins/pqc_fileplugin_pqt.h>

#ifdef PQMLIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTemporaryFile>

PQCFilePluginPQT::PQCFilePluginPQT() {

#ifdef PQMLIBARCHIVE
    setData({
             {78785,
              {{"PhotoQt config file"}, {"pqt"}, {}}},
             {78786,
              {{"PhotoQt Extension file"}, {"pqe"}, {}}}});
#endif

}

const QVariantList PQCFilePluginPQT::loadData(QString path) {

    const QString suffix = QFileInfo(path).suffix().toLower();

    if(suffix == "pqe") {

        QVariantList ret = PQCHelper::extractFileFromArchive(path, "", "", "definition.yml");

        if(!ret.length())
            return {false, "No data found"};

        if(!ret[0].toBool())
            return ret;

        QVariantMap dat = {
            {"version", "?"},
            {"name", "?"},
            {"description", "?"},
            {"author", "?"},
            {"contact", "?"},
            {"website", "?"},
            {"targetAPI", "?"},
        };

        const QString definition = QString::fromUtf8(ret[1].toByteArray());

        for(auto i = dat.cbegin(), end = dat.cend(); i != end; ++i) {
            if(definition.contains(i.key() % ":")) {
                QString val = definition.split(i.key() % ":")[1].split("\n")[0].trimmed();
                if(val.startsWith("\"") && val.endsWith("\"")) val = val.sliced(1, val.length()-2);
                dat[i.key()] = val;
            }
        }

        return {true, dat};

    } else if(suffix == "pqt") {

        const QStringList fileList = PQCHelper::extractFileListArchive(path);

        const QVariantList fileExtract = PQCHelper::extractFileFromArchive(path, "CFG_USERSETTINGS_DB");
        if(!fileExtract.length())
            return {false, "No data found"};
        if(!fileExtract[0].toBool())
            return fileExtract;

        const QByteArray settingsDB = fileExtract[1].toByteArray();

        QTemporaryFile tempFile;
        if(!tempFile.open()) {
            const QString msg = "Unable to create temporary file";
            qWarning() << msg;
            return {false, msg};
        }
        tempFile.write(settingsDB);

        QSqlDatabase db;
        if(QSqlDatabase::isDriverAvailable("QSQLITE3"))
            db = QSqlDatabase::addDatabase("QSQLITE3", QString("usersettings-%1").arg(QDateTime::currentSecsSinceEpoch()));
        else if(QSqlDatabase::isDriverAvailable("QSQLITE"))
            db = QSqlDatabase::addDatabase("QSQLITE", QString("usersettings-%1").arg(QDateTime::currentSecsSinceEpoch()));
        db.setDatabaseName(tempFile.fileName());

        if(!db.open()) {
            const QString msg = "Unable to open temporary SQLite database";
            qWarning() << msg;
            return {false, msg};
        }

        QString versionNumber = "";

        QSqlQuery query(db);
        if(query.exec("SELECT `value` FROM `general` WHERE `name`='Version'")) {
            query.next();
            versionNumber = query.value(0).toString();
        } else {
            qWarning() << "Unable to execute query:" << query.lastError().text();
        }
        query.clear();
        db.close();

        QVariantMap dat = {{"filelist", fileList},
                           {"version", versionNumber}};

        return {true, dat};

    }

    return {false, "Unsupported file type. How did you get here?"};

}

const QJsonObject PQCFilePluginPQT::loadJSON(QString path, QVariantMap extraArguments) {

    const QString suffix1 = QFileInfo(path).suffix().toLower();
    const QString suffix2 = QFileInfo(path).completeSuffix().toLower();
    QMimeDatabase db;
    QString mime = db.mimeTypeForFile(path).name();

    if(!getSuffixes().contains(suffix1) && !getSuffixes().contains(suffix2) && !getMimetypes().contains(mime))
        return {};

    QVariantList data = loadData(path);
    if(!data.length() || !data[0].toBool())
        return {};

    QJsonObject json;
    json["supported"] = true;
    json["filename"] = QFileInfo(path).fileName();
    json["type"] = "photoqt";
    json["mimetype"] = mime;
    json["loadpath"] = path;

    QJsonObject metadata;

    QMapIterator<QString, QVariant> i(data[1].toMap());
    while(i.hasNext()) {
        i.next();
        if(i.key() == "filelist")
            metadata[i.key()] = QJsonValue::fromVariant(i.value().toStringList());
        else
            metadata[i.key()] = i.value().toString();
    }

    json["data"] = metadata;

    return json;

}
