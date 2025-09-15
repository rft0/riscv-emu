#include "cpu.h"

#include "def.h"
#include "instr.h"

#define OP_LUI      0x37
#define OP_AUIPC    0x17
#define OP_JAL      0x6F
#define OP_JALR     0x67
#define OP_BRANCH   0x63
#define OP_LOAD     0x03
#define OP_STORE    0x23
#define OP_IMM      0x13
#define OP_REG      0x33

static instr_t decode(uint32_t instr) {
    instr_t d;

    d.raw = instr;
    d.opcode = extract32(instr, 6, 0);
    d.rd = extract32(instr, 11, 7);
    d.funct3 = extract32(instr, 14, 12);
    d.rs1 = extract32(instr, 19, 15);
    d.rs2 = extract32(instr, 24, 20);
    d.funct7 = extract32(instr, 31, 25);

    switch (d.opcode) {
        case OP_REG:
            d.fmt = IFMT_R;
            break;
        case OP_JALR:
        case OP_LOAD:
        case OP_IMM:
            d.imm = sextract32(instr, 31, 20);
            d.fmt = IFMT_I;
            break;
        case OP_STORE:
            d.imm = sext((extract32(instr, 31, 25) << 5) | extract32(instr, 11, 7), 12);
            d.fmt = IFMT_S;
            break;
        case OP_BRANCH:
            d.imm = sext((((instr >> 31) & 1) << 12) | (((instr >> 7) & 1) << 11) | (extract32(instr, 30, 25) << 5) | (extract32(instr, 11, 8) << 1), 13);
            d.fmt = IFMT_B;
            break;
        case OP_LUI:
        case OP_AUIPC:
            d.fmt = IFMT_U;
            // d.imm = extract32(instr, 31, 12) << 12;
            //! TODO: Control sextract and sext probably wrong at rv64
            d.imm = (int64_t)((int32_t)(extract32(instr, 31, 12) << 12));
            break;
        case OP_JAL:
            d.imm = sext((((instr >> 31) & 1) << 20) | (extract32(instr, 19, 12) << 12) | (((instr >> 20) & 1) << 11) | (extract32(instr, 30, 21) << 1), 21);
            d.fmt = IFMT_J;
            break;
        default:
            d.fmt = IFMT_UNKNOWN;
            break;
    }

    return d;
}

static void execute(cpu_t* cpu, instr_t* instr) {
    switch (instr->opcode) {
        case IFMT_R:
            switch (instr->funct3) {
                case 0x0:
                    //! TODO: In case of overflow result is low XLEN bits of the result
                    if (instr->funct7 == 0x00) { // ADD
                        cpu->x[instr->rd] = cpu->x[instr->rs1] + cpu->x[instr->rs2];
                    } else if (instr->funct7 == 0x20) { // SUB
                        cpu->x[instr->rd] = cpu->x[instr->rs1] - cpu->x[instr->rs2];
                    }
                    break;
                case 0x1: // SLL
                    // cpu->x[instr->rd] = cpu->x[instr->rs1] << (cpu->x[instr->rs2] & 0x3F);
                    //! TODO: Check if genmask work as expected
                    cpu->x[instr->rd] = cpu->x[instr->rs1] << (cpu->x[instr->rs2] & GENMASK32(5, 0));
                    break;
                case 0x2: // SLT
                    cpu->x[instr->rd] = (cpu->x[instr->rs1] < cpu->x[instr->rs2]) ? 1 : 0;
                    break;
                case 0x3: // SLTU
                    cpu->x[instr->rd] = ((uint64_t)cpu->x[instr->rs1] < (uint64_t)cpu->x[instr->rs2]) ? 1 : 0;
                    break;
                case 0x4: // XOR
                    cpu->x[instr->rd] = cpu->x[instr->rs1] ^ cpu->x[instr->rs2];
                    break;
                case 0x5:
                    //! TODO: Check if genmask work as expected
                    if (instr->funct7 == 0x00) { // SRL
                        cpu->x[instr->rd] = (uint64_t)cpu->x[instr->rs1] >> (cpu->x[instr->rs2] & GENMASK32(5, 0));
                    } else if (instr->funct7 == 0x20) { // SRA
                        cpu->x[instr->rd] = cpu->x[instr->rs1] >> (cpu->x[instr->rs2] & GENMASK32(5, 0));
                    }
                    break;
                case 0x6: // OR
                    cpu->x[instr->rd] = cpu->x[instr->rs1] | cpu->x[instr->rs2];
                    break;
                case 0x7: // AND
                    cpu->x[instr->rd] = cpu->x[instr->rs1] & cpu->x[instr->rs2];
                    break;
                default:
                    //! TODO: Illegal instruction
                    break;
            }

        case IFMT_I:
            switch (instr->funct3) {
                case 0x0: // ADDI
                    cpu->x[instr->rd] = cpu->x[instr->rs1] + instr->imm;
                    break;
                case 0x2: // SLTI
                    cpu->x[instr->rd] = (cpu->x[instr->rs1] < instr->imm) ? 1 : 0;
                    break;
                case 0x3: // SLTIU
                    cpu->x[instr->rd] = ((uint64_t)cpu->x[instr->rs1] < (uint64_t)instr->imm) ? 1 : 0;
                    break;
                case 0x4: // XORI
                    cpu->x[instr->rd] = cpu->x[instr->rs1] ^ instr->imm;
                    break;
                case 0x6: // ORI
                    cpu->x[instr->rd] = cpu->x[instr->rs1] | instr->imm;
                    break;
                case 0x7: // ANDI
                    cpu->x[instr->rd] = cpu->x[instr->rs1] & instr->imm;
                    break;
                case 0x1: // SLLI
                    // shamt is imm[4:0] for RV32I, imm[5:0] for RV64I and we use RVV64I
                    cpu->x[instr->rd] = cpu->x[instr->rs1] << extract32(instr->imm, 5, 0);
                    break;
                case 0x5:
                    if (instr->funct7 == 0x00) { // SRLI
                        cpu->x[instr->rd] = (uint64_t)cpu->x[instr->rs1] >> extract32(instr->imm, 5, 0);
                    } else if (instr->funct7 == 0x20) { // SRAI
                        cpu->x[instr->rd] = cpu->x[instr->rs1] >> extract32(instr->imm, 5, 0);
                    }
                    break;

                default:
                    //! TODO: Illegal instruction
                    break;
            }

            cpu->pc += 4;
            break;
        case IFMT_S:
            break;
        case IFMT_B:
            break;
        case IFMT_U:
                switch (instr->opcode) {
                    case OP_LUI:
                        cpu->x[instr->rd] = instr->imm;
                        break;
                    case OP_AUIPC:
                        cpu->x[instr->rd] = cpu->pc + instr->imm;
                        break;
                    default:
                        //! TODO: Illegal instruction
                        break;
                }

            cpu->pc += 4;
            break;
        case IFMT_J:
            break;
        default:
            //! TODO: Illegal instruction
            break;
    }
}

void cpu_step(cpu_t* cpu) {
    uint32_t instr = *(uint32_t*)cpu->mem[cpu->pc];
    instr_t d = decode(instr);
    execute(cpu, &d);
}