#include "cpu.h"

#include "table.h"
#include "trap.h"
#include "mem.h"

#include <stdlib.h>
#include <stdio.h>

void cpu_step(cpu_t* cpu) {
    cpu->trapped = 0;

    // printf("fetching: 0x%lX\n", cpu->pc);

    // Break point check here
    uint64_t trigger_enabled = (cpu->csr.tcontrol >> 3) & 1;

    if (trigger_enabled) {
        uint64_t tdata1 = cpu->csr.tdata1;
        uint64_t type = (tdata1 >> 60) & 0xF;

        if (type == 2) {
            uint64_t action  = (tdata1 >> 7) & 0x7;     // action = 0 (exception)
            uint64_t execute = (tdata1 >> 6) & 1;       // execute = 1
            uint64_t m_mode  = (tdata1 >> 3) & 1;       // m_mode = 1

            // Check if it's an M-mode, execute breakpoint, with action=0
            if (cpu->mode == PRIV_M && m_mode && execute && action == 0) {
                if (cpu->pc == cpu->csr.tdata2) {
                    raise_trap(cpu, CAUSE_BREAKPOINT, cpu->pc, 0); 
                    return; 
                }
            }
        }
    }

    uint32_t instr = 0;
    if ((cpu->pc & 0xFFF) <= 0xFFC) { // Page boundary check
        if (!va_fetch(cpu, cpu->pc, &instr, 4))
            return;
    
        if ((instr & 0x3) != 0x3) {
            instr &= 0xFFFF;
            cpu->npc = cpu->pc + 2;
        } else {
            cpu->npc = cpu->pc + 4;
        }
    } else {
        uint16_t instr_low = 0;
        uint16_t instr_high = 0;
    
        if (!va_fetch(cpu, cpu->pc, &instr_low, 2))
            return;
    
        if ((instr_low & 0x3) != 0x3) {
            instr = instr_low;
            cpu->npc = cpu->pc + 2;
    
        } else {
            if (!va_fetch(cpu, cpu->pc + 2, &instr_high, 2))
                return;
    
            instr = (uint32_t)instr_high << 16 | instr_low;
            cpu->npc = cpu->pc + 4;
        }
    }

    printf("%lX: 0x%X\n", cpu->pc, instr);

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
    uint64_t mstatus = cpu->csr.mstatus;
    uint64_t pending_and_enabled = cpu->csr.mip & cpu->csr.mie;

    if (!pending_and_enabled)
        return 0;

    uint64_t s_interrupts = pending_and_enabled & cpu->csr.mideleg;
        uint64_t m_interrupts = pending_and_enabled & ~cpu->csr.mideleg;

    uint64_t mie = (mstatus >> 3) & 1; // MIE
    uint64_t sie = (mstatus >> 1) & 1; // SIE

    uint64_t can_take = 0;

    if (cpu->mode == PRIV_M) {
        if (mie == 1)
            can_take = m_interrupts | s_interrupts;
    } else {
        if (mie == 1)
            can_take |= m_interrupts;
        
        if (cpu->mode == PRIV_S && sie == 1)
            can_take |= s_interrupts;
    }

    if (!can_take)
        return 0;

    uint64_t cause = 0;
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
        raise_trap(cpu, cause | (1ULL << 63), 0, 1);
        cpu->pc = cpu->npc;
        return 1;
    }

    return 0;
}