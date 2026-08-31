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
#include <QtDebug>
#include <QProcess>
#include <QImageReader>

PQCFilePluginMuseScore::PQCFilePluginMuseScore() {

    loadedScore = "";
    musescoreCommand = "";

    scoreDir = PQCConfigFiles::get().CACHE_DIR() % "/mscz-" % QString::number(QDateTime::currentSecsSinceEpoch());

    setData({
        {55432,
             {{"MuseScore score file"}, {"mscz"}, {"application/x-musescore"}}},
        });

}

PQCFilePluginMuseScore::~PQCFilePluginMuseScore() {
    // if(scoreDir.startsWith(PQCConfigFiles::get().CACHE_DIR() % "/mscz-"))
        // QDir(scoreDir).removeRecursively();
}

const QSize PQCFilePluginMuseScore::loadSize(QString path) {

    // if(loadedScore == path) {
    //     QImageReader reader(scoreDir % "/page-1.png");
    //     reader.setAllocationLimit(0);
    //     qWarning() << ">>>|" << reader.size();
    //     return reader.size();
    // }

    // // if(scoreDir.startsWith(PQCConfigFiles::get().CACHE_DIR() % "/mscz-"))
    //     // QDir(scoreDir).removeRecursively();
    // scoreDir = PQCConfigFiles::get().CACHE_DIR() % "/mscz-" % QString::number(QDateTime::currentSecsSinceEpoch());
    // QDir(scoreDir).mkdir(scoreDir);

    // if(!detectMuseScore())
    //     return {};

    // exportPages(path);

    // QImageReader reader(scoreDir % "/page-1.png");
    // reader.setAllocationLimit(0);

    // loadedScore = path;

    // qWarning() << ">>>" << reader.size();
    // return reader.size();

    return {};

}

const QImage PQCFilePluginMuseScore::loadImage(QString path, QSize requestedSize, QSize &origSize, QString &error) {

    // QImage img;
    // if(loadImageFromCache(path, img, requestedSize)) {
    //     origSize = img.size();
    //     return img;
    // }

    // QStringList parts = path.split("::MSC::");
    // const QString page = (parts.length()>1 ? QString::number(parts.first().toInt()+1) : "1");
    // if(parts.length() > 1)
    //     parts.removeFirst();
    // path = parts.join("::MSC::");

    // if(loadedScore == path) {

    //     QImageReader reader(scoreDir % "/page-" % page % ".png");
    //     origSize = reader.size();
    //     reader.setAllocationLimit(0);
    //     if(!requestedSize.isEmpty())
    //         reader.setScaledSize(origSize.scaled(requestedSize, Qt::KeepAspectRatio));
    //     return reader.read();

    // }

    // // if(scoreDir.startsWith(PQCConfigFiles::get().CACHE_DIR() % "/mscz-"))
    //     // QDir(scoreDir).removeRecursively();
    // scoreDir = PQCConfigFiles::get().CACHE_DIR() % "/mscz-" % QString::number(QDateTime::currentSecsSinceEpoch());
    // QDir(scoreDir).mkdir(scoreDir);

    // if(!detectMuseScore())
    //     return {};

    // exportPages(path);

    // QImageReader reader(scoreDir % "/page-" % page % ".png");
    // origSize = reader.size();
    // reader.setAllocationLimit(0);
    // if(!requestedSize.isEmpty())
    //     reader.setScaledSize(origSize.scaled(requestedSize, Qt::KeepAspectRatio));

    // loadedScore = path;

    // return reader.read();

    return {};

}

const int PQCFilePluginMuseScore::loadNumPages(QString path) {

    // if(loadedScore == path) {

    //     const QStringList pngFiles = QDir(scoreDir).entryList({"*.png"}, QDir::Files);
    //     return pngFiles.size();

    // }

    // // if(scoreDir.startsWith(PQCConfigFiles::get().CACHE_DIR() % "/mscz-"))
    //     // QDir(scoreDir).removeRecursively();
    // scoreDir = PQCConfigFiles::get().CACHE_DIR() % "/mscz-" % QString::number(QDateTime::currentSecsSinceEpoch());
    // QDir(scoreDir).mkdir(scoreDir);

    // if(!detectMuseScore())
    //     return {};

    // exportPages(path);

    // const QStringList pngFiles = QDir(scoreDir).entryList({"*.png"}, QDir::Files);

    // loadedScore = path;

    // return pngFiles.size();

    return 1;

}

const QStringList PQCFilePluginMuseScore::loadContent(QString path) {

    // if(loadedScore == path)
    //     return QDir(scoreDir).entryList({"*.png"}, QDir::Files);

    // // if(scoreDir.startsWith(PQCConfigFiles::get().CACHE_DIR() % "/mscz-"))
    //     // QDir(scoreDir).removeRecursively();
    // scoreDir = PQCConfigFiles::get().CACHE_DIR() % "/mscz-" % QString::number(QDateTime::currentSecsSinceEpoch());
    // QDir(scoreDir).mkdir(scoreDir);

    // if(!detectMuseScore())
    //     return {};

    // exportPages(path);

    // loadedScore = path;

    // return QDir(scoreDir).entryList({"*.png"}, QDir::Files);

    return {};

}

const bool PQCFilePluginMuseScore::detectMuseScore() {

    // qDebug() << "";

    // if(musescoreCommand != "") return true;

    // // check for executable in PATH
    // QProcess which;
    // which.setStandardOutputFile(QProcess::nullDevice());
    // which.start("mscore", {"--version"});
    // if(!which.waitForStarted()) {
    //     qDebug() << "mscore not found, trying flatpak";
    // } else {
    //     musescoreCommand = "system";
    //     return true;
    // }

    // QProcess which2;
    // which2.setStandardOutputFile(QProcess::nullDevice());
    // which2.start("flatpak", {"run", "org.musescore.MuseScore", "--version"});
    // if(!which2.waitForStarted()) {
    //     qDebug() << "mscore flatpak not found";
    // } else {
    //     musescoreCommand = "flatpak";
    //     return true;
    // }

    return false;


}

void PQCFilePluginMuseScore::exportPages(QString path) {

    // qDebug() << "args: path =" << path;

    // qDebug() << "musescoreCommand =" << musescoreCommand;

    // QProcess exportProcess;
    // exportProcess.setStandardOutputFile(QProcess::nullDevice());

    // if(musescoreCommand == "system")
    //     exportProcess.start("mscore", {"--export-to", scoreDir % "/"%"page.png", path});
    // else if(musescoreCommand == "flatpak") {
    //     exportProcess.start("flatpak", {"run", "org.musescore.MuseScore", "--export-to", scoreDir % "/" % "page.png", path});
    // }

    // exportProcess.start();

    // if(!exportProcess.waitForStarted()) {
    //     qWarning() << "failed to start:" << exportProcess.errorString();
    //     return;
    // }

    // while(exportProcess.waitForFinished()) {
    //     qWarning() << "process failed or timed out:" << exportProcess.errorString();
    //     return;
    // }

}
