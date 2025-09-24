#include "csr.h"

#include <string.h>

void csr_write(cpu_t* cpu, uint32_t addr, uint64_t val) {
    uint8_t priv = extract32(addr, 11, 10);
    uint8_t ro = 0; // Assume all RW for now

    // if (priv < csr->priv) {
    //     trap_illegal_csr(cpu, addr);
    //     return;
    // }

    // if (ro) {
    //     trap_illegal_csr(cpu, addr);
    //     return;
    // }

    cpu->csr[addr] = val;
}

void csrs_init(cpu_t* cpu) {
    memset(cpu->csr, 0, sizeof(cpu->csr));

    // M Info (RO)
    cpu->csr[0XF11] = 0; // mvendorid
    cpu->csr[0XF12] = 0; // marchid
    cpu->csr[0XF13] = 0; // mimpid
    cpu->csr[0XF14] = 0; // mhartid
    cpu->csr[0xF15] = 0; // mconfigptr

    cpu->csr[0x305] = 0; // Trap Handler base address

    // S-Mode
    cpu->csr[0x105] = 0; // S-Mode Trap Handler base address
}