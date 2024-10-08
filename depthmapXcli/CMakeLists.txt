# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(dxproject depthmapXcli)

set(depthmapXcli_HDRS
    agentparser.h
    exportparser.h
    interfaceversion.h
    modeparserregistry.h
    printcommunicator.h
    segmentshortestpathparser.h
    visprepparser.h
    axialparser.h
    imodeparserfactory.h
    isovistparser.h
    parsingutils.h
    radiusconverter.h
    simpletimer.h
    commandlineparser.h
    imodeparser.h
    linkparser.h
    performancesink.h
    runmethods.h
    stepdepthparser.h
    exceptions.h
    importparser.h
    mapconvertparser.h
    performancewriter.h
    segmentparser.h
    vgaparser.h
)
set(depthmapXcli_SRCS
    main.cpp
    interfaceversion.h
    printcommunicator.cpp
    commandlineparser.cpp
    runmethods.cpp
    radiusconverter.cpp
    vgaparser.cpp
    linkparser.cpp
    performancewriter.cpp
    modeparserregistry.cpp
    visprepparser.cpp
    axialparser.cpp
    parsingutils.cpp
    agentparser.cpp
    isovistparser.cpp
    exportparser.cpp
    importparser.cpp
    stepdepthparser.cpp
    segmentparser.cpp
    mapconvertparser.cpp
    segmentshortestpathparser.cpp)

set(LINK_LIBS salalib)

set(modules_cli "" CACHE INTERNAL "modules_cli" FORCE)
set(MODULES_GUI FALSE)
set(MODULES_CLI TRUE)
set(MODULES_CLI_TEST FALSE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST FALSE)
add_subdirectory(../modules modules)

# The headers are added to the target primarily so that they appear as part
# of this particular project to IDEs that open it
add_executable(${dxproject} ${depthmapXcli_HDRS} ${depthmapXcli_SRCS})

target_compile_options(${dxproject} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${dxproject} ${LINK_LIBS} ${modules_cli} ${modules_core})

add_subdirectory(dxinterface)
