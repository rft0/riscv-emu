#ifndef __REG_H__
#define __REG_H__

// RV64GC
// I, M, A, F, D, C

#include <stdint.h>

typedef struct {
    uint64_t satp;
    uint64_t page_table[512];
} mmu_t;

typedef struct {
    uint64_t mstatus;
    uint64_t mepc;
    uint64_t mtvec;
    uint64_t mcause;
    uint64_t mtval;
    enum { U = 0, S = 1, M = 3 } priv;
} csr_t;

typedef struct {
    uint64_t x[32];
    uint64_t f[32];
    csr_t csr;

    uint64_t pc;
    mmu_t* mmu;
} cpu_t;

#endif