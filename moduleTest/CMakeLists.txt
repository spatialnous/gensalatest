# SPDX-FileCopyrightText: 2020 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(moduleTest moduleTest)

set(moduleTest_SRCS
    main.cpp
) # moduleTest_SRCS

set(external_SRCS
    ../ThirdParty/Catch/catch_amalgamated.cpp
) # external_SRCS

include_directories(SYSTEM "../ThirdParty/Catch" "../ThirdParty/FakeIt")

set(modules_coreTest "" CACHE INTERNAL "modules_coreTest" FORCE)
set(MODULES_GUI FALSE)
set(MODULES_CLI FALSE)
set(MODULES_CLI_TEST FALSE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST TRUE)
add_subdirectory(../modules modules)

add_executable(${moduleTest} ${moduleTest_SRCS})

target_sources(${moduleTest} PRIVATE ${external_SRCS})

set_source_files_properties(
  ${external_SRCS}
  PROPERTIES
  COMPILE_FLAGS "-w"
)
target_compile_options(${moduleTest} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${moduleTest} salalib genlib ${modules_coreTest} ${modules_core})
