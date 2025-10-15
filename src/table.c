// -----------------------------------------
// This file is auto-generated. (2025-10-15)
// -----------------------------------------

#include "table.h"

extern void exec_fence_tso(cpu_t* cpu, uint32_t instr);
extern void exec_pause(cpu_t* cpu, uint32_t instr);
extern void exec_ecall(cpu_t* cpu, uint32_t instr);
extern void exec_ebreak(cpu_t* cpu, uint32_t instr);
extern void exec_sret(cpu_t* cpu, uint32_t instr);
extern void exec_mret(cpu_t* cpu, uint32_t instr);
extern void exec_wfi(cpu_t* cpu, uint32_t instr);
extern void exec_c_nop(cpu_t* cpu, uint32_t instr);
extern void exec_c_ebreak(cpu_t* cpu, uint32_t instr);
extern void exec_c_jr(cpu_t* cpu, uint32_t instr);
extern void exec_c_jalr(cpu_t* cpu, uint32_t instr);
extern void exec_c_sub(cpu_t* cpu, uint32_t instr);
extern void exec_c_xor(cpu_t* cpu, uint32_t instr);
extern void exec_c_or(cpu_t* cpu, uint32_t instr);
extern void exec_c_and(cpu_t* cpu, uint32_t instr);
extern void exec_c_subw(cpu_t* cpu, uint32_t instr);
extern void exec_c_addw(cpu_t* cpu, uint32_t instr);
extern void exec_c_addi16sp(cpu_t* cpu, uint32_t instr);
extern void exec_c_srli(cpu_t* cpu, uint32_t instr);
extern void exec_c_srai(cpu_t* cpu, uint32_t instr);
extern void exec_c_andi(cpu_t* cpu, uint32_t instr);
extern void exec_c_mv(cpu_t* cpu, uint32_t instr);
extern void exec_c_add(cpu_t* cpu, uint32_t instr);
extern void exec_fmv_x_v(cpu_t* cpu, uint32_t instr);
extern void exec_fclass_s(cpu_t* cpu, uint32_t instr);
extern void exec_fmv_v_x(cpu_t* cpu, uint32_t instr);
extern void exec_fclass_d(cpu_t* cpu, uint32_t instr);
extern void exec_fmv_x_d(cpu_t* cpu, uint32_t instr);
extern void exec_fmv_d_x(cpu_t* cpu, uint32_t instr);
extern void exec_sfence_vma(cpu_t* cpu, uint32_t instr);
extern void exec_c_addi4spn(cpu_t* cpu, uint32_t instr);
extern void exec_c_fld(cpu_t* cpu, uint32_t instr);
extern void exec_c_lw(cpu_t* cpu, uint32_t instr);
extern void exec_c_ld(cpu_t* cpu, uint32_t instr);
extern void exec_c_fsd(cpu_t* cpu, uint32_t instr);
extern void exec_c_sw(cpu_t* cpu, uint32_t instr);
extern void exec_c_sd(cpu_t* cpu, uint32_t instr);
extern void exec_c_addi(cpu_t* cpu, uint32_t instr);
extern void exec_c_addiw(cpu_t* cpu, uint32_t instr);
extern void exec_c_li(cpu_t* cpu, uint32_t instr);
extern void exec_c_lui(cpu_t* cpu, uint32_t instr);
extern void exec_c_j(cpu_t* cpu, uint32_t instr);
extern void exec_c_beqz(cpu_t* cpu, uint32_t instr);
extern void exec_c_bnez(cpu_t* cpu, uint32_t instr);
extern void exec_c_slli(cpu_t* cpu, uint32_t instr);
extern void exec_c_fldsp(cpu_t* cpu, uint32_t instr);
extern void exec_c_lwsp(cpu_t* cpu, uint32_t instr);
extern void exec_c_ldsp(cpu_t* cpu, uint32_t instr);
extern void exec_c_fsdsp(cpu_t* cpu, uint32_t instr);
extern void exec_c_swsp(cpu_t* cpu, uint32_t instr);
extern void exec_c_sdsp(cpu_t* cpu, uint32_t instr);
extern void exec_lr_w(cpu_t* cpu, uint32_t instr);
extern void exec_lr_d(cpu_t* cpu, uint32_t instr);
extern void exec_fsqrt_s(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_w_s(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_wu_s(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_s_w(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_s_wu(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_l_s(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_lu_s(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_s_l(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_s_lu(cpu_t* cpu, uint32_t instr);
extern void exec_fsqrt_d(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_s_d(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_d_s(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_w_d(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_wu_d(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_d_w(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_d_wu(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_l_d(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_lu_d(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_d_l(cpu_t* cpu, uint32_t instr);
extern void exec_fcvt_d_lu(cpu_t* cpu, uint32_t instr);
extern void exec_add(cpu_t* cpu, uint32_t instr);
extern void exec_sub(cpu_t* cpu, uint32_t instr);
extern void exec_sll(cpu_t* cpu, uint32_t instr);
extern void exec_slt(cpu_t* cpu, uint32_t instr);
extern void exec_sltu(cpu_t* cpu, uint32_t instr);
extern void exec_xor(cpu_t* cpu, uint32_t instr);
extern void exec_srl(cpu_t* cpu, uint32_t instr);
extern void exec_sra(cpu_t* cpu, uint32_t instr);
extern void exec_or(cpu_t* cpu, uint32_t instr);
extern void exec_and(cpu_t* cpu, uint32_t instr);
extern void exec_slliw(cpu_t* cpu, uint32_t instr);
extern void exec_srliw(cpu_t* cpu, uint32_t instr);
extern void exec_sraiw(cpu_t* cpu, uint32_t instr);
extern void exec_addw(cpu_t* cpu, uint32_t instr);
extern void exec_subw(cpu_t* cpu, uint32_t instr);
extern void exec_sllw(cpu_t* cpu, uint32_t instr);
extern void exec_srlw(cpu_t* cpu, uint32_t instr);
extern void exec_sraw(cpu_t* cpu, uint32_t instr);
extern void exec_mul(cpu_t* cpu, uint32_t instr);
extern void exec_mulh(cpu_t* cpu, uint32_t instr);
extern void exec_mulhsu(cpu_t* cpu, uint32_t instr);
extern void exec_mulhu(cpu_t* cpu, uint32_t instr);
extern void exec_div(cpu_t* cpu, uint32_t instr);
extern void exec_divu(cpu_t* cpu, uint32_t instr);
extern void exec_rem(cpu_t* cpu, uint32_t instr);
extern void exec_remu(cpu_t* cpu, uint32_t instr);
extern void exec_mulw(cpu_t* cpu, uint32_t instr);
extern void exec_divw(cpu_t* cpu, uint32_t instr);
extern void exec_divuw(cpu_t* cpu, uint32_t instr);
extern void exec_remw(cpu_t* cpu, uint32_t instr);
extern void exec_remuw(cpu_t* cpu, uint32_t instr);
extern void exec_fsgnj_s(cpu_t* cpu, uint32_t instr);
extern void exec_fsgnjn_s(cpu_t* cpu, uint32_t instr);
extern void exec_fsgnjx_s(cpu_t* cpu, uint32_t instr);
extern void exec_fmin_s(cpu_t* cpu, uint32_t instr);
extern void exec_fmax_s(cpu_t* cpu, uint32_t instr);
extern void exec_feq_s(cpu_t* cpu, uint32_t instr);
extern void exec_flt_s(cpu_t* cpu, uint32_t instr);
extern void exec_fle_s(cpu_t* cpu, uint32_t instr);
extern void exec_fsgnj_d(cpu_t* cpu, uint32_t instr);
extern void exec_fsgnjn_d(cpu_t* cpu, uint32_t instr);
extern void exec_fsgnjx_d(cpu_t* cpu, uint32_t instr);
extern void exec_fmin_d(cpu_t* cpu, uint32_t instr);
extern void exec_fmax_d(cpu_t* cpu, uint32_t instr);
extern void exec_feq_d(cpu_t* cpu, uint32_t instr);
extern void exec_flt_d(cpu_t* cpu, uint32_t instr);
extern void exec_fle_d(cpu_t* cpu, uint32_t instr);
extern void exec_slli(cpu_t* cpu, uint32_t instr);
extern void exec_srli(cpu_t* cpu, uint32_t instr);
extern void exec_srai(cpu_t* cpu, uint32_t instr);
extern void exec_sc_w(cpu_t* cpu, uint32_t instr);
extern void exec_amoswap_w(cpu_t* cpu, uint32_t instr);
extern void exec_amoadd_w(cpu_t* cpu, uint32_t instr);
extern void exec_amoxor_w(cpu_t* cpu, uint32_t instr);
extern void exec_amoand_w(cpu_t* cpu, uint32_t instr);
extern void exec_amoor_w(cpu_t* cpu, uint32_t instr);
extern void exec_amomin_w(cpu_t* cpu, uint32_t instr);
extern void exec_amomax_w(cpu_t* cpu, uint32_t instr);
extern void exec_amominu_w(cpu_t* cpu, uint32_t instr);
extern void exec_amomaxu_w(cpu_t* cpu, uint32_t instr);
extern void exec_sc_d(cpu_t* cpu, uint32_t instr);
extern void exec_amoswap_d(cpu_t* cpu, uint32_t instr);
extern void exec_amoadd_d(cpu_t* cpu, uint32_t instr);
extern void exec_amoxor_d(cpu_t* cpu, uint32_t instr);
extern void exec_amoand_d(cpu_t* cpu, uint32_t instr);
extern void exec_amoor_d(cpu_t* cpu, uint32_t instr);
extern void exec_amomin_d(cpu_t* cpu, uint32_t instr);
extern void exec_amomax_d(cpu_t* cpu, uint32_t instr);
extern void exec_amominu_d(cpu_t* cpu, uint32_t instr);
extern void exec_amomaxu_d(cpu_t* cpu, uint32_t instr);
extern void exec_fadd_s(cpu_t* cpu, uint32_t instr);
extern void exec_fsub_s(cpu_t* cpu, uint32_t instr);
extern void exec_fmul_s(cpu_t* cpu, uint32_t instr);
extern void exec_fdiv_s(cpu_t* cpu, uint32_t instr);
extern void exec_fadd_d(cpu_t* cpu, uint32_t instr);
extern void exec_fsub_d(cpu_t* cpu, uint32_t instr);
extern void exec_fmul_d(cpu_t* cpu, uint32_t instr);
extern void exec_fdiv_d(cpu_t* cpu, uint32_t instr);
extern void exec_jalr(cpu_t* cpu, uint32_t instr);
extern void exec_beq(cpu_t* cpu, uint32_t instr);
extern void exec_bne(cpu_t* cpu, uint32_t instr);
extern void exec_blt(cpu_t* cpu, uint32_t instr);
extern void exec_bge(cpu_t* cpu, uint32_t instr);
extern void exec_bltu(cpu_t* cpu, uint32_t instr);
extern void exec_bgeu(cpu_t* cpu, uint32_t instr);
extern void exec_lb(cpu_t* cpu, uint32_t instr);
extern void exec_lh(cpu_t* cpu, uint32_t instr);
extern void exec_lw(cpu_t* cpu, uint32_t instr);
extern void exec_lbu(cpu_t* cpu, uint32_t instr);
extern void exec_lhu(cpu_t* cpu, uint32_t instr);
extern void exec_sb(cpu_t* cpu, uint32_t instr);
extern void exec_sh(cpu_t* cpu, uint32_t instr);
extern void exec_sw(cpu_t* cpu, uint32_t instr);
extern void exec_addi(cpu_t* cpu, uint32_t instr);
extern void exec_slti(cpu_t* cpu, uint32_t instr);
extern void exec_sltiu(cpu_t* cpu, uint32_t instr);
extern void exec_xori(cpu_t* cpu, uint32_t instr);
extern void exec_ori(cpu_t* cpu, uint32_t instr);
extern void exec_andi(cpu_t* cpu, uint32_t instr);
extern void exec_fence(cpu_t* cpu, uint32_t instr);
extern void exec_lwu(cpu_t* cpu, uint32_t instr);
extern void exec_ld(cpu_t* cpu, uint32_t instr);
extern void exec_sd(cpu_t* cpu, uint32_t instr);
extern void exec_addiw(cpu_t* cpu, uint32_t instr);
extern void exec_fence_i(cpu_t* cpu, uint32_t instr);
extern void exec_cssrw(cpu_t* cpu, uint32_t instr);
extern void exec_csrrs(cpu_t* cpu, uint32_t instr);
extern void exec_csrrc(cpu_t* cpu, uint32_t instr);
extern void exec_csrrwi(cpu_t* cpu, uint32_t instr);
extern void exec_csrrsi(cpu_t* cpu, uint32_t instr);
extern void exec_csrrci(cpu_t* cpu, uint32_t instr);
extern void exec_flw(cpu_t* cpu, uint32_t instr);
extern void exec_fsw(cpu_t* cpu, uint32_t instr);
extern void exec_fld(cpu_t* cpu, uint32_t instr);
extern void exec_fsd(cpu_t* cpu, uint32_t instr);
extern void exec_fmadd_s(cpu_t* cpu, uint32_t instr);
extern void exec_fmsub_s(cpu_t* cpu, uint32_t instr);
extern void exec_fnmsub_s(cpu_t* cpu, uint32_t instr);
extern void exec_fnmadd_s(cpu_t* cpu, uint32_t instr);
extern void exec_fmadd_d(cpu_t* cpu, uint32_t instr);
extern void exec_fmsub_d(cpu_t* cpu, uint32_t instr);
extern void exec_fnmsub_d(cpu_t* cpu, uint32_t instr);
extern void exec_fnmadd_d(cpu_t* cpu, uint32_t instr);
extern void exec_lui(cpu_t* cpu, uint32_t instr);
extern void exec_auipc(cpu_t* cpu, uint32_t instr);
extern void exec_jal(cpu_t* cpu, uint32_t instr);

instr_entry_t instr_table[] = {
    {0xFFFFFFFF, 0x8330000F, exec_fence_tso},
    {0xFFFFFFFF, 0x0100000F, exec_pause},
    {0xFFFFFFFF, 0x00000073, exec_ecall},
    {0xFFFFFFFF, 0x00100073, exec_ebreak},
    {0xFFFFFFFF, 0x10200073, exec_sret},
    {0xFFFFFFFF, 0x30200073, exec_mret},
    {0xFFFFFFFF, 0x10500073, exec_wfi},
    {0xFFFFFFFF, 0x00000001, exec_c_nop},
    {0xFFFFFFFF, 0x00009002, exec_c_ebreak},
    {0xFFFFF07F, 0x00008002, exec_c_jr},
    {0xFFFFF07F, 0x00009002, exec_c_jalr},
    {0xFFFFFC63, 0x00008C01, exec_c_sub},
    {0xFFFFFC63, 0x00008C21, exec_c_xor},
    {0xFFFFFC63, 0x00008C41, exec_c_or},
    {0xFFFFFC63, 0x00008C61, exec_c_and},
    {0xFFFFFC63, 0x00009C01, exec_c_subw},
    {0xFFFFFC63, 0x00009C21, exec_c_addw},
    {0xFFFFEF83, 0x00006101, exec_c_addi16sp},
    {0xFFFFEC03, 0x00008001, exec_c_srli},
    {0xFFFFEC03, 0x00008401, exec_c_srai},
    {0xFFFFEC03, 0x00008801, exec_c_andi},
    {0xFFFFF003, 0x00008002, exec_c_mv},
    {0xFFFFF003, 0x00009002, exec_c_add},
    {0xFFF0707F, 0xE0000053, exec_fmv_x_v},
    {0xFFF0707F, 0xE0001053, exec_fclass_s},
    {0xFFF0707F, 0xF0000053, exec_fmv_v_x},
    {0xFFF0707F, 0xE2001053, exec_fclass_d},
    {0xFFF0707F, 0xE2000053, exec_fmv_x_d},
    {0xFFF0707F, 0xF2000053, exec_fmv_d_x},
    {0xFE007FFF, 0x12000073, exec_sfence_vma},
    {0xFFFFE003, 0x00000000, exec_c_addi4spn},
    {0xFFFFE003, 0x00002000, exec_c_fld},
    {0xFFFFE003, 0x00004000, exec_c_lw},
    {0xFFFFE003, 0x00006000, exec_c_ld},
    {0xFFFFE003, 0x0000A000, exec_c_fsd},
    {0xFFFFE003, 0x0000C000, exec_c_sw},
    {0xFFFFE003, 0x0000E000, exec_c_sd},
    {0xFFFFE003, 0x00000001, exec_c_addi},
    {0xFFFFE003, 0x00002001, exec_c_addiw},
    {0xFFFFE003, 0x00004001, exec_c_li},
    {0xFFFFE003, 0x00006001, exec_c_lui},
    {0xFFFFE003, 0x0000A001, exec_c_j},
    {0xFFFFE003, 0x0000C001, exec_c_beqz},
    {0xFFFFE003, 0x0000E001, exec_c_bnez},
    {0xFFFFE003, 0x00000002, exec_c_slli},
    {0xFFFFE003, 0x00002002, exec_c_fldsp},
    {0xFFFFE003, 0x00004002, exec_c_lwsp},
    {0xFFFFE003, 0x00006002, exec_c_ldsp},
    {0xFFFFE003, 0x0000A002, exec_c_fsdsp},
    {0xFFFFE003, 0x0000C002, exec_c_swsp},
    {0xFFFFE003, 0x0000E002, exec_c_sdsp},
    {0xF9F0707F, 0x1000202F, exec_lr_w},
    {0xF9F0707F, 0x1000302F, exec_lr_d},
    {0xFFF0007F, 0x58000053, exec_fsqrt_s},
    {0xFFF0007F, 0xC0000053, exec_fcvt_w_s},
    {0xFFF0007F, 0xC0100053, exec_fcvt_wu_s},
    {0xFFF0007F, 0xD0000053, exec_fcvt_s_w},
    {0xFFF0007F, 0xD0100053, exec_fcvt_s_wu},
    {0xFFF0007F, 0xC0200053, exec_fcvt_l_s},
    {0xFFF0007F, 0xC0300053, exec_fcvt_lu_s},
    {0xFFF0007F, 0xD0200053, exec_fcvt_s_l},
    {0xFFF0007F, 0xD0300053, exec_fcvt_s_lu},
    {0xFFF0007F, 0x5A000053, exec_fsqrt_d},
    {0xFFF0007F, 0x40100053, exec_fcvt_s_d},
    {0xFFF0007F, 0x42000053, exec_fcvt_d_s},
    {0xFFF0007F, 0xC2000053, exec_fcvt_w_d},
    {0xFFF0007F, 0xC2100053, exec_fcvt_wu_d},
    {0xFFF0007F, 0xD2000053, exec_fcvt_d_w},
    {0xFFF0007F, 0xD2100053, exec_fcvt_d_wu},
    {0xFFF0007F, 0xC2200053, exec_fcvt_l_d},
    {0xFFF0007F, 0xC2300053, exec_fcvt_lu_d},
    {0xFFF0007F, 0xD2200053, exec_fcvt_d_l},
    {0xFFF0007F, 0xD2300053, exec_fcvt_d_lu},
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
    {0xFE00707F, 0x00001013, exec_slliw},
    {0xFE00707F, 0x00005013, exec_srliw},
    {0xFE00707F, 0x40005013, exec_sraiw},
    {0xFE00707F, 0x0000003B, exec_addw},
    {0xFE00707F, 0x4000003B, exec_subw},
    {0xFE00707F, 0x0000103B, exec_sllw},
    {0xFE00707F, 0x0000503B, exec_srlw},
    {0xFE00707F, 0x4000503B, exec_sraw},
    {0xFE00707F, 0x02000033, exec_mul},
    {0xFE00707F, 0x02001033, exec_mulh},
    {0xFE00707F, 0x02002033, exec_mulhsu},
    {0xFE00707F, 0x02003033, exec_mulhu},
    {0xFE00707F, 0x02004033, exec_div},
    {0xFE00707F, 0x02005033, exec_divu},
    {0xFE00707F, 0x02006033, exec_rem},
    {0xFE00707F, 0x02007033, exec_remu},
    {0xFE00707F, 0x0200003B, exec_mulw},
    {0xFE00707F, 0x0200403B, exec_divw},
    {0xFE00707F, 0x0200503B, exec_divuw},
    {0xFE00707F, 0x0200603B, exec_remw},
    {0xFE00707F, 0x0200703B, exec_remuw},
    {0xFE00707F, 0x20000053, exec_fsgnj_s},
    {0xFE00707F, 0x20001053, exec_fsgnjn_s},
    {0xFE00707F, 0x20002053, exec_fsgnjx_s},
    {0xFE00707F, 0x28000053, exec_fmin_s},
    {0xFE00707F, 0x28001053, exec_fmax_s},
    {0xFE00707F, 0xA0002053, exec_feq_s},
    {0xFE00707F, 0xA0001053, exec_flt_s},
    {0xFE00707F, 0xA0000053, exec_fle_s},
    {0xFE00707F, 0x22000053, exec_fsgnj_d},
    {0xFE00707F, 0x22001053, exec_fsgnjn_d},
    {0xFE00707F, 0x22002053, exec_fsgnjx_d},
    {0xFE00707F, 0x2A000053, exec_fmin_d},
    {0xFE00707F, 0x2A001053, exec_fmax_d},
    {0xFE00707F, 0xA2002053, exec_feq_d},
    {0xFE00707F, 0xA2001053, exec_flt_d},
    {0xFE00707F, 0xA2000053, exec_fle_d},
    {0xFC00707F, 0x00001013, exec_slli},
    {0xFC00707F, 0x00005013, exec_srli},
    {0xFC00707F, 0x80005013, exec_srai},
    {0xF800707F, 0x1800202F, exec_sc_w},
    {0xF800707F, 0x0800202F, exec_amoswap_w},
    {0xF800707F, 0x0000202F, exec_amoadd_w},
    {0xF800707F, 0x2000202F, exec_amoxor_w},
    {0xF800707F, 0x6000202F, exec_amoand_w},
    {0xF800707F, 0x4000202F, exec_amoor_w},
    {0xF800707F, 0x8000202F, exec_amomin_w},
    {0xF800707F, 0xA000202F, exec_amomax_w},
    {0xF800707F, 0xC000202F, exec_amominu_w},
    {0xF800707F, 0xE000202F, exec_amomaxu_w},
    {0xF800707F, 0x1800302F, exec_sc_d},
    {0xF800707F, 0x0800302F, exec_amoswap_d},
    {0xF800707F, 0x0000302F, exec_amoadd_d},
    {0xF800707F, 0x2000302F, exec_amoxor_d},
    {0xF800707F, 0x6000302F, exec_amoand_d},
    {0xF800707F, 0x4000302F, exec_amoor_d},
    {0xF800707F, 0x8000302F, exec_amomin_d},
    {0xF800707F, 0xA000302F, exec_amomax_d},
    {0xF800707F, 0xC000302F, exec_amominu_d},
    {0xF800707F, 0xE000302F, exec_amomaxu_d},
    {0xFE00007F, 0x00000053, exec_fadd_s},
    {0xFE00007F, 0x08000053, exec_fsub_s},
    {0xFE00007F, 0x10000053, exec_fmul_s},
    {0xFE00007F, 0x18000053, exec_fdiv_s},
    {0xFE00007F, 0x02000053, exec_fadd_d},
    {0xFE00007F, 0x0A000053, exec_fsub_d},
    {0xFE00007F, 0x12000053, exec_fmul_d},
    {0xFE00007F, 0x1A000053, exec_fdiv_d},
    {0x0000707F, 0x00000067, exec_jalr},
    {0x0000707F, 0x00000063, exec_beq},
    {0x0000707F, 0x00001063, exec_bne},
    {0x0000707F, 0x00004063, exec_blt},
    {0x0000707F, 0x00005063, exec_bge},
    {0x0000707F, 0x00006063, exec_bltu},
    {0x0000707F, 0x00007063, exec_bgeu},
    {0x0000707F, 0x00000003, exec_lb},
    {0x0000707F, 0x00001003, exec_lh},
    {0x0000707F, 0x00002003, exec_lw},
    {0x0000707F, 0x00004003, exec_lbu},
    {0x0000707F, 0x00005003, exec_lhu},
    {0x0000707F, 0x00000023, exec_sb},
    {0x0000707F, 0x00001023, exec_sh},
    {0x0000707F, 0x00002023, exec_sw},
    {0x0000707F, 0x00000013, exec_addi},
    {0x0000707F, 0x00002013, exec_slti},
    {0x0000707F, 0x00003013, exec_sltiu},
    {0x0000707F, 0x00004013, exec_xori},
    {0x0000707F, 0x00006013, exec_ori},
    {0x0000707F, 0x00007013, exec_andi},
    {0x0000707F, 0x0000000F, exec_fence},
    {0x0000707F, 0x00006003, exec_lwu},
    {0x0000707F, 0x00003003, exec_ld},
    {0x0000707F, 0x00003023, exec_sd},
    {0x0000707F, 0x0000001B, exec_addiw},
    {0x0000707F, 0x0000100F, exec_fence_i},
    {0x0000707F, 0x00001073, exec_cssrw},
    {0x0000707F, 0x00002073, exec_csrrs},
    {0x0000707F, 0x00003073, exec_csrrc},
    {0x0000707F, 0x00005073, exec_csrrwi},
    {0x0000707F, 0x00006073, exec_csrrsi},
    {0x0000707F, 0x00007073, exec_csrrci},
    {0x0000707F, 0x00002007, exec_flw},
    {0x0000707F, 0x00002027, exec_fsw},
    {0x0000707F, 0x00003007, exec_fld},
    {0x0000707F, 0x00003027, exec_fsd},
    {0x0600007F, 0x00000043, exec_fmadd_s},
    {0x0600007F, 0x00000047, exec_fmsub_s},
    {0x0600007F, 0x0000004B, exec_fnmsub_s},
    {0x0600007F, 0x0000004F, exec_fnmadd_s},
    {0x0600007F, 0x02000043, exec_fmadd_d},
    {0x0600007F, 0x02000047, exec_fmsub_d},
    {0x0600007F, 0x0200004B, exec_fnmsub_d},
    {0x0600007F, 0x0200004F, exec_fnmadd_d},
    {0x0000007F, 0x00000037, exec_lui},
    {0x0000007F, 0x00000017, exec_auipc},
    {0x0000007F, 0x0000006F, exec_jal},
};

const size_t instr_table_size = sizeof(instr_table) / sizeof(instr_entry_t);
