#!/bin/bash
mkdir -p build
cd build
cmake ../src
make
make test
sudo make install
