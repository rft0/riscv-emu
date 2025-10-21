#include "cpu.h"

#include "table.h"
#include "trap.h"
#include "mem.h"

#include <stdio.h>
void cpu_step(cpu_t* cpu) {
    // printf("Fetch addr: %X", cpu->pc);

    // uint32_t fetch_size = (cpu->pc ) != 0x3? 2 : 4;
    // // uint32_t fetch_size = 4;
    // printf("%X, %d\n",  cpu->pc, fetch_size);
    // uint32_t instr = 0;
    // if (!va_fetch(cpu, cpu->pc, &instr, fetch_size))
    //     return;

    // printf("%X: %X\n", cpu->pc, instr);

    // if ((instr & 0x3) != 0x3) {
    //     instr &= 0xFFFF; // Set upper half to zero
    //     cpu->npc = cpu->pc + 2;
    // } else {
    //     cpu->npc = cpu->pc + 4;
    // }

    uint32_t instr = 0;
    if (!va_fetch(cpu, cpu->pc, &instr, 4))
        return;

    printf("%X: 0x%X\n", cpu->pc, instr);

    if ((instr & 0x3) != 0x3) {
        instr &= 0xFFFF;
        cpu->npc = cpu->pc + 2;
    } else {
        cpu->npc = cpu->pc + 4;
    }

    for (int i = 0; i < instr_table_size; i++) {
        if ((instr & instr_table[i].mask) == instr_table[i].match) {
            instr_table[i].func(cpu, instr);
            cpu->pc = cpu->npc;
            return;
        }
    }

    printf("Illegal instruction at 0x%lX: 0x%X\n", cpu->pc, instr);

    raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
}

void cpu_dump(cpu_t* cpu) {
    printf("CPU State:\n");
    printf("PC: 0x%lx\n", cpu->pc);
    printf("NPC: 0x%lx\n", cpu->npc);
    printf("Registers:\n");
    for (int i = 0; i < 32; i++) {
        printf("x[%d]: 0x%lx\n", i, cpu->x[i]);
    }
}