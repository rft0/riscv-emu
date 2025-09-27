#include "../cpu.h"
#include "../helpers.h"

#include "trap.h"
#include "csr.h"
#include "mem.h"

#define _RD                 extract32(instr, 11, 7)
#define _RS1                extract32(instr, 19, 15)
#define _RS2                extract32(instr, 24, 20)

#define PC                  (cpu->pc)
#define NPC                 (cpu->npc)

#define RD                  (cpu->x[_RD])
#define RS1                 (cpu->x[_RS1])
#define RS2                 (cpu->x[_RS2])

#define SET_RD(val)         do { if (_RD != 0) cpu->x[_RD] = (val); } while(0)
#define SET_PC(val)         (cpu->pc = (val))
#define SET_NPC(val)        (cpu->npc = (val))

#define IMM_I               sext(extract32(instr, 31, 20), 12)
#define IMM_S               sext((extract32(instr, 31, 25) << 5) | extract32(instr, 11, 7), 12)
#define IMM_B               sext((extract32(instr, 31, 31) << 12) | (extract32(instr, 7, 7) << 11) | (extract32(instr, 30, 25) << 5) | (extract32(instr, 11, 8) << 1), 13)
#define IMM_U               (extract32(instr, 31, 12) << 12)
#define IMM_J               sext((extract32(instr, 31, 31) << 20) | (extract32(instr, 19, 12) << 12) | (extract32(instr, 20, 20) << 11) | (extract32(instr, 30, 21) << 1), 21)
#define IMM_CSR             extract32(instr, 31, 20)

#define SHAMTRV64           extract32(instr, 25, 20)
#define SHAMT               extract32(instr, 24, 20)

#define AMO_AQ              extract32(instr, 26, 26)
#define AMO_RL              extract32(instr, 25, 25)

void exec_lui(cpu_t* cpu, uint32_t instr) {
    SET_RD(IMM_U); // Yes IMM_U is zero extended
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

void exec_fence_i(cpu_t* cpu, uint32_t instr) {
    // No icache, do nothing.
}

void exec_cssrw(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t value;
    if(!csr_read(cpu, addr, &value))
        return;

    SET_RD(value);
    csr_write(cpu, addr, RS1);
}

void exec_csrrs(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t t;
    if (!csr_read(cpu, addr, &t))
        return;

    SET_RD(t);
    csr_write(cpu, addr, t | RS1);
}

void exec_csrrc(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t t;
    if (!csr_read(cpu, addr, &t))
        return;

    SET_RD(t);
    csr_write(cpu, addr, t & ~RS1);
}

void exec_csrrwi(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t val;
    if(!csr_read(cpu, addr, &val))
        return;

    SET_RD(val);
    csr_write(cpu, addr, _RS1);
}

void exec_csrrsi(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t t;
    if (!csr_read(cpu, addr, &t))
        return;

    SET_RD(t);
    csr_write(cpu, addr, t | _RS1);
}

void exec_csrrci(cpu_t* cpu, uint32_t instr) {
    uint32_t addr = IMM_CSR;
    uint64_t t;
    if (!csr_read(cpu, addr, &t))
        return;

    SET_RD(t);
    csr_write(cpu, addr, t & ~_RS1);
}

void exec_ecall(cpu_t* cpu, uint32_t instr) {
    uint64_t cause;
    switch (cpu->mode) {
        case PRIV_U: cause = 8; break;
        case PRIV_S: cause = 9; break;
        case PRIV_M: cause = 11; break;
        default: cause = 11; break;
    }

    raise_trap(cpu, cause, PC, 0);
}

void exec_ebreak(cpu_t* cpu, uint32_t instr) {
    raise_trap(cpu, 3, PC, 0);
}

void exec_sret(cpu_t* cpu, uint32_t instr) {
    uint64_t sepc;
    if (!csr_read(cpu, CSR_SEPC, &sepc))
        return;

    uint64_t sstatus;
    if (!csr_read(cpu, CSR_SSTATUS, &sstatus))
        return;

    uint64_t spp = (sstatus >> 8) & 1;
    uint64_t spie = (sstatus >> 5) & 1;

    // set PC
    cpu->pc = sepc;

    // restore priv
    cpu->mode = spp ? PRIV_S : PRIV_U;

    // update sstatus
    sstatus = (sstatus & ~0x100UL) | (0UL << 8);   // SPP=U
    sstatus = (sstatus & ~0x2UL) | (spie << 1);    // SIE=SPIE
    sstatus |= (1UL << 5);                         // SPIE=1

    csr_write(cpu, CSR_SSTATUS, sstatus);
}

void exec_mret(cpu_t* cpu, uint32_t instr) {
    uint64_t mepc;
    if (!csr_read(cpu, CSR_MEPC, &mepc))
        return;

    uint64_t mstatus;
    if (!csr_read(cpu, CSR_MSTATUS, &mstatus))
        return;

    uint64_t mpp = (mstatus >> 11) & 3;
    uint64_t mpie = (mstatus >> 7) & 1;

    cpu->pc = mepc;
    cpu->mode = mpp; // 0=U,1=S,3=M

    // update mstatus
    mstatus = (mstatus & ~(3UL << 11));          // MPP=U
    mstatus = (mstatus & ~0x8UL) | (mpie << 3);  // MIE=MPIE
    mstatus |= (1UL << 7);                       // MPIE=1

    csr_write(cpu, CSR_MSTATUS, mstatus);
}

void exec_wfi(cpu_t* cpu, uint32_t instr) {
    //! TODO: Stall until interrupt pending.
    // For now, just do nothing.
}

void exec_sfence_vma(cpu_t* cpu, uint32_t instr) {
    //! TODO: YET TO BE IMPLEMENTED
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
    va_store(cpu, RS1 + IMM_S, &RS2, 4);
}

void exec_jal(cpu_t* cpu, uint32_t instr) {
    SET_RD(PC + 4);
    SET_NPC(PC + IMM_J);
}

void exec_jalr(cpu_t* cpu, uint32_t instr) {
    SET_RD(PC + 4);
    SET_NPC((RS1 + IMM_I) & ~1);
}

void exec_beq(cpu_t* cpu, uint32_t instr) {
    if (RS1 == RS2)
        SET_NPC(PC + IMM_B);
}

void exec_bne(cpu_t* cpu, uint32_t instr) {
    if (RS1 != RS2)
        SET_NPC(PC + IMM_B);
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
    SET_RD((int64_t)(int32_t)(RS1 >> SHAMT));
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
    SET_RD((int64_t)(int32_t)(RS1 >> extract32(RS2, 4, 0)));
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
        SET_RD(UINT32_MAX);
    else
        SET_RD((int64_t)(uint32_t)(dividend / divisor));
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
        SET_RD((int64_t)dividend);
    else
        SET_RD((int64_t)(uint32_t)(dividend % divisor));
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
