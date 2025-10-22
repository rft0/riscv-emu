#include <stdio.h>

#include "emu.h"

#include "helpers.h"
#include "mem.h"
#include "cli.h"

// NOTE TO ME: TESTS TO TAKE A LOOK
// TODO if -test flag passed find tohost while parsing elf header
/*
rv64ui-p-ld_st
rv64ui-p-ma_data
*/

cli_args_t g_cli_args;

int main(int argc, char** argv) {
    cli_parse_args(argc, argv, &g_cli_args);
    if (g_cli_args.help) {
        cli_help();
        return 0;
    }

    emu_t* emulator = emu_new();
    if (!emulator) {
        printf("Failed to create emulator\n");
        return 1;
    }

    if (g_cli_args.test_path) {
        if (!load_elf(emulator, g_cli_args.test_path, 1)) {
            fprintf(stderr, "Failed to load ELF file: %s\n", g_cli_args.test_path);
            emu_free();
            return 1;
        }

        emu_run();
        emu_free();
        return 0;
    }

    if (g_cli_args.sbi_path) {
        if (!load_elfex(emulator, g_cli_args.sbi_path, 0, SBI_LOAD_ADDR)) {
            fprintf(stderr, "Failed to load SBI file: %s\n", g_cli_args.sbi_path);
            emu_free();
            return 1;
        }

        if (g_cli_args.kernel_path) {
            if (!load_elf(emulator, g_cli_args.kernel_path, 0)) {
                fprintf(stderr, "Failed to load kernel file: %s\n", g_cli_args.kernel_path);
                emu_free();
                return 1;
            }
        } else {
            printf("Warning: No kernel file specified\n");
        }

        if (g_cli_args.dtb_path) {
            size_t dtb_size;
            uint8_t* dtb = load_binary(g_cli_args.dtb_path, &dtb_size);
            if (!dtb) {
                fprintf(stderr, "Failed to load DTB file: %s\n", g_cli_args.dtb_path);
                emu_free();
                return 1;
            }

            phys_write(&emulator->cpu, DTB_LOAD_ADDR, dtb, dtb_size);
        } else {
            printf("Warning: No DTB file specified\n");
        }

        // Todo: We can add dtb etc too here
    }

    emu_run();
    emu_free();

    return 0;
}