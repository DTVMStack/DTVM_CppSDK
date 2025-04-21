// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "hostapi.h"
#include <array>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace dtvm {
inline std::vector<uint8_t> unpadded_string(const std::string &str) {
  size_t need_pad_zeros = 31 - (str.size() + 31) % 32;
  std::vector<uint8_t> unpadded_key;
  for (size_t i = 0; i < str.size(); i++) {
    unpadded_key.push_back((uint8_t)str[i]);
  }
  for (size_t i = 0; i < need_pad_zeros; i++) {
    unpadded_key.push_back('\0');
  }
  return unpadded_key;
}

inline std::array<uint8_t, 32> as_bytes32(const std::vector<uint8_t> &data) {
  std::array<uint8_t, 32> ret;
  for (size_t i = 0; i < 32; i++) {
    if (i < data.size()) {
      ret[i] = data[i];
    } else {
      ret[i] = 0;
    }
  }
  return ret;
}

inline std::string hex(const std::vector<uint8_t> &bytes) {
  std::stringstream ss;
  for (const auto &byte : bytes) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }
  return ss.str();
}

inline std::string hex(const std::array<uint8_t, 32> &bytes) {
  std::stringstream ss;
  for (const auto &byte : bytes) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }
  return ss.str();
}

inline std::string hex(const std::array<uint8_t, 20> &bytes) {
  std::stringstream ss;
  for (const auto &byte : bytes) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }
  return ss.str();
}

inline std::vector<uint8_t> unhex(const std::string &str) {
  std::vector<uint8_t> ret;
  size_t i = 0;
  if (str.size() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
    i = 2; // ignore 0x/0X prefix
  }
  if (str.size() % 2 != 0) {
    std::string msg("unhex error: odd length string");
    ::revert((int32_t) reinterpret_cast<intptr_t>(msg.data()),
             (int32_t)msg.size());
  }
  for (; i < str.size(); i += 2) {
    uint8_t char1 = str[i];
    uint8_t char2 = str[i + 1];
    uint32_t char1_digit, char2_digit;
    if (char1 >= '0' && char1 <= '9') {
      char1_digit = (int32_t)(char1 - '0');
    } else if (char1 >= 'a' && char1 <= 'f') {
      char1_digit = 10 + (int32_t)(char1 - 'a');
    } else if (char1 >= 'A' && char1 <= 'F') {
      char1_digit = 10 + (int32_t)(char1 - 'A');
    } else {
      std::string msg = std::string("unhex error: invalid hex ") + str;
      ::revert((int32_t) reinterpret_cast<intptr_t>(msg.data()),
               (int32_t)msg.size());
    }

    if (char2 >= '0' && char2 <= '9') {
      char2_digit = (int32_t)(char2 - '0');
    } else if (char2 >= 'a' && char2 <= 'f') {
      char2_digit = 10 + (int32_t)(char2 - 'a');
    } else if (char2 >= 'A' && char2 <= 'F') {
      char2_digit = 10 + (int32_t)(char2 - 'A');
    } else {
      std::string msg = std::string("unhex error: invalid hex ") + str;
      ::revert((int32_t) reinterpret_cast<intptr_t>(msg.data()),
               (int32_t)msg.size());
    }
    uint8_t cur_byte = (uint8_t)(char1_digit * 16 + char2_digit);
    ret.push_back(cur_byte);
  }
  return ret;
}
} // namespace dtvm
