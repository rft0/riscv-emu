#ifndef __DEF_H__
#define __DEF_H__

#include <stdint.h>
#include <stddef.h>

#define GENMASK32(h, l) (((~0U) - (1U << (l)) + 1) & (~0U >> (31 - (h))))
#define GENMASK64(h, l) (((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (63 - (h))))

static inline uint32_t extract32(uint32_t value, int h, int l) {
    return (value >> l) & ((1U << (h - l + 1)) - 1);
}

static inline uint64_t extract64(uint64_t value, int h, int l) {
    return (value >> l) & ((1ULL << (h - l + 1)) - 1);
}

static inline int64_t sext(int64_t value, int size) {
    return (value << (64 - size)) >> (64 - size);
}

static inline int64_t sextract32(int32_t value, int h, int l) {
    return sext((value >> l) & ((1U << (h - l + 1)) - 1), h - l + 1);
}

static inline int64_t sextract64(int64_t value, int h, int l) {
    return sext((value >> l) & ((1ULL << (h - l + 1)) - 1), h - l + 1);
}

#endif