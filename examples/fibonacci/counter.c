#include "hostapi.h"

static uint8_t selector[4];

static void uint32_to_big_endian_bytes(uint32_t value, uint8_t* bytes) {
    int64_t *p = (int64_t *)bytes;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    int32_t *p2 = (int32_t *) (bytes + 24);
    p2[0] = 0;
    bytes[28+0] = (value >> 24) & 0xFF;
    bytes[28+1] = (value >> 16) & 0xFF;
    bytes[28+2] = (value >> 8) & 0xFF;
    bytes[28+3] = value & 0xFF;
}

static uint32_t uint32_from_big_endian_bytes(uint8_t* bytes) {
  uint32_t value = 0;
  value |= ((uint32_t)bytes[28+0] << 24);
  value |= ((uint32_t)bytes[28+1] << 16);
  value |= ((uint32_t)bytes[28+2] << 8);
  value |= ((uint32_t)bytes[28+3]);
  return value;
}

static uint8_t n_bytes32[32] = {0};
static uint8_t tmp_result[32] = {0};

static uint8_t counter_slot[32] = {0};

void call() {
    callDataCopy((int32_t) &selector, 0, 4);
    if (selector[0] == 0xe8) { // increase: 0xe8927fbc
        
        storageLoad((int32_t) &counter_slot, (int32_t) &n_bytes32);
        uint32_t n = uint32_from_big_endian_bytes(&n_bytes32);
        n++;
        uint32_to_big_endian_bytes(n, tmp_result);
        storageStore((int32_t) &counter_slot, (int32_t) &tmp_result);
        finish(0, 0);
    } else if (selector[0] == 0x06) { // count: 0x06661abd
        storageLoad((int32_t) &counter_slot, (int32_t) &n_bytes32);
        finish((int32_t) &n_bytes32, 32);
    } else if (selector[0] == 0xd7) { // decrease: 0xd732d955
        storageLoad((int32_t) &counter_slot, (int32_t) &n_bytes32);
        uint32_t n = uint32_from_big_endian_bytes(&n_bytes32);
        n--;
        uint32_to_big_endian_bytes(n, tmp_result);
        storageStore((int32_t) &counter_slot, (int32_t) &tmp_result);
        finish(0, 0);
    }
}

void deploy() {
}