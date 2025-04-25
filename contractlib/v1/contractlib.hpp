// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

// Version information
#define DTVM_CPP_SDK_VERSION_MAJOR 0
#define DTVM_CPP_SDK_VERSION_MINOR 1
#define DTVM_CPP_SDK_VERSION_PATCH 0
#define DTVM_CPP_SDK_VERSION_STRING "0.1.0"

#include "encoding.hpp"
#include "hostio.hpp"
#include "math.hpp"
#include "storage.hpp"
#include "types.hpp"
#include "wasi.hpp"
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#define SOLIDITY(contract_name, solidity_code)

namespace dtvm {

class Input {
public:
  inline Input(const uint8_t *data, uint32_t len) {
    data_ = data;
    len_ = len;
    offset_ = 0;
  }

  static Input from_hostio() {
    return Input(dtvm::hostio::get_args(), dtvm::hostio::get_args_len());
  }

  template <typename T,
            std::enable_if_t<std::is_same<T, Address>::value, bool> = true>
  Address read() {
    uint32_t read_count = 0;
    Address result =
        abi_decode<Address>(data_ + offset_, data_ + len_, read_count);
    offset_ += read_count;
    return result;
  }

  template <typename T,
            std::enable_if_t<std::is_same<T, std::string>::value, bool> = true>
  std::string read() {
    // read offset first
    uint32_t read_count = 0;
    uint256 str_offset =
        abi_decode<uint256>(data_ + offset_, data_ + len_, read_count);
    offset_ += read_count;
    uint32_t str_offset_int = str_offset.to_uint32();
    // then read string in new position
    std::string str = abi_decode<std::string>(data_ + str_offset_int,
                                              data_ + len_, read_count);
    return str;
  }

  template <typename T,
            std::enable_if_t<std::is_integral<T>::value, bool> = true>
  T read() {
    uint32_t read_count = 0;
    T result = abi_decode<T>(data_ + offset_, data_ + len_, read_count);
    offset_ += read_count;
    return result;
  }

  template <typename T,
            std::enable_if_t<std::is_same<T, bool>::value, bool> = true>
  bool read() {
    uint32_t read_count = 0;
    bool result = abi_decode<bool>(data_ + offset_, data_ + len_, read_count);
    offset_ += read_count;
    return result;
  }

  template <typename T,
            std::enable_if_t<std::is_same<T, uint256>::value, bool> = true>
  uint256 read() {
    uint32_t read_count = 0;
    uint256 result =
        abi_decode<uint256>(data_ + offset_, data_ + len_, read_count);
    offset_ += read_count;
    return result;
  }

  inline uint32_t read_selector() {
    if (data_ + offset_ + 4 > data_ + len_) {
      hostio::revert("abi_decode: data is too short");
    }
    const uint8_t *int_bytes_begin = data_ + offset_;
    uint32_t result = static_cast<uint32_t>(int_bytes_begin[0]) << 24 |
                      static_cast<uint32_t>(int_bytes_begin[1]) << 16 |
                      static_cast<uint32_t>(int_bytes_begin[2]) << 8 |
                      static_cast<uint32_t>(int_bytes_begin[3]);
    offset_ += 4; // 4 bytes of int
    return result;
  }

  inline bool empty() const { return len_ == 0; }

  inline bool eof() const { return offset_ >= len_; }

  inline const uint8_t *data() const { return data_; }
  inline uint32_t size() const { return len_; }

private:
  const uint8_t *data_;
  uint32_t len_;
  uint32_t offset_ = 0;
};

class CResult {
public:
  inline CResult(const std::vector<uint8_t> &data, bool success,
                 int32_t ret_code) {
    data_ = data;
    success_ = success;
    ret_code_ = ret_code;
  }

