#include "hostapi.h"

// Recursive Fibonacci implementation
unsigned int fibonacci(unsigned int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Tail-recursive Fibonacci helper function
unsigned int fibonacciTailRecursive(unsigned int n, unsigned int a, unsigned int b) {
    if (n == 0) return a;
    if (n == 1) return b;
    return fibonacciTailRecursive(n - 1, b, a + b);
}

// Tail-optimized Fibonacci implementation using recursion
unsigned int fibonacciTailOptimized(unsigned int n) {
    return fibonacciTailRecursive(n, 0, 1);
}

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

static uint8_t n_bytes32[32] = {0};
static uint8_t tmp_result[32] = {0};

void call() {
    callDataCopy((int32_t) &selector, 0, 4);
    if (selector[0] == 0x61) {
        callDataCopy((int32_t) &n_bytes32, 4, 32);
        uint32_t n = (uint32_t) n_bytes32[31];
        uint32_t result = fibonacci(n);
        uint32_to_big_endian_bytes(result, tmp_result);
        finish((int32_t) &tmp_result, 32);
    } else if (selector[0] == 0xac) {
        callDataCopy((int32_t) &n_bytes32, 4, 32);
        uint32_t n = (uint32_t) n_bytes32[31];
        uint32_t result = fibonacciTailOptimized(n);
        uint32_to_big_endian_bytes(result, tmp_result);
        finish((int32_t) &tmp_result, 32);
    }
}

void deploy() {
}