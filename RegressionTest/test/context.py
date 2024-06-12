# SPDX-FileCopyrightText: 2017 Christian Sailer
#
# SPDX-License-Identifier: GPL-3.0-or-later

import os
import sys
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

import cmdlinewrapper
import config
import depthmaprunner
import RegressionTestRunner
import runhelpers
import performanceregressionconfig
import performancerunner
