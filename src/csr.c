#include "csr.h"
#include "def.h"
#include "trap.h"

#include <stdio.h>

// NOTE:
// IMPLEMENTING CSRS LIKE THIS WAS A BIG MISTAKE JUST CREATE A FLAT 4096 BYTE ARRAY NEXT TIME
// BUT TOO MANY THINGS DEPEND ON CURRENT SETUP SO LET IT BE FOR NOW
// Note ends.

int csr_read(cpu_t* cpu, uint32_t addr, uint64_t* out) {
    uint8_t priv = (addr >> 10) & 0x3;
    if (priv > cpu->mode) {
        printf("Illegal CSR read at 0x%lX: 0x%X\n", cpu->pc, addr);
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, addr, 0);
        return 0;
    }
        
    switch (addr) {
        case CSR_FFLAGS:        *out = cpu->csr.fcsr & 0x1F; break;
        case CSR_FRM:           *out = (cpu->csr.fcsr >> 5) & 0x7; break;
        case CSR_FCSR:          *out = cpu->csr.fcsr; break;
        case CSR_VSTART:        *out = cpu->csr.vstart; break;
        case CSR_VXSAT:         *out = cpu->csr.vxsat; break;
        case CSR_VXRM:          *out = cpu->csr.vxrm; break;
        case CSR_VCSR:          *out = cpu->csr.vcsr; break;
        case CSR_VL:            *out = cpu->csr.vl; break;
        case CSR_VTYPE:         *out = cpu->csr.vtype; break;
        case CSR_VLENB:         *out = cpu->csr.vlenb; break;
        case CSR_SSP:           *out = cpu->csr.ssp; break;
        case CSR_SEED:          *out = cpu->csr.seed; break;
        case CSR_JVT:           *out = cpu->csr.jvt; break;
        case CSR_CYCLE:         *out = cpu->csr.mcycle; break;
        case CSR_TIME:          *out = cpu->csr.time; break;
        case CSR_INSTRET:       *out = cpu->csr.minstret; break;
        
        case CSR_SSTATUS:       *out = csr_read_sstatus(cpu); break;
        case CSR_SIE:           *out = cpu->csr.mie & (1ULL << 1 | 1ULL << 5 | 1ULL << 9); break;
        case CSR_STVEC:         *out = cpu->csr.stvec; break;
        case CSR_SCOUNTEREN:    *out = cpu->csr.scounteren; break;
        case CSR_SENVCFG:       *out = cpu->csr.senvcfg; break;
        case CSR_SCOUNTINHIBIT: *out = cpu->csr.scountinhibit; break;
        case CSR_SSCRATCH:      *out = cpu->csr.sscratch; break;
        case CSR_SEPC:          *out = cpu->csr.sepc; break;
        case CSR_SCAUSE:        *out = cpu->csr.scause; break;
        case CSR_STVAL:         *out = cpu->csr.stval; break;
        case CSR_SIP:           *out = cpu->csr.mip & cpu->csr.mideleg; break;
        case CSR_SCOUNTOVF:     *out = cpu->csr.scountovf; break;
        case CSR_SATP:          { if ((cpu->csr.mstatus >> 20) & 1) { raise_trap(cpu, CAUSE_ILLEGAL_INSTR, 0, 0 ); return 0; } *out = cpu->csr.satp;} break;
        case CSR_SCONTEXT:      *out = cpu->csr.scontext; break;
        case CSR_SSTATEEN0:     *out = cpu->csr.sstateen0; break;
        case CSR_SSTATEEN1:     *out = cpu->csr.sstateen1; break;
        case CSR_SSTATEEN2:     *out = cpu->csr.sstateen2; break;
        case CSR_SSTATEEN3:     *out = cpu->csr.sstateen3; break;
        
        case CSR_MVENDORID:     *out = cpu->csr.mvendorid; break;
        case CSR_MARCHID:       *out = cpu->csr.marchid; break;
        case CSR_MIMPID:        *out = cpu->csr.mimpid; break;
        case CSR_MHARTID:       *out = cpu->csr.mhartid; break;
        case CSR_MCONFIGPTR:    *out = cpu->csr.mconfigptr; break;
        case CSR_MSTATUS:       *out = cpu->csr.mstatus; break;
        case CSR_MISA:          *out = cpu->csr.misa; break;
        case CSR_MEDELEG:       *out = cpu->csr.medeleg; break;
        case CSR_MIDELEG:       *out = cpu->csr.mideleg; break;
        case CSR_MIE:           *out = cpu->csr.mie; break;
        case CSR_MTVEC:         *out = cpu->csr.mtvec; break;
        case CSR_MCOUNTEREN:    *out = cpu->csr.mcounteren; break;
        case CSR_MSCRATCH:      *out = cpu->csr.mscratch; break;
        case CSR_MEPC:          *out = cpu->csr.mepc; break;
        case CSR_MCAUSE:        *out = cpu->csr.mcause; break;
        case CSR_MTVAL:         *out = cpu->csr.mtval; break;
        case CSR_MIP:           *out = cpu->csr.mip; break;
        case CSR_MTINST:        *out = cpu->csr.mtinst; break;
        case CSR_MTVAL2:        *out = cpu->csr.mtval2; break;
        case CSR_MENVCFG:       *out = cpu->csr.menvcfg; break;
        case CSR_MSECCFG:       *out = cpu->csr.mseccfg; break;
        case CSR_MSTATEEN0:     *out = cpu->csr.mstateen0; break;
        case CSR_MSTATEEN1:     *out = cpu->csr.mstateen1; break;
        case CSR_MSTATEEN2:     *out = cpu->csr.mstateen2; break;
        case CSR_MSTATEEN3:     *out = cpu->csr.mstateen3; break;

        case CSR_MNSCRATCH:     *out = cpu->csr.mnscratch; break;
        case CSR_MNEPC:         *out = cpu->csr.mnepc; break;
        case CSR_MNCAUSE:       *out = cpu->csr.mncause; break;
        case CSR_MNSTATUS:      *out = cpu->csr.mnstatus; break;
        case CSR_MCYCLE:        *out = cpu->csr.mcycle; break;
        case CSR_MINSTRET:      *out = cpu->csr.minstret; break;
        case CSR_MCOUNTINHIBIT: *out = cpu->csr.mcountinhibit; break;

        case CSR_TSELECT:       *out = cpu->csr.tselect; break;
        case CSR_TDATA1:        *out = cpu->csr.tdata1; break;
        case CSR_TDATA2:        *out = cpu->csr.tdata2; break;
        case CSR_TDATA3:        *out = cpu->csr.tdata3; break;
        case CSR_TCONTROL:      *out = cpu->csr.tcontrol; break;
        case CSR_MCONTEXT:      *out = cpu->csr.mcontext; break;
        case CSR_DCSR:          *out = cpu->csr.dcsr; break;
        case CSR_DPC:           *out = cpu->csr.dpc; break;
        case CSR_DSCRATCH0:     *out = cpu->csr.dscratch0; break;
        case CSR_DSCRATCH1:     *out = cpu->csr.dscratch1; break;
        default:
            if (addr >= CSR_HPMCOUNTER3 && addr <= CSR_HPMCOUNTER3 + 0x1C) {
                *out = cpu->csr.mhpmcounter3[addr - CSR_HPMCOUNTER3];
            } else if (addr >= CSR_MHPMCOUNTER3 && addr <= CSR_MHPMCOUNTER3 + 0x1C) {
                *out = cpu->csr.mhpmcounter3[addr - CSR_MHPMCOUNTER3];
            } else if (addr >= CSR_MHPMEVENT3 && addr <= CSR_MHPMEVENT3 + 0x1C) {
                *out = cpu->csr.mhpmevent3[addr - CSR_MHPMEVENT3];
            } else if (addr >= CSR_PMPCFG0  && addr <= CSR_PMPCFG0 + 0xF) {
                if ((addr & 1) == 0) {
                    *out = cpu->csr.pmpcfg[(addr - CSR_PMPCFG0 ) >> 1];
                } else {
                    raise_trap(cpu, CAUSE_ILLEGAL_INSTR, addr, 0);
                    return 0;
                }
            }  else if (addr >= CSR_PMPADDR0 && addr <= CSR_PMPADDR0 + 0x3F) {
                *out = cpu->csr.pmpaddr[addr - CSR_PMPADDR0];
            }  else {
                printf("Illegal CSR read: 0x%X\n", addr);
                raise_trap(cpu, CAUSE_ILLEGAL_INSTR, addr, 0);
                return 0;
            }
            break;
    }
    
    return 1; // Success
}

