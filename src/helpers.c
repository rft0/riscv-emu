#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t* load_binary(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    uint8_t* buffer = (uint8_t*)malloc(size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);

    if (out_size)
        *out_size = size;

    return buffer;
}

uint64_t umul128(uint64_t a, uint64_t b, uint64_t* hi_result) {
    uint64_t a_lo = a & 0xFFFFFFFFULL;
    uint64_t a_hi = a >> 32;
    uint64_t b_lo = b & 0xFFFFFFFFULL;
    uint64_t b_hi = b >> 32;
    
    uint64_t p0 = a_lo * b_lo;  // LL
    uint64_t p1 = a_lo * b_hi;  // LH
    uint64_t p2 = a_hi * b_lo;  // HL
    uint64_t p3 = a_hi * b_hi;  // HH
    
    uint64_t mid = (p0 >> 32) + (p1 & 0xFFFFFFFFULL) + (p2 & 0xFFFFFFFFULL);
    uint64_t lo = (p0 & 0xFFFFFFFFULL) + (mid << 32);
    
    uint64_t carry = mid >> 32;
    uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + carry;
    
    *hi_result = hi;
    return lo;
}

int64_t mul128(int64_t a, int64_t b, int64_t* hi_result) {
    uint64_t hi;
    uint64_t lo = umul128((uint64_t)a, (uint64_t)b, &hi);
    
    if (a < 0) hi -= (uint64_t)b;
    if (b < 0) hi -= (uint64_t)a;
    
    *hi_result = (int64_t)hi;
    return (int64_t)lo;
}

uint64_t mulsu128(int64_t a, uint64_t b, int64_t* hi_result) {
    uint64_t hi;
    uint64_t lo = umul128((uint64_t)a, b, &hi);
    
    // Adjust only for signed a
    if (a < 0) hi -= b;
    
    *hi_result = (int64_t)hi;
    return lo;
}