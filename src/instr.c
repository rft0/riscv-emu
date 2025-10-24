#include <fenv.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "helpers.h"
#include "trap.h"
#include "csr.h"
#include "mem.h"

#define DBG_PRINT(x)       do { printf("\nDBG: 0x%lX\n\n", (uint64_t)(x)); } while(0)

#define _RD                 extract32(instr, 11, 7)
#define _RS1                extract32(instr, 19, 15)
#define _RS2                extract32(instr, 24, 20)
#define _RS3                extract32(instr, 31, 27)

#define _C_RD               extract32(instr, 4, 2)
#define _C_RS1              extract32(instr, 9, 7)  
#define _C_RS2              extract32(instr, 4, 2)
#define _C_WIDE_RS2         extract32(instr, 6, 2)

#define PC                  (cpu->pc)
#define NPC                 (cpu->npc)

#define RD                  (cpu->x[_RD])
#define RS1                 (cpu->x[_RS1])
#define RS2                 (cpu->x[_RS2])
#define RS3                 (cpu->x[_RS3])

#define SP                  (cpu->x[2])

#define FRS1                (cpu->f[_RS1])
#define FRS2                (cpu->f[_RS2])
#define FRS3                (cpu->f[_RS3])

#define C_RD                (cpu->x[_C_RD + 8])
#define C_RS1               (cpu->x[_C_RS1 + 8])
#define C_RS2               (cpu->x[_C_RS2 + 8])
#define C_WIDE_RS2           (cpu->x[_C_WIDE_RS2])

#define C_FRS1              (cpu->f[_C_RS1 + 8])
#define C_FRS2              (cpu->f[_C_RS2 + 8]) 
#define C_WIDE_FRS2         (cpu->f[_C_WIDE_RS2])
#define C_FRD               (cpu->f[_C_RD + 8])
            
#define SET_RD(val)         do { if (_RD != 0) cpu->x[_RD] = (val); } while(0)
#define SET_FRD(val)        do { cpu->f[_RD] = (val); if (((cpu->csr.mstatus << 13 ) & 3) != 0) cpu->csr.mstatus |= (3ULL << 13); } while(0)

#define C_SET_FRD(val)      do { cpu->f[_C_RD + 8] = (val); } while(0)
#define C_SET_RS1(val)      do { cpu->x[_C_RS1 + 8] = (val); } while(0)
#define C_SET_RD(val)       do { cpu->x[_C_RD + 8] = (val); } while(0)

#define SET_PC(val)         (cpu->pc = (val))
#define SET_NPC(val)        (cpu->npc = (val))
#define SET_SP(val)         (cpu->x[2] = (val))

#define IMM_I               sext(extract32(instr, 31, 20), 12)
#define IMM_S               sext((extract32(instr, 31, 25) << 5) | extract32(instr, 11, 7), 12)
#define IMM_B               sext((extract32(instr, 31, 31) << 12) | (extract32(instr, 7, 7) << 11) | (extract32(instr, 30, 25) << 5) | (extract32(instr, 11, 8) << 1), 13)
#define IMM_U               sext((extract32(instr, 31, 12) << 12), 32)
#define IMM_J               sext((extract32(instr, 31, 31) << 20) | (extract32(instr, 19, 12) << 12) | (extract32(instr, 20, 20) << 11) | (extract32(instr, 30, 21) << 1), 21)
#define IMM_CSR             extract32(instr, 31, 20)

#define ZIMM                _RS1

#define IMM_CADDI4SPN       (extract32(instr, 12, 11) << 4  | extract32(instr, 10, 7) << 6 | extract32(instr, 6, 6) << 2 | extract32(instr, 5, 5) << 3)
// c.fld, c.ld, c.fsd, c.sd
#define IMM_CFLD            (extract32(instr, 12, 10) << 3 | extract32(instr, 6, 5) << 6)
// c.lw, c.sw
#define IMM_CLW             (extract32(instr, 12, 10) << 3 | extract32(instr, 6, 6) << 2 | extract32(instr, 5, 5) << 6 )
// c.nop, c.addi, c.addiw, c.li
#define IMM_CI              sext(extract32(instr, 12, 12) << 5 | extract32(instr, 6, 2), 6)
// c.j, c.jal
#define IMM_CJ              sext((extract32(instr, 12, 12) << 11 | extract32(instr, 11, 11) << 4 | extract32(instr, 10, 9) << 8 | extract32(instr, 8, 8) << 10 | extract32(instr, 7, 7) << 6 | extract32(instr, 6, 6) << 7 | extract32(instr, 5, 3) << 1 | extract32(instr, 2, 2) << 5), 12)
#define IMM_CADDI16SP       sext((extract32(instr, 12, 12) << 9 | extract32(instr, 6, 6) << 4 | extract32(instr, 5, 5) << 6 | extract32(instr, 4, 3) << 7 | extract32(instr, 2, 2) << 5), 10)
#define IMM_CLUI            sext((extract32(instr, 12, 12) << 17 | extract32(instr, 6, 2) << 12), 18)
// c.srli, c.srai, c.slli
#define IMM_CUI             (extract32(instr, 12, 12) << 5 | extract32(instr, 6 , 2))
// c.beqz, c.bnez
#define IMM_CB              sext((extract32(instr, 12, 12) << 8 | extract32(instr, 11, 10) << 3 | extract32(instr, 6, 5) << 6 | extract32(instr, 4, 3) << 1 | extract32(instr, 2, 2) << 5), 9)
// c.fldsp, c.ldsp
#define IMM_CFLDSP          (extract32(instr, 12, 12) << 5 | extract32(instr, 6, 5) << 3 | extract32(instr, 4, 2) << 6)
// c.lwsp
#define IMM_CLWSP           (extract32(instr, 12, 12) << 5 | extract32(instr, 6, 4) << 2 | extract32(instr, 3, 2) << 6)
// c.fsdsp, c.sdsp
#define IMM_CFSDSP          (extract32(instr, 12, 10) << 3 | extract32(instr, 9, 7) << 6)
// c.swsp, c.fswsp
#define IMM_CSWSP           (extract32(instr, 12, 9) << 2 | extract32(instr, 8, 7) << 6)

