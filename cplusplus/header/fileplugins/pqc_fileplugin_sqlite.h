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
#pragma once

#include <fileplugins/pqc_fileplugin.h>
#include <QSet>
#include <QSize>
#include <QImage>
#include <QtSql/QSqlDatabase>

class PQCFilePluginSQLite : public PQCFilePlugin {

public:
    PQCFilePluginSQLite();

    const QString name() override { return "SQLite"; }
    const QSize loadSize(QString) override { return QSize(); };
    const QImage loadImage(QString, QSize, QSize&, QString&) override { return QImage(); };
    const QVariantList loadData(QString path) override;
    const int loadNumPages(QString path) override { return 1; }
    const QStringList loadContent(QString path) override { return {path}; }

private:

    struct ColumnInfo {
        QString name;
        QString type;
        bool notNull = false;
        QVariant defaultValue;
        bool primaryKey = false;
        bool hidden = false;
        bool generated = false;
    };

    struct IndexInfo {
        QString name;
        bool unique = false;
        QStringList columns;
    };

    struct ForeignKeyInfo {
        QString table;
        QString from;
        QString to;

        QString onUpdate;
        QString onDelete;
    };

    struct TriggerInfo {
        QString name;
        QString table;
    };

    struct TableInfo {
        QString name;
        QString sql;

        bool withoutRowId = false;
        bool strict = false;

        qint64 rowCount = -1;

        QVector<ColumnInfo> columns;
        QVector<IndexInfo> indexes;
        QVector<ForeignKeyInfo> foreignKeys;
    };

    struct DatabaseInfo {
        QString sqliteVersion;
        QString encoding;
        QString journalMode;

        qint64 pageSize = 0;
        qint64 pageCount = 0;

        bool foreignKeys = false;

        QVector<TableInfo> tables;
        QVector<TriggerInfo> triggers;
    };

    QSqlDatabase m_db;

    bool querySingleValue(const QString sql, QVariant &value);

    bool inspectDatabase(DatabaseInfo &info);
    bool inspectTables(DatabaseInfo &info);
    bool inspectTable(TableInfo &table);
    bool inspectIndexes(TableInfo &table);
    bool inspectForeignKeys(TableInfo &table);
    bool inspectTriggers(DatabaseInfo &info);

    QString escapeIdentifier(const QString identifier) const;

    bool execute(const QString sql, std::function<bool(QSqlQuery &)> callback);

};
