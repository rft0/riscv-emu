#ifndef __CLINT_H__
#define __CLINT_H__

#include "mem.h"

#define CLINT_BASE                      0x02000000
#define CLINT_SIZE                      0x10000

int clint_read(uint64_t addr, void* out, size_t size);
int clint_write(uint64_t addr, void* pval, size_t size);

void clint_tick(uint64_t increment);
void clint_update(cpu_t* cpu);

#endif