#define SHAMTRV64           extract32(instr, 25, 20)
#define SHAMT               extract32(instr, 24, 20)

#define AMO_AQ              extract32(instr, 26, 26)
#define AMO_RL              extract32(instr, 25, 25)
#define RMODE               extract32(instr, 14, 12)

#define CHECK_FPU                                               \
    if (((cpu->csr.mstatus >> 13) & 3) == 0) {                  \
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);         \
        return;                                                 \
    }                                                           \

#define FE_SETUP_RM                                             \
    int rm = RMODE;                                             \
    if (rm == RM_DYN)                                           \
        rm = ((cpu->csr.fcsr >> 5) & 0x7);                      \
    int old_rm = fegetround();                                  \
    int new_rm;                                                 \
    switch (rm) {                                               \
        case RM_RNE: new_rm = FE_TONEAREST; break;              \
        case RM_RTZ: new_rm = FE_TOWARDZERO; break;             \
        case RM_RDN: new_rm = FE_DOWNWARD; break;               \
        case RM_RUP: new_rm = FE_UPWARD; break;                 \
        case RM_RMM: new_rm = FE_TONEAREST; break;              \
        default:                                                \
            raise_trap(cpu, CAUSE_ILLEGAL_INSTR, PC, 0);        \
            return;                                             \
    }                                                           \
    int changed = 0;                                            \
    if (old_rm != new_rm) {                                     \
        fesetround(new_rm);                                     \
        changed = 1;                                            \
    }                                                           \

#define FE_SETUP_CSR_RM                                         \
    int old_rm = fegetround();                                  \
    int new_rm;                                                 \
    switch ((cpu->csr.fcsr >> 5) & 0x7) {                       \
        case RM_RNE: new_rm = FE_TONEAREST; break;              \
        case RM_RTZ: new_rm = FE_TOWARDZERO; break;             \
        case RM_RDN: new_rm = FE_DOWNWARD; break;               \
        case RM_RUP: new_rm = FE_UPWARD; break;                 \
        case RM_RMM: new_rm = FE_TONEAREST; break;              \
        default:                                                \
            raise_trap(cpu, CAUSE_ILLEGAL_INSTR, PC, 0);        \
            return;                                             \
    }                                                           \
    int changed = 0;                                            \
    if (old_rm != new_rm) {                                     \
        fesetround(new_rm);                                     \
        changed = 1;                                            \
    }                                                           \

#define FE_RESTORE_RM                                           \
    if (changed)                                                \
        fesetround(old_rm);

enum {
    RM_RNE = 0,
    RM_RTZ = 1,
    RM_RDN = 2,
    RM_RUP = 3,
    RM_RMM = 4,
    RM_DYN = 7
};

static inline float f32_from_fpr(uint64_t rv) {
    if ((rv & 0xFFFFFFFF00000000ULL) != 0xFFFFFFFF00000000ULL) {
        union { uint32_t bits; float f; } u = {.bits = 0x7FC00000};
        return u.f;
    }
    
    union { uint32_t bits; float f; } u = {.bits = (uint32_t)rv};
    return u.f;
}

static inline uint64_t f32_to_fpr(float val) {
    union { float f; uint32_t bits; } u = {.f = val};
    return 0xFFFFFFFF00000000ULL | u.bits;
}

static inline double f64_from_fpr(uint64_t rv) {
    union { uint64_t bits; double d; } u = {.bits = rv};
    return u.d;
}

static inline uint64_t f64_to_fpr(double val) {
    union { double d; uint64_t bits; } u = {.d = val};
    return u.bits;
}

static inline void fflags_check_and_set(cpu_t* cpu) {
    int excs = fetestexcept(FE_ALL_EXCEPT);

    uint8_t fflags = 0;
    if (excs & FE_INVALID)    fflags |= (1 << 4);
    if (excs & FE_DIVBYZERO)  fflags |= (1 << 3);
    if (excs & FE_OVERFLOW)   fflags |= (1 << 2);
    if (excs & FE_UNDERFLOW)  fflags |= (1 << 1);
    if (excs & FE_INEXACT)    fflags |= (1 << 0);

    cpu->csr.fcsr |= fflags;
}

void exec_lui(cpu_t* cpu, uint32_t instr) {
    SET_RD(IMM_U);
}

void exec_auipc(cpu_t* cpu, uint32_t instr) {
    SET_RD(cpu->pc + IMM_U);
}

