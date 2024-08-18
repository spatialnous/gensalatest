// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "genlib/bsptree.h"
#include "genlib/comm.h"
#include "genlib/p2dpoly.h"

#include "catch_amalgamated.hpp"

TEST_CASE("BSPTree::pickMidpointLine") {
    std::vector<Line> lines;
    lines.push_back(Line(Point2f(1, 2), Point2f(2, 2)));
    lines.push_back(Line(Point2f(2, 2), Point2f(3, 2)));
    lines.push_back(Line(Point2f(3, 2), Point2f(4, 2)));

    BSPNode node;

    REQUIRE(BSPTree::pickMidpointLine(lines, 0) == 1);

    SECTION("Additional lines") {
        lines.push_back(Line(Point2f(4, 2), Point2f(5, 2)));
        REQUIRE(BSPTree::pickMidpointLine(lines, 0) == 1);

        lines.push_back(Line(Point2f(5, 1), Point2f(6, 1)));
        REQUIRE(BSPTree::pickMidpointLine(lines, 0) == 2);

        // the only line with height > width becomes chosen
        lines.push_back(Line(Point2f(15, 4), Point2f(15, 0)));
        REQUIRE(BSPTree::pickMidpointLine(lines, 0) == 5);
    }
    SECTION("rotated middle") {

        // height > width, rotated, close to midpoint
        lines.push_back(Line(Point2f(4.5, 1), Point2f(4.5, 3)));

        lines.push_back(Line(Point2f(5, 2), Point2f(6, 2)));
        lines.push_back(Line(Point2f(6, 2), Point2f(7, 2)));

        // height > width, rotated, not close to midpoint
        lines.push_back(Line(Point2f(6.5, 1), Point2f(6.5, 3)));

        REQUIRE(BSPTree::pickMidpointLine(lines, 0) == 3);
    }
}

void compareLines(const Line &l1, const Line &l2, float EPSILON) {
    REQUIRE(l1.start().x == Catch::Approx(l2.start().x).epsilon(EPSILON));
    REQUIRE(l1.start().y == Catch::Approx(l2.start().y).epsilon(EPSILON));
    REQUIRE(l1.end().x == Catch::Approx(l2.end().x).epsilon(EPSILON));
    REQUIRE(l1.end().y == Catch::Approx(l2.end().y).epsilon(EPSILON));
}

TEST_CASE("BSPTree::makeLines") {
    const float EPSILON = 0.001f;
    typedef std::pair<std::vector<Line>, std::vector<Line>> LineVecPair;

    std::vector<Line> lines;
    lines.push_back(Line(Point2f(1, 2), Point2f(2, 2)));
    lines.push_back(Line(Point2f(2, 2), Point2f(3, 2)));
    lines.push_back(Line(Point2f(3, 2), Point2f(4, 2)));
    lines.push_back(Line(Point2f(4, 2), Point2f(5, 2)));

    std::unique_ptr<BSPNode> node(new BSPNode());

    LineVecPair result = BSPTree::makeLines(0, 0, lines, node.get());

    REQUIRE(result.first.size() == 3);
    REQUIRE(result.second.size() == 0);

    compareLines(result.first[0], lines[0], EPSILON);
    compareLines(result.first[1], lines[2], EPSILON);
    compareLines(result.first[2], lines[3], EPSILON);

    SECTION("One on the right") {
        lines.push_back(Line(Point2f(5, 1), Point2f(6, 1)));

        result = BSPTree::makeLines(0, 0, lines, node.get());

        REQUIRE(result.first.size() == 3);
        REQUIRE(result.second.size() == 1);

        compareLines(result.second[0], lines[4], EPSILON);
    }
    SECTION("One line with height > width becomes chosen") {
        // height > width, rotated, not close to midpoint
        lines.push_back(Line(Point2f(5.5, 1), Point2f(5.5, 3)));

        lines.push_back(Line(Point2f(6, 2), Point2f(7, 2)));

        result = BSPTree::makeLines(0, 0, lines, node.get());

        REQUIRE(result.first.size() == 4);
        REQUIRE(result.second.size() == 1);

        compareLines(result.first[0], lines[0], EPSILON);
        compareLines(result.first[1], lines[1], EPSILON);
        compareLines(result.first[2], lines[2], EPSILON);
        compareLines(result.first[3], lines[3], EPSILON);
        compareLines(result.second[0], lines[5], EPSILON);
    }

    SECTION("One broken between") {
        // height > width, rotated, close to midpoint
        lines.push_back(Line(Point2f(5.5, 1), Point2f(5.5, 3)));

        lines.push_back(Line(Point2f(6, 2), Point2f(7, 2)));
        lines.push_back(Line(Point2f(7, 2), Point2f(8, 2)));
        lines.push_back(Line(Point2f(8, 2), Point2f(9, 2)));
        lines.push_back(Line(Point2f(9, 2), Point2f(10, 2)));

        // line with two points at different sides of chosen
        lines.push_back(Line(Point2f(3, -2), Point2f(6, -2)));

        result = BSPTree::makeLines(0, 0, lines, node.get());

        // adds one on each side
        REQUIRE(result.first.size() == 5);
        REQUIRE(result.second.size() == 5);

        compareLines(result.first[0], lines[0], EPSILON);
        compareLines(result.first[1], lines[1], EPSILON);
        compareLines(result.first[2], lines[2], EPSILON);
        compareLines(result.first[3], lines[3], EPSILON);

        compareLines(result.second[0], lines[5], EPSILON);
        compareLines(result.second[1], lines[6], EPSILON);
        compareLines(result.second[2], lines[7], EPSILON);
        compareLines(result.second[3], lines[8], EPSILON);

        compareLines(result.first[4], Line(Point2f(3, -2), Point2f(5.5, -2)), EPSILON);
        compareLines(result.second[4], Line(Point2f(5.5, -2), Point2f(6, -2)), EPSILON);
    }
}

