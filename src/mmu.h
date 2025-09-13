#ifndef __MMU_H__
#define __MMU_H__

#include "def.h"
#include "cpu.h"

typedef struct {
    cpu_t* cpu;
    uint8_t* ram;
} mmu_t;

uint64_t mmu_translate(cpu_t* cpu, uint64_t va, int write);


#endif