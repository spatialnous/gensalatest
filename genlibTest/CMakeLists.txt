# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(genlibtest genlibTest)

set(genlibTest_SRCS
    testreadwritehelpers.cpp
    main.cpp
    testsimplematrix.cpp
    testbspnode.cpp
    teststringutils.cpp
    testcontainerutils.cpp
) # genlibTest_SRCS

set(external_SRCS
    ../ThirdParty/Catch/catch_amalgamated.cpp
) # external_SRCS

set(LINK_LIBS
    salalib)

include_directories(SYSTEM "../ThirdParty/Catch")

add_executable(${genlibtest} ${genlibTest_SRCS})

target_sources(${genlibtest} PRIVATE ${external_SRCS})

set_source_files_properties(
  ${external_SRCS}
  PROPERTIES
  COMPILE_FLAGS "-w"
)
target_compile_options(${genlibtest} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${genlibtest} ${LINK_LIBS})
    
