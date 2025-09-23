#include "emu.h"

#include <stdlib.h>
#include <string.h>

#include "cpu.h"

emu_t* emu_new() {
    emu_t* emulator = (emu_t*)malloc(sizeof(emu_t));
    if (!emulator)
        return NULL;

    memset(emulator, 0, sizeof(emu_t));

    return emulator;
}

void emu_run(emu_t* emu) {
    while (!emu->cpu.halted)
        cpu_step(&emu->cpu);
}

void emu_free(emu_t* emulator) {
    if (emulator) {
        free(emulator);
    }
}