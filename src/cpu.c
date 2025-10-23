#include "cpu.h"

#include "table.h"
#include "trap.h"
#include "mem.h"

#include <stdio.h>
void cpu_step(cpu_t* cpu) {
    uint32_t instr = 0;
    if (!va_fetch(cpu, cpu->pc, &instr, 4))
        return;

    // printf("%X: 0x%X\n", cpu->pc, instr);

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

int cpu_check_interrupts(cpu_t* cpu) {
    uint64_t cause = 0;
    uint64_t ie_bit;
    uint64_t deleg_mask;

    switch (cpu->mode) {
        case PRIV_M:
            ie_bit = (1UL << PRIV_M);
            deleg_mask = 0;
            break;
        case PRIV_S:
            ie_bit = (1UL << PRIV_S);
            deleg_mask = cpu->csr.mideleg;
            break;
        default:
            return 0;
    }

    if (cpu->mode != 0 && (cpu->csr.mstatus & ie_bit) == 0)
        return 0;

    uint64_t pending_and_enabled = cpu->csr.mip & cpu->csr.mie;

    uint64_t can_take;
    if (cpu->mode == PRIV_M)
        can_take = pending_and_enabled & ~cpu->csr.mideleg;
    else
        can_take = pending_and_enabled & cpu->csr.mideleg;

    if (!can_take)
        return 0;

    // Priority: MEI > MSI > MTI > SEI > SSI > STI > UEI > USI > UTI
    if ((can_take >> CAUSE_INT_MACHINE_EXT) & 1)                // MEI
        cause = CAUSE_INT_MACHINE_EXT;
    else if ((can_take >> CAUSE_INT_MACHINE_SOFT) & 1)          // MSI
        cause = CAUSE_INT_MACHINE_SOFT;
    else if ((can_take >> CAUSE_INT_MACHINE_TIMER) & 1)         // MTI
        cause = CAUSE_INT_MACHINE_TIMER;
    else if ((can_take >> CAUSE_INT_SUPERVISOR_EXT) & 1)        // SEI
        cause = CAUSE_INT_SUPERVISOR_EXT;
    else if ((can_take >> CAUSE_INT_SUPERVISOR_SOFT) & 1)       // SSI
        cause = CAUSE_INT_SUPERVISOR_SOFT;
    else if ((can_take >> CAUSE_INT_SUPERVISOR_TIMER) & 1)      // STI
        cause = CAUSE_INT_SUPERVISOR_TIMER;


    if (cause != 0) {
        raise_trap(cpu, cause | (1ULL < 63), 0, 1);
        cpu->pc = cpu->npc;
        return 1;
    }

    return 0;
}