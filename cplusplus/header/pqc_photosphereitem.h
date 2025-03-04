/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
 ** Contact: https://previewqt.org                                       **
 **                                                                      **
 ** This file is part of PreviewQt.                                      **
 **                                                                      **
 ** PreviewQt is free software: you can redistribute it and/or modify    **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PreviewQt is distributed in the hope that it will be useful,         **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.    **
 **                                                                      **
 **************************************************************************/

#ifndef PQC_PHOTOSPHERE
#define PQC_PHOTOSPHERE

#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class PQCPhotoSphereItem {

public:

    PQCPhotoSphereItem();

    void setup();
    void drawSphere();

    QOpenGLVertexArrayObject vertexArrowObject;
    QOpenGLBuffer vertexDataBuffer;
    QOpenGLBuffer textureCoordinateBuffer;
    QVector<QVector3D> sphereVertices;
    QVector<QVector2D> textureCoords;

private:
    bool isSetup = false;

};

#endif

