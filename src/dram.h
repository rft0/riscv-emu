#ifndef __RAM_H__
#define __RAM_H__

#include "def.h"

#define DRAM_BASE 0x80000000
#define DRAM_SIZE (512 * 1024 * 1024)

int dram_init();
void dram_free();

int dram_read(uint64_t addr, void* out, size_t size);
int dram_write(uint64_t addr, void* pval, size_t size);

void dram_dump(uint64_t addr, size_t size);
void dram_dumpfile(const char* filename, uint64_t addr, size_t size);

#endif