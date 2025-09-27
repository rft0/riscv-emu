#include "cpu.h"

// #include <stdio.h>

#include "instr/table.h"
#include "instr/trap.h"
#include "instr/mem.h"

void cpu_step(cpu_t* cpu) {
    uint32_t instr;
    if (!va_fetch(cpu, cpu->pc, &instr, 4))
        return;

    // printf("PC: 0x%lx, Instruction: 0x%08x\n", cpu->pc, instr);
    cpu->npc = cpu->pc + 4;

    for (int i = 0; i < instr_table_size; i++) {
        if ((instr & instr_table[i].mask) == instr_table[i].match) {
            instr_table[i].func(cpu, instr);
            cpu->pc = cpu->npc;
            return;
        }
    }

    raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
}