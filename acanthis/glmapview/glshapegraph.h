// Copyright (C) 2017, Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "glshapemap.h"

#include "gllinesuniform.h"
#include "salalib/mgraph.h"

class GLShapeGraph : public GLShapeMap {
  public:
    GLShapeGraph(ShapeGraph &shapeGraph, int pointSides, float pointRadius)
        : GLShapeMap(GLMap::GLMapType::SHAPEGRAPH, shapeGraph, pointSides, pointRadius),
          m_shapeGraph(shapeGraph){};

    void initializeGL(bool m_core) override {
        GLShapeMap::initializeGL(m_core);
        m_linkLines.initializeGL(m_core);
        m_linkFills.initializeGL(m_core);
        m_unlinkFills.initializeGL(m_core);
        m_unlinkLines.initializeGL(m_core);
    }

    void updateGL(bool m_core, bool reloadGLObjects) override {
        if (m_datasetChanged)
            return;
        GLShapeMap::updateGL(m_core, false);
        if (reloadGLObjects)
            loadGLObjects();
        m_linkLines.updateGL(m_core);
        m_linkFills.updateGL(m_core);
        m_unlinkFills.updateGL(m_core);
        m_unlinkLines.updateGL(m_core);
        m_datasetChanged = false;
    }

    void cleanup() override {
        GLShapeMap::cleanup();
        m_linkLines.cleanup();
        m_linkFills.cleanup();
        m_unlinkFills.cleanup();
        m_unlinkLines.cleanup();
    }

    void paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                 const QMatrix4x4 &m_mModel) override {
        GLShapeMap::paintGL(m_mProj, m_mView, m_mModel);
        if (m_showLinks) {
            QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
            glFuncs->glLineWidth(3);
            m_linkLines.paintGL(m_mProj, m_mView, m_mModel);
            m_linkFills.paintGL(m_mProj, m_mView, m_mModel);
            m_unlinkLines.paintGL(m_mProj, m_mView, m_mModel);
            m_unlinkFills.paintGL(m_mProj, m_mView, m_mModel);
            glFuncs->glLineWidth(1);
        }
    }

    void showLinks(bool showLinks) { m_showLinks = showLinks; }
    void loadGLObjects() override;

  private:
    ShapeGraph &m_shapeGraph;
    GLLinesUniform m_linkLines;
    GLTrianglesUniform m_linkFills;
    GLLinesUniform m_unlinkLines;
    GLTrianglesUniform m_unlinkFills;

    bool m_showLinks = false;
};
