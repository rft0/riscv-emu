#ifndef __DEF_H__
#define __DEF_H__

#include <stdint.h>
#include <stddef.h>

#define GENMASK(h, l) (((~0U) - (1U << (l)) + 1) & (~0U >> (31 - (h))))
#define GENMASK64(h, l) (((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (63 - (h))))

static inline uint32_t extract32(uint32_t value, int h, int l) {
    return (value << (31 - h)) >> (31 - h + l);
}

static inline uint64_t extract64(uint64_t value, int h, int l) {
    return (value << (63 - h)) >> (63 - h + l);
}

static inline int64_t sext(int64_t value, int size) {
    return (value << (64 - size)) >> (64 - size);
}

static inline int64_t sextract32(int32_t value, int h, int l) {
    return sext(extract32(value, h, l), h - l + 1);
}

static inline int64_t sextract64(int64_t value, int h, int l) {
    return sext(extract64(value, h, l), h - l + 1);
}

static inline uint64_t reg_read_multi(uint64_t regval, uint32_t offset, uint32_t size) {
    return (regval >> (offset * 8)) & ((1ULL << (size * 8)) - 1);
}

static inline void reg_write_multi(uint64_t *reg, uint32_t offset, uint32_t size, uint64_t val) {
    uint64_t mask = ((1ULL << (size * 8)) - 1) << (offset * 8);
    *reg = (*reg & ~mask) | ((val << (offset * 8)) & mask);
}

#endif