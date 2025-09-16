#include "cpu.h"

#include "instr/table.h"
#include <stdio.h>

void cpu_step(cpu_t* cpu) {
    if (cpu->halted) return;

    uint32_t instr = *((uint32_t*)(cpu->mem[0] + cpu->pc));

    for (int i = 0; i < instr_table_size; i++) {
        if ((instr & instr_dispatch_table[i].mask) != instr_dispatch_table[i].match) {
            instr_dispatch_table[i].func(cpu, instr);
            cpu->pc += 4;
            return;
        }
    }

    // If we reach here, the instruction is illegal
    printf("Illegal instruction at PC=0x%lx: 0x%08x\n", cpu->pc - 4, instr);
    cpu->halted = 1;
}