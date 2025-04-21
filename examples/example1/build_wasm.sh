#!/bin/bash

CUR_DIR=`pwd`
cd ../..
bash build_lib.sh
cd $CUR_DIR

/opt/solidcpp build --input my_token.cpp --contractlib-dir=../.. --generated-dir ./generated --output my_token.wasm
wasm2wat -o my_token.wat my_token.wasm
