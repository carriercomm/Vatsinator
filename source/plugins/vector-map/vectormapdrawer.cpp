/*
 * vectormapdrawer.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtCore>
#include <QtGui>

#include "worldmap.h"

#include "vectormapdrawer.h"

static Q_CONSTEXPR int VertexLocation = 0;

VectorMapDrawer::VectorMapDrawer(QObject* parent) :
    QObject (parent),
    __worldMap(new WorldMap(this)),
    __borders(QOpenGLBuffer::VertexBuffer),
    __triangles(QOpenGLBuffer::IndexBuffer) {
  __worldMap->initialize();
}

VectorMapDrawer::~VectorMapDrawer() {
  
}

MapDrawer::MapDrawerFlags
VectorMapDrawer::flags() const {
  return MapDrawerFlag::RequireOpenGLContextOnInitialize;
}

void
VectorMapDrawer::initialize() {
  Q_ASSERT(QOpenGLContext::currentContext());
  
  if (!__buildShaders()) {
    qCritical("Failed compiling shaders");
    return;
  }
  
  if (!__initializeBuffers()) {
    qCritical("Failed initializing buffers");
    return;
  }
}

void
VectorMapDrawer::draw(const QRectF& screen, qreal zoom) {
  __vao.bind();
  glDrawElements(GL_TRIANGLES, __vertices, GL_UNSIGNED_INT, 0);
  __vao.release();
}

bool
VectorMapDrawer::__buildShaders() {
  bool result;
  /* Create identity shader */
  __identityProgram = new QOpenGLShaderProgram(this);
  QOpenGLShader* vertex = new QOpenGLShader(QOpenGLShader::Vertex, __identityProgram);
  result = vertex->compileSourceFile(":/shaders/identity.vert");
  Q_ASSERT(result);
  QOpenGLShader* fragment = new QOpenGLShader(QOpenGLShader::Fragment, __identityProgram);
  result = fragment->compileSourceFile(":/shaders/identity.frag");
  Q_ASSERT(result);
  __identityProgram->addShader(vertex);
  __identityProgram->addShader(fragment);
  
  __identityProgram->bindAttributeLocation("vertex", VertexLocation);
  
  result = __identityProgram->link();
  Q_ASSERT(result);
  __identityProgram->bind();
  __matrixLocation = __identityProgram->uniformLocation("matrix");
  Q_ASSERT(__identityMatrixLocation >= 0);
  __colorLocation = __identityProgram->uniformLocation("color");
  Q_ASSERT(__identityColorLocation >= 0);
  __identityProgram->release();
}

bool
VectorMapDrawer::__initializeBuffers() {
  QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();
  
  const QVector<Point>& bordersData = __worldMap->borders();
  Q_ASSERT(!bordersData.isEmpty());
  
  const QVector<unsigned int>& trianglesData = __worldMap->triangles();
  Q_ASSERT(!trianglesData.isEmpty());
  
  if (!__vao.create())
    return;
  
  __vao.bind();
  
  if (!__borders.create())
    return;
  
  __borders.setUsagePattern(QOpenGLBuffer::StaticDraw);
  __borders.bind();
  __borders.allocate(bordersData.constData(), sizeof(Point) * bordersData.size());
  
  __triangles.create();
  Q_ASSERT(__triangles.isCreated());
  __triangles.setUsagePattern(QOpenGLBuffer::StaticDraw);
  __triangles.bind();
  __triangles.allocate(trianglesData.constData(), sizeof(unsigned int) * trianglesData.size());
  
  __borders.bind();
  __triangles.bind();
  gl->glVertexAttribPointer(VertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
  gl->glEnableVertexAttribArray(VertexLocation);
  __vao.release();
  __borders.release();
  __triangles.release();
}
