#ifndef __CPU_H__
#define __CPU_H__

#include "def.h"

// Single core for now
#define HART_COUNT 1

enum {
    PRIV_U = 0,
    PRIV_S = 1,
    PRIV_H = 2,
    PRIV_M = 3
};

typedef struct {
    // User
    uint64_t fflags;            // 0x001
    uint64_t frm;               // 0x002
    uint64_t fcsr;              // 0x003
    uint64_t vstart;            // 0x008
    uint64_t vxsat;             // 0x009
    uint64_t vxrm;              // 0x00A
    uint64_t vcsr;              // 0x00F
    uint64_t vl;                // 0xC20 (RO)
    uint64_t vtype;             // 0xC21 (RO)
    uint64_t vlenb;             // 0xC22 (RO)
    uint64_t ssp;               // 0x011
    uint64_t seed;              // 0x015
    uint64_t jvt;               // 0x17
    uint64_t mcycle;            // 0xC00 (RO)
    uint64_t time;              // 0xC01 (RO)
    uint64_t minstret;          // 0xC02 (RO)
    uint64_t mhpmcounter3[29];  // 0xC03-0xC1F (RO)

    // Supervisor
    uint64_t sstatus;           // 0x100
    uint64_t sie;               // 0x104
    uint64_t stvec;             // 0x105
    uint64_t scounteren;        // 0x106
    uint64_t senvcfg;           // 0x10A
    uint64_t scountinhibit;     // 0x10B
    uint64_t sscratch;          // 0x140
    uint64_t sepc;              // 0x141
    uint64_t scause;            // 0x142
    uint64_t stval;             // 0x143
    uint64_t sip;               // 0x144
    uint64_t scountovf;         // 0xDA0 (RO)
    uint64_t satp;              // 0x180
    uint64_t scontext;          // 0x5A8 (RO)
    uint64_t sstateen0;         // 0x10C
    uint64_t sstateen1;         // 0x10D
    uint64_t sstateen2;         // 0x10E
    uint64_t sstateen3;         // 0x10F

    // Machine
    uint64_t mvendorid;         // 0xF11 (RO)
    uint64_t marchid;           // 0xF12 (RO)
    uint64_t mimpid;            // 0xF13 (RO)
    uint64_t mhartid;           // 0xF14 (RO)
    uint64_t mconfigptr;        // 0xF15 (RO)
    uint64_t mstatus;           // 0x300
    uint64_t misa;              // 0x301
    uint64_t medeleg;           // 0x302
    uint64_t mideleg;           // 0x303
    uint64_t mie;               // 0x304
    uint64_t mtvec;             // 0x305
    uint64_t mcounteren;        // 0x306
    uint64_t mscratch;          // 0x340
    uint64_t mepc;              // 0x341
    uint64_t mcause;            // 0x342
    uint64_t mtval;             // 0x343
    uint64_t mip;               // 0x344
    uint64_t mtinst;            // 0x34A
    uint64_t mtval2;            // 0x34B
    uint64_t menvcfg;           // 0x30A
    uint64_t mseccfg;           // 0x747
    uint64_t pmpcfg[8];         // 0x3A0, (0, 2, 4 ... 14)
    uint64_t pmpaddr[64];       // 0x3B0-0x3EF
    uint64_t mstateen0;         // 0x30C
    uint64_t mstateen1;         // 0x30D
    uint64_t mstateen2;         // 0x30E
    uint64_t mstateen3;         // 0x30F

    uint64_t mnscratch;         // 0x740
    uint64_t mnepc ;            // 0x741
    uint64_t mncause;           // 0x742
    uint64_t mnstatus;          // 0x744
    uint64_t mcountinhibit;     // 0x320
    uint64_t mhpmevent3[29];    // 0x323-0x33F

    uint64_t tselect;           // 0x7A0
    uint64_t tdata1;            // 0x7A1
    uint64_t tdata2;            // 0x7A2
    uint64_t tdata3;            // 0x7A3
    uint64_t tcontrol;          // 0x7A5
    uint64_t mcontext;          // 0x7A8
    uint64_t dcsr;              // 0x7B0
    uint64_t dpc;               // 0x7B1
    uint64_t dscratch0;         // 0x7B2
    uint64_t dscratch1;         // 0x7B3
} csr_file_t;

typedef struct {
    uint8_t halted;
    uint8_t mode;

    uint64_t x[32];
    uint64_t f[32];

    uint64_t pc;
    uint64_t npc;
    
    uint64_t lr_addr;
    uint8_t lr_valid;

    csr_file_t csr;
} cpu_t;

void cpu_step(cpu_t* cpu);
void cpu_dump(cpu_t* cpu);

int cpu_check_interrupts(cpu_t* cpu);

#endif