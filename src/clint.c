#include "clint.h"

#include "emu.h"

#include <string.h>

#define MIP_MSIP                        (1 << 3)
#define MIP_MTIP                        (1 << 7)
#define MIP_MEIP                        (1 << 11)

#define CLINT_OFF_MSIP                  0x0
#define CLINT_OFF_MTIMECMP              0x4000
#define CLINT_OFF_MTIME                 0xBFF8

#define TIMER_FREQ                      10000000  // 10 MHz

extern emu_t* g_emu;

static uint64_t mtimecmp[HART_COUNT];
static uint32_t msip[HART_COUNT];

static uint64_t mtime = 0;

void clint_tick(uint64_t increment) {
    mtime += increment;
}

void clint_update(cpu_t* cpu) {
    if (mtime >= mtimecmp[cpu->csr.mhartid])
        cpu->csr.mip |= MIP_MTIP;    
}

int clint_read(uint64_t addr, void* out, size_t size) {
    uint64_t offset = addr - CLINT_BASE;

    if (offset >= CLINT_OFF_MSIP && offset < CLINT_OFF_MSIP + 0x4 * HART_COUNT) {
        uint32_t hartid = (offset - CLINT_OFF_MSIP) / 4;
        uint8_t reg_offset = (offset - CLINT_OFF_MSIP) % 4;

        if (hartid < HART_COUNT)
            memcpy(out, (uint8_t*)&msip[hartid] + reg_offset, size);
        else
            memset(out, 0, size);

        return 1;
    } else if (offset >= CLINT_OFF_MTIMECMP && offset < CLINT_OFF_MTIMECMP + 0x8 * HART_COUNT) {
        uint32_t hartid = (offset - CLINT_OFF_MTIMECMP) / 8;
        uint8_t reg_offset = (offset - CLINT_OFF_MTIMECMP) % 8;

        if (hartid < HART_COUNT)
            memcpy(out, (uint8_t*)&mtimecmp[hartid] + reg_offset, size);
        else
            memset(out, 0, size);

        return 1;
    } else if (offset >= CLINT_OFF_MTIME && offset < CLINT_OFF_MTIME + 0x8) {
        uint8_t reg_offset = offset - CLINT_OFF_MTIME;
        memcpy(out, (uint8_t*)&mtime + reg_offset, size);
        return 1;
    }

    memset(out, 0, size);
    return 1;
}

int clint_write(uint64_t addr, void* pval, size_t size) {
    uint64_t offset = addr - CLINT_BASE;

    if (offset >= CLINT_OFF_MSIP && offset < CLINT_OFF_MSIP + 0x8 * HART_COUNT) {
        uint32_t hartid = (offset - CLINT_OFF_MSIP) / 4;
        uint8_t reg_offset = (offset - CLINT_OFF_MSIP) % 4;
        
        if (hartid < HART_COUNT) {
            memcpy((uint8_t*)&msip[hartid] + reg_offset, pval, size);
            
            //! TODO: Update target CPU, it is ok for now our emulator is single core.
            // cpu_t* target_cpu = get_cpu_by_hartid(hartid);
            if (msip[hartid] & 0x1)
                g_emu->cpu.csr.mip |= MIP_MSIP;
            else
                g_emu->cpu.csr.mip &= ~MIP_MSIP;
        }

        return 1;
    } else if (offset >= CLINT_OFF_MTIMECMP && offset < CLINT_OFF_MTIMECMP + 0x8 * HART_COUNT) {
        uint32_t hartid = (offset - CLINT_OFF_MTIMECMP) / 8;
        uint8_t reg_offset = (offset - CLINT_OFF_MTIMECMP) % 8;
        
        if (hartid < HART_COUNT) {
            memcpy((uint8_t*)&mtimecmp[hartid] + reg_offset, pval, size);
            
            if (mtime >= mtimecmp[hartid])
                g_emu->cpu.csr.mip |= MIP_MTIP;
            else
                g_emu->cpu.csr.mip &= ~MIP_MTIP;
        }

        return 1;
    } else if (offset >= CLINT_OFF_MTIME && offset < CLINT_OFF_MTIME + 0x8) {
        uint8_t reg_offset = offset - CLINT_OFF_MTIME;
        memcpy((uint8_t*)&mtime + reg_offset, pval, size);
        return 1;
    }

    return 1;
}