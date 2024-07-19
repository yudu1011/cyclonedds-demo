#!/bin/sh
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../../Cddslib
cmake --build .
cmake --build . --target install
