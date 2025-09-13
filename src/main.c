#include <stdio.h>

#include "emu.h"

int main() {
    emu_t* emulator = emu_new();
    if (!emulator) {
        fprintf(stderr, "Failed to create emulator\n");
        return 1;
    }

    emu_run(emulator);

    emu_free(emulator);

    return 0;
}