#include "hostapi.h"

// Standard Fibonacci implementation
unsigned int fibonacci(unsigned int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;

    unsigned int a = 0;
    unsigned int b = 1;
    unsigned int result;

    for (unsigned int i = 2; i <= n; i++) {
        result = a + b;
        a = b;
        b = result;
    }

    return result;
}

// Tail-optimized Fibonacci implementation
unsigned int fibonacciTailOptimized(unsigned int n) {
    if (n == 0) return 0;
    
    unsigned int a = 0;
    unsigned int b = 1;

    while (n > 1) {
        unsigned int temp = b;
        b = a + b;
        a = temp;
        n--;
    }

    return b;
}

static uint8_t selector[4];

static void uint32_to_big_endian_bytes(uint32_t value, uint8_t* bytes) {
  // clear first 28bytes by 3*i64 +i32 pointer
  int64_t *p = (int64_t *)bytes;
  p[0] = 0;
  p[1] = 0;
  p[2] = 0;
  int32_t *p2 = (int32_t *) (bytes + 24);
  p2[0] = 0;
  // write last 4 bytes
  bytes[28+0] = (value >> 24) & 0xFF;
  bytes[28+1] = (value >> 16) & 0xFF;
  bytes[28+2] = (value >> 8) & 0xFF;
  bytes[28+3] = value & 0xFF;
}

static uint8_t n_bytes32[32] = {0};
static uint8_t tmp_result[32] = {0};

void call() {
    // Read 4 bytes selector from calldata
    callDataCopy((int32_t) &selector, 0, 4);
    if (selector[0] == 0x61) {
        // fibonacci(uint256) == 0x61047ff4
        callDataCopy((int32_t) &n_bytes32, 4, 32);
        uint32_t n = (uint32_t) n_bytes32[31];
        uint32_t result = fibonacci(n);
        uint32_to_big_endian_bytes(result, tmp_result);
        finish((int32_t) &tmp_result, 32);
    } else if (selector[0] == 0xac) {
        // fibonacciTailOptimized(uint256) == 0xac8da0ab
        callDataCopy((int32_t) &n_bytes32, 4, 32);
        uint32_t n = (uint32_t) n_bytes32[31];
        uint32_t result = fibonacciTailOptimized(n);
        uint32_to_big_endian_bytes(result, tmp_result);
        finish((int32_t) &tmp_result, 32);
    }
}

void deploy() {
}
