#include "trap.h"

#include <stdio.h>

void raise_trap(cpu_t* cpu, uint64_t cause, uint64_t tval, int is_interrupt) {
    uint64_t medeleg = cpu->csr.medeleg;
    int delegate = 0;

    printf("trap: cause=%lu, tval=%lx, is_interrupt=%d, mode=%d\n", cause, tval, is_interrupt, cpu->mode);

    // only delegate if current priv <= S
    if (!is_interrupt && cpu->mode <= PRIV_S && (medeleg >> cause) & 1)
        delegate = 1;

    if (delegate) {
        uint64_t sstatus = cpu->csr.sstatus;

        cpu->csr.sepc = cpu->pc;
        cpu->csr.scause = cause | ((uint64_t)is_interrupt << 63);
        cpu->csr.stval = tval;

        uint64_t spp = cpu->mode;
        sstatus = (sstatus & ~(1UL << 8)) | ((spp & 1) << 8);  // SPP
        sstatus = (sstatus & ~(1UL << 5)) | (((sstatus >> 1) & 1) << 5); // SPIE = SIE
        sstatus &= ~0x2UL; // SIE = 0
        cpu->csr.sstatus = sstatus;

        uint64_t stvec = cpu->csr.stvec;
        if ((stvec & 1) == 0)
            cpu->npc = stvec;
        else
            cpu->npc = (stvec & ~0x3) + 4 * cause; // vectored

        cpu->mode = PRIV_S;
    } else {
        uint64_t mstatus = cpu->csr.mstatus;

        cpu->csr.mepc = cpu->pc;
        cpu->csr.mcause = cause | ((uint64_t)is_interrupt << 63);
        cpu->csr.mtval = tval;

        uint64_t mpp = cpu->mode;
        mstatus = (mstatus & ~(3UL << 11)) | (mpp << 11); // MPP
        mstatus = (mstatus & ~(1UL << 7)) | (((mstatus >> 3) & 1) << 7); // MPIE = MIE
        mstatus &= ~(1UL << 3); // MIE = 0
        cpu->csr.mstatus = mstatus;

        uint64_t mtvec = cpu->csr.mtvec;
        if ((mtvec & 1) == 0)
            cpu->npc = mtvec;
        else
            cpu->npc = (mtvec & ~0x3) + 4 * cause;

        cpu->mode = PRIV_M;
    }
}