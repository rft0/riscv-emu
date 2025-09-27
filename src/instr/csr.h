#ifndef __CSR_H__
#define __CSR_H__

#include "../cpu.h"

#define CSR_FFLAGS                  0x001
#define CSR_FRM                     0x002
#define CSR_FCSR                    0x003
#define CSR_VSTART                  0x008
#define CSR_VXSAT                   0x009
#define CSR_VXRM                    0x00A
#define CSR_VCSR                    0x00F
#define CSR_VL                      0xC20
#define CSR_VTYPE                   0xC21
#define CSR_VLENB                   0xC22
#define CSR_SSP                     0x011
#define CSR_SEED                    0x015
#define CSR_JVT                     0x017
#define CSR_CYCLE                   0xC00
#define CSR_TIME                    0xC01
#define CSR_INSTRET                 0xC02
#define CSR_HPMCOUNTER3             0xC03
#define CSR_SSTATUS                 0x100
#define CSR_SIE                     0x104
#define CSR_STVEC                   0x105
#define CSR_SCOUNTEREN              0x106
#define CSR_SENVCFG                 0x10A
#define CSR_SCOUNTINHIBIT           0x10B
#define CSR_SSCRATCH                0x140
#define CSR_SEPC                    0x141
#define CSR_SCAUSE                  0x142
#define CSR_STVAL                   0x143
#define CSR_SIP                     0x144
#define CSR_SCOUNTOVF               0xDA0
#define CSR_SATP                    0x180
#define CSR_SCONTEXT                0x5A8
#define CSR_SSTATEEN0               0x10C
#define CSR_SSTATEEN1               0x10D
#define CSR_SSTATEEN2               0x10E
#define CSR_SSTATEEN3               0x10F
#define CSR_MVENDORID               0xF11
#define CSR_MARCHID                 0xF12
#define CSR_MIMPID                  0xF13
#define CSR_MHARTID                 0xF14
#define CSR_MCONFIGPTR              0xF15
#define CSR_MSTATUS                 0x300
#define CSR_MISA                    0x301
#define CSR_MEDELEG                 0x302
#define CSR_MIDELEG                 0x303
#define CSR_MIE                     0x304
#define CSR_MTVEC                   0x305
#define CSR_MCOUNTEREN              0x306
#define CSR_MSCRATCH                0x340
#define CSR_MEPC                    0x341
#define CSR_MCAUSE                  0x342
#define CSR_MTVAL                   0x343
#define CSR_MIP                     0x344
#define CSR_MTINST                  0x34A
#define CSR_MTVAL2                  0x34B
#define CSR_MENVCFG                 0x30A
#define CSR_MSECCFG                 0x747
#define CSR_PMPCFG0                 0x3A0
#define CSR_PMPADDR0                0x3B0
#define CSR_MSTATEEN0               0x30C
#define CSR_MSTATEEN1               0x30D
#define CSR_MSTATEEN2               0x30E
#define CSR_MSTATEEN3               0x30F

int csr_write(cpu_t* cpu, uint32_t addr, uint64_t val);
int csr_read(cpu_t* cpu, uint32_t addr, uint64_t* out);

#endif