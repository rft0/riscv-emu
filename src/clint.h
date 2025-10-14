#ifndef __CLINT_H__
#define __CLINT_H__

#include "mem.h"

#define CLINT_BASE                      0x02000000
#define CLINT_SIZE                      0x10000

int clint_read(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* out, size_t size);
int clint_write(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* val, size_t size);

#endif