/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2024 Lukas Spies                                  **
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

#ifndef VQCLOADIMAGEDEVIL_H
#define VQCLOADIMAGEDEVIL_H

#include <QMutex>

class QSize;
class QImage;

// class to provide a global mutex
// we need to have one and only one for each thread
// this is needed because DevIL is not threadsafe
class VQCLoadImageDevilMutex {

public:
        static VQCLoadImageDevilMutex& get() {
            static VQCLoadImageDevilMutex instance;
            return instance;
        }
        VQCLoadImageDevilMutex(VQCLoadImageDevilMutex const&)     = delete;
        void operator=(VQCLoadImageDevilMutex const&) = delete;
#ifdef VQMDEVIL
        // DevIL is not threadsafe -> this ensures only one image is loaded at a time
        QMutex devilMutex;
#endif
private:
        VQCLoadImageDevilMutex() {}
};

class VQCLoadImageDevil {

public:
    VQCLoadImageDevil();

    static QString load(QString filename, QSize maxSize, QSize &origSize, QImage &img);

private:

#ifdef VQMDEVIL
    static QString checkForError();
#endif

};

#endif // VQCLOADIMAGEDEVIL_H
