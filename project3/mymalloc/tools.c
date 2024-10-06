#include <limits.h>
#include "tools.h"

// Lookup table for the De Bruijn sequence
static const int debruijn_log2[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5
};

int log2_power_of_2(size_t n) {
    if (n == 0) return -1;

    uint64_t v = (uint64_t)n;

    // Multiply by the De Bruijn constant
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;

    // Look up the position in the De Bruijn table
    return debruijn_log2[((uint64_t)((v - (v >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

size_t round_up_to_power_of_2(size_t v) {
    v--;
    for (size_t i = 1; i < sizeof(size_t) * CHAR_BIT; i *= 2) {
        v |= v >> i;
    }
    v++;

    return v;
}