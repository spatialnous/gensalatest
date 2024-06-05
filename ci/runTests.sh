#!/bin/sh

echo Running unit tests
./cliTest/cliTest && ./GuiUnitTest/GuiUnitTest && ./salaTest/salaTest && ./genlibTest/genlibTest && ./depthmapXTest/depthmapXTest && ./moduleTest/moduleTest || exit 1
# if that succeeds, run regression tests
echo testing regression test framework
cd ../RegressionTest/test && echo pwd && python3.5 -u test_main.py || exit 1
echo running standard regression tests
cd .. && pwd && python3.5 -u RegressionTestRunner.py || exit 1
echo running agent test
python3.5 -u RegressionTestRunner.py regressionconfig_agents.json || exit 1
# search the modules directory for regression tests and run them
for subdir in ../modules/*/; do
    regressionFile="${subdir}RegressionTest/regressionconfig.json"
    if [ -e "$regressionFile" ]; then
        python3.5 -u RegressionTestRunner.py "${regressionFile}" || exit 1
    fi 
done