void exec_addi(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 + IMM_I);
}

void exec_slti(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)RS1 < IMM_I); // Signed comparison
}

void exec_sltiu(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 < (uint64_t)IMM_I); // Unsigned comparison
}

void exec_xori(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 ^ IMM_I);
}

void exec_ori(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 | IMM_I);
}

void exec_andi(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 & IMM_I);
}

void exec_slli(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 << SHAMTRV64);
}

void exec_srli(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 >> SHAMTRV64);
}
void exec_srai(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)RS1 >> SHAMTRV64);
}

void exec_add(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 + RS2);
}

void exec_sub(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 - RS2);
}

void exec_sll(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 << extract32(RS2, 5, 0)); // Use only lower 6 bits
}

void exec_slt(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)RS1 < (int64_t)RS2);
}

void exec_sltu(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 < RS2);
}

void exec_xor(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 ^ RS2);
}

void exec_srl(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 >> extract32(RS2, 5, 0));
}

void exec_sra(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)RS1 >> extract32(RS2, 5, 0));
}
void exec_or(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 | RS2);
}

void exec_and(cpu_t* cpu, uint32_t instr) {
    SET_RD(RS1 & RS2);
}

void exec_fence(cpu_t* cpu, uint32_t instr) {
    // Do nothing.
}

void exec_fence_tso(cpu_t* cpu, uint32_t instr) {
    // Do nothing.
}

void exec_pause(cpu_t* cpu, uint32_t instr) {
    // Pause hint, it is ok to do nothing.
}

void exec_fence_i(cpu_t* cpu, uint32_t instr) {
    // No icache, do nothing.
}

void exec_csrrw(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t old_val;

    if (_RD != 0) {
        if(!csr_read(cpu, addr, &old_val))
            return;
    }

    csr_write(cpu, addr, RS1);
    SET_RD(old_val);
}

void exec_csrrs(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t old_val = 0;
    uint32_t rs1_idx = _RS1;

    if (_RD != 0 || rs1_idx != 0) {
        if (!csr_read(cpu, addr, &old_val))
            return;
    }

    if (rs1_idx != 0)
        csr_write(cpu, addr, old_val | RS1);

    SET_RD(old_val);
}

void exec_csrrc(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t old_val = 0;
    uint32_t rs1_idx = _RS1;

    if (_RD != 0 || rs1_idx != 0) {
        if (!csr_read(cpu, addr, &old_val))
            return;
    }

    if (rs1_idx != 0)
        csr_write(cpu, addr, old_val & ~RS1);

    SET_RD(old_val);
}

void exec_csrrwi(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t old_val;

    if (_RD != 0) {
        if(!csr_read(cpu, addr, &old_val))
            return;
    }

    csr_write(cpu, addr, ZIMM);
    SET_RD(old_val);
}

void exec_csrrsi(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t old_val = 0;
    uint64_t zimm = ZIMM;

    if (_RD != 0 || zimm != 0) {
        if (!csr_read(cpu, addr, &old_val))
            return;
    }

    if (zimm != 0)
        csr_write(cpu, addr, old_val | zimm);

    SET_RD(old_val);
}

void exec_csrrci(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t old_val = 0;
    uint64_t zimm = ZIMM;

    if (_RD != 0 || zimm != 0) {
        if (!csr_read(cpu, addr, &old_val))
            return;
    }

    if (zimm != 0)
        csr_write(cpu, addr, old_val & ~zimm);

    SET_RD(old_val);
}

void exec_ecall(cpu_t* cpu, uint32_t instr) {
    uint64_t cause;
    switch (cpu->mode) {
        case PRIV_U: cause = CAUSE_ECALL_FROM_U; break;
        case PRIV_S: cause = CAUSE_ECALL_FROM_HS; break;
        case PRIV_H: cause = CAUSE_ECALL_FROM_VS; break;
        case PRIV_M: cause = CAUSE_ECALL_FROM_M; break;
        default: cause = CAUSE_ECALL_FROM_M; break;
    }

    raise_trap(cpu, cause, PC, 0);
}

void exec_ebreak(cpu_t* cpu, uint32_t instr) {
    raise_trap(cpu, CAUSE_BREAKPOINT, PC, 0);
}

void exec_sret(cpu_t* cpu, uint32_t instr) {
    if ((cpu->csr.mstatus >> 22) & 1) {
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, 0, 0);
        return;
    }

    uint64_t sstatus = csr_read_sstatus(cpu);

    uint64_t spp = (sstatus >> 8) & 1;
    uint64_t spie = (sstatus >> 5) & 1;

    SET_NPC(cpu->csr.sepc);
    cpu->mode = spp ? PRIV_S : PRIV_U;

    sstatus = (sstatus & ~0x100UL) | (0UL << 8);    // SPP=U
    sstatus = (sstatus & ~0x2UL) | (spie << 1);     // SIE=SPIE
    sstatus |= (1UL << 5);                          // SPIE=1

    csr_write_sstatus(cpu, sstatus);
}

// void exec_mret(cpu_t* cpu, uint32_t instr) {
//     uint64_t mepc;
//     if (!csr_read(cpu, CSR_MEPC, &mepc))
//         return;

//     uint64_t mstatus;
//     if (!csr_read(cpu, CSR_MSTATUS, &mstatus))
//         return;

