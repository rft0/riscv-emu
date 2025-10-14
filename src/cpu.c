#include "cpu.h"

#include "table.h"
#include "trap.h"
#include "mem.h"

#include <stdio.h>
void cpu_step(cpu_t* cpu) {
    uint32_t instr;
    if (!va_fetch(cpu, cpu->pc, &instr, 4))
        return;
    printf("%X\n", instr);
    if ((instr & 0x3) != 0x3) {
        instr &= 0xFFFF;
        cpu->npc = cpu->pc + 2;
    } else {
        cpu->npc = cpu->pc + 4;
    }

    for (int i = 0; i < instr_table_size; i++) {
        if ((instr & instr_table[i].mask) == instr_table[i].match) {
            printf("%d\n", __LINE__);
            instr_table[i].func(cpu, instr);
            cpu->pc = cpu->npc;
            return;
        }
    }

    raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
}