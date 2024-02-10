#!/bin/sh

set -e

cd ./docs
rm -rf latex cmake-build-debug* || true
doxygen #> /dev/null
cd latex
make #> /dev/null
cp refman.pdf ../README.pdf
cd ..
rm -rf latex || true

open README.pdf