//     uint64_t mpp = (mstatus >> 11) & 3;
//     uint64_t mpie = (mstatus >> 7) & 1;

//     SET_NPC(mepc);
//     cpu->mode = mpp; // 0=U,1=S,3=M

//     mstatus = (mstatus & ~(3UL << 11));             // MPP=U
//     mstatus = (mstatus & ~0x8UL) | (mpie << 3);     // MIE=MPIE
//     mstatus |= (1UL << 7);                          // MPIE=1
    
//     csr_write(cpu, CSR_MSTATUS, mstatus);
// }

void exec_mret(cpu_t* cpu, uint32_t instr) {
    uint64_t mstatus = cpu->csr.mstatus;
    uint64_t mepc = cpu->csr.mepc;

    uint64_t mpp = (mstatus >> 11) & 3;
    uint64_t mpie = (mstatus >> 7) & 1;

    SET_NPC(mepc);
    cpu->mode = mpp;

    mstatus &= ~(3UL << 11);

    // MIE (bit 3) is set to the value of MPIE
    mstatus = (mstatus & ~(1UL << 3)) | (mpie << 3);

    // MPIE (bit 7) is set to 1
    mstatus |= (1UL << 7);
    // MPRV (bit 17) must be set to 0
    mstatus &= ~(1UL << 17);

    cpu->csr.mstatus = mstatus;
}

void exec_wfi(cpu_t* cpu, uint32_t instr) {
    //! TODO: Stall until interrupt pending.
    // For now, just do nothing.
}

void exec_sfence_vma(cpu_t* cpu, uint32_t instr) {
    uint64_t tw = (cpu->csr.mstatus >> 20) & 1;
    if (cpu->mode == PRIV_S && tw == 1) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, 0, 0);
        return;
    }

    // Else normal sfence.vma behaviour
    // Does nothing.
}

void exec_lb(cpu_t* cpu, uint32_t instr) {
    uint8_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 1))
        return;

    SET_RD((int64_t)(int8_t)val);
}

void exec_lh(cpu_t* cpu, uint32_t instr) {
    uint16_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 2))
        return;

    SET_RD((int64_t)(int16_t)val);
}

void exec_lw(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);
}

void exec_lbu(cpu_t* cpu, uint32_t instr) {
    uint8_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 1))
        return;

    SET_RD((uint64_t)val);
}

void exec_lhu(cpu_t* cpu, uint32_t instr) {
    uint16_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 2))
        return;

    SET_RD((uint64_t)val);
}

void exec_sb(cpu_t* cpu, uint32_t instr) {
    va_store(cpu, RS1 + IMM_S, &RS2, 1);
}

void exec_sh(cpu_t* cpu, uint32_t instr) {
    va_store(cpu, RS1 + IMM_S, &RS2, 2);
}

void exec_sw(cpu_t* cpu, uint32_t instr) {
    printf("PC: 0x%lX, GP: %lu\n", cpu->pc, cpu->x[3]);
    va_store(cpu, RS1 + IMM_S, &RS2, 4);
}

void exec_jal(cpu_t* cpu, uint32_t instr) {
    SET_RD(PC + 4);
    SET_NPC(PC + IMM_J);
}

void exec_jalr(cpu_t* cpu, uint32_t instr) {
    uint64_t target = (RS1 + IMM_I) & ~1ULL;
    SET_RD(PC + 4);
    SET_NPC(target);
}

void exec_beq(cpu_t* cpu, uint32_t instr) {
    if (RS1 == RS2)
        SET_NPC(PC + IMM_B);
}

void exec_bne(cpu_t* cpu, uint32_t instr) {
    uint64_t rs1 = RS1;
    uint64_t rs2 = RS2;
    if (RS1 != RS2)
        SET_NPC(PC + IMM_B);

    printf("BNE, RS1: 0x%lX, RS2: 0x%lX\n", RS1, RS2);
}

void exec_blt(cpu_t* cpu, uint32_t instr) {
    if ((int64_t)RS1 < (int64_t)RS2)
        SET_NPC(PC + IMM_B);
}

void exec_bge(cpu_t* cpu, uint32_t instr) {
    if ((int64_t)RS1 >= (int64_t)RS2)
        SET_NPC(PC + IMM_B);
}

void exec_bltu(cpu_t* cpu, uint32_t instr) {
    if (RS1 < RS2)
        SET_NPC(PC + IMM_B);
}

void exec_bgeu(cpu_t* cpu, uint32_t instr) {
    if (RS1 >= RS2)
        SET_NPC(PC + IMM_B);
}
void exec_addiw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)(RS1 + IMM_I));
}

void exec_slliw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)(RS1 << SHAMT));
}

void exec_srliw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)((uint32_t)RS1 >> SHAMT));
}

void exec_sraiw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)((int32_t)RS1 >> SHAMT));
}

void exec_addw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)(RS1 + RS2));
}
void exec_subw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)(RS1 - RS2));
}

void exec_sllw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)(RS1 << extract32(RS2, 4, 0)));
}

void exec_srlw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)((uint32_t)RS1 >> extract32(RS2, 4, 0)));
}

void exec_sraw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)((int32_t)RS1 >> extract32(RS2, 4, 0)));
}

void exec_lwu(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 4))
        return;
    
    SET_RD((uint64_t)val);
}

