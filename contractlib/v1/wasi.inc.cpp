// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "wasi.hpp"

#ifdef IN_WASM_ENV
// implement some wasi in emsdk
extern "C" {
void wasi_proc_exit(
    /**
     * The exit code returned by the process.
     */
    int rval) __attribute__((__import_module__("wasi_snapshot_preview1"),
                             __import_name__("proc_exit"))) {
  if (rval == 0) {
    finish((ADDRESS_UINT) reinterpret_cast<intptr_t>(""), 0);
  } else {
    revert((ADDRESS_UINT) reinterpret_cast<intptr_t>("proc_exit"),
           9); // strlen("proc_exit")==9
  }
}
}
#endif
