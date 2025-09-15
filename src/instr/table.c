// This file is auto-generated. (2025-09-16 00)

#include "table.h"
#include "../cpu.h"

extern void exec_lui(cpu_t *cpu, uint32_t instr);
extern void exec_auipc(cpu_t *cpu, uint32_t instr);
extern void exec_addi(cpu_t *cpu, uint32_t instr);
extern void exec_slti(cpu_t *cpu, uint32_t instr);
extern void exec_sltiu(cpu_t *cpu, uint32_t instr);
extern void exec_xori(cpu_t *cpu, uint32_t instr);
extern void exec_ori(cpu_t *cpu, uint32_t instr);
extern void exec_andi(cpu_t *cpu, uint32_t instr);
extern void exec_slli(cpu_t *cpu, uint32_t instr);
extern void exec_srli(cpu_t *cpu, uint32_t instr);
extern void exec_srai(cpu_t *cpu, uint32_t instr);
extern void exec_add(cpu_t *cpu, uint32_t instr);
extern void exec_sub(cpu_t *cpu, uint32_t instr);
extern void exec_sll(cpu_t *cpu, uint32_t instr);
extern void exec_slt(cpu_t *cpu, uint32_t instr);
extern void exec_sltu(cpu_t *cpu, uint32_t instr);
extern void exec_xor(cpu_t *cpu, uint32_t instr);
extern void exec_srl(cpu_t *cpu, uint32_t instr);
extern void exec_sra(cpu_t *cpu, uint32_t instr);
extern void exec_or(cpu_t *cpu, uint32_t instr);
extern void exec_and(cpu_t *cpu, uint32_t instr);
extern void exec_fence(cpu_t *cpu, uint32_t instr);
extern void exec_fence_i(cpu_t *cpu, uint32_t instr);
extern void exec_cssrw(cpu_t *cpu, uint32_t instr);
extern void exec_csrrs(cpu_t *cpu, uint32_t instr);
extern void exec_csrrc(cpu_t *cpu, uint32_t instr);
extern void exec_csrrwi(cpu_t *cpu, uint32_t instr);
extern void exec_csrrsi(cpu_t *cpu, uint32_t instr);
extern void exec_csrrci(cpu_t *cpu, uint32_t instr);
extern void exec_ecall(cpu_t *cpu, uint32_t instr);
extern void exec_ebreak(cpu_t *cpu, uint32_t instr);
extern void exec_sret(cpu_t *cpu, uint32_t instr);
extern void exec_mret(cpu_t *cpu, uint32_t instr);
extern void exec_wfi(cpu_t *cpu, uint32_t instr);
extern void exec_sfence_vma(cpu_t *cpu, uint32_t instr);
extern void exec_lb(cpu_t *cpu, uint32_t instr);
extern void exec_lh(cpu_t *cpu, uint32_t instr);
extern void exec_lw(cpu_t *cpu, uint32_t instr);
extern void exec_lbu(cpu_t *cpu, uint32_t instr);
extern void exec_lhu(cpu_t *cpu, uint32_t instr);
extern void exec_sb(cpu_t *cpu, uint32_t instr);
extern void exec_sh(cpu_t *cpu, uint32_t instr);
extern void exec_sw(cpu_t *cpu, uint32_t instr);
extern void exec_jal(cpu_t *cpu, uint32_t instr);
extern void exec_jalr(cpu_t *cpu, uint32_t instr);
extern void exec_beq(cpu_t *cpu, uint32_t instr);
extern void exec_bne(cpu_t *cpu, uint32_t instr);
extern void exec_blt(cpu_t *cpu, uint32_t instr);
extern void exec_bge(cpu_t *cpu, uint32_t instr);
extern void exec_bltu(cpu_t *cpu, uint32_t instr);
extern void exec_bgeu(cpu_t *cpu, uint32_t instr);
extern void exec_addiw(cpu_t *cpu, uint32_t instr);
extern void exec_slliw(cpu_t *cpu, uint32_t instr);
extern void exec_srliw(cpu_t *cpu, uint32_t instr);
extern void exec_sraiw(cpu_t *cpu, uint32_t instr);
extern void exec_addw(cpu_t *cpu, uint32_t instr);
extern void exec_subw(cpu_t *cpu, uint32_t instr);
extern void exec_sllw(cpu_t *cpu, uint32_t instr);
extern void exec_srlw(cpu_t *cpu, uint32_t instr);
extern void exec_sraw(cpu_t *cpu, uint32_t instr);
extern void exec_lwu(cpu_t *cpu, uint32_t instr);
extern void exec_ld(cpu_t *cpu, uint32_t instr);
extern void exec_sd(cpu_t *cpu, uint32_t instr);

