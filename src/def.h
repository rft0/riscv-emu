#ifndef __DEF_H__
#define __DEF_H__

#include <stdint.h>
#include <stddef.h>

static inline uint16_t extract16(uint32_t value, int h, int l) {
    return (value << (15 - h)) >> (15 - h + l);
}

static inline uint32_t extract32(uint32_t value, int h, int l) {
    return (value << (31 - h)) >> (31 - h + l);
}

static inline uint64_t extract64(uint64_t value, int h, int l) {
    return (value << (63 - h)) >> (63 - h + l);
}

static inline int64_t sext(int64_t value, int size) {
    return (value << (64 - size)) >> (64 - size);
}

static inline int64_t sextract16(int32_t value, int h, int l) {
    return sext(extract16(value, h, l), h - l + 1);
}

static inline int64_t sextract32(int32_t value, int h, int l) {
    return sext(extract32(value, h, l), h - l + 1);
}

static inline int64_t sextract64(int64_t value, int h, int l) {
    return sext(extract64(value, h, l), h - l + 1);
}

#endif