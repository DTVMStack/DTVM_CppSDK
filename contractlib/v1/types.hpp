// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "math.hpp"
#include "utils.hpp"
#include <cstring>
#include <vector>

namespace dtvm {
class Address {
public:
  Address() { data_ = {0}; }
  Address(const bytes32 &bs) { ::memcpy(data_.data(), bs.data() + 12, 20); }
  Address(const std::string &hex_str) {
    const auto &bs = unhex(hex_str);
    ::memcpy(data_.data(), bs.data(), 20);
  }

  bytes32 to_bytes32() const {
    bytes32 result = {0};
    ::memcpy(result.data() + 12, data_.data(), 20);
    return result;
  }
  const uint8_t *data() const { return data_.data(); }

  bool operator==(const Address &rhs) const {
    return ::memcmp(data_.data(), rhs.data(), 20) == 0;
  }

public:
  static Address zero() { return Address(); }

private:
  bytes20 data_;
};

class Bytes {
public:
  Bytes(const std::vector<uint8_t> &data) : data_(data) {}
  Bytes() {}

  const uint8_t *data() const { return data_.data(); }

  size_t size() const { return data_.size(); }

  const std::vector<uint8_t> &bytes() const { return data_; }

private:
  std::vector<uint8_t> data_;
};
} // namespace dtvm
