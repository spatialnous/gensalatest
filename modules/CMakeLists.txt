# SPDX-FileCopyrightText: 2020 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

file(GLOB children "*")
foreach(child ${children})
    if(IS_DIRECTORY ${child} AND EXISTS ${child}/CMakeLists.txt)
        add_subdirectory(${child})
    endif()
endforeach()