void exec_ld(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 8))
        return;

    SET_RD(val);
}

void exec_sd(cpu_t* cpu, uint32_t instr) {
    va_store(cpu, RS1 + IMM_S, &RS2, 8);
}

void exec_mul(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)RS1 * (int64_t)RS2);
}

void exec_mulh(cpu_t* cpu, uint32_t instr) {
    int64_t hi;
    mul128((int64_t)RS1, (int64_t)RS2, &hi);
    SET_RD(hi);
}

void exec_mulhsu(cpu_t* cpu, uint32_t instr) {
    int64_t hi;
    mulsu128((int64_t)RS1, RS2, &hi);
    SET_RD(hi);
}

void exec_mulhu(cpu_t* cpu, uint32_t instr) {
    uint64_t hi;
    mulu128(RS1, RS2, &hi);
    SET_RD(hi);
}

void exec_div(cpu_t* cpu, uint32_t instr) {
    int64_t dividend = (int64_t)RS1;
    int64_t divisor = (int64_t)RS2;
    
    if (divisor == 0)
        SET_RD(-1);
    else if (dividend == INT64_MIN && divisor == -1)
        SET_RD(dividend);
    else
        SET_RD(dividend / divisor);
}

void exec_divu(cpu_t* cpu, uint32_t instr) {
    uint64_t dividend = RS1;
    uint64_t divisor = RS2;
    
    if (divisor == 0)
        SET_RD(UINT64_MAX);
    else
        SET_RD(dividend / divisor);
}

void exec_rem(cpu_t* cpu, uint32_t instr) {
    int64_t dividend = (int64_t)RS1;
    int64_t divisor = (int64_t)RS2;
    
    if (divisor == 0)
        SET_RD(dividend);
    else if (dividend == INT64_MIN && divisor == -1)
        SET_RD(0);
    else
        SET_RD(dividend % divisor);
}

void exec_remu(cpu_t* cpu, uint32_t instr) {
    uint64_t dividend = RS1;
    uint64_t divisor = RS2;
    
    if (divisor == 0)
        SET_RD(dividend);
    else
        SET_RD(dividend % divisor);
}

void exec_mulw(cpu_t* cpu, uint32_t instr) {
    SET_RD((int64_t)(int32_t)((int32_t)RS1 * (int32_t)RS2));
}

void exec_divw(cpu_t* cpu, uint32_t instr) {
    int32_t dividend = (int32_t)RS1;
    int32_t divisor = (int32_t)RS2;
    
    if (divisor == 0)
        SET_RD(-1);
    else if (dividend == INT32_MIN && divisor == -1)
        SET_RD((int64_t)dividend);
    else
        SET_RD((int64_t)(int32_t)(dividend / divisor));
}

void exec_divuw(cpu_t* cpu, uint32_t instr) {
    uint32_t dividend = (uint32_t)RS1;
    uint32_t divisor = (uint32_t)RS2;
    
    if (divisor == 0)
        SET_RD(UINT64_MAX);
    else
        SET_RD((int64_t)(int32_t)(dividend / divisor));
}

void exec_remw(cpu_t* cpu, uint32_t instr) {
    int32_t dividend = (int32_t)RS1;
    int32_t divisor = (int32_t)RS2;
    
    if (divisor == 0)
        SET_RD((int64_t)dividend);
    else if (dividend == INT32_MIN && divisor == -1)
        SET_RD(0);
    else
        SET_RD((int64_t)(int32_t)(dividend % divisor));
}

void exec_remuw(cpu_t* cpu, uint32_t instr) {
    uint32_t dividend = (uint32_t)RS1;
    uint32_t divisor = (uint32_t)RS2;
    
    if (divisor == 0)
        SET_RD((int64_t)(int32_t)dividend);
    else
        SET_RD((int64_t)(int32_t)(dividend % divisor));
}

// Ignore aq and rl for single core
void exec_lr_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    cpu->lr_addr = RS1;
    cpu->lr_valid = 1;
}

void exec_sc_w(cpu_t* cpu, uint32_t instr) {
    if (!cpu->lr_valid || cpu->lr_addr != RS1) {
        SET_RD(1); // failure
        return;
    }

    if (!va_store(cpu, RS1, &RS2, 4)) {
        SET_RD(1);
        return;
    }

    SET_RD(0); // success
    cpu->lr_valid = 0;
}

void exec_amoswap_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    if (!va_store(cpu, RS1, &RS2, 4))
        return;
}

void exec_amoadd_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t sum = val + (uint32_t)RS2;
    if (!va_store(cpu, RS1, &sum, 4))
        return;
}

