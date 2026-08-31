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

    setData({
        {55432,
             {{"MuseScore score file"}, {"mscz"}, {"application/x-musescore"}}},
        });

}

// any submethods are currently not used
// the content of a musescore file is loaded asynchronously using PQCScriptsExternalTools
// resulting in a folder of SVG files that are loaded by the QML element as normal SVGs
