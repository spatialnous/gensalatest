// Copyright (C) 2022, Petros Koutsolampros

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

#include "aglobjects.h"

#include "agllinesuniform.h"
#include "agltrianglesuniform.h"

class AGLGraph : AGLObjects {

    enum class GraphDisplay { MIDPOINT, CORNERLINE, CORNERARC };

    GraphDisplay m_graphDisplay = GraphDisplay::CORNERLINE;

    std::vector<std::pair<SimpleLine, Point2f>> m_connections;
    std::vector<SimpleLine> m_links;
    std::vector<Point2f> m_unlinks;

    AGLLinesUniform m_lines;
    AGLTrianglesUniform m_fills;

    AGLLinesUniform m_intersectionLines;
    AGLTrianglesUniform m_intersectionFills;

    AGLLinesUniform m_linkLines;
    AGLTrianglesUniform m_linkFills;

    AGLLinesUniform m_unlinkLines;
    AGLTrianglesUniform m_unlinkFills;

    float m_nodeSize;
    float m_graphCornerRadius;

  public:
    void addConnection(SimpleLine connection, Point2f intersection) {
        m_connections.push_back(std::make_pair(connection, intersection));
    }
    void setLinks(std::vector<SimpleLine> links) { m_links = links; }
    void setUnlinks(std::vector<Point2f> unlinks) { m_unlinks = unlinks; }

    void initializeGL(bool m_core) override {
        m_lines.initializeGL(m_core);
        m_fills.initializeGL(m_core);
        m_intersectionLines.initializeGL(m_core);
        m_intersectionFills.initializeGL(m_core);
        m_linkLines.initializeGL(m_core);
        m_linkFills.initializeGL(m_core);
        m_unlinkFills.initializeGL(m_core);
        m_unlinkLines.initializeGL(m_core);
    }
    void updateGL(bool m_core) override {
        m_lines.updateGL(m_core);
        m_fills.updateGL(m_core);
        m_intersectionLines.updateGL(m_core);
        m_intersectionFills.updateGL(m_core);
        m_linkLines.updateGL(m_core);
        m_linkFills.updateGL(m_core);
        m_unlinkFills.updateGL(m_core);
        m_unlinkLines.updateGL(m_core);
    }
    void cleanup() override {
        m_lines.cleanup();
        m_fills.cleanup();
        m_intersectionLines.cleanup();
        m_intersectionFills.cleanup();
        m_linkLines.cleanup();
        m_linkFills.cleanup();
        m_unlinkFills.cleanup();
        m_unlinkLines.cleanup();
    }
    void paintGL(const QMatrix4x4 &mProj, const QMatrix4x4 &mView,
                 const QMatrix4x4 &mModel) override {
        QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
        glFuncs->glLineWidth(3);
        m_lines.paintGL(mProj, mView, mModel);
        m_fills.paintGL(mProj, mView, mModel);
        m_intersectionLines.paintGL(mProj, mView, mModel);
        m_intersectionFills.paintGL(mProj, mView, mModel);
        m_linkLines.paintGL(mProj, mView, mModel);
        m_linkFills.paintGL(mProj, mView, mModel);
        m_unlinkLines.paintGL(mProj, mView, mModel);
        m_unlinkFills.paintGL(mProj, mView, mModel);
        glFuncs->glLineWidth(1);
    }
    void loadGLObjects() override;
    void loadGLObjectsRequiringGLContext() override {}

    void setNodeSize(float nodeSize) { m_nodeSize = nodeSize; }
    void setGraphCornerRadius(float graphCornerRadius) { m_graphCornerRadius = graphCornerRadius; }
};
