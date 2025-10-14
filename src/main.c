#include <stdio.h>

#include "helpers.h"
#include "emu.h"

int main() {
    emu_t* emulator = emu_new();
    if (!emulator) {
        fprintf(stderr, "Failed to create emulator\n");
        return 1;
    }

    load_elf(emulator, "tests/isa/rv32ua-p-amoadd_w");

    emu_run(emulator);
    emu_free(emulator);

    return 0;
}