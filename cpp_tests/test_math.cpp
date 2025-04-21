// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "utils.hpp"
#include "gtest/gtest.h"
#include <contractlib/v1/math.hpp>
#include <contractlib/v1/utils.hpp>

using namespace dtvm;

TEST(MathTest, U256Add) {
  {
    // small int
    uint256 a = 1234;
    uint256 b = 6788;
    uint256 r = a + b;
    EXPECT_EQ(r, uint256(8022));
  }
  {
    // int great than 32bytes
    uint256 a = 0x1234;
    uint256 b = __uint128_t(1) << 12 * 8;
    uint256 r = a + b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "0000000000000000000000000000000000000001000000000000000000001234");
  }
  {
    // int with u128 max carry
    uint256 a = (__uint128_t)__int128_t(-1);
    uint256 b = 10;
    uint256 r = a + b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "0000000000000000000000000000000100000000000000000000000000000009");
  }
  {
    // very large u256
    uint256 a = __uint128_t(3) << 126;
    uint256 b = __uint128_t(1) << 127;
    uint256 r = (a + b) + uint256(3);
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "0000000000000000000000000000000140000000000000000000000000000003");
  }
}

TEST(MathTest, U256Max) {
  uint256 max_value = uint256::max();
  EXPECT_EQ(bytesToHex(max_value.bytes()),
            "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
}

TEST(MathTest, U256Sub) {
  {
    // small int
    uint256 a = 1234;
    uint256 b = 6788;
    uint256 r = b - a;
    EXPECT_EQ(r, uint256(5554));
  }
  {
    // int great than 32bytes
    uint256 a = 0x1234;
    uint256 b = __uint128_t(1) << 12 * 8;
    uint256 r = b - a;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "0000000000000000000000000000000000000000ffffffffffffffffffffedcc");
  }
  {
    // int with u128 max carry
    uint256 a = (__uint128_t)__int128_t(-1);
    uint256 b = 10;
    uint256 r = a - b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "00000000000000000000000000000000fffffffffffffffffffffffffffffff5");
  }
}

TEST(MathTest, U256Mul) {
  {
    // small int
    uint256 a = 1234;
    uint256 b = 6788;
    uint256 r = a * b;
    EXPECT_EQ(r, uint256(8376392));
  }
  {
    // int great than 32bytes
    uint256 a = 0x1234;
    uint256 b = __uint128_t(1) << 12 * 8;
    uint256 r = a * b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "0000000000000000000000000000000000001234000000000000000000000000");
  }
  {
    // int with u128 max carry
    uint256 a = (__uint128_t)__int128_t(-1);
    EXPECT_EQ(
        bytesToHex(a.bytes()),
        "00000000000000000000000000000000ffffffffffffffffffffffffffffffff");
    uint256 b = 10;
    uint256 r = a * b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "00000000000000000000000000000009fffffffffffffffffffffffffffffff6");
  }
}

TEST(MathTest, U256ShiftLeftRight) {
  uint256 a1 = (__uint128_t)__int128_t(-1);
  EXPECT_EQ(bytesToHex(a1.bytes()),
            "00000000000000000000000000000000ffffffffffffffffffffffffffffffff");
  uint256 a = a1 * 10000; // big number larger than u128 max
  {
    // small shift
    uint256 r = (a << 13) + (a >> 13);
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "00000000000000000000000004e20001387ffffffffffffffffffffffb1dfffe");
  }
  {
    // small shift larger than 31
    uint256 r = (a << 40) + (a >> 40);
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "00000000000000000027100000000000000000270fffffffffd8efffffffffff");
  }
  {
    // small shift larger than 128
    uint256 r = (a << 129) + (a >> 129);
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "ffffffffffffffffffffffffffffb1e000000000000000000000000000001387");
  }
}

TEST(MathTest, U256AndOrXor) {
  uint256 a1 = (__uint128_t)__int128_t(-1);
  EXPECT_EQ(bytesToHex(a1.bytes()),
            "00000000000000000000000000000000ffffffffffffffffffffffffffffffff");
  uint256 a = a1 * 10000; // big number larger than u128 max
  uint256 b = a1 * 999;
  {
    uint256 r = a & b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "00000000000000000000000000000306ffffffffffffffffffffffffffffd810");
  }
  {
    uint256 r = a | b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "000000000000000000000000000027effffffffffffffffffffffffffffffcf9");
  }
  {
    uint256 r = a ^ b;
    EXPECT_EQ(
        bytesToHex(r.bytes()),
        "000000000000000000000000000024e9000000000000000000000000000024e9");
  }
}

TEST(MathTest, U256Decoding) {
  {
    uint256 a(as_bytes32(unhex(
        "10000000000000000000000000000000fffffffffffffffffffffffffffffff6")));
    EXPECT_EQ(
        bytesToHex(a.bytes()),
        "10000000000000000000000000000000fffffffffffffffffffffffffffffff6");
    EXPECT_EQ(a.high, __uint128_t(1) << 31 * 4);
    EXPECT_EQ(a.low << 64 >> 64, __uint128_t(0xfffffffffffffff6L));
    EXPECT_EQ(uint64_t(a.low >> 64), uint64_t(0xffffffffffffffffL));
  }
}
