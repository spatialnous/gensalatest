#!/bin/sh

source /opt/qt511/bin/qt511-env.sh
if [ ! -d build ]; then
    mkdir build
fi
cd build
# build
echo Building
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . || exit 1
# if succeeds, run unit tests
source ./runTests.sh
