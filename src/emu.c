#include "emu.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "csr.h"
#include "mem.h"
#include "cpu.h"
#include "clint.h"

emu_t* g_emu;

emu_t* emu_new() {
    emu_t* emu = (emu_t*)malloc(sizeof(emu_t));
    if (!emu)
        return NULL;

    g_emu = emu;
    memset(emu, 0, sizeof(emu_t));

    emu->running = 1;

    // Initialize global memory
    mem_init();

    // Initialize CPU
    emu->cpu.mode = PRIV_M;

    emu->cpu.pc = SBI_LOAD_ADDR;            // Entry point for sbi
    emu->cpu.x[10] = 0;                     // a0 = hartid
    emu->cpu.x[11] = DTB_LOAD_ADDR;         // a1 = physical address of the FDT

    hardwire_mstatus(&emu->cpu);
    emu->cpu.csr.mhartid = 0;
    emu->cpu.csr.mstatus |= (3ULL << 11); // M Mode
    emu->cpu.csr.misa = (2ULL << 62) | (1ULL << ('I'-'A')) | (1ULL << ('M'-'A')) | (1ULL << ('A'-'A')) | (1ULL << ('F'-'A')) | (1ULL << ('D'-'A')) | (1ULL << ('C'-'A'));

    return emu;
}

// void emu_run() {
//     while (g_emu->running) {
//         clint_tick(1);
        
//         if (!g_emu->cpu.halted) {
//             cpu_step(&g_emu->cpu);
//             clint_update(&g_emu->cpu);
//             cpu_check_interrupts(&g_emu->cpu);
//         }
//     }
// }

void emu_run() {
    while (g_emu->running) {
        clint_tick(1);

        // Per hart
        clint_update(&g_emu->cpu);

        uint64_t pending_interrupts = g_emu->cpu.csr.mip & ((1 << 3) | (1 << 7) | (1 << 11));
        if (g_emu->cpu.halted && (g_emu->cpu.csr.mip & (pending_interrupts != 0)))
            g_emu->cpu.halted = 0;

        if (!g_emu->cpu.halted) {
            int tt = cpu_check_interrupts(&g_emu->cpu);
            if (!tt) // Trap taken this cycle
                cpu_step(&g_emu->cpu);
        }
    }
}

void emu_stop() {
    g_emu->running = 0;
}

void emu_resume() {
    g_emu->running = 1;
}

void emu_free() {
    mem_free();
    
    if (g_emu) {
        free(g_emu);
    }
}

void emu_dump() {
    cpu_dump(&g_emu->cpu);
}