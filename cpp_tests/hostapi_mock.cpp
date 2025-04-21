// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "utils.hpp"
#include <array>
#include <contractlib/v1/hostapi.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

extern "C" {
// hostapis

#ifndef NDEBUG
__attribute__((import_module("env"), import_name("debug_bytes"))) void
debug_bytes(ADDRESS_UINT data_offset, int32_t data_length) {
  std::cout << "debug_log: "
            << std::string((char *)data_offset, (size_t)data_length)
            << std::endl;
}
#else
#define debug_bytes(data_offset, data_length)
#endif

static std::array<uint8_t, 20> MOCK_CURRENT_CONTRACT_ADDR = {
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};

__attribute__((import_module("env"), import_name("getAddress"))) void
getAddress(ADDRESS_UINT result_offset) {
  uint8_t *result_offset_ptr = (uint8_t *)result_offset;
  // use mocked current contract addr
  memcpy(result_offset_ptr, MOCK_CURRENT_CONTRACT_ADDR.data(), 20);
}

__attribute__((import_module("env"), import_name("getCaller"))) void
getCaller(ADDRESS_UINT result_offset) {
  uint8_t *result_offset_ptr = (uint8_t *)result_offset;
  // use mocked current caller addr
  memset(result_offset_ptr, 0x22, 20);
}

__attribute__((import_module("env"), import_name("getCallValue"))) void
getCallValue(ADDRESS_UINT result_offset) {
  // uint256 big endian value
  memset((uint8_t *)result_offset, 0x0, 32);
}

__attribute__((import_module("env"), import_name("getCallDataSize"))) int32_t
getCallDataSize() {
  return 4; // mocked call data size
}

__attribute__((import_module("env"), import_name("callDataCopy"))) void
callDataCopy(ADDRESS_UINT target, ADDRESS_UINT offset, int32_t len) {
  // mocked calldata
  uint8_t mocked_calldata[] = {0x11, 0x22, 0x33, 0x44};
  memcpy((uint8_t *)target, (uint8_t *)(&mocked_calldata + offset),
         (size_t)len);
}

__attribute__((import_module("env"), import_name("getBlockHash"))) int32_t
getBlockHash(int64_t number, ADDRESS_UINT result_offset) {
  uint8_t mocked_blockhash[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                0x99, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                                0x77, 0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44,
                                0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0x11, 0x22};
  memcpy((uint8_t *)result_offset, (uint8_t *)&mocked_blockhash, 32);
}

__attribute__((import_module("env"), import_name("getBlockCoinbase"))) void
getBlockCoinbase(ADDRESS_UINT result_offset) {
  uint8_t mocked_blockcoinbase[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                    0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44,
                                    0x55, 0x66, 0x77, 0x88, 0x99, 0x00};
  memcpy((uint8_t *)result_offset, (uint8_t *)&mocked_blockcoinbase, 20);
}

__attribute__((import_module("env"), import_name("getBlockPrevRandao"))) void
getBlockPrevRandao(ADDRESS_UINT result_offset) {
  uint8_t mocked_blockprevrandao[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                      0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44,
                                      0x55, 0x66, 0x77, 0x88, 0x99, 0x00};
  memcpy((uint8_t *)result_offset, (uint8_t *)&mocked_blockprevrandao, 32);
}

__attribute__((import_module("env"), import_name("getBlockGasLimit"))) int64_t
getBlockGasLimit() {
  return 300000000;
}

__attribute__((import_module("env"), import_name("getBlockTimestamp"))) int64_t
getBlockTimestamp() {
  return 1234567890;
}

__attribute__((import_module("env"), import_name("getGasLeft"))) int64_t
getGasLeft() {
  return 1000000;
}

__attribute__((import_module("env"), import_name("getBlockNumber"))) int64_t
getBlockNumber() {
  return 12345;
}

__attribute__((import_module("env"), import_name("getTxGasPrice"))) void
getTxGasPrice(ADDRESS_UINT value_offset) {
  __uint128_t *value_ptr = (__uint128_t *)value_offset;
  *value_ptr = 2000000000L;
}

__attribute__((import_module("env"), import_name("getTxOrigin"))) void
getTxOrigin(ADDRESS_UINT result_offset) {
  // use current tx origin sender addr
  memset((uint8_t *)result_offset, 0x11, 20);
}

__attribute__((import_module("env"), import_name("getBaseFee"))) void
getBaseFee(ADDRESS_UINT result_offset) {
  __uint128_t *value_ptr = (__uint128_t *)result_offset;
  *value_ptr = 1000000000L;
}

__attribute__((import_module("env"), import_name("getBlobBaseFee"))) void
getBlobBaseFee(ADDRESS_UINT result_offset) {
  __uint128_t *value_ptr = (__uint128_t *)result_offset;
  *value_ptr = 1000000000L;
}

__attribute__((import_module("env"), import_name("getChainId"))) void
getChainId(ADDRESS_UINT result_offset) {
  // chainId is 32bytes
  memset((uint8_t *)result_offset, 0x00, 32 - 1);
  ((uint8_t *)result_offset)[31] = 0x11;
}

__attribute__((import_module("env"), import_name("getExternalBalance"))) void
getExternalBalance(ADDRESS_UINT address_offset, ADDRESS_UINT result_offset) {
  // balance is uint256 big endian bytes
  std::memset((uint8_t *)result_offset, 0x0, 32);
}

__attribute__((import_module("env"), import_name("getExternalCodeHash"))) void
getExternalCodeHash(ADDRESS_UINT address_offset, ADDRESS_UINT result_offset) {
  uint8_t mocked_codehash[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                               0x99, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                               0x77, 0x88, 0x99, 0x00, 0x11, 0x22, 0x33, 0x44,
                               0x55, 0x66, 0x77, 0x88, 0x99, 0x00, 0x11, 0x22};
  memcpy((uint8_t *)result_offset, (uint8_t *)&mocked_codehash, 32);
}

class MockStorageHolder {
public:
  static MockStorageHolder *getInstance() {
    static MockStorageHolder instance;
    return &instance;
  }
  static MockStorageHolder *getTransientInstance() {
    static MockStorageHolder transient_instance;
    return &transient_instance;
  }
  void setStorage(const std::string &contract_addr_hex,
                  const std::string &key_hex,
                  const std::array<uint8_t, 32> &value) {
    if (storage_data_.find(contract_addr_hex) == storage_data_.end()) {
      storage_data_[contract_addr_hex] =
          std::map<std::string, std::array<uint8_t, 32>>();
    }
    storage_data_[contract_addr_hex][key_hex] = value;
    std::cout << "setStorage: " << contract_addr_hex << "." << key_hex << ": "
              << bytesToHex(value) << std::endl;
  }
  std::array<uint8_t, 32> getStorage(const std::string &contract_addr_hex,
                                     const std::string &key_hex) {
    if (storage_data_.find(contract_addr_hex) == storage_data_.end()) {
      std::array<uint8_t, 32> zeros = {0};
      std::cout << "getStorage: " << contract_addr_hex << "." << key_hex << ": "
                << bytesToHex(zeros) << std::endl;
      return zeros;
    }
    if (storage_data_[contract_addr_hex].find(key_hex) ==
        storage_data_[contract_addr_hex].end()) {
      std::array<uint8_t, 32> zeros = {0};
      std::cout << "getStorage: " << contract_addr_hex << "." << key_hex << ": "
                << bytesToHex(zeros) << std::endl;
      return zeros;
    }
    const auto &result = storage_data_[contract_addr_hex][key_hex];
    std::cout << "getStorage: " << contract_addr_hex << "." << key_hex << ": "
              << bytesToHex(result) << std::endl;
    return result;
  }
  void clear() {
    storage_data_.clear();
    std::cout << "mocked storage cleared" << std::endl;
  }

private:
  std::map<std::string, std::map<std::string, std::array<uint8_t, 32>>>
      storage_data_;
};

void clear_mock_storage() { MockStorageHolder::getInstance()->clear(); }

static std::array<uint8_t, 32> read_bytes32_from_address(ADDRESS_UINT address) {
  std::array<uint8_t, 32> bytes;
  memcpy(bytes.data(), (uint8_t *)address, 32);
  return bytes;
}

__attribute__((import_module("env"), import_name("storageLoad"))) void
storageLoad(ADDRESS_UINT key_offset, ADDRESS_UINT result_offset) {
  const auto &data = MockStorageHolder::getInstance()->getStorage(
      bytesToHex(MOCK_CURRENT_CONTRACT_ADDR),
      bytesToHex(read_bytes32_from_address(key_offset)));
  memcpy((uint8_t *)result_offset, data.data(), 32);
}

__attribute__((import_module("env"), import_name("storageStore"))) void
storageStore(ADDRESS_UINT key_offset, ADDRESS_UINT value_offset) {
  MockStorageHolder::getInstance()->setStorage(
      bytesToHex(MOCK_CURRENT_CONTRACT_ADDR),
      bytesToHex(read_bytes32_from_address(key_offset)),
      read_bytes32_from_address(value_offset));
}

__attribute__((import_module("env"), import_name("transientStore"))) void
transientStore(ADDRESS_UINT key_offset, ADDRESS_UINT value_offset) {
  MockStorageHolder::getTransientInstance()->setStorage(
      bytesToHex(MOCK_CURRENT_CONTRACT_ADDR),
      bytesToHex(read_bytes32_from_address(key_offset)),
      read_bytes32_from_address(value_offset));
}

__attribute__((import_module("env"), import_name("transientLoad"))) void
transientLoad(ADDRESS_UINT key_offset, ADDRESS_UINT result_offset) {
  const auto &data = MockStorageHolder::getTransientInstance()->getStorage(
      bytesToHex(MOCK_CURRENT_CONTRACT_ADDR),
      bytesToHex(read_bytes32_from_address(key_offset)));
  memcpy((uint8_t *)result_offset, data.data(), 32);
}

__attribute__((import_module("env"), import_name("codeCopy"))) void
codeCopy(ADDRESS_UINT result_offset, int32_t code_offset, int32_t length) {
  uint8_t mocked_code[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00};
  // use mocked code
  memcpy((uint8_t *)result_offset, (uint8_t *)&mocked_code, (size_t)length);
}

__attribute__((import_module("env"), import_name("getCodeSize"))) int32_t
getCodeSize() {
  uint8_t mocked_code[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00};
  return (int32_t)(sizeof(mocked_code) / sizeof(uint8_t));
}

__attribute__((import_module("env"), import_name("externalCodeCopy"))) void
externalCodeCopy(ADDRESS_UINT address_offset, ADDRESS_UINT result_offset,
                 int32_t code_offset, int32_t length) {
  uint8_t mocked_code[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00};
  // mocked code
  memcpy((uint8_t *)result_offset, (uint8_t *)&mocked_code, (size_t)length);
}

__attribute__((import_module("env"), import_name("getExternalCodeSize")))
int32_t
getExternalCodeSize(ADDRESS_UINT address_offset) {
  uint8_t mocked_code[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00};
  return (int32_t)(sizeof(mocked_code) / sizeof(uint8_t));
}

__attribute__((import_module("env"), import_name("callContract"))) int32_t
callContract(int64_t gas, ADDRESS_UINT addressOffset, ADDRESS_UINT valueOffset,
             ADDRESS_UINT dataOffset, int32_t dataLength) {
  // no mock now
  return 0;
}

__attribute__((import_module("env"), import_name("callCode"))) int32_t
callCode(int64_t gas, ADDRESS_UINT addressOffset, ADDRESS_UINT valueOffset,
         ADDRESS_UINT dataOffset, int32_t dataLength) {
  // no mock now
  return 0;
}

__attribute__((import_module("env"), import_name("callDelegate"))) int32_t
callDelegate(int64_t gas, ADDRESS_UINT addressOffset, ADDRESS_UINT dataOffset,
             int32_t dataLength) {
  // no mock now
  return 0;
}

__attribute__((import_module("env"), import_name("callStatic"))) int32_t
callStatic(int64_t gas, ADDRESS_UINT addressOffset, ADDRESS_UINT dataOffset,
           int32_t dataLength) {
  // no mock now
  return 0;
}

__attribute__((import_module("env"), import_name("createContract"))) int32_t
createContract(ADDRESS_UINT valueOffset, ADDRESS_UINT codeOffset,
               int32_t codeLength, ADDRESS_UINT dataOffset, int32_t dataLength,
               ADDRESS_UINT saltOffset, int32_t is_create2,
               ADDRESS_UINT resultOffset) {
  // no mock now
  return 0;
}

__attribute__((import_module("env"), import_name("finish"))) void
finish(ADDRESS_UINT data_offset, int32_t length) {
  std::cout << "wasm context finished with data: "
            << std::string((char *)data_offset, length) << std::endl;
  // exit the wasm context
}

__attribute__((import_module("env"), import_name("revert"))) void
revert(ADDRESS_UINT data_offset, int32_t length) {
  std::cout << "wasm context reverted with data: "
            << std::string((char *)data_offset, length) << std::endl;
  // exit the wasm context with revert status
}

__attribute__((import_module("env"), import_name("emitLogEvent"))) void
emitLogEvent(ADDRESS_UINT data_offset, int32_t length, int32_t number_of_topics,
             ADDRESS_UINT topic1, ADDRESS_UINT topic2, ADDRESS_UINT topic3,
             ADDRESS_UINT topic4) {
  std::cout << "emited log event, topics: " << std::endl;
  if (number_of_topics > 0) {
    std::cout << bytesToHex(std::vector<uint8_t>((uint8_t *)topic1,
                                                 (uint8_t *)topic1 + 32))
              << std::endl;
  }
  if (number_of_topics > 1) {
    std::cout << bytesToHex(std::vector<uint8_t>((uint8_t *)topic2,
                                                 (uint8_t *)topic2 + 32))
              << std::endl;
  }
  if (number_of_topics > 2) {
    std::cout << bytesToHex(std::vector<uint8_t>((uint8_t *)topic3,
                                                 (uint8_t *)topic3 + 32))
              << std::endl;
  }
  if (number_of_topics > 3) {
    std::cout << bytesToHex(std::vector<uint8_t>((uint8_t *)topic4,
                                                 (uint8_t *)topic4 + 32))
              << std::endl;
  }
  std::cout << "data: "
            << bytesToHex(std::vector<uint8_t>((uint8_t *)data_offset,
                                               (uint8_t *)data_offset + length))
            << std::endl;
}

__attribute__((import_module("env"), import_name("getReturnDataSize"))) int32_t
getReturnDataSize() {
  return 0;
}

__attribute__((import_module("env"), import_name("returnDataCopy"))) void
returnDataCopy(ADDRESS_UINT resultOffset, int32_t dataOffset, int32_t length) {
  // copy no data
}

__attribute__((import_module("env"), import_name("selfDestruct"))) void
selfDestruct(ADDRESS_UINT address_offset) {
  std::cout << "self destruct to addr "
            << bytesToHex(std::vector<uint8_t>((uint8_t *)address_offset,
                                               (uint8_t *)address_offset + 20))
            << std::endl;
}

// this is a mock implementation for test
std::array<uint8_t, 32> mockHash(const std::vector<uint8_t> &input) {
  // Assume a fixed-size hash output of 256 bits
  std::array<uint8_t, 32> hash = {0};
  // Simulate mixing input bytes with a simple XOR operation
  for (size_t i = 0; i < input.size(); ++i) {
    hash[i % 32] ^= input[i];
    hash[i % 32] += 1;
  }
  if (input.size() == 32) {
    // input and hash cannot be the same,
    // otherwise it will cause conflicts with dynamic structures like bytes,
    // string, array, etc.
    bool same = true;
    for (size_t i = 0; i < 32; i++) {
      if (input[i] != hash[i]) {
        same = false;
        break;
      }
    }
    if (same) {
      throw std::runtime_error("mock hash can't be same with input");
    }
  }
  // Return the "hashed" result
  return hash;
}

__attribute__((import_module("env"), import_name("keccak256"))) void
keccak256(ADDRESS_UINT inputOffset, int32_t inputLength,
          ADDRESS_UINT resultOffset) {
  std::vector<uint8_t> input((uint8_t *)inputOffset,
                             (uint8_t *)inputOffset + inputLength);
  const auto &hash = mockHash(input);
  std::cout << "mock keccak256(" << bytesToHex(input)
            << ") = " << bytesToHex(hash) << std::endl;
  memcpy((uint8_t *)resultOffset, (uint8_t *)&hash, 32);
}

__attribute__((import_module("env"), import_name("sha256"))) void
sha256(ADDRESS_UINT inputOffset, int32_t inputLength,
       ADDRESS_UINT resultOffset) {
  std::vector<uint8_t> input((uint8_t *)inputOffset,
                             (uint8_t *)inputOffset + inputLength);
  const auto &hash = mockHash(input);
  std::cout << "mock sha256(" << bytesToHex(input) << ") = " << bytesToHex(hash)
            << std::endl;
  memcpy((uint8_t *)resultOffset, (uint8_t *)&hash, 32);
}
}
