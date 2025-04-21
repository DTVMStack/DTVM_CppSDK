// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "hostio.hpp"
#include "math.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <tuple>
#include <type_traits>
#include <vector>

namespace dtvm {
// Some types are dynamic types, in Ethereum ABI encoding/decoding, they are
// first stored with an offset, and then the actual encoded content is placed
// later. The encoding result of n elements is divided into two parts: the first
// part is n*32 bytes, where each element in the first part only occupies 32
// bytes. Dynamic elements in the first part store the offset from the beginning
// of the first part, pointing to the actual position of the element content in
// the second part.

// Provide a template function to determine if a type is a dynamic_encoding_type
template <typename T> bool is_dynamic_encoding_type(const T &value) {
  return false;
}
template <> bool is_dynamic_encoding_type(const std::string &value) {
  return true;
}
template <typename T>
bool is_dynamic_encoding_type(const std::vector<T> &value) {
  return true;
}

template <typename, typename = std::void_t<>>
struct is_vector : std::false_type {};
// Specialization template to check if T has std::vector member functions
template <typename T>
struct is_vector<T, std::void_t<decltype(std::declval<T>().push_back(
                        std::declval<typename T::value_type>()))>>
    : std::true_type {};

template <typename, typename = std::void_t<>>
struct is_dynamic_t : std::false_type {};
// std::string or std::vector are considered dynamic types
template <typename T>
struct is_dynamic_t<T, std::void_t<decltype(std::declval<T>().push_back(
                           std::declval<typename T::value_type>()))>>
    : std::true_type {};

// abi_encode does not currently support encoding vectors; use abi_encode_vector
// for vectors
template <typename T,
          std::enable_if_t<!std::is_integral<T>::value, bool> = true>
std::vector<uint8_t> abi_encode(const T &value);

template <> std::vector<uint8_t> abi_encode(const std::string &value) {
  // bytes32 length
  uint256 length = uint256(value.size());
  const auto &length_bytes = length.bytes();
  // unpadded string bytes
  const std::vector<uint8_t> &unpadded_bytes = unpadded_string(value);

  std::vector<uint8_t> result;
  result.insert(result.end(), length_bytes.begin(), length_bytes.end());
  result.insert(result.end(), unpadded_bytes.begin(), unpadded_bytes.end());
  return result;
}

template <typename T,
          std::enable_if_t<std::is_same<T, const char *>::value ||
                               std::is_convertible<T, const char *>::value,
                           bool> = true>
std::vector<uint8_t> abi_encode(const char *value) {
  return abi_encode(std::string(value));
}

template <typename Integer,
          std::enable_if_t<std::is_integral<Integer>::value &&
                               !std::is_signed<Integer>::value,
                           bool> = true>
std::vector<uint8_t> abi_encode(const Integer &value) {
  const auto &value_bytes = uint256(value).bytes();
  std::vector<uint8_t> result;
  result.insert(result.end(), value_bytes.begin(), value_bytes.end());
  return result;
}

template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value &&
                                                 std::is_signed<Integer>::value,
                                             bool> = true>
std::vector<uint8_t> abi_encode(const Integer &value) {
  // we cannot directly cast to __int128_t here, as it would sign-extend
  // negative numbers
  const auto &value_bytes = uint256(__uint128_t(value)).bytes();
  std::vector<uint8_t> result;
  result.insert(result.end(), value_bytes.begin(), value_bytes.end());
  // Since negative numbers extended to uint128 will still be signed, to ensure
  // correctness, we zero out the leading bytes based on the size of the integer
  // type.
  memset(result.data(), 0x0, 32 /* sizeof(uint256_t) */ - sizeof(Integer));
  return result;
}

template <> std::vector<uint8_t> abi_encode(const uint256 &value) {
  const auto &value_bytes = value.bytes();
  std::vector<uint8_t> result;
  result.insert(result.end(), value_bytes.begin(), value_bytes.end());
  return result;
}

template <> std::vector<uint8_t> abi_encode(const Address &value) {
  const auto &addr_bytes = value.to_bytes32();
  std::vector<uint8_t> result;
  result.insert(result.end(), addr_bytes.begin(), addr_bytes.end());
  return result;
}

template <typename T>
void process_abi_encode_tuple_element(std::vector<uint8_t> &prefix_part,
                                      std::vector<uint8_t> &suffix_part,
                                      uint32_t prefix_part_bytes_count,
                                      const T &arg) {
  const auto &arg_encoded =
      abi_encode<T>(arg); // now arg can't be a vector type
  if (is_dynamic_encoding_type<T>(arg)) {
    // write offset in prefix part, and write data in suffix part
    uint32_t offset = prefix_part_bytes_count + (uint32_t)suffix_part.size();
    const auto &offset_bytes = uint256(offset).bytes();
    prefix_part.insert(prefix_part.end(), offset_bytes.begin(),
                       offset_bytes.end());
    suffix_part.insert(suffix_part.end(), arg_encoded.begin(),
                       arg_encoded.end());
  } else {
    prefix_part.insert(prefix_part.end(), arg_encoded.begin(),
                       arg_encoded.end());
  }
}

template <typename T1, typename... Args>
std::vector<uint8_t> abi_encode(const std::tuple<T1, Args...> &value) {
  // If the element type is dynamic, write the offset first,
  // then append to the end (return type cannot be simply std::vector<uint8_t>)
  std::vector<uint8_t> prefix_part;
  std::vector<uint8_t> suffix_part;
  uint32_t prefix_part_bytes_count =
      std::tuple_size<std::tuple<T1, Args...>>::value * 32;

  std::apply(
      [&prefix_part, &suffix_part, &prefix_part_bytes_count](auto &&...args) {
        (void(process_abi_encode_tuple_element(prefix_part, suffix_part,
                                               prefix_part_bytes_count, args)),
         ...);
      },
      value);
  std::vector<uint8_t> result = prefix_part;
  result.insert(result.end(), suffix_part.begin(), suffix_part.end());
  return result;
}

template <typename T>
std::vector<uint8_t> abi_encode_vector(const std::vector<T> &value) {
  std::vector<uint8_t> result;
  // 1. write length
  const auto &length = uint256(value.size());
  const auto &length_bytes = length.bytes();
  result.insert(result.end(), length_bytes.begin(), length_bytes.end());
  // 2. write elements
  std::vector<uint8_t> all_dynamic_contents;
  for (size_t i = 0; i < value.size(); i++) {
    const auto &item = value[i];
    const auto &element_encoded = abi_encode(item);
    if (is_dynamic_encoding_type<T>(item)) {
      // write offset in prefix part, and write data in suffix part
      uint32_t offset =
          32 + 32 * value.size() +
          all_dynamic_contents.size(); // first 32bytes is vector length, second
                                       // 32bytes is current offset used bytes
      const auto &offset_bytes = uint256(offset).bytes();
      result.insert(result.end(), offset_bytes.begin(), offset_bytes.end());
      all_dynamic_contents.insert(all_dynamic_contents.end(),
                                  element_encoded.begin(),
                                  element_encoded.end());
    } else {
      result.insert(result.end(), element_encoded.begin(),
                    element_encoded.end());
    }
  }
  result.insert(result.end(), all_dynamic_contents.begin(),
                all_dynamic_contents.end());
  return result;
}

// abi_decode does not currently support decoding vectors; use abi_decode_vector
// for vectors
template <typename T, std::enable_if_t<!std::is_integral<T>::value ||
                                           std::is_same<T, bool>::value,
                                       bool> = true>
T abi_decode(const uint8_t *data, const uint8_t *data_end,
             uint32_t &read_bytes_out /* out */);

// other ints abi_decode
template <typename T, typename = typename std::enable_if<
                          std::is_integral<T>::value &&
                          !std::is_same<T, bool>::value>::type>
T abi_decode(const uint8_t *data, const uint8_t *data_end,
             uint32_t &read_bytes_out) {
  if ((data + 32) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  bytes32 value_bytes;
  memcpy(value_bytes.data(), data, 32);
  read_bytes_out = 32;
  __uint128_t u128_value = uint256(value_bytes).to_uint128();
  return (T)u128_value;
}

template <>
uint256 abi_decode(const uint8_t *data, const uint8_t *data_end,
                   uint32_t &read_bytes_out) {
  if ((data + 32) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  bytes32 value_bytes;
  memcpy(value_bytes.data(), data, 32);
  read_bytes_out = 32;
  auto result = uint256(value_bytes);
  return result;
}

template <>
bool abi_decode(const uint8_t *data, const uint8_t *data_end,
                uint32_t &read_bytes_out) {
  if ((data + 1) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  read_bytes_out = 32;
  for (size_t i = 0; i < 32; i++) {
    if (data[i] != 0) {
      return true;
    }
  }
  return false;
}

template <>
Address abi_decode(const uint8_t *data, const uint8_t *data_end,
                   uint32_t &read_bytes_out) {
  if ((data + 32) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  bytes32 addr_bytes;
  memcpy(addr_bytes.data(), data, 32);
  read_bytes_out = 32;
  return Address(addr_bytes);
}

template <>
std::string abi_decode(const uint8_t *data, const uint8_t *data_end,
                       uint32_t &read_bytes_out) {
  // read bytes32 to uint256 as big endian
  if ((data + 32) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  bytes32 length_bytes;
  memcpy(length_bytes.data(), data, 32);
  uint32_t length = uint256(length_bytes).to_uint32();
  uint32_t unpadded_parts = (length + 31) / 32;
  std::vector<uint8_t> unpadded_bytes;
  int offset = 32; // read 32 bytes as length
  if ((data + offset + 32 * unpadded_parts) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  for (uint32_t i = 0; i < unpadded_parts; ++i) {
    unpadded_bytes.insert(unpadded_bytes.end(), data + offset + 32 * i,
                          data + offset + 32 * (i + 1));
  }
  read_bytes_out = 32 + unpadded_parts * 32;
  return std::string(unpadded_bytes.begin(), unpadded_bytes.begin() + length);
}

template <typename E>
std::vector<E> abi_decode_vector(const uint8_t *data, const uint8_t *data_end,
                                 uint32_t &read_bytes_out) {
  // decode length, then decode elements
  std::vector<E> result;
  if ((data + 32) > data_end) {
    hostio::revert("abi_decode: data is too short");
  }
  bytes32 length_bytes;
  memcpy(length_bytes.data(), data, 32);
  uint32_t length = uint256(length_bytes).to_uint32();
  for (uint32_t i = 0; i < length; i++) {
    uint32_t offset = 32 + i * 32;
    if (is_dynamic_t<E>()) {
      bytes32 offset_bytes;
      memcpy(offset_bytes.data(), data + 32 + i * 32, 32);
      offset = uint256(offset_bytes).to_uint32();
      if ((data + offset + 32) > data_end) {
        hostio::revert("abi_decode: data is too short");
      }
    }

    uint32_t element_read_bytes_count = 0;
    E element =
        abi_decode<E>(data + offset, data_end, element_read_bytes_count);
    result.push_back(element);
  }
  read_bytes_out = 32 + length * 32;
  return result;
}

template <typename T> T abi_decode_all(const std::vector<uint8_t> &data) {
  uint32_t read_bytes = 0;
  T result = abi_decode<T>(data.data(), data.data() + data.size(), read_bytes);
  if (read_bytes != data.size()) {
    hostio::revert("abi_decode_single: data is not fully consumed");
  }
  return result;
}
template <typename E>
std::vector<E> abi_decode_vector_all(const std::vector<uint8_t> &data) {
  uint32_t read_bytes = 0;
  std::vector<E> result =
      abi_decode_vector<E>(data.data(), data.data() + data.size(), read_bytes);
  return result;
}

} // namespace dtvm
