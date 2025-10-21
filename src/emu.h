#ifndef __EMU_H__
#define __EMU_H__

#include "def.h"

#include "cpu.h"

#define KERNEL_LOAD_ADDR        0x80200000
#define SBI_LOAD_ADDR           0x80000000
#define DTB_LOAD_ADDR           0x88000000

typedef struct {
    cpu_t cpu;
    int running;

    uint8_t mem[0x10000];
} emu_t;

emu_t* emu_new();
void emu_run();

void emu_stop();
void emu_resume();
void emu_free();

void emu_dump();

#endif