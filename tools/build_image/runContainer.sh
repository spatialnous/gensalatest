# SPDX-FileCopyrightText: 2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

#!/bin/bash

podman run -it -v ../../:/code gensalatest:test
