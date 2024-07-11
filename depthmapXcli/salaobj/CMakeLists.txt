# SPDX-FileCopyrightText: 2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_sources(depthmapXcli
    PRIVATE
        metagraphcli.cpp
        pointmapcli.cpp
        shapemapcli.cpp
        shapegraphcli.cpp
    PUBLIC
        metagraphcli.h
        pointmapcli.h
        shapemapcli.h
        shapegraphcli.h
        shapemapgroupdatacli.h
        attributemapcli.h
)

