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

#include <fileplugins/pqc_fileplugin_sqlite.h>
#include <QCryptographicHash>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>

PQCFilePluginSQLite::PQCFilePluginSQLite() {

    setData({
        {22556,
         {{"SQLite database"}, {"db"}, {"application/vnd.sqlite3"}}}});

}

const QVariantList PQCFilePluginSQLite::loadData(QString path) {

    const QString dbname = QString("sqlite-%1").arg(QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Md5).toHex());

    if(QSqlDatabase::isDriverAvailable("QSQLITE3"))
        m_db = QSqlDatabase::addDatabase("QSQLITE3", dbname);
    else if(QSqlDatabase::isDriverAvailable("QSQLITE"))
        m_db = QSqlDatabase::addDatabase("QSQLITE", dbname);
    m_db.setDatabaseName(path);

    if(!m_db.open()) {
        const QString msg = "ERROR: Unable to open database file, cannot provide any info here...";
        qWarning() << msg;
        return {false, msg};
    }

    DatabaseInfo info;

    inspectDatabase(info);
    inspectTables(info);
    inspectTriggers(info);

    // first construct the table structures
    QStringList tableNames;
    QVariantList tableList;
    for(const TableInfo &table : std::as_const(info.tables)) {

        QVariantList columnList;
        for(const ColumnInfo &column : table.columns) {
            QVariantMap entry = {
                {"name", column.name},
                {"type", column.type},
                {"primaryKey", column.primaryKey},
                {"notNull", column.notNull},
                {"defaultValue", column.defaultValue}
            };
            columnList.append(entry);
        }

        QVariantList indexList;
        for(const IndexInfo &index : table.indexes) {
            QVariantMap entry = {
                {"name", index.name},
                {"unique", index.unique},
                {"columns", index.columns}
            };
            indexList.append(entry);
        }

        QVariantList foreignKeyList;
        for(const ForeignKeyInfo &fk : table.foreignKeys) {
            QVariantMap entry = {
                {"from", fk.from},
                {"to", QString(fk.table % "." % fk.to)},
                {"onDelete", fk.onDelete},
                {"onUpdate", fk.onUpdate}
            };
            foreignKeyList.append(entry);
        }

        QVariantMap tableMap = {
            {"name", table.name},
            {"withoutRowId", false},
            {"strict", false},
            {"rowCount", table.rowCount},
            {"columnCount", columnList.length()},
            {"columns", columnList},
            {"indexCount", indexList.length()},
            {"indexes", indexList},
            {"foreignKeyCount", foreignKeyList.length()},
            {"foreignKeys", foreignKeyList}
        };

        tableNames.append(table.name);
        tableList.append(tableMap);

    }

    QVariantList triggerList;
    for(const TriggerInfo &trigger : std::as_const(info.triggers)) {
        QVariantMap entry = {
            {"name", trigger.name},
            {"table", trigger.table}
        };
        triggerList.append(entry);
    }

    QVariantMap dat = {
        {"sqliteVersion", info.sqliteVersion},
        {"encoding", info.encoding},
        {"journalMode", info.journalMode},

        {"pageSize", info.pageSize},
        {"pageCount", info.pageCount},
        {"databaseSize", info.pageSize*info.pageCount},

        {"tableCount", info.tables.size()},
        {"tableNames", tableNames},
        {"tables", tableList},

        {"triggerCount", triggerList.length()},
        {"triggers", triggerList}
    };


    return {false, dat};
}

bool PQCFilePluginSQLite::execute(const QString sql, std::function<bool(QSqlQuery &)> callback) {

    QSqlQuery query(m_db);

    if(!query.exec(sql)) {
        qWarning() << "SQL error:" << query.lastError().text();
        qWarning() << "SQL:" << sql;
        return false;
    }

    return callback(query);

}

bool PQCFilePluginSQLite::querySingleValue(const QString sql, QVariant &value) {

    QSqlQuery query(m_db);

    if(!query.exec(sql)) {
        qWarning() << query.lastError().text();
        return false;
    }

    if(!query.next())
        return false;

    value = query.value(0);
    return true;

}

bool PQCFilePluginSQLite::inspectDatabase(DatabaseInfo &info) {

    QVariant value;

    // SQLite version
    if(querySingleValue("SELECT sqlite_version()", value))
        info.sqliteVersion = value.toString();

    // encoding
    if(querySingleValue("PRAGMA encoding", value))
        info.encoding = value.toString();

    // page size
    if(querySingleValue("PRAGMA page_size", value))
        info.pageSize = value.toLongLong();

    // number of pages
    if(querySingleValue("PRAGMA page_count", value))
        info.pageCount = value.toLongLong();

    // journal mode
    if(querySingleValue("PRAGMA journal_mode", value))
        info.journalMode = value.toString();

    // foreign keys
    if(querySingleValue("PRAGMA foreign_keys", value))
        info.foreignKeys = value.toBool();

    return true;
}

