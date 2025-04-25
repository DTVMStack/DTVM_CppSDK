// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "utils.hpp"
#include "gtest/gtest.h"
#include <contractlib/v1/storage.hpp>

using namespace dtvm;

extern "C" void clear_mock_storage();

TEST(StorageTest, BasicTestUint256) {
  StorageSlot slot(1, 0);
  {
    // read empty storage is zeros
    uint256 value = read_storage_value<uint256>(slot);
    EXPECT_EQ(value, uint256(0));
  }
  // clear between cases
  clear_mock_storage();
  {
    uint256 value = uint256(__uint128_t(1234), __uint128_t(5678));
    write_storage_value(slot, value);
    uint256 read_value = read_storage_value<uint256>(slot);
    EXPECT_EQ(value, read_value);
  }
}

TEST(StorageTest, BasicTestString) {
  StorageSlot slot(2, 0);
  {
    // read empty storage is zeros
    const auto &value = read_storage_value<std::string>(slot);
    EXPECT_EQ(value, std::string(""));
  }
  // clear between cases
  clear_mock_storage();
  {
    std::string value = std::string("hello");
    write_storage_value(slot, value);
    const auto &read_value = read_storage_value<std::string>(slot);
    EXPECT_EQ(value, read_value);
  }
  // clear between cases
  clear_mock_storage();
  {
    // test 31 length string
    std::string value = std::string(31, 'a');
    write_storage_value(slot, value);
    const auto &read_value = read_storage_value<std::string>(slot);
    EXPECT_EQ(value, read_value);
  }
  // clear between cases
  clear_mock_storage();
  {
    // test 64(32*n) length string
    std::string value = std::string(64, 'a');
    write_storage_value(slot, value);
    const auto &read_value = read_storage_value<std::string>(slot);
    EXPECT_EQ(value, read_value);
  }
  // clear between cases
  clear_mock_storage();
  {
    // test string with > 31 and not 32*n length
    std::string value = std::string(33, 'a');
    write_storage_value(slot, value);
    const auto &read_value = read_storage_value<std::string>(slot);
    EXPECT_EQ(value, read_value);
  }
}

TEST(StorageTest, BasicTestBytes) {
  StorageSlot slot(3, 0);
  {
    // read empty storage is zeros
    auto value = read_storage_value<std::vector<uint8_t>>(slot);
    EXPECT_EQ(value.size(), 0);
  }
  // clear between cases
  clear_mock_storage();
  {
    auto value = std::vector<uint8_t>(5, 0x13);
    write_storage_value(slot, value);
    auto read_value = read_storage_value<std::vector<uint8_t>>(slot);
    EXPECT_EQ(value, read_value);
  }
  // clear between cases
  clear_mock_storage();
  {
    // test 31 length bytes
    auto value = std::vector<uint8_t>(31, 0x13);
    write_storage_value(slot, value);
    auto read_value = read_storage_value<std::vector<uint8_t>>(slot);
    EXPECT_EQ(value, read_value);
  }
  // clear between cases
  clear_mock_storage();
  {
    // test 64(32*n) length bytes
    auto value = std::vector<uint8_t>(64, 0x13);
    write_storage_value(slot, value);
    auto read_value = read_storage_value<std::vector<uint8_t>>(slot);
    EXPECT_EQ(value, read_value);
  }
  // clear between cases
  clear_mock_storage();
  {
    // test bytes with > 31 and not 32*n length
    auto value = std::vector<uint8_t>(33, 0x13);
    write_storage_value(slot, value);
    auto read_value = read_storage_value<std::vector<uint8_t>>(slot);
    EXPECT_EQ(value, read_value);
  }
}

TEST(StorageTest, BasicTestBool) {
  StorageSlot slot(4, 0);
  {
    // read empty storage is zeros
    bool value = read_storage_value<bool>(slot);
    EXPECT_EQ(value, false);
  }
  // clear between cases
  clear_mock_storage();
  {
    bool value = true;
    write_storage_value(slot, value);
    auto read_value = read_storage_value<bool>(slot);
    EXPECT_EQ(value, read_value);
  }
  clear_mock_storage();
  {
    bool value = false;
    write_storage_value(slot, value);
    auto read_value = read_storage_value<bool>(slot);
    EXPECT_EQ(value, read_value);
  }
}

TEST(StorageTest, BasicTestStorageSlotOffset) {
  // storage one value(type uint8, int16, bool) in one storage slot
  StorageSlot slot1(5, 0);
  StorageSlot slot2(5, 5);
  StorageSlot slot3(5, 31);
  uint8_t value1 = 0x7a;
  int16_t value2 = 0x6b5c;
  bool value3 = true;
  write_storage_value(slot1, value1);
  write_storage_value(slot2, value2);
  write_storage_value(slot3, value3);
  const auto read_bytes = hostio::read_storage(slot1.get_slot());
  EXPECT_EQ(bytesToHex(read_bytes),
            "7a000000006b5c00000000000000000000000000000000000000000000000001");
  uint8_t decoded_value1 = read_storage_value<uint8_t>(slot1);
  int16_t decoded_value2 = read_storage_value<int16_t>(slot2);
  bool decoded_value3 = read_storage_value<bool>(slot3);
  EXPECT_EQ(value1, decoded_value1);
  EXPECT_EQ(value2, decoded_value2);
  EXPECT_EQ(value3, decoded_value3);
}
