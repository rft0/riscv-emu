#include "emu.h"

#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "mem.h"
#include "cpu.h"

emu_t* g_emu;

emu_t* emu_new() {
    emu_t* emu = (emu_t*)malloc(sizeof(emu_t));
    if (!emu)
        return NULL;

    memset(emu, 0, sizeof(emu_t));

    // Initialize global memory
    mem_init();

    // Initialize CPU
    emu->cpu.mode = PRIV_M;

    // Load for SBI
    load_elf(emu, "/path/to/sbi", 0);

    // Load Linux Kernel
    load_elf(emu, "/path/to/kernel", 0);

    // Load DTB
    size_t dtb_size;
    uint8_t* dtb = load_binary("/path/to/dtb", &dtb_size);
    phys_write(&emu->cpu, DTB_LOAD_ADDR, dtb, dtb_size);

    emu->cpu.pc = SBI_LOAD_ADDR;            // Entry point for the kernel
    emu->cpu.x[10] = 0;                     // a0 = hartid
    emu->cpu.x[11] = DTB_LOAD_ADDR;         // a1 = dtb ptr (just after the 1MB ROM)

    // Initialize needed CSRs per CPU
    emu->cpu.csr.mstatus = (PRIV_M << 11) | (0b01 << 13);
    emu->cpu.csr.misa = (2ULL << 62) | (1ULL << ('I'-'A')) | (1ULL << ('M'-'A')) | (1ULL << ('A'-'A')) | (1ULL << ('F'-'A')) | (1ULL << ('D'-'A')) | (1ULL << ('C'-'A'));
    emu->cpu.csr.mhartid = 0;

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