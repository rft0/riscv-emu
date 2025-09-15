#ifndef __TABLE_H__
#define __TABLE_H__

#include "../cpu.h"

typedef struct instr_dispatch_table {
    uint32_t match;
    uint32_t mask;
    void (*func)(cpu_t* cpu, uint32_t instr);
} instr_dispatch_table_t;

extern instr_dispatch_table_t instr_dispatch_table[];

#endif