bool PQCFilePluginSQLite::inspectTables(DatabaseInfo &info) {

    QSqlQuery query(m_db);

    const QString sql = "SELECT name,sql FROM sqlite_schema WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name";

    if(!query.exec(sql)) {
        qWarning() << query.lastError().text();
        return false;
    }

    while(query.next()) {

        TableInfo table;

        table.name = query.value(0).toString();
        table.sql  = query.value(1).toString();

        table.withoutRowId = table.sql.contains("WITHOUT ROWID", Qt::CaseInsensitive);
        table.strict = table.sql.contains("STRICT", Qt::CaseInsensitive);

        if(!inspectTable(table))
            return false;

        info.tables.append(table);
    }

    return true;
}

bool PQCFilePluginSQLite::inspectTable(TableInfo &table) {

    QSqlQuery query(m_db);

    const QString tableName = escapeIdentifier(table.name);
    const QString sql = QString("PRAGMA table_xinfo(%1)").arg(tableName);

    if(!query.exec(sql)) {
        qWarning() << query.lastError().text();
        return false;
    }

    while(query.next()) {

        ColumnInfo column;

        column.name         = query.value(1).toString();
        column.type         = query.value(2).toString();
        column.notNull      = query.value(3).toInt() != 0;
        column.defaultValue = query.value(4);
        column.primaryKey   = query.value(5).toInt() != 0;
        const int hidden    = query.value(6).toInt(); // table_xinfo has a hidden column at index 6
        column.hidden       = hidden != 0;
        column.generated    = hidden == 2 || hidden == 3;
        table.columns.append(column);

    }

    if(!inspectIndexes(table))
        return false;

    if(!inspectForeignKeys(table))
        return false;

    // exact row count
    QSqlQuery countQuery(m_db);
    const QString countSql = QString("SELECT COUNT(*) FROM %1").arg(escapeIdentifier(table.name));
    if(countQuery.exec(countSql) && countQuery.next())
        table.rowCount = countQuery.value(0).toLongLong();

    return true;

}

QString PQCFilePluginSQLite::escapeIdentifier(const QString identifier) const {
    return m_db.driver()->escapeIdentifier(identifier, QSqlDriver::TableName);
}

bool PQCFilePluginSQLite::inspectIndexes(TableInfo &table) {

    QSqlQuery query(m_db);

    const QString sql = QString("PRAGMA index_list(%1)").arg(escapeIdentifier(table.name));

    if(!query.exec(sql)) {
        qWarning() << query.lastError().text();
        return false;
    }

    while(query.next()) {

        IndexInfo index;

        // index_list:
        // seq, name, unique, origin, partial

        index.name    = query.value(1).toString();
        index.unique  = query.value(2).toInt() != 0;

        QSqlQuery indexQuery(m_db);
        const QString indexSql = QString("PRAGMA index_info(%1)").arg(escapeIdentifier(index.name));
        if(!indexQuery.exec(indexSql)) {
            qWarning() << indexQuery.lastError().text();
            return false;
        }

        while(indexQuery.next()) {
            // seqno, cid, name
            QString columnName = indexQuery.value(2).toString();
            index.columns.append(columnName);
        }

        table.indexes.append(index);

    }

    return true;
}

bool PQCFilePluginSQLite::inspectForeignKeys(TableInfo &table) {

    QSqlQuery query(m_db);

    const QString sql = QString("PRAGMA foreign_key_list(%1)").arg(escapeIdentifier(table.name));

    if(!query.exec(sql)) {
        qWarning() << query.lastError().text();
        return false;
    }

    while(query.next()) {

        ForeignKeyInfo fk;

        // foreign key infos:
        // id, sequence,table, local column, referenced column, onUpdate, onDelete, match

        fk.table    = query.value(2).toString(); // referenced table
        fk.from     = query.value(3).toString(); // local column
        fk.to       = query.value(4).toString(); // referenced column
        fk.onUpdate = query.value(5).toString();
        fk.onDelete = query.value(6).toString();

        table.foreignKeys.append(fk);
    }

    return true;
}

bool PQCFilePluginSQLite::inspectTriggers(DatabaseInfo &info) {

    QSqlQuery query(m_db);

    const QString sql = "SELECT name,tbl_name,sql FROM sqlite_schema WHERE type='trigger' ORDER BY name";

    if(!query.exec(sql)) {
        qWarning() << query.lastError().text();
        return false;
    }

    while(query.next()) {

        TriggerInfo trigger;

        trigger.name  = query.value(0).toString();
        trigger.table = query.value(1).toString();

        info.triggers.append(trigger);
    }

    return true;
}