  bool success() const { return success_; }
  int32_t ret_code() const { return ret_code_; }
  const std::vector<uint8_t> &data() const { return data_; }

private:
  bool success_;
  int32_t ret_code_;
  std::vector<uint8_t> data_;
};

template <typename T> CResult Ok(const T &arg) {
  // abi encode arg to bytes
  std::vector<uint8_t> data = abi_encode<T>(arg);
  return CResult(data, true, 0);
}

CResult Ok() { return CResult({}, true, 0); }

template <typename T> CResult Revert(const T &arg, int32_t ret_code = -1) {
  // abi encode arg to bytes
  std::vector<uint8_t> data = abi_encode<T>(arg);
  return CResult(data, false, ret_code);
}

void require(bool condition, const std::string &err) {
  if (!condition) {
    hostio::revert((const uint8_t *)err.c_str(), (int32_t)err.size());
  }
}

Address get_msg_sender() {
  static std::shared_ptr<Address> addr = nullptr;
  if (!addr) {
    addr = std::make_shared<Address>();
    ::getCaller((int32_t) reinterpret_cast<intptr_t>(addr->data()));
  }
  return *addr;
}

uint256 get_msg_value() {
  static std::shared_ptr<uint256> value = nullptr;
  if (!value) {
    bytes32 value_be_bytes;
    ::getCallValue((int32_t) reinterpret_cast<intptr_t>(value_be_bytes.data()));
    value = std::make_shared<uint256>(value_be_bytes);
  }
  return uint256(*value);
}

uint64_t get_gas_left() {
  static std::shared_ptr<uint64_t> gas = nullptr;
  if (!gas) {
    gas = std::make_shared<uint64_t>(0);
    auto gas_left = ::getGasLeft();
    *gas = gas_left;
  }
  return *gas;
}

std::vector<uint8_t> get_msg_data() {
  static std::shared_ptr<std::vector<uint8_t>> data = nullptr;
  if (!data) {
    data = std::make_shared<std::vector<uint8_t>>();
    int32_t data_len = ::getCallDataSize();
    data->resize(data_len);
    ::callDataCopy((int32_t) reinterpret_cast<intptr_t>(data->data()), 0,
                   data_len);
  }
  return *data;
}

uint64_t get_block_timestamp() {
  static std::shared_ptr<uint64_t> timestamp = nullptr;
  if (!timestamp) {
    timestamp = std::make_shared<uint64_t>(0);
    *timestamp = (uint64_t)::getBlockTimestamp();
  }
  return *timestamp;
}

uint64_t get_block_number() {
  static std::shared_ptr<uint64_t> number = nullptr;
  if (!number) {
    number = std::make_shared<uint64_t>(0);
    *number = (uint64_t)::getBlockNumber();
  }
  return *number;
}

std::vector<uint8_t> get_block_coinbase() {
  static std::shared_ptr<std::vector<uint8_t>> coinbase = nullptr;
  if (!coinbase) {
    coinbase = std::make_shared<std::vector<uint8_t>>(32);
    ::getBlockCoinbase((int32_t) reinterpret_cast<intptr_t>(coinbase->data()));
  }
  return *coinbase;
}

uint64_t get_block_gas_limit() {
  static std::shared_ptr<uint64_t> gas_limit = nullptr;
  if (!gas_limit) {
    gas_limit = std::make_shared<uint64_t>(0);
    *gas_limit = (uint64_t)::getBlockGasLimit();
  }
  return *gas_limit;
}

Address get_tx_origin() {
  static std::shared_ptr<Address> origin = nullptr;
  if (!origin) {
    origin = std::make_shared<Address>();
    ::getTxOrigin((int32_t) reinterpret_cast<intptr_t>(origin->data()));
  }
  return *origin;
}

Address get_current_contract() {
  static std::shared_ptr<Address> contract = nullptr;
  if (!contract) {
    contract = std::make_shared<Address>();
    ::getAddress((int32_t) reinterpret_cast<intptr_t>(contract->data()));
  }
  return *contract;
}

uint256 get_external_balance(const Address &addr) {
  bytes32 balance_be_bytes;
  ::getExternalBalance(
      (ADDRESS_UINT) reinterpret_cast<intptr_t>(addr.data()),
      (ADDRESS_UINT) reinterpret_cast<intptr_t>(balance_be_bytes.data()));
  return uint256(balance_be_bytes);
}

struct CallInfo {
  uint256 value = 0;
  uint64_t gas = 0;
};
typedef std::shared_ptr<CallInfo> CallInfoPtr;

inline CallInfoPtr default_call_info() {
  auto call_info = std::make_shared<CallInfo>();
  call_info->value = 0;
  call_info->gas = 0;
  return call_info;
}

inline CallInfoPtr current_call_info() {
  auto call_info = std::make_shared<CallInfo>();
  call_info->value = get_msg_value();
  call_info->gas =
      get_gas_left() * 63 /
      64; // left a little gas left for processing when sub contract out of gas
  return call_info;
}

template <typename Func>
CResult wrapper_call_contract(Func call_func, const Address &to,
                              const std::vector<uint8_t> &encoded_input,
                              uint256 value, uint64_t gas) {
  bytes32 value_be_bytes = value.bytes();
  int32_t ret =
      call_func((int64_t)gas, (int32_t) reinterpret_cast<intptr_t>(to.data()),
                (int32_t) reinterpret_cast<intptr_t>(value_be_bytes.data()),
                (int32_t) reinterpret_cast<intptr_t>(encoded_input.data()),
                (int32_t)encoded_input.size());
  if (ret == 0) {
    int32_t result_len = ::getReturnDataSize();
    if (result_len == 0) {
      return Ok();
    }
    std::vector<uint8_t> call_result;
    ::returnDataCopy((int32_t) reinterpret_cast<intptr_t>(call_result.data()),
                     0, result_len);
    return CResult(call_result, true, 0);
  }
  // get hostio result and revert
  int32_t result_len = ::getReturnDataSize();
  if (result_len == 0) {
    return Revert("call failed", ret);
  }
  std::vector<uint8_t> err;
  ::returnDataCopy((int32_t) reinterpret_cast<intptr_t>(err.data()), 0,
                   result_len);
  return CResult(err, false, ret);
}

inline CResult call(const Address &to,
                    const std::vector<uint8_t> &encoded_input, uint256 value,
                    uint64_t gas) {
  return wrapper_call_contract(::callContract, to, encoded_input, value, gas);
}

inline CResult call_code(const Address &to,
                         const std::vector<uint8_t> &encoded_input,
                         uint256 value, uint64_t gas) {
  return wrapper_call_contract(::callCode, to, encoded_input, value, gas);
}

inline CResult call_delegate(const Address &to,
                             const std::vector<uint8_t> &encoded_input,
                             uint64_t gas) {
  return wrapper_call_contract(dtvm::hostio::callDelegateWithZeroValue, to,
                               encoded_input, uint256(0), gas);
}

inline CResult call_static(const Address &to,
                           const std::vector<uint8_t> &encoded_input,
                           uint64_t gas) {
  return wrapper_call_contract(dtvm::hostio::callStaticWithZeroValue, to,
                               encoded_input, uint256(0), gas);
}

class Contract {
protected:
  // The CONSTRUCTOR macro is currently difficult to design, so contract
  // constructors should be implemented in subclasses
  virtual CResult constructor(CallInfoPtr msg, Input &input) = 0;

public:
  virtual void receive() {
    // can be override to receive token directly
  }
  virtual void fallback() {
    // can be override to process if no method matched
    const std::string err = "no method matched";
    hostio::revert((const uint8_t *)err.c_str(), (int32_t)err.size());
  }
};

namespace contract {
inline void write_result(const CResult &result) {
  if (result.success()) {
    hostio::finish(result.data().data(), (int32_t)result.data().size());
  } else {
    hostio::revert(result.data().data(), (int32_t)result.data().size());
  }
}
} // namespace contract
} // namespace dtvm

#define ENTRYPOINT(ContractImpl)                                               \
  extern "C" void call() {                                                     \
    dtvm::Input input = dtvm::Input::from_hostio();                            \
    ContractImpl impl = ContractImpl();                                        \
    if (input.empty()) {                                                       \
      impl.receive();                                                          \
      return;                                                                  \
    }                                                                          \
    auto result = impl.dispatch(dtvm::current_call_info(), input);             \
    dtvm::contract::write_result(result);                                      \
  }                                                                            \
  extern "C" void deploy() {                                                   \
    dtvm::Input input = dtvm::Input::from_hostio();                            \
    ContractImpl impl = ContractImpl();                                        \
    if (input.empty()) {                                                       \
      impl.receive();                                                          \
      return;                                                                  \
    }                                                                          \
    auto result = impl.dispatch_constructor(dtvm::current_call_info(), input); \
    dtvm::contract::write_result(result);                                      \
  }