instr_dispatch_table_t instr_dispatch_table[] = {
    {0x0000007F, 0x00000037, exec_lui},
    {0x0000007F, 0x00000017, exec_auipc},
    {0x0000707F, 0x00000013, exec_addi},
    {0x0000707F, 0x00002013, exec_slti},
    {0x0000707F, 0x00003013, exec_sltiu},
    {0x0000707F, 0x00004013, exec_xori},
    {0x0000707F, 0x00006013, exec_ori},
    {0x0000707F, 0x00007013, exec_andi},
    {0xFC00707F, 0x00001013, exec_slli},
    {0xFC00707F, 0x00005013, exec_srli},
    {0xFC00707F, 0x40005013, exec_srai},
    {0xFE00707F, 0x00000033, exec_add},
    {0xFE00707F, 0x40000033, exec_sub},
    {0xFE00707F, 0x00001033, exec_sll},
    {0xFE00707F, 0x00002033, exec_slt},
    {0xFE00707F, 0x00003033, exec_sltu},
    {0xFE00707F, 0x00004033, exec_xor},
    {0xFE00707F, 0x00005033, exec_srl},
    {0xFE00707F, 0x40005033, exec_sra},
    {0xFE00707F, 0x00006033, exec_or},
    {0xFE00707F, 0x00007033, exec_and},
    {0x0000707F, 0x0000000F, exec_fence},
    {0x0000707F, 0x0000100F, exec_fence_i},
    {0x0000707F, 0x00001073, exec_cssrw},
    {0x0000707F, 0x00002073, exec_csrrs},
    {0x0000707F, 0x00003073, exec_csrrc},
    {0x0000707F, 0x00005073, exec_csrrwi},
    {0x0000707F, 0x00006073, exec_csrrsi},
    {0x0000707F, 0x00007073, exec_csrrci},
    {0xFFFFFFFF, 0x00000073, exec_ecall},
    {0xFFFFFFFF, 0x00100073, exec_ebreak},
    {0xFFFFFFFF, 0x10200073, exec_sret},
    {0xFFFFFFFF, 0x30200073, exec_mret},
    {0xFFFFFFFF, 0x10500073, exec_wfi},
    {0xFE007FFF, 0x12000073, exec_sfence_vma},
    {0x0000707F, 0x00000003, exec_lb},
    {0x0000707F, 0x00001003, exec_lh},
    {0x0000707F, 0x00002003, exec_lw},
    {0x0000707F, 0x00004003, exec_lbu},
    {0x0000707F, 0x00005003, exec_lhu},
    {0x0000707F, 0x00000023, exec_sb},
    {0x0000707F, 0x00001023, exec_sh},
    {0x0000707F, 0x00002023, exec_sw},
    {0x0000007F, 0x0000007F, exec_jal},
    {0x0000707F, 0x00000077, exec_jalr},
    {0x0000707F, 0x00000063, exec_beq},
    {0x0000707F, 0x00001063, exec_bne},
    {0x0000707F, 0x00004063, exec_blt},
    {0x0000707F, 0x00005063, exec_bge},
    {0x0000707F, 0x00006063, exec_bltu},
    {0x0000707F, 0x00007063, exec_bgeu},
    {0x0000707F, 0x0000001B, exec_addiw},
    {0x0000707F, 0x00001013, exec_slliw},
    {0xFE00707F, 0x00005013, exec_srliw},
    {0xFE00707F, 0x40005013, exec_sraiw},
    {0xFE00707F, 0x0000003B, exec_addw},
    {0xFE00707F, 0x4000003B, exec_subw},
    {0xFE00707F, 0x0000103B, exec_sllw},
    {0xFE00707F, 0x0000503B, exec_srlw},
    {0xFE00707F, 0x4000503B, exec_sraw},
    {0x0000707F, 0x00006003, exec_lwu},
    {0x0000707F, 0x00003003, exec_ld},
    {0x0000707F, 0x00003023, exec_sd},
};
