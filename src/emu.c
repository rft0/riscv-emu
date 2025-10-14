#include "emu.h"

#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "cpu.h"

emu_t* emu_new() {
    emu_t* emu = (emu_t*)malloc(sizeof(emu_t));
    if (!emu)
        return NULL;

    memset(emu, 0, sizeof(emu_t));

    // Initialize global memory
    mem_init();

    // Initialize CPU
    emu->cpu.mode = PRIV_M;

    // Setup for SBI
    emu->cpu.pc = 0x80000000; // Entry point for the kernel
    emu->cpu.x[10] = 0; // a0 = hartid
    emu->cpu.x[11] = 0x40000000; // a1 = dtb ptr (just after the 1MB ROM)

    // Initialize needed CPU CSRs
    // ...

    return emu;
}

void emu_run(emu_t* emu) {
    while (!emu->cpu.halted) {
        cpu_step(&emu->cpu);
    }
}

void emu_free(emu_t* emulator) {
    mem_free();
    
    if (emulator) {
        free(emulator);
    }
}