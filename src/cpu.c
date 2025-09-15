#include "cpu.h"

#define RS1                 extract32(instr, 15, 5)
#define X_RS1               cpu->x[RS1]
#define RS2                 extract32(instr, 20, 5)
#define X_RS2               cpu->x[RS2]
#define RD                  extract32(instr, 7, 5)

#define IMM                 ((int32_t)1)
#define WRITE_RD(value)     do { if (RD != 0) cpu->x[RD] = (value); } while(0)
#define WRITE_PC(value)     do { cpu->pc = (value); return; } while(0)

typedef void (*instr_func_t)(cpu_t* cpu, uint32_t instr);

// Instruction implementations - Spike style, each is standalone
static void instr_add(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(X_RS1 + X_RS2);
}

static void instr_addi(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 + IMM);
}

static void instr_sub(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 - RS2);
}

static void instr_xor(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 ^ RS2);
}

static void instr_xori(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 ^ IMM);
}

static void instr_or(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 | RS2);
}

static void instr_ori(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 | IMM);
}

static void instr_and(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 & RS2);
}

static void instr_andi(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 & IMM);
}

static void instr_sll(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 << (RS2 & 0x1F));
}

static void instr_slli(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 << (IMM & 0x1F));
}

static void instr_srl(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 >> (RS2 & 0x1F));
}

static void instr_srli(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(RS1 >> (IMM & 0x1F));
}

static void instr_sra(cpu_t* cpu, uint32_t instr) {
    WRITE_RD((int32_t)RS1 >> (RS2 & 0x1F));
}

static void instr_srai(cpu_t* cpu, uint32_t instr) {
    WRITE_RD((int32_t)RS1 >> (IMM & 0x1F));
}

static void instr_slt(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(((int32_t)RS1 < (int32_t)RS2) ? 1 : 0);
}

static void instr_slti(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(((int32_t)RS1 < IMM) ? 1 : 0);
}

static void instr_sltu(cpu_t* cpu, uint32_t instr) {
    WRITE_RD((RS1 < RS2) ? 1 : 0);
}

static void instr_sltiu(cpu_t* cpu, uint32_t instr) {
    WRITE_RD((RS1 < (uint32_t)IMM) ? 1 : 0);
}

static void instr_lui(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(imm);  // imm already contains upper 20 bits
}

static void instr_auipc(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(cpu->pc + imm);
}

static void instr_jal(cpu_t* cpu, uint32_t instr) {
    WRITE_RD(cpu->pc + 4);
    WRITE_PC(cpu->pc + imm);
}

static void instr_jalr(cpu_t* cpu, uint32_t instr) {
    uint32_t target = (RS1 + IMM) & ~1;
    WRITE_RD(cpu->pc + 4);
    WRITE_PC(target);
}

static void instr_beq(cpu_t* cpu, uint32_t instr) {
    if (RS1 == RS2) WRITE_PC(cpu->pc + imm);
}

static void instr_bne(cpu_t* cpu, uint32_t instr) {
    if (RS1 != RS2) WRITE_PC(cpu->pc + imm);
}

static void instr_blt(cpu_t* cpu, uint32_t instr) {
    if ((int32_t)RS1 < (int32_t)RS2) WRITE_PC(cpu->pc + imm);
}

static void instr_bge(cpu_t* cpu, uint32_t instr) {
    if ((int32_t)RS1 >= (int32_t)RS2) WRITE_PC(cpu->pc + imm);
}

static void instr_bltu(cpu_t* cpu, uint32_t instr) {
    if (RS1 < RS2) WRITE_PC(cpu->pc + imm);
}

static void instr_bgeu(cpu_t* cpu, uint32_t instr) {
    if (RS1 >= RS2) WRITE_PC(cpu->pc + imm);
}

static void instr_illegal(cpu_t* cpu, uint32_t instr) {
    printf("Illegal instruction at PC: 0x%08x\n", cpu->pc);
}

typedef struct {
    uint32_t match;      // Instruction bits that must match
    uint32_t mask;       // Mask for relevant bits
    instr_func_t func;    // Function to execute
    const char* name;    // Mnemonic
} instr_decode_t;

