// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "utils.hpp"
#include "gtest/gtest.h"
#include <climits>
#include <contractlib/v1/encoding.hpp>

using namespace dtvm;

TEST(TestEncoding, BasicEncodeUInts) {
  {
    // uint8 abi encoding test
    uint8_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "uint8 " << (uint32_t)value
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<uint8_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint16 abi encoding test
    uint16_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "uint16 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<uint16_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint32 abi encoding test
    uint32_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "uint32 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<uint32_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint64 abi encoding test
    uint64_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "uint64 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<uint64_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint128 abi encoding test
    __uint128_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "uint128 128"
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<__uint128_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint256 abi encoding test
    uint256 value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "uint256 123"
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<uint256>(encoded);
    EXPECT_EQ(decoded.high, value.high);
    EXPECT_EQ(decoded.low, value.low);
  }
}

// test uints max values abi encoding
TEST(TestEncoding, BasicEncodeUIntsMaxValues) {
  {
    // uint8 abi encoding test
    uint8_t value = 255;
    const auto &encoded = abi_encode(value);
    std::cout << "uint8 " << (uint32_t)value
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "00000000000000000000000000000000000000000000000000000000000000ff");
    const auto &decoded = abi_decode_all<uint8_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint16 abi encoding test
    uint16_t value = 65535;
    const auto &encoded = abi_encode(value);
    std::cout << "uint16 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000ffff");
    const auto &decoded = abi_decode_all<uint16_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint32 max abi encoding test
    uint32_t value = 4294967295;
    const auto &encoded = abi_encode(value);
    std::cout << "uint32 max " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "00000000000000000000000000000000000000000000000000000000ffffffff");
    const auto &decoded = abi_decode_all<uint32_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint64 max abi encoding test
    uint64_t value = 18446744073709551615L;
    const auto &encoded = abi_encode(value);
    std::cout << "uint64 max " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000ffffffffffffffff");
    const auto &decoded = abi_decode_all<uint64_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint128 max abi encoding test
    __uint128_t value = (__uint128_t)__int128_t(-1);
    const auto &encoded = abi_encode(value);
    std::cout << "uint128 max"
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "00000000000000000000000000000000ffffffffffffffffffffffffffffffff");
    const auto &decoded = abi_decode_all<__uint128_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // uint256 max abi encoding test
    uint256 value = uint256::max();
    const auto &encoded = abi_encode(value);
    std::cout << "uint256 max "
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    const auto &decoded = abi_decode_all<uint256>(encoded);
    EXPECT_EQ(decoded, value);
  }
}

TEST(TestEncoding, BasicEncodeInts) {
  {
    // int8 abi encoding test
    int8_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "int8 " << (uint32_t)value
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<int8_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int16 abi encoding test
    int16_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "int16 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<int16_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int32 abi encoding test
    int32_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "int32 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<int32_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int64 abi encoding test
    int64_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "int64 " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<int64_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int128 abi encoding test
    __int128_t value = 123;
    const auto &encoded = abi_encode(value);
    std::cout << "int128 128"
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007b");
    const auto &decoded = abi_decode_all<__int128_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
}

// int min and max values abi encoding test

TEST(TestEncoding, BasicEncodeIntsMinMaxValues) {
  {
    // int8 min abi encoding test
    int8_t value = -128;
    const auto &encoded = abi_encode(value);
    std::cout << "int8 min " << (uint32_t)value
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000080");
    const auto &decoded = abi_decode_all<int8_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int8 max abi encoding test
    int8_t value = 127;
    const auto &encoded = abi_encode(value);
    std::cout << "int8 max " << (uint32_t)value
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000007f");
    const auto &decoded = abi_decode_all<int8_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int16 min abi encoding test
    int16_t value = -32768;
    const auto &encoded = abi_encode(value);
    std::cout << "int16 min " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000008000");
    const auto &decoded = abi_decode_all<int16_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int16 max abi encoding test
    int16_t value = 32767;
    const auto &encoded = abi_encode(value);
    std::cout << "int16 max " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000007fff");
    const auto &decoded = abi_decode_all<int16_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int32 min abi encoding test
    int32_t value = -2147483648;
    const auto &encoded = abi_encode(value);
    std::cout << "int32 min " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000080000000");
    const auto &decoded = abi_decode_all<int32_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int32 max abi encoding test
    int32_t value = 2147483647;
    const auto &encoded = abi_encode(value);
    std::cout << "int32 max " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000007fffffff");
    const auto &decoded = abi_decode_all<int32_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int64 min abi encoding test
    int64_t value = -9223372036854775808L;
    const auto &encoded = abi_encode(value);
    std::cout << "int64 min " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000008000000000000000");
    const auto &decoded = abi_decode_all<int64_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int64 max abi encoding test
    int64_t value = 9223372036854775807L;
    const auto &encoded = abi_encode(value);
    std::cout << "int64 max " << value << " encoded: " << bytesToHex(encoded)
              << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000007fffffffffffffff");
    const auto &decoded = abi_decode_all<int64_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int128 min abi encoding test
    __int128_t value = -(__int128_t)__int128_t(1) << 127;
    const auto &encoded = abi_encode(value);
    std::cout << "int128 min"
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000080000000000000000000000000000000");
    const auto &decoded = abi_decode_all<__int128_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
  {
    // int128 max abi encoding test
    int64_t i64_max = 9223372036854775807L;
    __int128_t value =
        (__int128_t(__uint128_t(i64_max)) << 64) + __uint128_t(UINT64_MAX);
    const auto &encoded = abi_encode(value);
    std::cout << "int128 max"
              << " encoded: " << bytesToHex(encoded) << std::endl;

    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000007fffffffffffffffffffffffffffffff");
    const auto &decoded = abi_decode_all<__int128_t>(encoded);
    EXPECT_EQ(decoded, value);
  }
}

TEST(TestEncoding, BasicBoolEncodingDecoding) {
  {
    // true abi encoding
    const auto &encoded = abi_encode<bool>(true);
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000001");
    bool decoded = abi_decode_all<bool>(encoded);
    EXPECT_EQ(decoded, true);
  }
  {
    // false abi encoding
    const auto &encoded = abi_encode<bool>(false);
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000000");
    bool decoded = abi_decode_all<bool>(encoded);
    EXPECT_EQ(decoded, false);
  }
}

TEST(TestEncoding, BasicStringEncodingDecoding) {
  {
    // short string encoding
    const auto &encoded = abi_encode(std::string("hello"));
    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000000568656c"
        "6c6f000000000000000000000000000000000000000000000000000000");
    std::string decoded = abi_decode_all<std::string>(encoded);
    EXPECT_EQ(decoded, "hello");
  }
  {
    // long string encoding(length > 32 bytes)
    std::string origin_str = "hello worldhello worldhello worldhello world";
    const auto &encoded = abi_encode(origin_str);
    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000000000000000000000000000000000000000002c68656c"
        "6c6f20776f726c6468656c6c6f20776f726c6468656c6c6f20776f726c6468656c6c6f"
        "20776f726c640000000000000000000000000000000000000000");
    std::string decoded = abi_decode_all<std::string>(encoded);
    EXPECT_EQ(decoded, origin_str);
  }
  {
    // empty string encoding
    const auto &encoded = abi_encode(std::string(""));
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000000");
    std::string decoded = abi_decode_all<std::string>(encoded);
    EXPECT_EQ(decoded, "");
  }
  {
    // long string with 32*n length encoding
    std::string origin_str =
        "aaaaaaaabbbbbbbbccccccccddddddddaaaaaaaabbbbbbbbccccccccdddddddd";
    EXPECT_EQ(origin_str.length(), 64);
    const auto &encoded = abi_encode(origin_str);
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000040616161"
        "6161616161626262626262626263636363636363636464646464646464616161616161"
        "6161626262626262626263636363636363636464646464646464");
    std::string decoded = abi_decode_all<std::string>(encoded);
    EXPECT_EQ(decoded, origin_str);
  }
}

