// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "utils.hpp"
#include <array>
#include <climits>
#include <cstdint>
#include <type_traits>

namespace dtvm {

typedef std::array<uint8_t, 32> bytes32;
typedef std::array<uint8_t, 20> bytes20;

struct uint256 {
  __uint128_t high;
  __uint128_t low;
  uint256() : high(0), low(0) {}
  uint256(__uint128_t low) : high(0), low(low) {}
  uint256(const uint256 &other) : high(other.high), low(other.low) {}

  uint256(__uint128_t high_part, __uint128_t low_part)
      : high(high_part), low(low_part) {}
  uint256(const bytes32 &bs) { // big endian bytes
    __uint128_t highB_bytes = 0, low_bytes = 0;
    // Convert byte sequence from big-endian to little-endian __uint128_t
    for (size_t i = 0; i < sizeof(high); ++i) {
      highB_bytes |= static_cast<__uint128_t>(bs[i])
                     << (8 * (sizeof(high) - i - 1));
    }
    for (size_t i = 0; i < sizeof(low); ++i) {
      low_bytes |= static_cast<__uint128_t>(bs[i + 16])
                   << (8 * (sizeof(low) - i - 1));
    }
    high = highB_bytes;
    low = low_bytes;
  }
  // Addition operator
  uint256 operator+(const uint256 &other) const {
    __uint128_t newLow = low + other.low;
    __uint128_t carry = newLow < low; // Check if there is a carry
    newLow = newLow &
             ~(carry << (sizeof(__uint128_t) * 8 - 1)); // Clear the carry bit
    __uint128_t newHigh = high + other.high + carry;
    return uint256(newHigh, newLow);
  }

  uint256 operator-(const uint256 &other) const {
    __uint128_t newLow = low - other.low;
    __uint128_t borrow = newLow > low; // Check if there is a borrow
    newLow = newLow &
             ~(borrow << (sizeof(__uint128_t) * 8 - 1)); // Clear the borrow bit
    __uint128_t newHigh = high - other.high - borrow;
    return uint256(newHigh, newLow);
  }

  uint256 operator>>(size_t shift) const {
    if (shift == 0) {
      return *this;
    }
    if (shift == 128) {
      return uint256(0, high);
    }
    shift =
        shift > 256 ? 256 : shift; // Limit the shift to no more than 256 bits
    uint256 result;
    if (shift > 128) {
      // all low shifted
      result.low = high >> (shift - 128);
      result.high = 0;
    } else {
      result.high = high >> shift;
      result.low = (low >> shift) + (high << (128 - shift));
    }
    return result;
  }

  uint256 operator<<(size_t shift) const {
    if (shift == 0) {
      return *this;
    }
    if (shift == 128) {
      return uint256(low, 0);
    }
    shift =
        shift > 256 ? 256 : shift; // Limit the shift to no more than 256 bits
    uint256 result;
    if (shift > 128) {
      // all high shifted
      result.high = low << (shift - 128);
      result.low = 0;
    } else {
      result.high = (high << shift) + (low >> (128 - shift));
      result.low = low << shift;
    }
    return result;
  }

  uint256 operator&(const uint256 &rhs) const {
    return uint256(high & rhs.high, low & rhs.low);
  }

  uint256 operator|(const uint256 &rhs) const {
    return uint256(high | rhs.high, low | rhs.low);
  }

  uint256 operator^(const uint256 &rhs) const {
    return uint256(high ^ rhs.high, low ^ rhs.low);
  }

  static uint256 multiply128(const __uint128_t &a, const __uint128_t &b) {
    __uint128_t a_high = a >> 64;
    __uint128_t a_low = a & 0xFFFFFFFFFFFFFFFFull;
    __uint128_t b_high = b >> 64;
    __uint128_t b_low = b & 0xFFFFFFFFFFFFFFFFull;
    // new low part = a.low * b.low
    // new high part = a.high * b.low + b.high * a.low + overflow part from
    // (a.low * b.low) overflow high part = a.high * b.high + overflow part from
    // (new high part)
    __uint128_t new_low = a_low * b_low;
    __uint128_t new_high = a_high * b_low + b_high * a_low + (new_low >> 64);
    __uint128_t overflow_high = a_high * b_high + (new_high >> 64);
    __uint128_t not_overflow_part = ((new_high & 0xFFFFFFFFFFFFFFFFull) << 64) +
                                    (new_low & 0xFFFFFFFFFFFFFFFFull);
    return uint256(overflow_high, not_overflow_part);
  }

  uint256 operator*(const uint256 &other) const {
    __uint128_t a = low, b = other.low;
    __uint128_t ah = high, bh = other.high;
    // The high part * high part result has already overflowed, so no need to
    // consider it new high part = n1.high * n2.low + n2.high * n1.low +
    // overflow part (n1.low * n2.low) new low part = n1.low * n2.low
    __uint128_t new_high = ah * b + bh * a + multiply128(a, b).high;
    __uint128_t new_low = a * b;
    return uint256(new_high, new_low);
  }

  // TODO: operator / , %

  bool operator==(const uint256 &other) const {
    return high == other.high && low == other.low;
  }

  bool operator!=(const uint256 &other) const { return !(*this == other); }

  bool operator<(const uint256 &other) const {
    if (high < other.high) {
      return true;
    } else if (high == other.high) {
      return low < other.low;
    } else {
      return false;
    }
  }

  bool operator<=(const uint256 &other) const {
    return (*this == other) || (*this < other);
  }

  bool operator>(const uint256 &other) const {
    if (high > other.high) {
      return true;
    } else if (high == other.high) {
      return low > other.low;
    } else {
      return false;
    }
  }

  bool operator>=(const uint256 &other) const {
    return (*this == other) || (*this > other);
  }

  uint256 &operator=(const uint256 &other) {
    high = other.high;
    low = other.low;
    return *this;
  }

  static uint256 max() {
    return uint256((__uint128_t)__int128_t(-1), (__uint128_t)__int128_t(-1));
  }

  bytes32 bytes() const {
    // as big endian
    bytes32 result;
    auto bytes_high = reinterpret_cast<const uint8_t *>(&high);
    auto bytes_low = reinterpret_cast<const uint8_t *>(&low);
    // big endian byte order
    for (size_t i = 0; i < sizeof(high); ++i) {
      result[i] = bytes_high[sizeof(high) - i - 1];
    }
    for (size_t i = 0; i < sizeof(low); ++i) {
      result[i + 16] = bytes_low[sizeof(low) - i - 1];
    }
    return result;
  }

  __uint128_t to_uint128() const { return low; }

  uint64_t to_uint64() const { return uint64_t(low); }

  uint32_t to_uint32() const { return uint32_t(low); }

  uint16_t to_uint16() const { return uint16_t(low); }

  uint8_t to_uint8() const { return uint8_t(low); }
};
} // namespace dtvm
