#ifndef __EMU_H__
#define __EMU_H__

#include "def.h"

#include "cpu.h"

typedef struct {
    cpu_t cpu;

    uint8_t mem[0x10000];
} emu_t;

emu_t* emu_new();
void emu_run(emu_t* emulator);
void emu_free(emu_t* emulator);

void emu_verbose(emu_t* emulator);

#endif