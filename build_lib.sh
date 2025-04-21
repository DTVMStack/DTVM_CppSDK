#!/bin/bash
set -e

em++ -std=c++17 -o contractlib/v1/contractlib.o -O2 -c contractlib/v1/contractlib.cpp -I .
