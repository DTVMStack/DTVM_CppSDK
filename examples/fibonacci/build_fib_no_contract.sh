#!/bin/bash
set -e

export LLVM_DIR=/opt/llvm15/lib/cmake/llvm
export LLVM_SYS_150_PREFIX=/opt/llvm15
export PATH=/opt/llvm15/bin:$PATH
# cdt is clang with wasm-sysroot
clang --target=wasm32 -nostdlib -O2 -nostdlib -c fib_no_contract.c -I /opt/cdt/wasm-sysroot/include
wasm-ld fib_no_contract.o -o fib_no_contract.c.wasm --no-entry --strip-all --allow-undefined --export "fibonacci" --export "fibonacciTailOptimized" /opt/cdt/wasm-sysroot/lib/libclang_rt.builtins.a

wasm2wat -o fib_no_contract.c.wat fib_no_contract.c.wasm
