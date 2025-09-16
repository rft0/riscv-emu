#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "def.h"

uint8_t* read_file_binary(const char* path, size_t* out_size);

uint64_t umul128(uint64_t a, uint64_t b, uint64_t* hi_result);
int64_t mul128(int64_t a, int64_t b, int64_t* hi_result);
uint64_t mulsu128(int64_t a, uint64_t b, int64_t* hi_result);

#endif