void exec_amoxor_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = val ^ (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_amoand_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = val & (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_amoor_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = val | (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_amomin_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = (int32_t)val < (int32_t)RS2 ? val : (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_amomax_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = (int32_t)val > (int32_t)RS2 ? val : (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_amominu_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = val < (uint32_t)RS2 ? val : (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_amomaxu_w(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, RS1, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);

    uint32_t res = val > (uint32_t)RS2 ? val : (uint32_t)RS2;
    if (!va_store(cpu, RS1, &res, 4))
        return;
}

void exec_lr_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    cpu->lr_addr = RS1;
    cpu->lr_valid = 1;
}

void exec_sc_d(cpu_t* cpu, uint32_t instr) {
    if (!cpu->lr_valid || cpu->lr_addr != RS1) {
        SET_RD(1);
        return;
    }

    if (!va_store(cpu, RS1, &RS2, 8)) {
        SET_RD(1);
        return;
    }

    SET_RD(0);
    cpu->lr_valid = 0;
}

void exec_amoswap_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    if (!va_store(cpu, RS1, &RS2, 8))
        return;
}

void exec_amoadd_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t sum = val + RS2;
    if (!va_store(cpu, RS1, &sum, 8))
        return;
}

void exec_amoxor_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = val ^ RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_amoand_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = val & RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_amoor_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = val | RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_amomin_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = (int64_t)val < (int64_t)RS2 ? val : RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_amomax_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = (int64_t)val > (int64_t)RS2 ? val : RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_amominu_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = val < RS2 ? val : RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_amomaxu_d(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, RS1, &val, 8))
        return;

    SET_RD(val);

    uint64_t res = val > RS2 ? val : RS2;
    if (!va_store(cpu, RS1, &res, 8))
        return;
}

void exec_flw(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    uint32_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 4))
        return;

    SET_FRD(f32_to_fpr(val));
}

void exec_fsw(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    va_store(cpu, RS1 + IMM_S, &FRS2, 4);
}

