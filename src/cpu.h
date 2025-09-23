#ifndef __CPU_H__
#define __CPU_H__

#include "def.h"

typedef struct {
    int halted;

    uint64_t x[32];
    uint64_t f[32];

    uint64_t pc;
    uint64_t npc;
    
    uint64_t satp;

    uint8_t mem[0x10000];
} cpu_t;

void cpu_step(cpu_t* cpu);

#endif