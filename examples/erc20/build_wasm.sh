#!/bin/bash

CUR_DIR=`pwd`
cd ../..
bash build_lib.sh
cd $CUR_DIR

/opt/solidcpp build --input my_erc20.cpp --contractlib-dir=../.. --generated-dir ./generated --output my_erc20.wasm
wasm2wat -o my_erc20.wat my_erc20.wasm
