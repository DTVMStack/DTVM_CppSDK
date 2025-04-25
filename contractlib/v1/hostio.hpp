// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "hostapi.h"
#include "math.hpp"
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <string>
#include <vector>

namespace dtvm {

#ifndef NDEBUG
inline void debug_print(const std::string &str) {
  ::debug_bytes((ADDRESS_UINT) reinterpret_cast<intptr_t>(str.data()),
                (int32_t)str.size());
}
#else
#define debug_print(str)
#endif

namespace hostio {

inline void finish(const uint8_t *data, int32_t len) {
  ::finish((ADDRESS_UINT) reinterpret_cast<intptr_t>(data), len);
}

inline void revert(const uint8_t *data, int32_t len) {
  ::revert((ADDRESS_UINT) reinterpret_cast<intptr_t>(data), len);
}

inline void revert(const std::string &msg) {
  ::revert((ADDRESS_UINT) reinterpret_cast<intptr_t>(msg.data()),
           (int32_t)msg.size());
}

inline uint8_t *get_args() {
  int len = getCallDataSize();
  if (len <= 0) {
    return nullptr;
  }
  uint8_t *args = (uint8_t *)malloc(len);
  if (!args) {
    revert("malloc failed");
    return nullptr;
  }
  callDataCopy((ADDRESS_UINT) reinterpret_cast<intptr_t>(args), 0, len);
  return args;
}
inline uint32_t get_args_len() { return (uint32_t)getCallDataSize(); }

inline void emit_log(const std::vector<std::vector<uint8_t>> &topics,
                     const std::vector<uint8_t> &data) {
  int topics_count = (int)topics.size();
  const uint8_t *topic1 = nullptr;
  const uint8_t *topic2 = nullptr;
  const uint8_t *topic3 = nullptr;
  const uint8_t *topic4 = nullptr;
  if (topics_count > 0) {
    topic1 = topics[0].data();
  }
  if (topics_count > 1) {
    topic2 = topics[1].data();
  }
  if (topics_count > 2) {
    topic3 = topics[2].data();
  }
  if (topics_count > 3) {
    topic4 = topics[3].data();
  }
  ::emitLogEvent((ADDRESS_UINT) reinterpret_cast<intptr_t>(data.data()),
                 (int32_t)data.size(), (int32_t)topics_count,
                 (ADDRESS_UINT) reinterpret_cast<intptr_t>(topic1),
                 (ADDRESS_UINT) reinterpret_cast<intptr_t>(topic2),
                 (ADDRESS_UINT) reinterpret_cast<intptr_t>(topic3),
                 (ADDRESS_UINT) reinterpret_cast<intptr_t>(topic4));
}

inline uint64_t get_gas_left() { return (uint64_t)::getGasLeft(); }

inline bytes32 read_storage(const dtvm::uint256 &key) {
  // if not exist, return 32 bytes zero
  bytes32 result = {0};
  ::storageLoad((ADDRESS_UINT) reinterpret_cast<intptr_t>(key.bytes().data()),
                (ADDRESS_UINT) reinterpret_cast<intptr_t>(result.data()));
  return result;
}

inline void write_storage(const dtvm::uint256 &key, const bytes32 &value) {
  ::storageStore((ADDRESS_UINT) reinterpret_cast<intptr_t>(key.bytes().data()),
                 (ADDRESS_UINT) reinterpret_cast<intptr_t>(value.data()));
}

inline bytes32 keccak256(const std::vector<uint8_t> &data) {
  bytes32 result;
  ::keccak256((ADDRESS_UINT) reinterpret_cast<intptr_t>(data.data()),
              (int32_t)data.size(),
              (ADDRESS_UINT) reinterpret_cast<intptr_t>(result.data()));
  return result;
}

inline bytes32 keccak256(const bytes32 &data) {
  bytes32 result{};
  ::keccak256((ADDRESS_UINT) reinterpret_cast<intptr_t>(data.data()),
              (int32_t)data.size(),
              (ADDRESS_UINT) reinterpret_cast<intptr_t>(result.data()));
  return result;
}

// Wrap these call functions to have a unified function signature, making it
// easier to call them using templates
inline int32_t callDelegateWithZeroValue(int64_t gas, int32_t addressOffset,
                                         int32_t valueOffset,
                                         int32_t dataOffset,
                                         int32_t dataLength) {
  return ::callDelegate(gas, addressOffset, dataOffset, dataLength);
}
inline int32_t callStaticWithZeroValue(int64_t gas, int32_t addressOffset,
                                       int32_t valueOffset, int32_t dataOffset,
                                       int32_t dataLength) {
  return ::callStatic(gas, addressOffset, dataOffset, dataLength);
}
} // namespace hostio
} // namespace dtvm
