// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "math.hpp"
#include <cstdint>
#include <string>

namespace dtvm {
class StorageSlot {
public:
  inline StorageSlot() {
    slot_ = uint256::max();
    offset_ = 0;
  }
  inline StorageSlot(int slot, uint32_t offset) {
    slot_ = uint256(slot);
    offset_ = offset;
  }
  inline StorageSlot(uint256 slot, uint32_t offset) {
    slot_ = slot;
    offset_ = offset;
  }
  inline bool is_valid() { return slot_ != uint256::max(); }
  inline uint256 get_slot() const { return slot_; }
  inline uint32_t get_offset() const { return offset_; }
  inline bytes32 to_bytes32() const { return slot_.bytes(); }

private:
  uint256 slot_;
  // Multiple state variables may exist in the same slot, offset_ indicates the
  // offset
  uint32_t offset_;
};
} // namespace dtvm