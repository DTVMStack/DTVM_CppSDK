#!/bin/bash
set -e

export LLVM_DIR=/opt/llvm15/lib/cmake/llvm
export LLVM_SYS_150_PREFIX=/opt/llvm15
export PATH=/opt/llvm15/bin:$PATH
# cdt is clang with wasm-sysroot
clang --target=wasm32 -nostdlib -O2 -nostdlib -c erc20_c.c -I /opt/cdt/wasm-sysroot/include
wasm-ld erc20_c.o -o erc20_c.wasm --no-entry --strip-all --allow-undefined --export "deploy" --export "call" /opt/cdt/wasm-sysroot/lib/libclang_rt.builtins.a

wasm2wat -o erc20_c.wat erc20_c.wasm
