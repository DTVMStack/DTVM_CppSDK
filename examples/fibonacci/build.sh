#!/bin/bash
set -e

export LLVM_DIR=/opt/llvm15/lib/cmake/llvm
export LLVM_SYS_150_PREFIX=/opt/llvm15
export PATH=/opt/llvm15/bin:$PATH
# cdt is clang with wasm-sysroot
clang --target=wasm32 -nostdlib -O2 -nostdlib -c fib.c -I /opt/cdt/wasm-sysroot/include
wasm-ld fib.o -o fib.wasm --no-entry --strip-all --allow-undefined --export "deploy" --export "call" /opt/cdt/wasm-sysroot/lib/libclang_rt.builtins.a

wasm2wat -o fib.wat fib.wasm

# build recur version
clang --target=wasm32 -nostdlib -O2 -nostdlib -c fib_recur.c -I /opt/cdt/wasm-sysroot/include
wasm-ld fib_recur.o -o fib_recur.c.wasm --no-entry --strip-all --allow-undefined --export "call" --export "deploy" /opt/cdt/wasm-sysroot/lib/libclang_rt.builtins.a

wasm2wat -o fib_recur.c.wat fib_recur.c.wasm

# build counter.c
clang --target=wasm32 -nostdlib -O2 -nostdlib -c counter.c -I /opt/cdt/wasm-sysroot/include
wasm-ld counter.o -o counter.c.wasm --no-entry --strip-all --allow-undefined --export "call" --export "deploy" /opt/cdt/wasm-sysroot/lib/libclang_rt.builtins.a

wasm2wat -o counter.c.wat counter.c.wasm
