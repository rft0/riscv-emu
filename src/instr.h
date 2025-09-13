#ifndef __INSTR_H__
#define __INSTR_H__

#include "def.h"

typedef enum {
    IFMT_UNKNOWN = 0,
    IFMT_R,
    IFMT_I,
    IFMT_S,
    IFMT_B,
    IFMT_U,
    IFMT_J
} inst_fmt_t;

typedef struct {
    uint8_t opcode;   // bits 6:0
    uint8_t rd;       // bits 11:7
    uint8_t funct3;   // bits 14:12
    uint8_t rs1;      // bits 19:15
    uint8_t rs2;      // bits 24:20
    uint8_t funct7;   // bits 31:25
    inst_fmt_t fmt;
    int64_t imm;
} instr_t;

#endif