int csr_write(cpu_t* cpu, uint32_t addr, uint64_t val) {
    uint8_t priv = (addr >> 10) & 0x3;
    if (priv > cpu->mode) {
        raise_trap(cpu, CAUSE_ILLEGAL_INSTR, addr, 0);
        return 0;
    }
    
    switch (addr) {
        case CSR_FFLAGS:        cpu->csr.fcsr = (cpu->csr.fcsr & ~0x1F) | (val & 0x1F); break;
        case CSR_FRM:           cpu->csr.fcsr = (cpu->csr.fcsr & ~0xE0) | ((val & 0x7) << 5); break;
        case CSR_FCSR:          cpu->csr.fcsr = val; break;
        case CSR_VSTART:        cpu->csr.vstart = val; break;
        case CSR_VXSAT:         cpu->csr.vxsat = val; break;
        case CSR_VXRM:          cpu->csr.vxrm = val; break;
        case CSR_VCSR:          cpu->csr.vcsr = val; break;
        case CSR_VL:            break;
        case CSR_VTYPE:         break;
        case CSR_VLENB:         break;
        case CSR_SSP:           cpu->csr.ssp = val; break;
        case CSR_SEED:          cpu->csr.seed = val; break;
        case CSR_JVT:           cpu->csr.jvt = val; break;
        case CSR_CYCLE:         break;
        case CSR_TIME:          break;
        case CSR_INSTRET:       break;
        
        // Supervisor CSRs
        case CSR_SSTATUS:       csr_write_sstatus(cpu, val); break;
        case CSR_SIE:           { cpu->csr.mie &= ~(1ULL << 1 | 1ULL << 5 | 1ULL << 9); cpu->csr.mie |= (val & (1ULL << 1 | 1ULL << 5 | 1ULL << 9)); } break;
        case CSR_STVEC:         cpu->csr.stvec = val; break;
        case CSR_SCOUNTEREN:    cpu->csr.scounteren = val; break;
        case CSR_SENVCFG:       cpu->csr.senvcfg = val; break;
        case CSR_SCOUNTINHIBIT: cpu->csr.scountinhibit = val; break;
        case CSR_SSCRATCH:      cpu->csr.sscratch = val; break;
        case CSR_SEPC:          cpu->csr.sepc = val; break;
        case CSR_SCAUSE:        cpu->csr.scause = val; break;
        case CSR_STVAL:         cpu->csr.stval = val; break;
        case CSR_SIP:           { cpu->csr.mip &= ~(1ULL << 1); cpu->csr.mip |= (val & (1ULL << 1)); } break;
        case CSR_SCOUNTOVF:     break;
        case CSR_SATP:          { if ((cpu->csr.mstatus >> 20) & 1) { raise_trap(cpu, CAUSE_ILLEGAL_INSTR, 0, 0 ); return 0; } cpu->csr.satp = val;} break;
        case CSR_SCONTEXT:      cpu->csr.scontext = val; break;
        case CSR_SSTATEEN0:     cpu->csr.sstateen0 = val; break;
        case CSR_SSTATEEN1:     cpu->csr.sstateen1 = val; break;
        case CSR_SSTATEEN2:     cpu->csr.sstateen2 = val; break;
        case CSR_SSTATEEN3:     cpu->csr.sstateen3 = val; break;
        
        // Machine CSRs
        case CSR_MVENDORID:     break;
        case CSR_MARCHID:       break;
        case CSR_MIMPID:        break;
        case CSR_MHARTID:       break;
        case CSR_MCONFIGPTR:    break;
        case CSR_MSTATUS:       { cpu->csr.mstatus = val; hardwire_mstatus(cpu); } break;
        case CSR_MISA:          break;
        case CSR_MEDELEG:       cpu->csr.medeleg = val; break;
        case CSR_MIDELEG:       cpu->csr.mideleg = val; break;
        case CSR_MIE:           cpu->csr.mie = val; break;
        case CSR_MTVEC:         cpu->csr.mtvec = val; break;
        case CSR_MCOUNTEREN:    cpu->csr.mcounteren = val; break;
        case CSR_MSCRATCH:      cpu->csr.mscratch = val; break;
        case CSR_MEPC:          cpu->csr.mepc = val; break;
        case CSR_MCAUSE:        cpu->csr.mcause = val; break;
        case CSR_MTVAL:         cpu->csr.mtval = val; break;
        case CSR_MIP:           cpu->csr.mip = val; break;
        case CSR_MTINST:        cpu->csr.mtinst = val; break;
        case CSR_MTVAL2:        cpu->csr.mtval2 = val; break;
        case CSR_MENVCFG:       cpu->csr.menvcfg = val; break;
        case CSR_MSECCFG:       cpu->csr.mseccfg = val; break;
        case CSR_MSTATEEN0:     cpu->csr.mstateen0 = val; break;
        case CSR_MSTATEEN1:     cpu->csr.mstateen1 = val; break;
        case CSR_MSTATEEN2:     cpu->csr.mstateen2 = val; break;
        case CSR_MSTATEEN3:     cpu->csr.mstateen3 = val; break;

        case CSR_MNSCRATCH:     cpu->csr.mnscratch = val; break;
        case CSR_MNEPC:         cpu->csr.mnepc = val; break;
        case CSR_MNCAUSE:       cpu->csr.mncause = val; break;
        case CSR_MNSTATUS:      cpu->csr.mnstatus = val; break;
        case CSR_MCYCLE:        cpu->csr.mcycle = val; break;
        case CSR_MINSTRET:      cpu->csr.minstret = val; break;
        case CSR_MCOUNTINHIBIT: cpu->csr.mcountinhibit = val;

        case CSR_TSELECT:       cpu->csr.tselect = val; break;
        case CSR_TDATA1:        cpu->csr.tdata1 = val; break;
        case CSR_TDATA2:        cpu->csr.tdata2 = val; break;
        case CSR_TDATA3:        cpu->csr.tdata3 = val; break;
        case CSR_TCONTROL:      cpu->csr.tcontrol = val; break;
        case CSR_MCONTEXT:      cpu->csr.mcontext = val; break;
        case CSR_DCSR:          cpu->csr.dcsr = val; break;
        case CSR_DPC:           cpu->csr.dpc = val; break;
        case CSR_DSCRATCH0:     cpu->csr.dscratch0 = val; break;
        case CSR_DSCRATCH1:     cpu->csr.dscratch1 = val; break;
        default:
            if (addr >= CSR_HPMCOUNTER3 && addr <= CSR_HPMCOUNTER3 + 0x1C) {
                break;
            } else if (addr >= CSR_MHPMCOUNTER3 && addr <= CSR_MHPMCOUNTER3 + 0x1C) {
                cpu->csr.mhpmcounter3[addr - CSR_MHPMCOUNTER3] = val;
            } else if (addr >= CSR_MHPMEVENT3 && addr <= CSR_MHPMEVENT3 + 0x1C) {
                cpu->csr.mhpmevent3[addr - CSR_MHPMEVENT3] = val;
            } else if (addr >= CSR_PMPCFG0  && addr <= CSR_PMPCFG0 + 0xF) {
                if ((addr & 1) == 0) { // Only even addresses for RV64
                    cpu->csr.pmpcfg[(addr - CSR_PMPCFG0 ) >> 1] = val;
                } else {
                    raise_trap(cpu, CAUSE_ILLEGAL_INSTR, addr, 0);
                    return 0;
                }
            }  else if (addr >= CSR_PMPADDR0 && addr <= CSR_PMPADDR0 + 0x3F) {
                cpu->csr.pmpaddr[addr - CSR_PMPADDR0] = val;
            }  else {
                //! TODO: Raise trap if csr do not exist, if RO csr silently ignore
                raise_trap(cpu, CAUSE_ILLEGAL_INSTR, addr, 0);
                return 0;
            }
            break;
    }
    
    return 1;
}