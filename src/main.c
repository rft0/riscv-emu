#include <stdio.h>

#include "emu.h"

#include "helpers.h"

// NOTE TO ME: TESTS TO TAKE A LOOK
// TODO if -test flag passed find tohost while parsing elf header
/*
rv64ui-p-ld_st
rv64ui-p-ma_data
*/

int main() {
    emu_t* emulator = emu_new();
    if (!emulator) {
        fprintf(stderr, "Failed to create emulator\n");
        return 1;
    }

    load_elf(emulator, "tests/isa/rv64ui-p-ma_data", 0);

    emu_run();
    emu_free();

    return 0;
}