TEST_CASE("BSPTree::make (all horizontal lines)", "all-left tree") {
    const float EPSILON = 0.001f;

    std::vector<Line> lines;
    lines.push_back(Line(Point2f(1, 2), Point2f(2, 2)));
    lines.push_back(Line(Point2f(2, 2), Point2f(3, 2)));
    lines.push_back(Line(Point2f(3, 2), Point2f(4, 2)));
    lines.push_back(Line(Point2f(4, 2), Point2f(5, 2)));

    std::unique_ptr<BSPNode> node(new BSPNode());

    BSPTree::make(0, 0, lines, node.get());

    compareLines(node->getLine(), lines[1], EPSILON);

    REQUIRE(node->left != nullptr);
    REQUIRE(node->right == nullptr);

    compareLines(node->left->getLine(), lines[0], EPSILON);

    REQUIRE(node->left->left != nullptr);
    REQUIRE(node->left->right == nullptr);

    compareLines(node->left->left->getLine(), lines[2], EPSILON);

    REQUIRE(node->left->left->left != nullptr);
    REQUIRE(node->left->left->right == nullptr);

    compareLines(node->left->left->left->getLine(), lines[3], EPSILON);

    REQUIRE(node->left->left->left->left == nullptr);
    REQUIRE(node->left->left->left->right == nullptr);
}

TEST_CASE("BSPTree::make (all vertical lines)", "split tree") {
    const float EPSILON = 0.001f;

    std::vector<Line> lines;
    lines.push_back(Line(Point2f(1.5, 1), Point2f(1.5, 3)));
    lines.push_back(Line(Point2f(2.5, 1), Point2f(2.5, 3)));
    lines.push_back(Line(Point2f(3.5, 1), Point2f(3.5, 3)));
    lines.push_back(Line(Point2f(4.5, 1), Point2f(4.5, 3)));

    std::unique_ptr<BSPNode> node(new BSPNode());

    BSPTree::make(0, 0, lines, node.get());

    compareLines(node->getLine(), lines[1], EPSILON);

    REQUIRE(node->left != nullptr);
    REQUIRE(node->right != nullptr);

    compareLines(node->left->getLine(), lines[0], EPSILON);
    compareLines(node->right->getLine(), lines[2], EPSILON);

    REQUIRE(node->left->left == nullptr);
    REQUIRE(node->left->right == nullptr);

    REQUIRE(node->right->left == nullptr);
    REQUIRE(node->right->right != nullptr);

    compareLines(node->right->right->getLine(), lines[3], EPSILON);

    REQUIRE(node->right->right->left == nullptr);
    REQUIRE(node->right->right->right == nullptr);
}