// Decode table - like Spike's generated opcodes
static const instr_decode_t decode_table[] = {
    // R-type (OP: 0x33)
    {0x00000033, 0xfe00707f, instr_add,   "add"},
    {0x40000033, 0xfe00707f, instr_sub,   "sub"},
    {0x00004033, 0xfe00707f, instr_xor,   "xor"},
    {0x00006033, 0xfe00707f, instr_or,    "or"},
    {0x00007033, 0xfe00707f, instr_and,   "and"},
    {0x00001033, 0xfe00707f, instr_sll,   "sll"},
    {0x00005033, 0xfe00707f, instr_srl,   "srl"},
    {0x40005033, 0xfe00707f, instr_sra,   "sra"},
    {0x00002033, 0xfe00707f, instr_slt,   "slt"},
    {0x00003033, 0xfe00707f, instr_sltu,  "sltu"},
    
    // I-type (OP-IMM: 0x13)
    {0x00000013, 0x0000707f, instr_addi,  "addi"},
    {0x00004013, 0x0000707f, instr_xori,  "xori"},
    {0x00006013, 0x0000707f, instr_ori,   "ori"},
    {0x00007013, 0x0000707f, instr_andi,  "andi"},
    {0x00001013, 0xfc00707f, instr_slli,  "slli"},
    {0x00005013, 0xfc00707f, instr_srli,  "srli"},
    {0x40005013, 0xfc00707f, instr_srai,  "srai"},
    {0x00002013, 0x0000707f, instr_slti,  "slti"},
    {0x00003013, 0x0000707f, instr_sltiu, "sltiu"},
    
    // U-type
    {0x00000037, 0x0000007f, instr_lui,   "lui"},
    {0x00000017, 0x0000007f, instr_auipc, "auipc"},
    
    // J-type
    {0x0000006f, 0x0000007f, instr_jal,   "jal"},
    {0x00000067, 0x0000707f, instr_jalr,  "jalr"},
    
    // B-type (BRANCH: 0x63)
    {0x00000063, 0x0000707f, instr_beq,   "beq"},
    {0x00001063, 0x0000707f, instr_bne,   "bne"},
    {0x00004063, 0x0000707f, instr_blt,   "blt"},
    {0x00005063, 0x0000707f, instr_bge,   "bge"},
    {0x00006063, 0x0000707f, instr_bltu,  "bltu"},
    {0x00007063, 0x0000707f, instr_bgeu,  "bgeu"},
    
    {0, 0, NULL, NULL}  // End of table
};

void execute_instruction(cpu_t* cpu, uint32_t instr) {
    const instr_decode_t* entry = decode_table;
    while (entry->func != NULL) {
        if ((instr & entry->mask) == entry->match) {
            // Extract fields
            uint8_t rd = (instr >> 7) & 0x1F;
            uint8_t rs1 = (instr >> 15) & 0x1F;
            uint8_t rs2 = (instr >> 20) & 0x1F;
            
            // Extract immediate based on instruction format
            int32_t imm;
            uint8_t opcode = instr & 0x7F;
            
            if (opcode == 0x13 || opcode == 0x67) {  // I-type
                imm = (int32_t)instr >> 20;
            } else if (opcode == 0x63) {  // B-type
                imm = ((instr >> 31) & 0x1) << 12 |
                      ((instr >> 7) & 0x1) << 11 |
                      ((instr >> 25) & 0x3F) << 5 |
                      ((instr >> 8) & 0xF) << 1;
                imm = (int32_t)(imm << 19) >> 19;  // Sign extend
            } else if (opcode == 0x37 || opcode == 0x17) {  // U-type
                imm = instr & 0xFFFFF000;
            } else if (opcode == 0x6F) {  // J-type
                imm = ((instr >> 31) & 0x1) << 20 |
                      ((instr >> 12) & 0xFF) << 12 |
                      ((instr >> 20) & 0x1) << 11 |
                      ((instr >> 21) & 0x3FF) << 1;
                imm = (int32_t)(imm << 11) >> 11;  // Sign extend
            } else {
                imm = 0;
            }
            
            printf("Executing: %s (0x%08x)\n", entry->name, instr);
            entry->func(cpu, instr);
            cpu->pc += 4;

            return;
        }

        entry++;
    }
    
    // No match found
    instr_illegal(cpu, instr);
    cpu->pc += 4;
}