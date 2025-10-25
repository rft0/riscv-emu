#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cli_parse_args(int argc, char** argv, cli_args_t* out_args) {
    out_args->test_path = NULL;
    out_args->sbi_path = NULL;
    out_args->kernel_path = NULL;
    out_args->help = 0;

    if (argc <= 1) {
        out_args->help = 1;
        return;
    }

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
            out_args->help = 1;
        }
#ifdef EMU_TESTS_ENABLED
        else if (strcmp(argv[i], "--test") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --test requires a <path> argument\n");
                exit(1);
            }

            out_args->test_path = argv[++i];
        }
#endif
        else if (strcmp(argv[i], "--dtb") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --dtb requires a <path> argument\n");
                exit(1);
            }

            out_args->dtb_path = argv[++i];
        } else if (strcmp(argv[i], "--sbi") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --sbi requires a <path> argument\n");
                exit(1);
            }

            out_args->sbi_path = argv[++i];
        } else if (strcmp(argv[i], "--kernel") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --kernel requires a <path> argument\n");
                exit(1);
            }

            out_args->kernel_path = argv[++i];
        } else if (strcmp(argv[i], "--initrd") == 0) {
            if (i + 1 >= argc) {
                printf("Error: --initrd requires a <path> argument\n");
                exit(1);
            }

            out_args->initrd_path = argv[++i];
        } else {
            printf("Error: Unknown argument: %s\n", argv[i]);
            exit(1);
        }
    }

// #ifdef EMU_TESTS_ENABLED
//     if (!out_args->test_path || !out_args->sbi_path || !out_args->kernel_path) {
//         out_args->help = 1;
//         return;
//     }

//     if (out_args->test_path && (out_args->sbi_path || out_args->kernel_path)) {
//         printf("Warning: --test ignores both kernel and sbi\n");
//         exit(1);
//     }
// #else
//     if (!out_args->sbi_path && !out_args->kernel_path) {
//         out_args->help = 1;
//         return;
//     }
// #endif

//     if (out_args->kernel_path && !out_args->sbi_path) {
//         printf("Error: --kernel requires --sbi to be specified as well\n");
//         exit(1);
//     }

//     if (out_args->sbi_path && !out_args->kernel_path) {
//         printf("Warning: you didn't define a kernel to run\n");
//     }
}

void cli_help() {
    printf("Usage: riscv-emu [options]\n\n");
    printf("Available commands:\n");
    printf("  -h | --help       Print this help message\n");
#ifdef EMU_TESTS_ENABLED
    printf("  --test <path>     Path to RISC-V test binary to run\n");
#endif
    printf("  --dtb <path>      Path to device tree binary to load\n");
    printf("  --sbi <path>      Path to SBI binary to load\n");
    printf("  --kernel <path>   Path to kernel binary to load\n");
    printf("  --initrd <path>   Path to initial file system to load\n");
}