// address encoding and decoding
TEST(TestEncoding, BasicAddressEncodingDecoding) {

  {
    // address encoding
    const auto &encoded =
        abi_encode(Address("0x112233445566778899aa112233445566778899aa"));
    EXPECT_EQ(
        bytesToHex(encoded),
        "000000000000000000000000112233445566778899aa112233445566778899aa");
    Address decoded = abi_decode_all<Address>(encoded);
    EXPECT_EQ(decoded, Address("0x112233445566778899aa112233445566778899aa"));
  }
}

TEST(TestEncoding, BasicArrayEncodingDecoding) {
  {
    // int32 array encoding and decoding
    std::vector<int32_t> int32_array = {1, 2, 3};
    const auto &encoded = abi_encode_vector(int32_array);
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000003000000"
        "0000000000000000000000000000000000000000000000000000000001000000000000"
        "0000000000000000000000000000000000000000000000000002000000000000000000"
        "0000000000000000000000000000000000000000000003");
    std::vector<int32_t> decoded = abi_decode_vector_all<int32_t>(encoded);
    EXPECT_EQ(decoded, int32_array);
  }
  {
    // string array encoding and decoding
    std::vector<std::string> string_array = {"hello", "world"};
    const auto &encoded = abi_encode_vector(string_array);
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000002000000"
        "0000000000000000000000000000000000000000000000000000000060000000000000"
        "00000000000000000000000000000000000000000000000000a0000000000000000000"
        "000000000000000000000000000000000000000000000568656c6c6f00000000000000"
        "0000000000000000000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000005776f726c6400000000000000000000000000"
        "0000000000000000000000000000");
    std::vector<std::string> decoded =
        abi_decode_vector_all<std::string>(encoded);
    EXPECT_EQ(decoded, string_array);
  }
}

TEST(TestEncoding, BasicMultipleValueEncodingDecoding) {
  {
    // multiple value encoding and decoding
    std::string str_value = "hello";
    // std::vector<int32_t> int32_array = {1, 2, 3};
    Address addr_value = Address("0x112233445566778899aa112233445566778899aa");
    const auto &encoded = abi_encode(std::make_tuple(str_value, addr_value));
    EXPECT_EQ(
        bytesToHex(encoded),
        "0000000000000000000000000000000000000000000000000000000000000040000000"
        "000000000000000000112233445566778899aa112233445566778899aa000000000000"
        "000000000000000000000000000000000000000000000000000568656c6c6f00000000"
        "0000000000000000000000000000000000000000000000");
    uint32_t all_decoded_bytes = 0;
    uint32_t item_decoded_bytes;

    // now DTVM_CppSDK string encoding not compatible with evm abi
    all_decoded_bytes += 32;

    std::cout << "all_decoded_bytes after decode first string: "
              << all_decoded_bytes << std::endl;

    item_decoded_bytes = 0;
    Address decoded2 = abi_decode<Address>(encoded.data() + all_decoded_bytes,
                                           encoded.data() + encoded.size(),
                                           item_decoded_bytes);
    all_decoded_bytes += 32;
    EXPECT_EQ(decoded2, addr_value);
  }
}
