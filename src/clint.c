#include "clint.h"

#include <string.h>

#define MIP_MSIP                        (1 << 3)
#define MIP_MTIP                        (1 << 7)
#define MIP_MEIP                        (1 << 11)

#define CLINT_OFF_MSIP                  0x0
#define CLINT_OFF_MTIMECMP              0x4000
#define CLINT_OFF_MTIME                 0xBFF8

#define TIMER_FREQ   10000000  // 10 MHz

static uint64_t mtime = 0;

static uint32_t msip[MAX_HARTS];
static uint64_t mtimecmp[MAX_HARTS];

//! TODO: THIS IS BAD USE HOSTS SYSTEM CLOCK IF NEEDED.
void clint_tick(cpu_t* cpu, uint64_t cycles) {
    mtime += cycles;

    if (mtime >= mtimecmp[cpu->hartid])
        cpu->csr.mip |= MIP_MTIP;
    else
        cpu->csr.mip &= ~MIP_MTIP;
}


int clint_read(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* out, size_t size) {
    uint64_t offset = addr - CLINT_BASE;

    if (offset >= CLINT_OFF_MSIP && offset < CLINT_OFF_MSIP + 0x8 * MAX_HARTS) {
        uint8_t reg_offset = offset - CLINT_OFF_MSIP;
        memcpy(out, (uint8_t*)&msip + reg_offset, size);
        return 1;
    } else if (offset >= CLINT_OFF_MTIMECMP && offset < CLINT_OFF_MTIMECMP + 0x8 * MAX_HARTS) {
        uint8_t reg_offset = offset - CLINT_OFF_MTIMECMP;
        memcpy(out, (uint8_t*)&mtimecmp + reg_offset, size);
        return 1;
    } else if (offset >= CLINT_OFF_MTIME && offset < CLINT_OFF_MTIME + 0x8) {
        uint8_t reg_offset = offset - CLINT_OFF_MTIME;
        memcpy(out, (uint8_t*)&mtime + reg_offset, size);
        return 1;
    } else {
        memset(out, 0, size);
        return 1;
    }
}

int clint_write(mem_region_t* region, cpu_t* cpu, uint64_t addr, void* val, size_t size) {
    uint64_t offset = addr - CLINT_BASE;

    if (offset >= CLINT_OFF_MSIP && offset < CLINT_OFF_MSIP + 0x8 * MAX_HARTS) {
        uint8_t reg_offset = offset - CLINT_OFF_MSIP;
        memcpy((uint8_t*)&msip + reg_offset, val, size);
        
        if (msip[cpu->hartid] & 0x1)
            cpu->csr.mip |= MIP_MSIP;
        else
            cpu->csr.mip &= ~MIP_MSIP;

        return 1;
    } else if (offset >= CLINT_OFF_MTIMECMP && offset < CLINT_OFF_MTIMECMP + 0x8 * MAX_HARTS) {
        uint8_t reg_offset = offset - CLINT_OFF_MTIMECMP;
        memcpy((uint8_t*)&mtimecmp + reg_offset, val, size);
        
        if (mtime >= mtimecmp[cpu->hartid])
            cpu->csr.mip |= MIP_MTIP;
        else
            cpu->csr.mip &= ~MIP_MTIP;

        return 1;
    } else if (offset >= CLINT_OFF_MTIME && offset < CLINT_OFF_MTIME + 0x8) {
        uint8_t reg_offset = offset - CLINT_OFF_MTIME;
        memcpy((uint8_t*)&mtime + reg_offset, val, size);
        return 1;
    } else {
        return 1;
    }
}