void exec_fmadd_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(fmaf(f32_from_fpr(FRS1), f32_from_fpr(FRS2), f32_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmsub_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(fmaf(f32_from_fpr(FRS1), f32_from_fpr(FRS2), -f32_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fnmsub_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(-fmaf(f32_from_fpr(FRS1), f32_from_fpr(FRS2), f32_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fnmadd_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(-fmaf(f32_from_fpr(FRS1), f32_from_fpr(FRS2), -f32_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fadd_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(f32_from_fpr(FRS1) + f32_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fsub_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(f32_from_fpr(FRS1) - f32_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmul_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(f32_from_fpr(FRS1) * f32_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fdiv_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(f32_from_fpr(FRS1) / f32_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fsqrt_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr(sqrtf(f32_from_fpr(FRS1))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fsgnj_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD(0xFFFFFFFF00000000ULL | ((FRS1 & 0x7FFFFFFF) | (FRS2 & 0x80000000)));
}

void exec_fsgnjn_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD(0xFFFFFFFF00000000ULL | ((FRS1 & 0x7FFFFFFF) | (~FRS2 & 0x80000000)));
}

void exec_fsgnjx_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD(0xFFFFFFFF00000000ULL | ((FRS1 & 0x7FFFFFFF) | ((FRS1 ^ FRS2) & 0x80000000)));
}

void exec_fmin_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_FRD(f32_to_fpr(fminf(f32_from_fpr(FRS1), f32_from_fpr(FRS2))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmax_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_FRD(f32_to_fpr(fmaxf(f32_from_fpr(FRS1), f32_from_fpr(FRS2))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_w_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;
    
    //! TODO: HANDLE CHECKING INPUT IN ALL CONVERSIONS 
    int32_t res = (int32_t)nearbyintf(f32_from_fpr(FRS1));
    SET_RD((int64_t)res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_wu_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;
    
    uint32_t res = (uint32_t)nearbyintf(f32_from_fpr(FRS1)); 
    SET_RD((uint64_t)res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmv_x_v(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD((int64_t)(int32_t)FRS1);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_feq_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD(f32_from_fpr(FRS1) == f32_from_fpr(FRS2));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_flt_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD(f32_from_fpr(FRS1) < f32_from_fpr(FRS2));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fle_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD(f32_from_fpr(FRS1) <= f32_from_fpr(FRS2));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fclass_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    uint32_t bits = (uint32_t)FRS1;
    
    uint64_t result = 0;
    uint32_t exp = (bits >> 23) & 0xFF;
    uint32_t mant = bits & 0x7FFFFF;
    int sign = (bits >> 31) & 1;
    
    if (exp == 0) {
        if (mant == 0) {
            // Zero
            result = sign ? (1 << 3) : (1 << 4);  // -0 : +0
        } else {
            // Subnormal
            result = sign ? (1 << 2) : (1 << 5);  // -subnormal : +subnormal
        }
    } else if (exp == 0xFF) {
        if (mant == 0) {
            // Infinity
            result = sign ? (1 << 0) : (1 << 7);  // -inf : +inf
        } else {
            // NaN
            if (mant & 0x400000) {
                result = 1 << 9;  // Quiet NaN
            } else {
                result = 1 << 8;  // Signaling NaN
            }
        }
    } else {
        // Normal number
        result = sign ? (1 << 1) : (1 << 6);  // -normal : +normal
    }
    
    SET_RD(result);
}

void exec_fcvt_s_w(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr((float)(int32_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_s_wu(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr((float)(uint32_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmv_v_x(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD((uint32_t)(RS1 & 0xFFFFFFFF));
}

void exec_fcvt_l_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;
    
    int64_t res = (int64_t)nearbyintf(f32_from_fpr(FRS1)); 
    SET_RD(res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_lu_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;
    
    uint64_t res = (uint64_t)nearbyintf(f32_from_fpr(FRS1)); 
    SET_RD(res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_s_l(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr((float)(int64_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_s_lu(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr((float)(uint64_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fld(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    uint64_t val;
    if (!va_load(cpu, RS1 + IMM_I, &val, 8))
        return;

    SET_FRD(f64_to_fpr(val));
}

void exec_fsd(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    va_store(cpu, RS1 + IMM_S, &FRS2, 8);
}

void exec_fmadd_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(fma(f64_from_fpr(FRS1), f64_from_fpr(FRS2), f64_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmsub_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(fma(f64_from_fpr(FRS1), f64_from_fpr(FRS2), -f64_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fnmsub_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(-fma(f64_from_fpr(FRS1), f64_from_fpr(FRS2), f64_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fnmadd_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(-fma(f64_from_fpr(FRS1), f64_from_fpr(FRS2), -f64_from_fpr(FRS3))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fadd_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(f64_from_fpr(FRS1) + f64_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fsub_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(f64_from_fpr(FRS1) - f64_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmul_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(f64_from_fpr(FRS1) * f64_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fdiv_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(f64_from_fpr(FRS1) / f64_from_fpr(FRS2)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fsqrt_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr(sqrt(f64_from_fpr(FRS1))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fsgnj_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD((FRS1 & 0x7FFFFFFFFFFFFFFFULL) | (FRS2 & 0x8000000000000000ULL));
}

void exec_fsgnjn_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD((FRS1 & 0x7FFFFFFFFFFFFFFFULL) | (~FRS2 & 0x8000000000000000ULL));
}

void exec_fsgnjx_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD((FRS1 & 0x7FFFFFFFFFFFFFFFULL) | ((FRS1 ^ FRS2) & 0x8000000000000000ULL));
}

void exec_fmin_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_FRD(f64_to_fpr(fmin(f64_from_fpr(FRS1), f64_from_fpr(FRS2))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmax_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_FRD(f64_to_fpr(fmax(f64_from_fpr(FRS1), f64_from_fpr(FRS2))));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_s_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f32_to_fpr((float)f64_from_fpr(FRS1)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_d_s(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr((double)f32_from_fpr(FRS1)));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_feq_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD(f64_from_fpr(FRS1) == f64_from_fpr(FRS2));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_flt_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD(f64_from_fpr(FRS1) < f64_from_fpr(FRS2));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fle_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD(f64_from_fpr(FRS1) <= f64_from_fpr(FRS2));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fclass_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    uint64_t bits = FRS1;
    
    uint64_t result = 0;
    uint32_t exp = (bits >> 52) & 0x7FF;
    uint64_t mant = bits & 0xFFFFFFFFFFFFFULL;
    int sign = (bits >> 63) & 1;
    
    if (exp == 0) {
        if (mant == 0) {
            // Zero
            result = sign ? (1 << 3) : (1 << 4);  // -0 : +0
        } else {
            // Subnormal
            result = sign ? (1 << 2) : (1 << 5);  // -subnormal : +subnormal
        }
    } else if (exp == 0x7FF) {
        if (mant == 0) {
            // Infinity
            result = sign ? (1 << 0) : (1 << 7);  // -inf : +inf
        } else {
            // NaN
            if (mant & 0x8000000000000ULL) {
                result = 1 << 9;  // Quiet NaN
            } else {
                result = 1 << 8;  // Signaling NaN
            }
        }
    } else {
        // Normal number
        result = sign ? (1 << 1) : (1 << 6);  // -normal : +normal
    }
    
    SET_RD(result);
}

void exec_fcvt_w_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    int32_t res = (int32_t)nearbyint(f64_from_fpr(FRS1)); 
    SET_RD((int64_t)res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_wu_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    uint32_t res = (uint32_t)nearbyint(f64_from_fpr(FRS1)); 
    SET_RD((uint64_t)res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_d_w(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr((double)(int32_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_d_wu(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr((double)(uint32_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_l_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    int64_t res = (int64_t)nearbyint(f64_from_fpr(FRS1));
    SET_RD(res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_lu_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    uint64_t res = (uint64_t)nearbyint(f64_from_fpr(FRS1));
    SET_RD(res);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmv_x_d(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_CSR_RM;

    SET_RD((int64_t)FRS1);
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_d_l(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr((double)(int64_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fcvt_d_lu(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    FE_SETUP_RM;

    SET_FRD(f64_to_fpr((double)(uint64_t)RS1));
    fflags_check_and_set(cpu);

    FE_RESTORE_RM;
}

void exec_fmv_d_x(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    SET_FRD((uint64_t)RS1);
}

// (RES, uimm=0)
void exec_c_addi4spn(cpu_t* cpu, uint32_t instr) {
    uint16_t uimm = IMM_CADDI4SPN;
    if (uimm == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    C_SET_RD(uimm + SP);
}

void exec_c_fld(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    uint64_t val;
    if (!va_load(cpu, C_RS1 + IMM_CFLD, &val, 8))
        return;

    C_SET_FRD(f64_to_fpr(val));
}

void exec_c_lw(cpu_t* cpu, uint32_t instr) {
    uint32_t val;
    if (!va_load(cpu, C_RS1 + IMM_CLW, &val, 4))
        return;

    C_SET_RD((int64_t)(int32_t)val);
}

void exec_c_ld(cpu_t* cpu, uint32_t instr) {
    uint64_t val;
    if (!va_load(cpu, C_RS1 + IMM_CFLD, &val, 8))
        return;

    C_SET_RD(val);
}

void exec_c_fsd(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    va_store(cpu, C_RS1 + IMM_CFLD, &C_FRS2,  8);
}

void exec_c_sw(cpu_t* cpu, uint32_t instr) {


    va_store(cpu, C_RS1 + IMM_CLW, &C_RS2, 4);
}

void exec_c_sd(cpu_t* cpu, uint32_t instr) {
    va_store(cpu, C_RS1 + IMM_CFLD, &C_RS2, 8);
}

// (HINT, imm!=0)
void exec_c_nop(cpu_t* cpu, uint32_t instr) {
    // Just an alias for addi
}

// (HINT, imm=0)
void exec_c_addi(cpu_t* cpu, uint32_t instr) {
    SET_RD(RD + IMM_CI);
}

// (RES, rd=0)
void exec_c_addiw(cpu_t* cpu, uint32_t instr) {
    if (_RD == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    SET_RD((int64_t)(int32_t)(RD + IMM_CI));
}

// (HINT, rd=0)
void exec_c_li(cpu_t* cpu, uint32_t instr) {
    SET_RD(IMM_CI);
}

// (RES, imm=0)
void exec_c_addi16sp(cpu_t* cpu, uint32_t instr) {
    int16_t imm = IMM_CADDI16SP;
    if (imm == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    SET_SP(SP + imm);
}

// (RES, imm=0; HINT, rd=0)
void exec_c_lui(cpu_t* cpu, uint32_t instr) {
    uint64_t imm = IMM_CLUI;

    if (imm == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    SET_RD(imm);
}

// (HINT, uimm=0)
void exec_c_srli(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1(C_RS1 >> (IMM_CUI & 0x3F));
}

// (HINT, uimm=0)
void exec_c_srai(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1((int64_t)C_RS1 >> (IMM_CUI & 0x3F));
}

void exec_c_andi(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1(C_RS1 & IMM_CI);
}

void exec_c_sub(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1(C_RS1 - C_RS2);
}

void exec_c_xor(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1(C_RS1 ^ C_RS2);
}

void exec_c_or(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1(C_RS1 | C_RS2);
}

void exec_c_and(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1(C_RS1 & C_RS2);
}

void exec_c_subw(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1((int64_t)(int32_t)(C_RS1 - C_RS2));
}

void exec_c_addw(cpu_t* cpu, uint32_t instr) {
    C_SET_RS1((int64_t)(int32_t)(C_RS1 + C_RS2));
}

void exec_c_j(cpu_t* cpu, uint32_t instr) {
    SET_NPC(PC + IMM_CJ);
}

void exec_c_beqz(cpu_t* cpu, uint32_t instr) {
    if (C_RS1 == 0)
        SET_NPC(PC + IMM_CB);
}

void exec_c_bnez(cpu_t* cpu, uint32_t instr) {
    if (C_RS1 != 0)
        SET_NPC(PC + IMM_CB);
}

// (HINT, rd=0 or imm=0)
void exec_c_slli(cpu_t* cpu, uint32_t instr) {
    SET_RD(RD << IMM_CUI);
}

void exec_c_fldsp(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    uint64_t val;
    if (!va_load(cpu, SP + IMM_CFLDSP, &val, 8))
        return;

    SET_FRD(f64_to_fpr(val));
}

// (RES, rd=0)
void exec_c_lwsp(cpu_t* cpu, uint32_t instr) {
    if (_RD == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    uint32_t val;
    if (!va_load(cpu, SP + IMM_CLWSP, &val, 4))
        return;

    SET_RD((int64_t)(int32_t)val);
}

// (RES, rd=0)
void exec_c_ldsp(cpu_t* cpu, uint32_t instr) {
    if (_RD == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    uint64_t val;
    if (!va_load(cpu, SP + IMM_CFLDSP, &val, 8))
        return;

    SET_RD(val);
}

// (RES, rs1=0)
void exec_c_jr(cpu_t* cpu, uint32_t instr) {
    if (_RS1 == 0) {
        // SET_NPC(PC);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, instr, 0);
        return;
    }

    SET_NPC(RD);
}

// (HINT, rd=0)
void exec_c_mv(cpu_t* cpu, uint32_t instr) {
    SET_RD(C_WIDE_RS2);
}

void exec_c_ebreak(cpu_t* cpu, uint32_t instr) {
    raise_trap(cpu, CAUSE_BREAKPOINT, PC, 0);
}

void exec_c_jalr(cpu_t* cpu, uint32_t instr) {
    uint64_t target = RD;
    cpu->x[1] = PC + 2;
    SET_NPC(target);
}

// (HINT, rd=0)
void exec_c_add(cpu_t* cpu, uint32_t instr) {
    SET_RD(RD + C_WIDE_RS2);
}

void exec_c_fsdsp(cpu_t* cpu, uint32_t instr) {
    CHECK_FPU;

    va_store(cpu, SP + IMM_CFSDSP, &C_WIDE_FRS2, 8);
}

void exec_c_swsp(cpu_t* cpu, uint32_t instr) {
    va_store(cpu, SP + IMM_CSWSP, &C_WIDE_RS2, 4);
}

void exec_c_sdsp(cpu_t* cpu, uint32_t instr) {
    va_store(cpu, SP + IMM_CFSDSP, &C_WIDE_RS2, 8);
}
