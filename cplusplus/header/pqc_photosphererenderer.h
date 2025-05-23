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

#ifndef PQC_PHOTOSPHERERENDERER
#define PQC_PHOTOSPHERERENDERER

#include <QQuickFramebufferObject>
#include <QMatrix4x4>

#include <pqc_photosphere.h>
#include <pqc_photosphereitem.h>

class QOpenGLShaderProgram;
class QOpenGLTexture;

class PQCPhotoSphereRenderer : public QQuickFramebufferObject::Renderer {

public:
    PQCPhotoSphereRenderer();
    ~PQCPhotoSphereRenderer();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

    void render() override;

    void synchronize(QQuickFramebufferObject *item) override;

    QOpenGLShaderProgram *shader;
    QQuickWindow* window;

    QByteArray source;
    QByteArray oldSource;

    QOpenGLFramebufferObject *frameBufferObject;
    QMatrix4x4 theMatrix;

    PQCPhotoSphereItem sphere;
    QOpenGLTexture *texturePhotoSphere;

};

#endif
