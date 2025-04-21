// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "hostio.hpp"
#include "storage_slot.hpp"
#include <cstring>

namespace dtvm {

std::vector<uint8_t> decode_bytes_or_string_from_slot(const StorageSlot &slot) {
  const auto &cur_slot_bytes = hostio::read_storage(slot.get_slot());
  // Find the right non-zero byte in cur_slot_bytes (if any). If it's odd, the
  // length of the bytes to decode
  bool longer_than_31_bytes = false;
  uint32_t length = 0;
  for (int i = 31; i >= 0; --i) {
    uint8_t c = cur_slot_bytes[i];
    if (c == 0) {
      continue;
    }
    if (c % 2 != 0) { // Odd number, indicates bytes longer than 31 bytes
      longer_than_31_bytes = true;
      length = (uint256(cur_slot_bytes).to_uint32() - 1) / 2;
      if (length > 2048) {
        throw std::runtime_error(
            std::string("too large bytes length when decoding: ") +
            std::to_string(length));
      }
      break;
    } else {
      // If the length is less than or equal to 31 bytes, the last byte
      // indicates the length multiplied by 2
      length = uint32_t(c) / 2;
      break;
    }
  }

  if (!longer_than_31_bytes) {
    // <= 31 bytes
    if (length > 31) {
      throw std::runtime_error("invalid bytes length");
    }
    std::vector<uint8_t> ret(length);
    memcpy(ret.data(), cur_slot_bytes.data(), length);
    return ret;
  } else {
    // > 31 bytes
    std::vector<uint8_t> ret(length);
    const auto &content_begin_slot =
        uint256(hostio::keccak256(slot.to_bytes32()));
    for (size_t i = 0; i < length / 32; i++) {
      const auto &item_slot = content_begin_slot + uint256(i);
      const auto &item_bytes = hostio::read_storage(item_slot);
      memcpy(ret.data() + i * 32, item_bytes.data(), 32);
    }
    auto remaining_length = length % 32;
    if (remaining_length != 0) {
      const auto &item_slot = content_begin_slot + uint256(length / 32);
      const auto &item_bytes = hostio::read_storage(item_slot);
      memcpy(ret.data() + length - remaining_length, item_bytes.data(),
             remaining_length);
    }
    return ret;
  }
}

void encode_and_store_bytes_or_string_in_storage_slot(
    const StorageSlot &slot, const std::vector<uint8_t> &bytes) {
  auto length = bytes.size();
  if (length <= 31) {
    bytes32 encoded;
    memcpy(encoded.data(), bytes.data(), length);
    encoded[length] = uint8_t(length * 2);
    if (length < 31) {
      memset(encoded.data() + length + 1, 0x0, 31 - length);
    }
    hostio::write_storage(slot.get_slot(), encoded);
    return;
  }
  // length > 31
  bytes32 length_bytes = uint256(length * 2 + 1).bytes();
  hostio::write_storage(slot.get_slot(), length_bytes);
  const auto &content_begin_slot =
      uint256(hostio::keccak256(slot.to_bytes32()));
  for (size_t i = 0; i < length / 32; i++) {
    const auto &item_slot = content_begin_slot + uint256(i);
    bytes32 item_bytes;
    memcpy(item_bytes.data(), bytes.data() + i * 32, 32);
    hostio::write_storage(item_slot, item_bytes);
  }
  if (length % 32 != 0) {
    const auto &item_slot = content_begin_slot + uint256(length / 32);
    bytes32 item_bytes;
    memset(item_bytes.data(), 0x0, 32);
    memcpy(item_bytes.data(), bytes.data() + length - length % 32, length % 32);
    hostio::write_storage(item_slot, item_bytes);
  }
}

} // namespace dtvm