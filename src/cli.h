#ifndef __CLI_H__
#define __CLI_H__

typedef struct {
    char* test_path;
    char* kernel_path;
    char* sbi_path;
    char* dtb_path;
    char* initrd_path;
    int help;
} cli_args_t;

void cli_parse_args(int argc, char** argv, cli_args_t* out_args);
void cli_help();

#endif