#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "emu.h"

uint8_t* load_binary(const char* path, size_t* out_size);
int load_elf(emu_t* emu, const char* path);

uint64_t mulu128(uint64_t a, uint64_t b, uint64_t* hi_result);
int64_t mul128(int64_t a, int64_t b, int64_t* hi_result);
uint64_t mulsu128(int64_t a, uint64_t b, int64_t* hi_result);

#endif