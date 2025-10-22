#include "emu.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mem.h"
#include "cpu.h"

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

    //! TODO: take those sbi, kernel and dtb as user arguments
    // Load SBI
    // load_elf_ex(emu, "/usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.elf", 0, SBI_LOAD_ADDR);
    // size_t sbi_size;
    // uint8_t* sbi = load_binary("bin/fw_jump.bin", &sbi_size);
    // phys_write(&emu->cpu, SBI_LOAD_ADDR, sbi, sbi_size);
    // free(sbi);

    // Load Linux Kernel
    //! TODO: Load image as flat binary instead
    // load_elf(emu, "bin/kernel", 0);

    // Load DTB
    // size_t dtb_size;
    // uint8_t* dtb = load_binary("bin/virt.dtb", &dtb_size);
    // phys_write(&emu->cpu, DTB_LOAD_ADDR, dtb, dtb_size);
    // free(dtb);

    emu->cpu.pc = SBI_LOAD_ADDR;            // Entry point for the kernel
    emu->cpu.x[10] = 0;                     // a0 = hartid
    emu->cpu.x[11] = DTB_LOAD_ADDR;         // a1 = payload entry

    // Initialize needed CSRs per CPU
    emu->cpu.csr.mstatus = (PRIV_M << 11) | (0b01 << 13);
    emu->cpu.csr.misa = (2ULL << 62) | (1ULL << ('I'-'A')) | (1ULL << ('M'-'A')) | (1ULL << ('A'-'A')) | (1ULL << ('F'-'A')) | (1ULL << ('D'-'A')) | (1ULL << ('C'-'A'));
    emu->cpu.csr.mhartid = 0;
    // emu->cpu.csr.mtvec = 0x80000020;

    return emu;
}

void emu_run() {
    while (g_emu->running) {
        if (!g_emu->cpu.halted)
            cpu_step(&g_emu->cpu);
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