// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

inline std::string bytesToHex(const std::vector<uint8_t> &bytes) {
  std::stringstream ss;
  for (const auto &byte : bytes) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }
  return ss.str();
}

inline std::string bytesToHex(const std::array<uint8_t, 32> &bytes) {
  std::stringstream ss;
  for (const auto &byte : bytes) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }
  return ss.str();
}

inline std::string bytesToHex(const std::array<uint8_t, 20> &bytes) {
  std::stringstream ss;
  for (const auto &byte : bytes) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }
  return ss.str();
}
