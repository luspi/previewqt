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

#ifdef PQMLIBREOFFICEKIT
namespace lok { class Office; }
#endif

class PQCFilePluginLibreOffice : public PQCFilePlugin {

public:
    PQCFilePluginLibreOffice();
    ~PQCFilePluginLibreOffice();

    const QString name() override { return "LibreOffice"; }
    const QSize loadSize(QString path) override;
    const QImage loadImage(QString path, QSize requestedSize, QSize &origSize, QString &error) override;
    const QVariantList loadData(QString path) override { return {}; }
    const int loadNumPages(QString path) override;
    const QStringList loadContent(QString path) override { return {path}; }
    const QJsonObject loadJSON(QString path, QVariantMap extraArguments) override;

private:
#ifdef PQMLIBREOFFICEKIT
    lok::Office *office;
#endif

#ifdef PQMLIBREOFFICE
    QString m_tempDocumentPath;
    QString m_currentDocument;
    bool loadDocument(QString path);
#endif

    QStringList m_suffixesWithNoFixedSize;

};
