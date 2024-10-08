# SPDX-FileCopyrightText: 2017 Christian Sailer
# SPDX-FileCopyrightText: 2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

#!/bin/sh

echo Running unit tests
./cliTest/cliTest && ./salaTest/salaTest && ./genlibTest/genlibTest && ./moduleTest/moduleTest --allow-running-no-tests || exit 1
# if that succeeds, run regression tests
echo testing regression test framework
cd ..
ln -s build build-RegressionTest
cd RegressionTest/test && echo pwd && python3 -u test_main.py || exit 1
echo running standard regression tests
cd .. && pwd && python3 -u RegressionTestRunner.py || exit 1
echo running agent test
python3 -u RegressionTestRunner.py regressionconfig_agents.json || exit 1
# search the modules directory for regression tests and run them
for subdir in ../modules/*/; do
    regressionFile="${subdir}RegressionTest/regressionconfig.json"
    if [ -e "$regressionFile" ]; then
        python3 -u RegressionTestRunner.py "${regressionFile}" || exit 1
    fi 
done
