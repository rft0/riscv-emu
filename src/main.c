#include <stdio.h>
#include <string.h>

#include "emu.h"
#include "helpers.h"

int main() {
    emu_t* emulator = emu_new();
    if (!emulator) {
        fprintf(stderr, "Failed to create emulator\n");
        return 1;
    }

    size_t program_size = 0;
    uint8_t* program = load_binary("tests/isa/rv32mi-p-scall", &program_size);
    memcpy(emulator->cpu.mem, program, program_size);

    emu_run(emulator);

    emu_free(emulator);

    return 0;
}