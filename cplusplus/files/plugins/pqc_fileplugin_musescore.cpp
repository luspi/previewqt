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

#include <fileplugins/pqc_fileplugin_musescore.h>
#include <pqc_filehandler.h>
#include <pqc_settingscpp.h>
#include <QtDebug>
#include <QProcess>
#include <QCryptographicHash>

PQCFilePluginMuseScore::PQCFilePluginMuseScore() {

    setData({
        {55432,
             {{"MuseScore score file"}, {"mscz"}, {"application/x-musescore"}}},
        });

    musescoreProcess = new QProcess;
    musescoreCommand = "";
    musescoreTempDir = "";

}

const QVariantList PQCFilePluginMuseScore::loadData(QString path) {

    qDebug() << "args: path =" << path;

    if(PQCSettingsCPP::get().getCustomMuseScore()) {

        musescoreCommand = "::" % PQCSettingsCPP::get().getCustomMuseScorePath();

    } else if(musescoreCommand == "" || musescoreCommand.startsWith("::")) {

#ifdef Q_OS_WIN

        // check for executable in PATH
        QProcess which;
        which.setStandardOutputFile(QProcess::nullDevice());
        which.start("C:/Program Files/MuseScore 4/mscore.exe", {"--version"});
        if(!which.waitForStarted()) {
            const QString msg = "MuseScore not found";
            qWarning() << msg;
            return {false, msg};
        } else {
            musescoreCommand = "system";
        }

#else

        // check for executable in PATH
        QProcess which;
        which.setStandardOutputFile(QProcess::nullDevice());
        which.start("mscore", {"--version"});
        if(!which.waitForStarted()) {
            qDebug() << "mscore not found, trying flatpak";
        } else {
            musescoreCommand = "system";
        }

        if(musescoreCommand == "") {

            QProcess which2;
            which2.setStandardOutputFile(QProcess::nullDevice());
            which2.start("flatpak", {"run", "org.musescore.MuseScore", "--version"});
            if(!which2.waitForStarted()) {
                qDebug() << "mscore flatpak not found";
            } else {
                if(!which2.waitForFinished())
                    qDebug() << "mscore flatpak exited abnormally";
                musescoreCommand = "flatpak";
            }

        }
#endif
    }

    qDebug() << "using musescoreCommand =" << musescoreCommand;

    if(musescoreCommand == "") {
        const QString msg = "MuseScore was not found. If it is installed, however, then you might need to specify the path where it can be found in the settings.";
        qWarning() << msg;
        return {false, msg};
    }

    delete musescoreProcess;
    musescoreProcess = new QProcess;
    musescoreProcess->setStandardOutputFile(QProcess::nullDevice());

    if(musescoreTempDir.startsWith(PQCConfigFiles::get().CACHE_DIR() % "/mscz-"))
        QDir(musescoreTempDir).removeRecursively();
    musescoreTempDir = PQCConfigFiles::get().CACHE_DIR() % "/mscz-" % QString::fromUtf8(QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Md5).toHex());
    QDir(musescoreTempDir).mkdir(musescoreTempDir);

    if(musescoreCommand.startsWith("::")) {

        musescoreProcess->start(musescoreCommand.sliced(2), {"--export-to", musescoreTempDir % "/"%"page.svg", path});

    } else {

#ifdef Q_OS_WIN
        musescoreProcess->start("C:/Program Files/MuseScore 4/mscore.exe", {"--export-to", musescoreTempDir % "/"%"page.svg", path});
#else
        if(musescoreCommand == "system")
            musescoreProcess->start("mscore", {"--export-to", musescoreTempDir % "/"%"page.svg", path});
        else if(musescoreCommand == "flatpak")
            musescoreProcess->start("flatpak", {"run", "org.musescore.MuseScore", "--export-to", musescoreTempDir % "/" % "page.svg", path});
#endif

    }

    if(!musescoreProcess->waitForStarted()) {
        const QString msg = "MuseScore process failed to start";
        qWarning() << msg;
        return {false, msg};
    }

    if(!musescoreProcess->waitForFinished()) {
        const QString msg = "MuseScore process failed to finish";
        qWarning() << msg;
        return {false, msg};
    }

    return {true, musescoreTempDir};

}

const int PQCFilePluginMuseScore::loadNumPages(QString path) {
    return QDir(musescoreTempDir).entryList({"*.svg"}, QDir::Files).count();
}

const QJsonObject PQCFilePluginMuseScore::loadJSON(QString path, QVariantMap extraArguments) {

    const QString suffix1 = QFileInfo(path).suffix().toLower();
    const QString suffix2 = QFileInfo(path).completeSuffix().toLower();
    QMimeDatabase db;
    QString mime = db.mimeTypeForFile(path).name();

    if(!getSuffixes().contains(suffix1) && !getSuffixes().contains(suffix2) && !getMimetypes().contains(mime)) {
        return {};
    }

    QVariantList data = loadData(path);

    if(!data[0].toBool())
        return {};

    QString loadPath = data[1].toString() % "/page-1.svg";

    const QString targetFormat = extraArguments["targetFormat"].toString();

    QString returnPath = loadPath;

    if(targetFormat != "UNCHANGED") {

        returnPath = PQCConfigFiles::get().CACHE_DIR() % "/processed." % targetFormat.toLower();

        QSize orig;
        QString err;
        // we need to use a requested size here as otherwise the loader will fail
        QImage img = PQCFileHandler::get().getImage(loadPath, QSize(-1,-1), orig, err);

        if(img.isNull())
            return {};

        QImageWriter writer(returnPath);
        writer.write(img);

    }

    QJsonObject typeJson;
    typeJson["original"] = "musescore";
    typeJson["preview"] = "image";

    QJsonObject json;
    json["supported"] = true;
    json["filename"] = QFileInfo(path).fileName();
    json["type"] = typeJson;
    json["mimetype"] = mime;
    json["loadpath"] = returnPath;
    json["pageCount"] = loadNumPages(path);

    return json;

}
