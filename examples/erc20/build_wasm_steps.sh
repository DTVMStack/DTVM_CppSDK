#!/bin/bash
set -e

CUR_DIR=`pwd`
cd ../..
bash build_lib.sh
cd $CUR_DIR

/opt/solidcpp fetch-cpp-sol --input my_erc20.cpp --output ./generated

# use solc to generate meta and storage
/opt/solc --output-dir ./generated --metadata --storage-layout --overwrite ./generated/my_erc20.sol

# generate decl.hpp by solidcpp
/opt/solidcpp generate-hpp --input "generated/*_meta.json" --input "generated/*_storage.json" \
  --output generated/my_erc20_decl.hpp

# -s INITIAL_HEAP=1048576 to initial heap 1MB
# you can add  -D NDEBUG to disable debug print
# must use emscripten at leat 3.1.69(tested)
em++ -std=c++17 -o my_erc20.wasm -O3 my_erc20.cpp ../../contractlib/v1/contractlib.o -I ../.. -s 'EXPORTED_FUNCTIONS=["_call","_deploy"]' --no-entry -Wl,--allow-undefined -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
 -s WASM=1 -s STANDALONE_WASM=0 -s PURE_WASI=0 -s INITIAL_MEMORY=1048576 -s TOTAL_STACK=307200
wasm2wat -o my_erc20.wat my_erc20.wasm
