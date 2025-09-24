#ifndef __CPU_H__
#define __CPU_H__

#include "def.h"

enum {
    PRIV_U = 0,
    PRIV_S = 1,
    PRIV_H = 2,
    PRIV_M = 3
};

// typedef struct {
//     uint64_t value;
//     uint8_t priv;       // U=0, S=1, H=2, M=3
//     uint8_t ro;
// } csr_entry_t;

typedef struct {
    int halted;

    uint64_t x[32];
    uint64_t f[32];

    uint64_t pc;
    uint64_t npc;
    
    uint64_t csr[4096];
    uint8_t mem[0x10000];
} cpu_t;

void cpu_step(cpu_t* cpu);

#endif