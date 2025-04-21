// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "hostio.hpp"
#include "math.hpp"
#include "storage_slot.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <cstdint>
#include <string>

/// https://docs.soliditylang.org/en/latest/internals/layout_in_storage.html

namespace dtvm {

// Read and decode bytes or string from a storage slot
// https://docs.soliditylang.org/en/latest/internals/layout_in_storage.html#bytes-and-string
// For bytes not exceeding 31 bytes, the storage format is bytes +
// byte(length*2) padded to 32 bytes For bytes exceeding 32 bytes, the current
// slot stores the length*2+1 big endian encoding, and the actual content exists
// in several slots starting from keccak256(slot)
std::vector<uint8_t> decode_bytes_or_string_from_slot(const StorageSlot &slot);

// Encode and store bytes or string into a storage slot
// https://docs.soliditylang.org/en/latest/internals/layout_in_storage.html#bytes-and-string
// For bytes not exceeding 31 bytes, the encoding is bytes + byte(length*2)
// padded to 32 bytes For bytes exceeding 32 bytes, the encoding is length*2+1
// big endian encoding, and the actual content exists in several slots starting
// from keccak256(slot)
void encode_and_store_bytes_or_string_in_storage_slot(
    const StorageSlot &slot, const std::vector<uint8_t> &bytes);
void encode_and_store_bytes_or_string_in_storage_slot(
    const StorageSlot &slot, const std::vector<uint8_t> &bytes);

template <typename T> T read_storage_value(const StorageSlot &slot);

template <> __uint128_t read_storage_value(const StorageSlot &slot) {
  const auto &bytes = hostio::read_storage(slot.get_slot());
  return uint256(bytes).to_uint128();
}

template <> uint256 read_storage_value(const StorageSlot &slot) {
  const auto &bytes = hostio::read_storage(slot.get_slot());
  return uint256(bytes);
}

template <> std::string read_storage_value(const StorageSlot &slot) {
  const auto &bytes = decode_bytes_or_string_from_slot(slot);
  return std::string(bytes.begin(), bytes.end());
}

template <> std::vector<uint8_t> read_storage_value(const StorageSlot &slot) {
  const auto &bytes = decode_bytes_or_string_from_slot(slot);
  return bytes;
}

template <> dtvm::Bytes read_storage_value(const StorageSlot &slot) {
  const auto &bytes = decode_bytes_or_string_from_slot(slot);
  return dtvm::Bytes(bytes);
}

template <> Address read_storage_value(const StorageSlot &slot) {
  const auto &bytes = hostio::read_storage(slot.get_slot());
  return Address(bytes);
}

template <> bool read_storage_value(const StorageSlot &slot) {
  const auto &bytes = hostio::read_storage(slot.get_slot());
  return bytes[slot.get_offset()] != 0;
}

// Other types of read_storage_value, write_storage_value need to consider the
// case where StorageSlot.offset is non-zero

template <typename T, size_t int_bytes_count>
T read_storage_int_value(const StorageSlot &slot) {
  const auto &bytes = hostio::read_storage(slot.get_slot());
  // read big endian
  T value = 0;
  for (size_t i = 0; i < int_bytes_count; ++i) {
    value |= static_cast<T>(bytes[slot.get_offset() + i])
             << (8 * (int_bytes_count - i - 1));
  }
  return value;
}

#define DECLARE_INT_READ_STORAGE_VALUE_FUNC(IntType, int_bytes_count)          \
  template <> IntType read_storage_value(const StorageSlot &slot) {            \
    return read_storage_int_value<IntType, int_bytes_count>(slot);             \
  }

DECLARE_INT_READ_STORAGE_VALUE_FUNC(uint8_t, 1)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(int8_t, 1)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(uint16_t, 2)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(int16_t, 2)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(uint32_t, 4)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(int32_t, 4)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(uint64_t, 8)
DECLARE_INT_READ_STORAGE_VALUE_FUNC(int64_t, 8)

template <typename T>
void write_storage_value(const StorageSlot &slot, const T &value);

template <>
void write_storage_value(const StorageSlot &slot, const __uint128_t &value) {
  hostio::write_storage(slot.get_slot(), uint256(value).bytes());
}

template <>
void write_storage_value(const StorageSlot &slot, const uint256 &value) {
  hostio::write_storage(slot.get_slot(), value.bytes());
}

template <>
void write_storage_value(const StorageSlot &slot, const std::string &value) {
  std::vector<uint8_t> str_bytes(value.begin(), value.end());
  encode_and_store_bytes_or_string_in_storage_slot(slot, str_bytes);
}

template <>
void write_storage_value(const StorageSlot &slot, const dtvm::Bytes &value) {
  encode_and_store_bytes_or_string_in_storage_slot(slot, value.bytes());
}

template <>
void write_storage_value(const StorageSlot &slot,
                         const std::vector<uint8_t> &value) {
  encode_and_store_bytes_or_string_in_storage_slot(slot, value);
}

// For basic types that are not 32 bytes encoded, the entire slot 32 bytes must
// be read first, then updated, and written back. Directly overwriting 32 bytes
// is not allowed, even if offset=0, otherwise the values at offsets behind the
// same slot will be overwritten.

template <>
void write_storage_value(const StorageSlot &slot, const bool &value) {
  // read slot old value first
  bytes32 bytes = hostio::read_storage(slot.get_slot());
  bytes[slot.get_offset()] = value ? 1 : 0;
  hostio::write_storage(slot.get_slot(), bytes);
}

template <typename T, size_t int_bytes_count>
void write_storage_int_value(const StorageSlot &slot, const T &value) {
  // write big endian
  // read slot old value first
  bytes32 bs = hostio::read_storage(slot.get_slot());
  for (size_t i = 0; i < int_bytes_count; ++i) {
    bs[slot.get_offset() + i] =
        static_cast<uint8_t>(value >> (8 * (int_bytes_count - i - 1)));
  }
  hostio::write_storage(slot.get_slot(), bs);
}

#define DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(IntType, int_bytes_count)         \
  template <>                                                                  \
  void write_storage_value(const StorageSlot &slot, const IntType &value) {    \
    write_storage_int_value<IntType, int_bytes_count>(slot, value);            \
  }

DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(uint8_t, 1)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(int8_t, 1)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(uint16_t, 2)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(int16_t, 2)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(uint32_t, 4)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(int32_t, 4)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(uint64_t, 8)
DECLARE_INT_WRITE_STORAGE_VALUE_FUNC(int64_t, 8)

template <typename V> class StorageValue {
public:
  inline StorageValue(const StorageSlot &slot) { slot_ = slot; }

  inline V get() { return read_storage_value<V>(slot_); }

  inline void set(const V &value) { write_storage_value<V>(slot_, value); }

private:
  StorageSlot slot_;
};

// Various key types of storage map, get the corresponding slot for each key
// The value corresponding to a mapping key k is located at keccak256(h(k) . p)
// where . is concatenation and h is a function that is applied to the key
// depending on its type: for value types, h pads the value to 32 bytes in the
// same way as when storing the value in memory. for strings and byte arrays,
// h(k) is just the unpadded data.
template <typename K>
bytes32 to_map_key_slot(const StorageSlot &map_slot, const K &key);

template <>
bytes32 to_map_key_slot(const StorageSlot &map_slot, const Address &key) {
  bytes32 bs = key.to_bytes32();
  std::vector<uint8_t> key_merger;
  key_merger.insert(key_merger.end(), bs.begin(), bs.end());
  // concat map slot
  const auto &map_slot_bytes = map_slot.get_slot().bytes();
  key_merger.insert(key_merger.end(), map_slot_bytes.begin(),
                    map_slot_bytes.end());
  return hostio::keccak256(key_merger);
}

template <>
bytes32 to_map_key_slot(const StorageSlot &map_slot, const std::string &key) {
  // string key to unpadded data
  std::vector<uint8_t> unpadded_key = unpadded_string(key);
  std::vector<uint8_t> key_merger = unpadded_key;
  // concat map slot
  const auto &map_slot_bytes = map_slot.get_slot().bytes();
  key_merger.insert(key_merger.end(), map_slot_bytes.begin(),
                    map_slot_bytes.end());
  return hostio::keccak256(key_merger);
}

// TODO: more base types and bytes and string key of to_map_key_slot

template <typename V> class StorageArray {
public:
  inline StorageArray(const StorageSlot &slot) {
    slot_ = slot;
    const auto &body_slot_begin_bytes =
        hostio::read_storage(slot_.get_slot().bytes());
    ;
    body_slot_begin_ = uint256(body_slot_begin_bytes);
  }
  inline V get(size_t index) {
    const auto &value_slot = StorageSlot(body_slot_begin_ + uint256(index));
    return read_storage_value<V>(value_slot);
  }

  void set(size_t index, const V &value) {
    size_t old_size = size();
    const auto &value_slot = StorageSlot(body_slot_begin_ + uint256(index));
    write_storage_value<V>(value_slot, value);
    if (old_size == index) {
      // size add 1
      write_storage_value<uint256>(slot_, uint256(old_size + 1));
    }
  }

  void push(const V &value) {
    size_t old_size = size();
    auto index = old_size;
    const auto &value_slot = StorageSlot(body_slot_begin_ + uint256(index), 0);
    // size add 1
    write_storage_value<uint256>(slot_, uint256(old_size + 1));
  }

  bool pop() {
    size_t old_size = size();
    if (old_size == 0) {
      return false;
    }
    // not erase the old element(maybe nested array/mapping or complex data)
    // size minus 1
    write_storage_value<uint256>(slot_, uint256(old_size - 1));
    return true;
  }

  inline size_t size() {
    uint256 size_value = read_storage_value<uint256>(slot_);
    return size_value.to_uint64();
  }

private:
  StorageSlot slot_;
  // array content slot starts from keccak256(slot_)
  uint256 body_slot_begin_;
};

// TODO: Currently, map and array do not support basic types that do not occupy
// 32 bytes
template <typename K, typename V> class StorageMap {
public:
  StorageMap(const StorageSlot &slot) { slot_ = slot; }

  V get(const K &key) const {
    const auto &key_slot = get_slot_of_key(key);
    V value = read_storage_value<V>(key_slot);
    return value;
  }

  template <typename K2, typename V2>
  StorageMap<K2, V2> get_nested_map(const K &key) {
    const auto &key_slot = get_slot_of_key(key);
    return StorageMap<K2, V2>(key_slot);
  }

  template <typename V2> StorageArray<V2> get_nested_array(const K &key) {
    const auto &key_slot = get_slot_of_key(key);
    return StorageArray<V2>(key_slot);
  }

  // Reading from a nested mapping requires explicitly passing the template
  // parameters K2 and V2
  template <typename K2, typename V2> V2 get(const K &key, const K2 &key2) {
    const StorageMap<K2, V2> &nested = get_nested_map<K2, V2>(key);
    return nested.get(key2);
  }

  void set(const K &key, const V &value) {
    const auto &key_slot = get_slot_of_key(key);
    write_storage_value(key_slot, value);
  }

  // Writing to a nested mapping requires explicitly passing the template
  // parameters K2 and V2
  template <typename K2, typename V2>
  void set(const K &key, const K2 &key2, const V2 &value) {
    StorageMap<K2, V2> nested = get_nested_map<K2, V2>(key);
    nested.set(key2, value);
  }

private:
  StorageSlot get_slot_of_key(const K &key) const {
    bytes32 key_slot_bytes = to_map_key_slot(slot_, key);
    return StorageSlot(uint256(key_slot_bytes), 0);
  }

private:
  StorageSlot slot_;
};

} // namespace dtvm
