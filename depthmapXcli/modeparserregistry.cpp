// SPDX-FileCopyrightText: 2017 Christian Sailer
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "modeparserregistry.h"

#include "agentparser.h"
#include "axialparser.h"
#include "exportparser.h"
#include "importparser.h"
#include "isovistparser.h"
#include "linkparser.h"
#include "mapconvertparser.h"
#include "segmentparser.h"
#include "segmentshortestpathparser.h"
#include "stepdepthparser.h"
#include "vgaparser.h"
#include "visprepparser.h"

void ModeParserRegistry::populateParsers() {
    // Register any mode parsers here
    REGISTER_PARSER(VgaParser);
    REGISTER_PARSER(LinkParser);
    REGISTER_PARSER(VisPrepParser);
    REGISTER_PARSER(AxialParser);
    REGISTER_PARSER(SegmentParser);
    REGISTER_PARSER(AgentParser);
    REGISTER_PARSER(IsovistParser);
    REGISTER_PARSER(ExportParser);
    REGISTER_PARSER(ImportParser);
    REGISTER_PARSER(StepDepthParser);
    REGISTER_PARSER(MapConvertParser);
    REGISTER_PARSER(SegmentShortestPathParser);
    // *********
}
