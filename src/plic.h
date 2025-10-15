#ifndef __PLIC_H__
#define __PLIC_H__

#include "mem.h"

#define PLIC_BASE 0x0C000000
#define PLIC_SIZE 0x10000

int plic_read(uint64_t addr, void* out, size_t size);
int plic_write(uint64_t addr, void* pval, size_t size);

#endif