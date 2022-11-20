#include "aglgraph.h"

#include "salalib/geometrygenerators.h"

void AGLGraph::loadGLObjects() {

    {
        std::vector<Point2f> intersectionLocations;
        for (auto &connection : m_connections) {
            intersectionLocations.push_back(connection.second);
        }

        const std::vector<Point2f> &nodeFillTriangles =
            GeometryGenerators::generateMultipleDiskTriangles(32, m_nodeSize,
                                                              intersectionLocations);

        m_intersectionFills.loadTriangleData(nodeFillTriangles, qRgb(0, 0, 255));

        std::vector<SimpleLine> nodeEdgeLines =
            GeometryGenerators::generateMultipleCircleLines(32, m_nodeSize, intersectionLocations);
        m_intersectionLines.loadLineData(nodeEdgeLines, qRgb(0, 255, 255));
    }

    std::vector<Point2f> nodeLocations;
    std::vector<SimpleLine> nodeEdgeLines;

    switch (m_graphDisplay) {
    case GraphDisplay::MIDPOINT: {
        for (auto &connection : m_connections) {
            nodeLocations.push_back(connection.first.start());
            nodeLocations.push_back(connection.first.end());
        }
        break;
    }
    case GraphDisplay::CORNERLINE:
    case GraphDisplay::CORNERARC: {
        for (auto &connection : m_connections) {
            const Point2f &intp = connection.second;
            intp.angle();

            Point2f intFrom = (connection.first.start() - intp).normalise();
            intFrom.x *= m_graphCornerRadius;
            intFrom.y *= m_graphCornerRadius;
            intFrom += intp;
            nodeLocations.push_back(intFrom);
            Point2f intTo = (connection.first.end() - intp).normalise();
            intTo.x *= m_graphCornerRadius;
            intTo.y *= m_graphCornerRadius;
            intTo += intp;
            nodeLocations.push_back(intTo);
            if (m_graphDisplay == GraphDisplay::CORNERLINE)
                nodeEdgeLines.push_back(SimpleLine(intFrom.x, intFrom.y, intTo.x, intTo.y));
        }

        break;
    }
    };

    const std::vector<Point2f> &nodeFillTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(32, m_nodeSize, nodeLocations);

    {
        std::vector<SimpleLine> nodePetrimeters =
            GeometryGenerators::generateMultipleCircleLines(32, m_nodeSize, nodeLocations);
        nodeEdgeLines.insert(nodeEdgeLines.end(), nodePetrimeters.begin(), nodePetrimeters.end());
    }

    switch (m_graphDisplay) {
    case GraphDisplay::CORNERLINE: {
        break;
    }
    case GraphDisplay::MIDPOINT: {
        for (auto &connection : m_connections)
            nodeEdgeLines.push_back(connection.first);
        break;
    }
    case GraphDisplay::CORNERARC: {
        for (auto &connection : m_connections) {
            const Point2f &intp = connection.second;
            intp.angle();
            Point2f intFrom = (connection.first.start() - intp).normalise();
            float fromAngle = atan2(intFrom.y, intFrom.x);
            if (fromAngle < 0) {
                fromAngle += 2 * M_PI;
            }
            Point2f intTo = (connection.first.end() - intp).normalise();
            float toAngle = atan2(intTo.y, intTo.x);
            if (toAngle < 0) {
                toAngle += 2 * M_PI;
            }

            if (fromAngle > toAngle) {
                std::swap(fromAngle, toAngle);
            }
            intFrom = Point2f(m_graphCornerRadius * cos(fromAngle),
                              m_graphCornerRadius * sin(fromAngle)) +
                      intp;
            intTo =
                Point2f(m_graphCornerRadius * cos(toAngle), m_graphCornerRadius * sin(toAngle)) +
                intp;
            float angleDiff = 2 * M_PI / 16.0;

            if (toAngle - fromAngle > M_PI) {
                int startFrom = floor(fromAngle / angleDiff) - 1;
                for (float a = startFrom * angleDiff; a > toAngle - 2 * M_PI; a -= angleDiff) {
                    Point2f newTo =
                        Point2f(m_graphCornerRadius * cos(a), m_graphCornerRadius * sin(a)) + intp;
                    nodeEdgeLines.push_back(SimpleLine(intFrom.x, intFrom.y, newTo.x, newTo.y));
                    intFrom = newTo;
                }
                nodeEdgeLines.push_back(SimpleLine(intFrom.x, intFrom.y, intTo.x, intTo.y));
            } else {
                for (float a = 0; a < 2 * M_PI; a += angleDiff) {
                    if (a < fromAngle)
                        continue;
                    if (a >= toAngle)
                        break;
                    Point2f newTo =
                        Point2f(m_graphCornerRadius * cos(a), m_graphCornerRadius * sin(a)) + intp;
                    nodeEdgeLines.push_back(SimpleLine(intFrom.x, intFrom.y, newTo.x, newTo.y));
                    intFrom = newTo;
                }
            }
            nodeEdgeLines.push_back(SimpleLine(intFrom.x, intFrom.y, intTo.x, intTo.y));
        }
        break;
    }
    };

    m_fills.loadTriangleData(nodeFillTriangles, qRgb(0, 0, 0));
    m_lines.loadLineData(nodeEdgeLines, qRgb(0, 255, 0));

    std::vector<Point2f> linkPointLocations;
    for (auto &link : m_links) {
        linkPointLocations.push_back(link.start());
        linkPointLocations.push_back(link.end());
    }

    const std::vector<Point2f> &linkFillTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(32, m_nodeSize, linkPointLocations);
    m_linkFills.loadTriangleData(linkFillTriangles, qRgb(0, 0, 0));

    std::vector<SimpleLine> linkFillPerimeters =
        GeometryGenerators::generateMultipleCircleLines(32, m_nodeSize, linkPointLocations);
    linkFillPerimeters.insert(linkFillPerimeters.end(), m_links.begin(), m_links.end());
    m_linkLines.loadLineData(linkFillPerimeters, qRgb(0, 255, 0));

    const std::vector<Point2f> &unlinkFillTriangles =
        GeometryGenerators::generateMultipleDiskTriangles(32, m_nodeSize, m_unlinks);
    m_unlinkFills.loadTriangleData(unlinkFillTriangles, qRgb(255, 255, 255));

    const std::vector<SimpleLine> &unlinkFillPerimeters =
        GeometryGenerators::generateMultipleCircleLines(32, m_nodeSize, m_unlinks);
    m_unlinkLines.loadLineData(unlinkFillPerimeters, qRgb(255, 0, 0));
}
