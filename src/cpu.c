#include "cpu.h"

#include "instr/table.h"
#include <stdio.h>

void cpu_step(cpu_t* cpu) {
    uint32_t instr = *((uint32_t*)(cpu->mem + cpu->pc));
    printf("PC: 0x%llx, Instruction: 0x%08x\n", cpu->pc, instr);

    cpu->npc = cpu->pc + 4;

    for (int i = 0; i < instr_table_size; i++) {
        if ((instr & instr_table[i].mask) == instr_table[i].match) {
            instr_table[i].func(cpu, instr);
            cpu->pc = cpu->npc;
            return;
        }
    }

    printf("Illegal instruction at PC=0x%llx: 0x%08x\n", cpu->pc - 4, instr);
}