#ifndef __MEM_H__
#define __MEM_H__

#include "def.h"
#include "cpu.h"

typedef struct {
    cpu_t* cpu;
    uint8_t* ram;
} mem_t;

mem_t* mem_new(cpu_t* cpu);
void mem_free(mem_t* mem);
uint8_t mem_read8(mem_t* mem, uint64_t addr);
void mem_write8(mem_t* mem, uint64_t addr, uint8_t value);

#endif